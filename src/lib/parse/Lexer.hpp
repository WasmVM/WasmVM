#ifndef WASMVM_PP_PARSE_Slicer_DEF
#define WASMVM_PP_PARSE_Slicer_DEF

#include <istream>
#include <queue>
#include <string>
#include <vector>
#include <iterator>

#include "Token.hpp"

namespace WasmVM {

struct SourceIter {

    using traits_type = std::char_traits<char>;
    using difference_type = traits_type::off_type;
    using value_type = traits_type::char_type;
    using pointer = void;
    using reference = const traits_type::char_type&;
    using iterator_category = std::input_iterator_tag;
    constexpr static value_type eof = traits_type::eof();
    
    SourceIter(std::istream& stream);

    reference operator*() const;
    SourceIter& operator++();
    SourceIter operator++(int);
    inline bool operator==(const SourceIter&) const;
    inline bool operator!=(const SourceIter&) const;
    operator bool();
    std::pair<size_t, size_t>& location();

private:
    std::reference_wrapper<std::istream> stream;
    constexpr static difference_type end_pos = -1;
    difference_type pos;
    std::queue<std::pair<value_type, std::pair<size_t, size_t>>> buf;
    void advance();
    size_t lineno, colno;
    traits_type::int_type get();
    bool newline;
};
static_assert(std::input_iterator<SourceIter>);

// struct LexerSlicer : public LexerStream {

//     using difference_type = std::ptrdiff_t;

//     LexerSlicer(std::istream& stream);

// private:

//     std::string current;
// };
// static_assert(std::forward_iterator<LexerSlicer>);

// struct Lexer : protected LexerSlicer {

//     Lexer(std::istream& stream);


//     std::vector<Token> consume(size_t index = 0);

// private:
//     std::vector<Token> tokens;
// };

}

#endif