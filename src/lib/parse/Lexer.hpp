#ifndef WASMVM_PP_PARSE_LEXER_DEF
#define WASMVM_PP_PARSE_LEXER_DEF

#include <istream>
#include <queue>
#include <string>
#include <vector>
#include <forward_list>
#include <initializer_list>
#include <ranges>
#include <iterator>
#include <variant>

#include "Token.hpp"

namespace WasmVM {

using TokenVar = std::variant<
    Token::Paren<'('>,
    Token::Paren<')'>,
    Token::Id,
    Token::String
>;

struct Cursor {
    using traits_type = std::char_traits<char>;
    using difference_type = traits_type::off_type;
    using value_type = traits_type::char_type;
    using pointer = void;
    using reference = const value_type&;
    using iterator_category = std::input_iterator_tag;
    constexpr static traits_type::int_type eof = traits_type::eof();
    
    Cursor();
    Cursor(const Cursor&);
    Cursor(std::istream& stream);

    value_type operator*() const;
    Cursor& operator++();
    Cursor operator++(int);
    bool operator==(const Cursor&) const;
    bool operator!=(const Cursor&) const;
    operator bool();
    Token::Location& location();

private:
    constexpr static difference_type end_pos = -1;
    std::reference_wrapper<std::istream> stream;
    difference_type pos;
    std::queue<std::pair<value_type, Token::Location>> buf;
    void advance();
    size_t lineno, colno;
    traits_type::int_type get();
    bool newline;
};
static_assert(std::input_iterator<Cursor>);

struct Lexer : std::ranges::view_interface<Lexer> {

    struct iterator {

        using difference_type = std::ptrdiff_t;
        using value_type = TokenVar;
        using pointer = void;
        using reference = const value_type&;
        using iterator_category = std::forward_iterator_tag;

        iterator();
        iterator(Lexer&, std::forward_list<TokenVar>::iterator);
        iterator(const iterator&);

        TokenVar operator*() const;
        iterator& operator++();
        iterator operator++(int);
        bool operator==(const iterator&) const;
        bool operator!=(const iterator&) const;
    private:
        std::reference_wrapper<Lexer> lexer;
        std::forward_list<TokenVar>::iterator it;
    };

    Lexer() = default;
    Lexer(std::istream& stream);

    iterator begin();
    iterator end();
    bool empty();

private:
    Cursor cursor;
    std::forward_list<TokenVar> buf;
    void advance(std::forward_list<TokenVar>::iterator&);

    friend iterator;
};
static_assert(std::ranges::viewable_range<Lexer> && std::ranges::forward_range<Lexer>);

}

#endif