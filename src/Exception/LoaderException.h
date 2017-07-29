#ifndef LOADEREXCEPTION_DEF
#define LOADEREXCEPTION_DEF

#include <string>
#include <cstdint>
#include <iomanip>
#include <ostream>
#include <Exception.h>

class LoaderException : public Exception{
public:
    std::uint64_t address;
    bool showAddress;
    LoaderException(std::string desc, bool showAddress = false, std::uint64_t addr = 0);
};

std::ostream &operator<<(std::ostream &os, LoaderException &e);

#endif