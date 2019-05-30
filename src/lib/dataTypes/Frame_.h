#include <dataTypes/Frame.h>
#include <dataTypes/Entry.h>
#include <dataTypes/vector.h>
#include <instance/ModuleInst.h>

struct Frame_ {
    Entry parent;
    vector* locals;       // Value
    ModuleInst* moduleInst;
};