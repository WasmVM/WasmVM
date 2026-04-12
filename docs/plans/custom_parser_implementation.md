# Custom Parser Implementation Plan

## Overview
This document outlines the implementation plan for replacing the ANTLR4-based parser with a custom C++ parser for WebAssembly text format (.wat files) in WasmVM.

## Grammar Analysis

### Core Components
1. **Module Structure** - `(module ...)` with various sections
2. **Sections** - types, imports, functions, tables, memories, globals, exports, etc.
3. **Types** - function types, value types, reference types
4. **Instructions** - control flow, memory, numeric, reference, table operations
5. **Tokens** - integers, floats, strings, identifiers, comments, whitespace

## Parser Architecture

### Design Approach
- **Recursive Descent Parser** - Simple, maintainable, and efficient
- **LL(1) Grammar** - Left-to-right, leftmost derivation with 1 token lookahead
- **Error Recovery** - Graceful handling of malformed input
- **AST Construction** - Build equivalent structures to ANTLR-generated ones

### Core Parser Components
1. **Tokenizer** - Tokenizes input stream into tokens
2. **Parser** - Parses tokens into structured AST
3. **Error Handler** - Manages parsing errors and recovery
4. **Visitor Pattern** - Transforms AST into WasmModule structures

## Implementation Phases

### Phase 1: Tokenizer Implementation (Week 1)
#### Tasks:
1. Implement token recognition for:
   - Integers (decimal and hexadecimal)
   - Floats (decimal and hexadecimal)
   - Strings with escape sequences
   - Identifiers (starting with $)
   - Comments (line and block)
   - Whitespace handling

#### Key Functions:
```cpp
class Tokenizer {
    std::vector<Token> tokenize(std::istream& input);
    Token readToken(std::istream& input);
    bool isWhitespace(char c);
    bool isCommentStart(char c);
    // ... other tokenization functions
};
```

### Phase 2: Core Parser Implementation (Week 2-3)
#### Tasks:
1. Implement module parsing:
   - Module structure with optional name
   - Section parsing (types, imports, functions, etc.)
2. Implement section-specific parsers:
   - Type section
   - Import section
   - Function section
   - Table section
   - Memory section
   - Global section
   - Export section
   - Start section
   - Element section
   - Data section

#### Key Functions:
```cpp
class Parser {
    WasmModule parseModule();
    std::vector<ModuleField> parseModuleFields();
    FuncType parseFuncType();
    ImportSection parseImportSection();
    FuncSection parseFuncSection();
    // ... other parsing functions
};
```

### Phase 3: Instruction Parser (Week 4)
#### Tasks:
1. Implement instruction parsing:
   - Control flow instructions (block, loop, if, br, etc.)
   - Reference instructions (ref.null, ref.is_null, ref.func)
   - Parametric instructions (drop, select)
   - Variable instructions (local.get, local.set, etc.)
   - Table instructions (table.get, table.set, etc.)
   - Memory instructions (memory.size, memory.grow, etc.)
   - Numeric instructions (i32.add, f64.div, etc.)

#### Key Functions:
```cpp
class InstructionParser {
    std::vector<WasmInstr> parseInstrSequence();
    WasmInstr parseInstruction();
    std::vector<WasmInstr> parseBlockInstr();
    // ... instruction parsing functions
};
```

### Phase 4: Integration and Testing (Week 5)
#### Tasks:
1. Integrate parser with existing WasmVM framework
2. Replace ANTLR calls in `src/lib/parse/parse.cpp`
3. Run existing test suite to ensure compatibility
4. Performance benchmarking
5. Documentation updates

## Technical Details

### Token Structure
```cpp
enum class TokenType {
    INTEGER, FLOAT, STRING, IDENTIFIER, 
    LPAREN, RPAREN, 
    COMMENT, WHITESPACE,
    EOF_TOKEN
};

struct Token {
    TokenType type;
    std::string value;
    size_t line, column;
};
```

### Parser State Management
```cpp
class ParserState {
    std::vector<Token> tokens;
    size_t current;
    std::vector<std::string> errors;
    
    Token peek();
    Token consume();
    bool match(TokenType type);
    void error(const std::string& message);
    // ... state management functions
};
```

