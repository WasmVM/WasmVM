#include "utils.h"

#include <stdio.h>

void _dump_stack(wasm_stack stack){
    for(u32_t i = 0; stack; ++i, stack = stack->next){
        fprintf(stderr, "#%u ", i);
        switch (stack->type){
        case Entry_value:
            fprintf(stderr, "[Value]");
            break;
        case Entry_frame:
            fprintf(stderr, "[Frame]");
            break;
        case Entry_label:
            fprintf(stderr, "[Label]");
            break;
        default:
            break;
        }
        fprintf(stderr, "\n");
    }
}