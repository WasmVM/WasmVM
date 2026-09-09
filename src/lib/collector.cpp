// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Reference counting with a cycle collector on top.
//
// The counts do the everyday work: an object is freed the moment the last
// handle on it goes away, which is prompt and needs no bookkeeping. What a
// count cannot see is a cycle, and a guest builds one as soon as it stores a
// reference back into its own graph -- a doubly-linked list, a tree with
// parent pointers, any graph at all.
//
// So this is Bacon and Rajan's synchronous cycle collection. When a count is
// decremented and does not reach zero, the object might be the root of a
// cycle that just became garbage, so it is coloured purple and buffered.
// Collection then runs trial deletion over the buffer: subtract the internal
// references, see which objects would be left at zero, and free exactly
// those. Nothing outside the buffer is ever looked at, so the collector never
// needs to know where the roots are -- which matters here, because a Value
// can be sitting in the embedder's own code where nothing could find it.

#include <Types.hpp>

#include <algorithm>
#include <mutex>
#include <vector>

using namespace WasmVM;

namespace {

// One lock over the buffer, the colouring and the live count. It is
// recursive because freeing an object runs its destructor, which lets go of
// its children and comes back through gc_release. Two embedder threads may
// drive two stores at once -- which the shared_ptr this replaces allowed --
// and the buffer is shared between them, so the lock is what makes that safe.
std::recursive_mutex gc_mutex;
std::vector<GcObject*> gc_roots;
std::size_t gc_live = 0;
// Collection is not free, so it waits until enough candidates have gathered.
// Every candidate is an object that survived a decrement, so this bounds how
// much garbage can sit in cycles at once, not how much memory is in use.
constexpr std::size_t gc_threshold = 8192;

// Walk an object's children with a plain function pointer, so that GcObject
// needs no <functional> and no allocation per visit.
template <typename Fn>
void for_each_child(const GcObject* object, Fn&& fn){
    struct Trampoline {
        static void call(GcObject* child, void* context){
            (*static_cast<Fn*>(context))(child);
        }
    };
    object->gc_children(&Trampoline::call, &fn);
}

// A count reached zero. Deleting runs the destructor, which lets go of the
// children and so may free them in turn. An object the collector has taken
// charge of is left alone: it will be freed with the rest of its cycle.
void free_at_zero(GcObject* object){
    if(object->gc_doomed || object->gc_buffered){
        return;
    }
    gc_live -= 1;
    delete object;
}

// ── trial deletion ──────────────────────────────────────────────────────────

void mark_gray(GcObject* object){
    if(object->gc_colour == GcObject::GcColour::gray){
        return;
    }
    object->gc_colour = GcObject::GcColour::gray;
    for_each_child(object, [](GcObject* child){
        child->gc_count.fetch_sub(1);
        mark_gray(child);
    });
}

void scan_black(GcObject* object){
    object->gc_colour = GcObject::GcColour::black;
    for_each_child(object, [](GcObject* child){
        child->gc_count.fetch_add(1);
        if(child->gc_colour != GcObject::GcColour::black){
            scan_black(child);
        }
    });
}

void scan(GcObject* object){
    if(object->gc_colour != GcObject::GcColour::gray){
        return;
    }
    // Anything still counted from outside the candidate set is alive, and so
    // is everything it reaches.
    if(object->gc_count.load() > 0){
        scan_black(object);
        return;
    }
    object->gc_colour = GcObject::GcColour::white;
    for_each_child(object, [](GcObject* child){
        scan(child);
    });
}

void collect_white(GcObject* object, std::vector<GcObject*>& doomed){
    if((object->gc_colour != GcObject::GcColour::white) || object->gc_doomed){
        return;
    }
    object->gc_colour = GcObject::GcColour::black;
    object->gc_doomed = true;
    doomed.push_back(object);
    for_each_child(object, [&doomed](GcObject* child){
        collect_white(child, doomed);
    });
}

std::size_t collect_locked(){
    // Candidates whose count has since gone back up, or which were already
    // emptied, are not the root of anything.
    std::vector<GcObject*> candidates;
    candidates.reserve(gc_roots.size());
    std::vector<GcObject*> emptied;
    for(GcObject* object : gc_roots){
        object->gc_buffered = false;
        if((object->gc_colour == GcObject::GcColour::purple) && (object->gc_count.load() > 0)){
            candidates.push_back(object);
        }else if(object->gc_count.load() == 0){
            emptied.push_back(object);
        }
    }
    gc_roots.clear();
    std::size_t freed = 0;
    for(GcObject* object : emptied){
        const std::size_t live_before = gc_live;
        free_at_zero(object);
        freed += (live_before - gc_live);
    }

    for(GcObject* object : candidates){
        mark_gray(object);
    }
    for(GcObject* object : candidates){
        scan(object);
    }
    std::vector<GcObject*> doomed;
    for(GcObject* object : candidates){
        collect_white(object, doomed);
    }

    // Break the cycle before deleting anything. Each member lets go of its
    // children while every member is still allocated, so no destructor can
    // reach a pointer that has already been freed; then the memory goes.
    for(GcObject* object : doomed){
        object->gc_clear_children();
    }
    for(GcObject* object : doomed){
        gc_live -= 1;
        delete object;
    }
    return freed + doomed.size();
}

} // namespace

