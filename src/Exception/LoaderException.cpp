#include <LoaderException.h>

LoaderException::LoaderException(std::string desc, bool showAddress, std::uint64_t addr): Exception(desc),
    address(addr), showAddress(showAddress){
}

std::ostream& operator<<(std::ostream &os, LoaderException &e){
    os << "Loader error";
    if(e.showAddress){
        os << "at 0x" << std::setbase(16) << e.address;
    }
    os << ": " << e.desc;
    return os;
}