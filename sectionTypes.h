// define each section structure here 
#ifndef SECTIONTYPES
#define SECTIONTYPES

#include <vector>
#include <string>

using namespace std;

// define element structure in Type Section (Based on num_types)
class types{
    int function_form;
    int num_param;
    int num_result;
    vector<int> param_type;
    vector<int> result_type;
};

// define element structure in Import Section
class imports{
    int module_len;
    string module_name;
    int function_len;
    string function_name;
    int kind;
    int signature_index;
};

// define element structure in Function Section
class functions{
    int func_signature_index;
}

// define element structure in Table Section 
class tables{
    int table_type;
    int table_limit_flag;
    int table_limit_init;
    int table_limit_max;
}

// define element structure in Memory Section
class memories{
    int mem_limit_flag;
    int mem_limit_init;
    int mem_limit_max;
}

#endif