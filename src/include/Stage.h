#ifndef WASMVM_STAGE_DEF
#define WASMVM_STAGE_DEF

typedef struct _stage {
    void*   input;
    void*   output;
    int     (*run)(struct _stage *stage);
} Stage;

#endif