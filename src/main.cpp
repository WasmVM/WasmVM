#include <vector>
#include <string>
#include <iostream>
#include <exception>
#include <Store.h>
#include <Loader.h>
#include <Core.h>
#include <Exception.h>
#include <LoaderException.h>
#include <Util.h>

int main(int argc, char const *argv[]){
    // Check args
    if(argc < 2){
        std::cerr << "Usage: Wasm main_module [module...]" << std::endl;
        return -1;
    }
    // Store
    Store mainStore;
    // Modules
    std::map<std::string, ModuleInst *> moduleInsts;
    Exception::setModuleInsts(&moduleInsts);
    // Check endianess
    Util::checkEndian();
    // Load modules
    {
        Loader loader(mainStore, moduleInsts);
        for (int i = 1; i < argc; i++){
            try {
                loader.load(argv[i]);
            } catch (LoaderException &e){
                std::cerr << e << std::endl;
                return -1;
            } catch (std::exception &e){
                std::cerr << "System error: " << e.what() << std::endl;
            }
        }
    }
    // Get main module name
    std::string mainModule(argv[1]);
    mainModule = mainModule.substr(mainModule.rfind(PATH_SEPARATOR) + 1);
    // Run
    Core core(mainStore);
    try{
        core.run(moduleInsts[mainModule]);
    } catch (Exception &e){
        std::cerr << e.desc << std::endl;
        return -1;
    }
    // Clean module
    for(std::map<std::string, ModuleInst *>::iterator moduleIt = moduleInsts.begin(); moduleIt != moduleInsts.end(); ++moduleIt){
        delete moduleIt->second->start;
        delete moduleIt->second;
    }
    return 0;
}