#ifndef EXCEPTION_DEF
#define EXCEPTION_DEF

#include <string>
#include <ostream>

class Exception {
public:
    Exception(std::string desc) : desc(desc){
    };
    std::string desc;
};

#endif