void WasmVM::gc_acquire(GcObject* object){
    std::lock_guard<std::recursive_mutex> lock(gc_mutex);
    object->gc_colour = GcObject::GcColour::black;
    if(object->gc_count.fetch_add(1) == 0){
        gc_live += 1;
    }
}

void WasmVM::gc_release(GcObject* object){
    std::lock_guard<std::recursive_mutex> lock(gc_mutex);
    if(object->gc_count.fetch_sub(1) == 1){
        free_at_zero(object);
        return;
    }
    // It survived, so it may be the root of a cycle that has just become
    // unreachable. Buffer it for the collector to judge.
    object->gc_colour = GcObject::GcColour::purple;
    if(!object->gc_buffered && !object->gc_doomed){
        object->gc_buffered = true;
        gc_roots.push_back(object);
        if(gc_roots.size() >= gc_threshold){
            collect_locked();
        }
    }
}

std::size_t WasmVM::gc_collect(){
    std::lock_guard<std::recursive_mutex> lock(gc_mutex);
    return collect_locked();
}

std::size_t WasmVM::gc_live_count(){
    std::lock_guard<std::recursive_mutex> lock(gc_mutex);
    return gc_live;
}

// ── what each object holds ──────────────────────────────────────────────────

namespace {
void visit_values(const std::vector<Value>& values,
                  void (*visit)(GcObject*, void*), void* context){
    for(const Value& value : values){
        if(std::holds_alternative<structref_t>(value)){
            if(StructObj* object = std::get<structref_t>(value).get()){
                visit(object, context);
            }
        }else if(std::holds_alternative<arrayref_t>(value)){
            if(ArrayObj* object = std::get<arrayref_t>(value).get()){
                visit(object, context);
            }
        }else if(std::holds_alternative<exnref_t>(value)){
            if(WasmException* object = std::get<exnref_t>(value).value.get()){
                visit(object, context);
            }
        }
    }
}
} // namespace

void StructObj::gc_children(void (*visit)(GcObject*, void*), void* context) const {
    visit_values(fields, visit, context);
}
void ArrayObj::gc_children(void (*visit)(GcObject*, void*), void* context) const {
    visit_values(elems, visit, context);
}
void WasmException::gc_children(void (*visit)(GcObject*, void*), void* context) const {
    visit_values(values, visit, context);
}

namespace {
void clear_values(std::vector<Value>& values){
    for(Value& value : values){
        if(std::holds_alternative<structref_t>(value)){
            value = Value(structref_t());
        }else if(std::holds_alternative<arrayref_t>(value)){
            value = Value(arrayref_t());
        }else if(std::holds_alternative<exnref_t>(value)){
            value = Value(exnref_t());
        }
    }
}
} // namespace

void StructObj::gc_clear_children(){ clear_values(fields); }
void ArrayObj::gc_clear_children(){ clear_values(elems); }
void WasmException::gc_clear_children(){ clear_values(values); }
