#ifndef WASMVM_STAGE_DEF
#define WASMVM_STAGE_DEF

typedef struct _stage {
    void*   input;
    void*   output;
    void    (*free)(struct _stage* stage);
    void    (*setInput)(struct _stage *stage, void* input);
    void*   (*getOutput)(struct _stage *stage);
    int     (*run)(struct _stage *stage);
} Stage;

#endif