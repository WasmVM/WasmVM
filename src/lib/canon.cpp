// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Store-wide type identity for the GC and reference-type proposals.
//
// A struct, an array or a function type is written in a module as a local
// index into that module's type section. The index means nothing in another
// module, so a cast or an import check that compares two indices across a
// module boundary is a type-confusion bug (#39, #38). This gives every type a
// Store-wide CanonId instead.
//
// The method is the spec's isorecursive rolling (`valid/conventions`,
// rolling-and-unrolling): a recursion group is closed by rewriting each
// reference to a type in the same group as a relative index, and each
// reference to an earlier group as the CanonId that group already resolved to.
// Two groups that roll to the same closed form are the same type, so the
// registry de-duplicates them to one run of CanonIds. The type section's own
// validation forbids a forward reference across groups
// (`valid/modules`, valid-type / valid-types), so one ordered pass assigns
// every identity. (Deduced from those anchors; not spec verbatim.)

#include <instances/CanonRegistry.hpp>
#include <structures/WasmModule.hpp>
#include <Util.hpp>

#include <algorithm>
#include <functional>
#include <map>

using namespace WasmVM;

namespace {

using IndexMap = std::function<index_t(index_t)>;

// Rewrite every concrete reference's type index in a value type through `map`.
void map_valtype(ValueType& type, const IndexMap& map){
    if(type.is_concrete()){
        type.typeidx = map(type.typeidx);
    }
}

void map_storage(StorageType& storage, const IndexMap& map){
    if(std::holds_alternative<ValueType>(storage)){
        map_valtype(std::get<ValueType>(storage), map);
    }
}

// Rewrite every type index a subtype mentions -- its supertypes and the
// references inside its composite type -- through `map`.
void map_subtype(SubType& sub, const IndexMap& map){
    for(index_t& super : sub.supers){
        super = map(super);
    }
    std::visit(overloaded {
        [&](FuncType& func){
            for(ValueType& v : func.params){ map_valtype(v, map); }
            for(ValueType& v : func.results){ map_valtype(v, map); }
        },
        [&](StructType& st){
            for(FieldType& field : st.fields){ map_storage(field.storage, map); }
        },
        [&](ArrayType& arr){
            map_storage(arr.field.storage, map);
        }
    }, sub.comp);
}

// The closed key of one group member. References are encoded relative within
// the group (`R<offset>`) and as the resolved CanonId outside it (`C<id>`), so
// the key does not depend on where the group lands in the registry -- only on
// its shape and on the identities it already depends on.
std::string member_key(const SubType& sub, const IndexMap& encode){
    std::string key;
    auto ref = [&](index_t idx){
        key += '[';
        key += std::to_string(encode(idx));
        key += ']';
    };
    auto valtype = [&](const ValueType& v){
        if(v.is_concrete()){
            key += 'r';
            key += (v.nullable ? '1' : '0');
            ref(v.typeidx);
        }else{
            key += 'k';
            key += std::to_string((int)v.kind);
            if(v.is_ref()){ key += (v.nullable ? '1' : '0'); }
        }
        key += ',';
    };
    auto field = [&](const FieldType& f){
        key += f.mut ? 'm' : 'c';
        std::visit(overloaded {
            [&](const ValueType& v){ valtype(v); },
            [&](PackedType p){ key += (p == PackedType::i8) ? 'b' : 'h'; }
        }, f.storage);
        key += '.';
    };
    key += sub.final ? 'F' : 'f';
    for(index_t super : sub.supers){
        key += 's';
        ref(super);
    }
    key += ':';
    std::visit(overloaded {
        [&](const FuncType& func){
            key += 'U';
            for(const ValueType& v : func.params){ valtype(v); }
            key += '>';
            for(const ValueType& v : func.results){ valtype(v); }
        },
        [&](const StructType& st){
            key += 'S';
            for(const FieldType& f : st.fields){ field(f); }
        },
        [&](const ArrayType& arr){
            key += 'A';
            field(arr.field);
        }
    }, sub.comp);
    return key;
}

} // namespace

