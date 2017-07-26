#ifndef LOADER_DEF
#define LOADER_DEF

#include <string>
#include <Store.h>
#include <Stack.h>

class Loader {
public:
    Loader(Store &store);
    void load(const char *fileName);
private:
    Store &store;
};

#endif