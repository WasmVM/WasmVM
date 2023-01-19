#include "parse.hpp"

#include <iostream>
#include "nodes.hpp"

using namespace WasmVM;

struct Printer {

    void operator()(Token::ParenL& tok){
        std::cout << "[" << tok.location.first << "," << tok.location.second << "] ParenL " << tok.value << std::endl; 
    }

    void operator()(Token::ParenR& tok){
        std::cout << "[" << tok.location.first << "," << tok.location.second << "] ParenR " << tok.value << std::endl; 
    }

    void operator()(Token::Id& tok){
        std::cout << "[" << tok.location.first << "," << tok.location.second << "] Id " << tok.value << std::endl; 
    }

    void operator()(Token::String& tok){
        std::cout << "[" << tok.location.first << "," << tok.location.second << "] String " << tok.value << std::endl; 
    }

    void operator()(Token::Number& tok){
        std::cout << "[" << tok.location.first << "," << tok.location.second << "] Number " << tok.value << std::endl; 
    }

    void operator()(Token::KeywordBase& tok){
        std::cout << "[" << tok.location.first << "," << tok.location.second << "] Keyword " << tok.value << std::endl; 
    }

    template <typename T>
    void operator()(Parse::Optional<T>& op){
        if(op.has_value()){
            std::cout << "Optional "; 
            this->operator()(*op);
        }else{
            std::cout << "Optional [null]" << std::endl; 
        }
    }

    template <typename... T>
    void operator()(Parse::Rule<T...>& op){
        std::cout << "Rule {" << std::endl; 
        print_tuple(op, std::make_index_sequence<sizeof...(T)>());
        std::cout << "}" << std::endl; 
    }

    template <typename T, size_t Min, size_t Max>
    void operator()(Parse::Repeat<T, Min, Max>& op){
        std::cout << "Repeat: " << std::endl; 
        for(T item : op){
            std::cout << "* ";
            this->operator()(item);
        }
    }

    template <typename... T>
    void operator()(Parse::OneOf<T...>& op){
        std::visit(Printer(), op);
    }

    void operator()(Parse::FuncType& op){
        std::cout << "FuncType {" << std::endl;
        std::cout << "  * param: ";
        for(auto param : op.params){
            if(!param.first.empty()){
                std::cout << param.first << ":";
            }
            this->operator()(param.second);
            std::cout << " ";
        }
        std::cout << std::endl; 
        std::cout << "  * result: ";
        for(auto result : op.results){
            this->operator()(result);
            std::cout << " ";
        }
        std::cout << std::endl;
    }

    void operator()(ValueType& val){
        switch(val){
            case ValueType::i32:
                std::cout << "i32";
            break;
            case ValueType::i64:
                std::cout << "i64";
            break;
            case ValueType::f32:
                std::cout << "f32";
            break;
            case ValueType::f64:
                std::cout << "f64";
            break;
            case ValueType::funcref:
                std::cout << "funcref";
            break;
            case ValueType::externref:
                std::cout << "externref";
            break;
        }
    }

private:
    template<class Tup, size_t... I>
    void print_tuple(Tup& tup, std::index_sequence<I...>){
        (..., (this->operator()(std::get<I>(tup))));
    }

};