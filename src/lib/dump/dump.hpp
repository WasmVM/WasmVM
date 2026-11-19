#ifndef WASMVM_PP_DUMP_DEF
#define WASMVM_PP_DUMP_DEF

#include <ostream>
#include <string>
#include <vector>
#include <optional>
#include <set>
#include <utility>
#include <WasmVM.hpp>

namespace WasmVM {

// An index space a dump refers into.
enum class DumpSpace { func, table, mem, global, elem, data, tag, type, local, label };

// The names a dump prints as $ids, taken from a module's name section. Only a
// name the text parser reads back is kept: one the tokenizer takes as a
// single id, and the first of equal names in one index space. Everything else
// prints as its index. The text parser takes no forward reference, so a
// reference prints by name only once its definition has been printed (see
// define()). Parameter names (locals below the parameter count and
// tag parameters) are not printed: a typeuse cannot carry them next to an
// explicit (type N) without changing the type.
//
// While a DumpNames::Scope is alive, the dump functions on this thread print
// through it; with none, they print indices as before.
class DumpNames {
public:
    explicit DumpNames(const WasmModule& module);

    struct Scope {
        explicit Scope(DumpNames& names);
        ~Scope();
        Scope(const Scope&) = delete;
        Scope& operator=(const Scope&) = delete;
    private:
        DumpNames* previous;
    };
    static DumpNames* current();

    // A printable name, or nullptr.
    const std::string* module() const;
    const std::string* func(index_t idx) const;
    const std::string* table(index_t idx) const;
    const std::string* mem(index_t idx) const;
    const std::string* global(index_t idx) const;
    const std::string* elem(index_t idx) const;
    const std::string* data(index_t idx) const;
    const std::string* tag(index_t idx) const;
    const std::string* type(index_t idx) const;
    const std::string* field(index_t typeidx, index_t fieldidx) const;
    const std::string* type_param(index_t typeidx, index_t paramidx) const;
    // Within the function begun with begin_func.
    const std::string* local(index_t idx) const;
    // The label `depth` levels out from the innermost open block.
    const std::string* label(index_t depth) const;

    // A definition is printed: returns its printable name, and from now on
    // references to it print by that name. For module-level spaces only.
    const std::string* define(DumpSpace space, index_t idx);
    // The printable name of a reference, nullptr when it has none or its
    // definition was not printed yet.
    const std::string* reference(DumpSpace space, index_t idx) const;

    // The function whose body is being printed: its locals and labels.
    void begin_func(index_t funcidx);
    // The parameter count of a defined function, 0 for anything else.
    index_t param_count(index_t funcidx) const;
    void end_func();
    // A block opens: returns the name of its label, if printable.
    const std::string* open_label();
    void close_label();

private:
    WasmNames names;
    std::vector<index_t> param_counts; // by function index
    std::optional<index_t> func_idx;
    index_t label_count = 0;
    std::vector<index_t> open_labels; // function-wide label indices
    std::set<std::pair<DumpSpace, index_t>> defined;
    const std::string* lookup(DumpSpace space, index_t idx) const;
};

// Prints a reference to an index: `$name` through DumpNames::current() when
// it has one, the index otherwise.
std::ostream& dump_ref(std::ostream& stream, DumpSpace space, index_t idx);
// The same for a struct field.
std::ostream& dump_field_ref(std::ostream& stream, index_t typeidx, index_t fieldidx);

// Prints `$name` when `name` is set, the index otherwise.
std::ostream& dump_index(std::ostream& stream, const std::string* name, index_t idx);
// Prints ` $name` after a definition's keyword, nothing when unnamed.
std::ostream& dump_def_name(std::ostream& stream, const std::string* name);

// Per-section dumps, with each definition named by its index. They print
// through DumpNames::current() like every other dump function.
std::ostream& dump_types(std::ostream& stream, const WasmModule& module);
std::ostream& dump_imports(std::ostream& stream, const WasmModule& module);
std::ostream& dump_funcs(std::ostream& stream, const WasmModule& module);
std::ostream& dump_tables(std::ostream& stream, const WasmModule& module);
std::ostream& dump_mems(std::ostream& stream, const WasmModule& module);
std::ostream& dump_tags(std::ostream& stream, const WasmModule& module);
std::ostream& dump_globals(std::ostream& stream, const WasmModule& module);
std::ostream& dump_exports(std::ostream& stream, const WasmModule& module);
std::ostream& dump_start(std::ostream& stream, const WasmModule& module);
std::ostream& dump_elems(std::ostream& stream, const WasmModule& module);
std::ostream& dump_datas(std::ostream& stream, const WasmModule& module);

std::ostream& operator<<(std::ostream& stream, const ValueType& type);
std::ostream& operator<<(std::ostream& stream, const FuncType& type);
std::ostream& operator<<(std::ostream& stream, const SubType& type);
// One `(type ...)` line with the given indentation; the rec-group form indents its members.
std::ostream& dump_subtype(std::ostream& stream, const SubType& type, const char* indent, std::optional<index_t> typeidx = std::nullopt);
std::ostream& operator<<(std::ostream& stream, const WasmImport& import);
std::ostream& operator<<(std::ostream& stream, const WasmFunc& func);
std::ostream& dump_func(std::ostream& stream, const WasmFunc& func, std::optional<index_t> funcidx);
std::ostream& operator<<(std::ostream& stream, const WasmInstr& instr);
std::ostream& operator<<(std::ostream& stream, const ConstInstr& instr);
std::ostream& operator<<(std::ostream& stream, const TableType& table);
std::ostream& operator<<(std::ostream& stream, const MemType& mem);
std::ostream& operator<<(std::ostream& stream, const WasmGlobal& global);
std::ostream& operator<<(std::ostream& stream, const WasmExport& export_);
std::ostream& operator<<(std::ostream& stream, const WasmElem& elem);
std::ostream& operator<<(std::ostream& stream, const WasmData& data);
// The same, naming the definition `name` when set.
std::ostream& dump_import(std::ostream& stream, const WasmImport& import, const std::string* name);
std::ostream& dump_table(std::ostream& stream, const TableType& table, const std::string* name);
std::ostream& dump_mem(std::ostream& stream, const MemType& mem, const std::string* name);
std::ostream& dump_global(std::ostream& stream, const WasmGlobal& global, const std::string* name);
std::ostream& dump_elem(std::ostream& stream, const WasmElem& elem, const std::string* name);
std::ostream& dump_data(std::ostream& stream, const WasmData& data, const std::string* name);

}

#endif