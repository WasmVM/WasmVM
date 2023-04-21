#ifndef WASMVM_PP_PARSE_NODES_MACRO_DEF
#define WASMVM_PP_PARSE_NODES_MACRO_DEF

#define SemaTemplate(Ins) \
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Ins>(Parse::Instr::Ins& node){ \
    instrs.emplace_back(node); \
}

#define MemSema(Ins) \
template<> void InstrVisitor::Sema::operator()<Parse::Instr::Ins>(Parse::Instr::Ins& node){ \
    if(node.index){ \
        node.memidx = std::visit(Parse::Index::Visitor(module.mem_indices.id_map), node.index.value()); \
    } \
    instrs.emplace_back(node); \
}

#endif