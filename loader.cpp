#include <sys/stat.h>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include "Module.h"
#include "commonTypes.h"

struct stat results;

uint32_t file_size(const char *filename){
    if(stat(filename,&results) == 0){
        std::cout << "Binary file size: " << results.st_size << std::endl;
        return results.st_size;
    }
    else{
        std::cout << "Error when using clib to stat a file" << std::endl;
    }
}

void load_file(const char *filename,Module *wa){
    // Get reading file size
    int filesize = file_size(filename);
    // Input file stream 
    std::ifstream infile;
    infile.open(filename, std::ios::in | std::ios::binary);
    // one byte read
    char onebyte;
    // reading 
    for(int index = 0; index < filesize; index++){
        infile.read(&onebyte,1);
        wa->i32_store8(onebyte);
    }
}

void help_cmd(){
    // print out useful message to help the user
    std::cout << std::left << std::setw(36) << "WebAssembly Helper manual:" << std::endl;
    std::cout << std::setfill('-') << std::setw(36) << "-" << std::setfill(' ') << std::endl; // setfill to - and then set back to blank
    std::cout << std::left << std::setw(16) << "-h" << std::setw(4) << "|" << std::setw(16) << "List all the information of loader program." << std::endl;
    std::cout << std::left << std::setw(16) << "-o" << std::setw(4) << "|" << std::setw(16) << "Specify the output filename. (default is using \"a.kevin\".)" << std::endl;
    std::cout << std::left << std::setw(16) << "-i" << std::setw(4) << "|" << std::setw(16) << "Specify the input filename. (default is using \"Test/addTwo.wasm\".)" << std::endl;
    std::cout << std::left << std::setw(16) << "-m" << std::setw(4) << "|" << std::setw(16) << "Specify multiple input filename to do loading work." << std::endl;
    // end
    std::cout << std::setfill('-') << std::setw(36) << "-" << std::setfill(' ') << std::endl;
    std::cout << std::endl;
}

int main(int argc,char *argv[]){
    // implement getopt 
    int c = 0,flag = 0;
    std::string input_file = "Test/addTwo.wasm";
    std::string output_file = "a.kevin";
    std::vector<std::string> input_files(0);
    // getopt
    while((c = getopt(argc,argv,"imo:hv")) != -1){
        switch(c){
            case 'h':
                help_cmd();
                break;
            case 'i':
                if(*argv[optind] == '-'){
                    std::cout << "Error - " << *argv[optind-1] << ",Please specify the input filename" << optind << std::endl;
                    return 0;
                }
                else{
                    input_file.assign(argv[optind]);
                    std::cout << "Get input file: " << input_file << std::endl;
                }
                break;
            case 'm':
                // several input value 
                std::cout << "several input files(multiple file loading)" << std::endl;
                // get several files from argv
                optind--;
                for(; optind < argc && *argv[optind] != '-'; optind++){
                    // store the input files
                    input_files.push_back(argv[optind]);
                }
                break;
            case 'o': 
                std::cout << "Output name specified" << std::endl;
                std::cout << argv[--optind] << std::endl;
                break;
            case 'v':
                flag = 1;
                break;
            case '?':
                std::cout << "Not match variable: " << (char)optopt << std::endl;
                break;
            default:
                std::cout << "Not match, print help" << std::endl;
                return 0;
        }
    }

    Module *wa_bin = new Module();
    if(input_files.size() == 0){
        // only one file => load one file into wa
        load_file(input_file.c_str(),wa_bin);
    }

    if(flag){
        // Print out the linear map (treat as verbose)
        wa_bin->mem_map();
    }
} 