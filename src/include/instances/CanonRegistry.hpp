#ifndef WASMVM_PP_INSTANCES_CanonRegistry_DEF
#define WASMVM_PP_INSTANCES_CanonRegistry_DEF

#include <Types.hpp>
#include <vector>
#include <string>
#include <unordered_map>

namespace WasmVM {

struct WasmModule;

// A Store-wide identity for every type that any module instantiated into the
// Store has declared. A local type index is meaningful only inside the module
// that wrote it, so comparing two indices across modules is a type-confusion
// bug. The registry rolls each module's recursion groups to a closed form --
// internal references become relative, external ones become the CanonIds they
// already resolved to -- which makes recursively-equal types compare equal.
// Equal types are de-duplicated to one CanonId, so identity is `==` and
// subtyping is the declared-supertype chain, both Store-wide.
struct CanonRegistry {
    // Indexed by CanonId. A concrete reference inside one of these SubTypes has
    // its `typeidx` already rewritten to the CanonId it names, so the whole
    // table is closed and the existing structural `comp_matches` runs over it
    // directly.
    std::vector<SubType> types;
    // A rolled group's closed key -> the CanonId of its first member. A later
    // module declaring the same group reuses these CanonIds instead of adding
    // new ones.
    std::unordered_map<std::string, CanonId> groups;
};

// Register every type of `module` into `reg`, de-duplicating by rolled (closed)
// identity, and fill `type_ids` so that `type_ids[i]` is the CanonId of the
// module's local type `i`. `type_ids` is resized to `module.types.size()`.
void canonicalize_types(std::vector<CanonId>& type_ids, const WasmModule& module, CanonRegistry& reg);

// Whether the type `a` is a subtype of the type `b`, both named Store-wide.
// Equality of CanonId is identity; otherwise `a`'s declared supertypes are
// followed, exactly as `comp_matches` does within a single module.
bool canon_matches(CanonId a, CanonId b, const CanonRegistry& reg);

// Whether `a` is `b` or has `b` among its transitive *declared* supertypes --
// the spec's defined-type matching, which follows only declared edges and does
// not accept an undeclared type that merely happens to refine `b`'s shape. Used
// for function-type casts (#38), where a function matches a concrete target
// type exactly when its declared type is at or below it. (The structural
// refinement that `canon_matches` also admits is the well-formedness condition
// on a declared supertype, checked at validation, not a cast-time rule.)
bool canon_cast_matches(CanonId a, CanonId b, const CanonRegistry& reg);

}

#endif