### AST Node Structure
```cpp
struct ModuleField {
    enum Type { TYPE, IMPORT, FUNC, TABLE, MEMORY, GLOBAL, EXPORT, START, ELEM, DATA };
    Type type;
    // ... specific fields for each type
};

struct WasmModule {
    std::vector<FuncType> types;
    std::vector<WasmImport> imports;
    std::vector<WasmFunc> funcs;
    // ... other module fields
};
```

## Error Handling Strategy

### Error Types
1. **Lexical Errors** - Invalid tokens, malformed numbers/strings
2. **Syntactic Errors** - Missing parentheses, incorrect structure
3. **Semantic Errors** - Type mismatches, undefined references

### Recovery Mechanisms
1. **Token Skipping** - Skip invalid tokens to recover
2. **Structure Recovery** - Insert missing delimiters where possible
3. **Graceful Degradation** - Continue parsing when possible

## Performance Considerations

### Optimization Techniques
1. **Memory Pooling** - Pre-allocate token and AST nodes
2. **String Interning** - Avoid duplicate string allocations
3. **Lookahead Optimization** - Minimize token consumption
4. **Inline Functions** - Reduce function call overhead

### Benchmarking
1. **Parse Time** - Measure parsing speed vs ANTLR
2. **Memory Usage** - Compare memory footprint
3. **Binary Size** - Measure final binary size reduction

## Testing Strategy

### Unit Tests
1. **Tokenization Tests** - Verify all token types are recognized correctly
2. **Parsing Tests** - Test each section type individually
3. **Integration Tests** - Full module parsing
4. **Edge Case Tests** - Malformed inputs, complex nesting

### Regression Tests
1. **Existing Test Suite** - Ensure all existing tests pass
2. **Performance Tests** - Verify no performance regression
3. **Compatibility Tests** - Validate same output as ANTLR parser

## Migration Plan

### Step-by-Step Migration
1. **Phase 1**: Implement tokenizer and basic parsing
2. **Phase 2**: Add section parsing (types, imports, functions)
3. **Phase 3**: Add instruction parsing
4. **Phase 4**: Full integration and testing

### Compatibility Assurance
1. **Output Equivalence** - Same WasmModule structures
2. **Error Messages** - Consistent error reporting
3. **Performance** - No regression in performance
4. **API Compatibility** - Same public interface

## Risk Mitigation

### Potential Risks
1. **Incomplete Grammar Coverage** - Missing edge cases
2. **Performance Regression** - Parser slower than ANTLR
3. **Compatibility Issues** - Different behavior with edge cases
4. **Maintenance Overhead** - Custom parser maintenance

### Mitigation Strategies
1. **Comprehensive Testing** - Extensive test coverage
2. **Incremental Development** - Gradual feature addition
3. **Performance Profiling** - Regular benchmarking
4. **Documentation** - Clear documentation of grammar rules

## Expected Benefits

### Technical Benefits
- **Reduced Dependencies** - No ANTLR runtime required
- **Smaller Binary** - Reduced memory footprint
- **Better Performance** - Optimized parsing for WebAssembly
- **Simplified Build** - Easier compilation process

### User Benefits
- **Faster Installation** - Fewer dependencies to install
- **Better Performance** - Faster parsing of .wat files
- **Easier Maintenance** - Single codebase without external tools
- **Improved Portability** - Easier cross-platform compilation

## Timeline

### Week 1: Tokenizer Implementation
- Complete token recognition
- Basic tokenizer functionality

### Week 2-3: Core Parser Implementation
- Module structure parsing
- Section parsing (types, imports, functions)
- Basic error handling

### Week 4: Instruction Parser
- Complete instruction parsing
- Complex nested structures
- Error recovery implementation

### Week 5: Integration and Testing
- Full integration with WasmVM
- Performance testing
- Regression testing
- Documentation updates

## Success Criteria

1. **Functionality** - Parser produces identical output to ANTLR
2. **Performance** - Parsing speed is competitive or better
3. **Compatibility** - All existing tests pass
4. **Maintainability** - Clean, well-documented code
5. **Size** - Reduced binary size and dependencies