void WasmVM::canonicalize_types(std::vector<CanonId>& type_ids, const WasmModule& module, CanonRegistry& reg){
    const index_t count = (index_t)module.types.size();
    type_ids.assign(count, 0);

    // Where each explicit `(rec ...)` group starts and how long it is. A type
    // outside every explicit group is its own group of one.
    std::map<index_t, index_t> explicit_groups;
    for(const RecGroup& group : module.recs){
        explicit_groups[group.first] = group.count;
    }

    index_t first = 0;
    while(first < count){
        index_t group_count = 1;
        auto found = explicit_groups.find(first);
        if(found != explicit_groups.end()){
            group_count = found->second;
        }
        const index_t group_first = first;
        const index_t group_last = first + group_count; // exclusive

        // References inside this group are relative; references to an earlier
        // group are its already-assigned CanonId.
        IndexMap encode = [&](index_t idx) -> index_t {
            if((idx >= group_first) && (idx < group_last)){
                return idx - group_first; // relative, base-independent
            }
            return type_ids[idx]; // earlier group, already resolved
        };

        std::string key;
        key += std::to_string(group_count);
        key += '/';
        for(index_t i = group_first; i < group_last; ++i){
            key += member_key(module.types[i], encode);
            key += ';';
        }

        CanonId base;
        auto existing = reg.groups.find(key);
        if(existing != reg.groups.end()){
            base = existing->second;
        }else{
            base = (CanonId)reg.types.size();
            reg.groups.emplace(key, base);
            // Store the closed form: within-group references become the
            // CanonIds this group is being assigned, earlier ones stay as the
            // CanonIds they resolved to.
            IndexMap absolute = [&](index_t idx) -> index_t {
                if((idx >= group_first) && (idx < group_last)){
                    return base + (idx - group_first);
                }
                return type_ids[idx];
            };
            for(index_t i = group_first; i < group_last; ++i){
                SubType closed = module.types[i];
                map_subtype(closed, absolute);
                reg.types.emplace_back(std::move(closed));
            }
        }
        for(index_t i = 0; i < group_count; ++i){
            type_ids[group_first + i] = base + i;
        }
        first = group_last;
    }
}

bool WasmVM::canon_matches(CanonId a, CanonId b, const CanonRegistry& reg){
    // The registry is one closed type section indexed by CanonId, so the
    // module-local structural check is exactly right here: equal CanonId is
    // identity, and a declared supertype is a strictly smaller CanonId, which
    // keeps the recursion finite.
    return comp_matches(a, b, reg.types);
}

bool WasmVM::canon_cast_matches(CanonId a, CanonId b, const CanonRegistry& reg){
    // Declared edges only: `a` matches `b` when it is `b` or declares a
    // supertype that matches `b`. No structural-refinement fallback, so an
    // undeclared type that merely refines `b`'s shape does not match. A
    // declared supertype is always a strictly smaller CanonId, so this
    // terminates.
    if(a == b){
        return true;
    }
    if(a >= reg.types.size()){
        return false;
    }
    for(CanonId super : reg.types[a].supers){
        if(canon_cast_matches(super, b, reg)){
            return true;
        }
    }
    return false;
}

CanonId WasmVM::canonicalize_functype(const FuncType& type, CanonRegistry& reg){
    // A host function's type has no type section around it: treat it as a group
    // of one -- a final composite type with no supertypes -- so its closed key
    // matches a module's identically-shaped singleton function type and the two
    // de-duplicate to one CanonId. A host function type carries no concrete
    // references, so the within-group encoding never actually fires.
    SubType sub(type);
    IndexMap encode = [](index_t idx) -> index_t { return idx; };
    std::string key = std::to_string(1);
    key += '/';
    key += member_key(sub, encode);
    key += ';';
    auto found = reg.groups.find(key);
    if(found != reg.groups.end()){
        return found->second;
    }
    CanonId base = (CanonId)reg.types.size();
    reg.groups.emplace(key, base);
    SubType closed = sub;
    IndexMap absolute = [&](index_t idx) -> index_t { return base + idx; };
    map_subtype(closed, absolute);
    reg.types.emplace_back(std::move(closed));
    return base;
}
