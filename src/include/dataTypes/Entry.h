#ifndef WASMVM_DATATYPES_ENTRY_DEF
#define WASMVM_DATATYPES_ENTRY_DEF

typedef enum {
    Entry_Unspecified = 0,
    Entry_Value,
    Entry_Label,
    Entry_Frame
} EntryType;

typedef struct {
    EntryType entryType;
} Entry;

#endif