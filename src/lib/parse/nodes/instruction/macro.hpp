#ifndef WASMVM_PP_PARSE_NODES_MACRO_DEF
#define WASMVM_PP_PARSE_NODES_MACRO_DEF

#define SemaTemplate(Ins) \
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Ins>(Parse::Instr::Ins& node){ \
    instrs.emplace_back(node); \
}

#endif