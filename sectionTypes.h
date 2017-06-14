// define each section structure here 
#ifndef SECTIONTYPES
#define SECTIONTYPES

#include <vector>
#include <string>

using namespace std;

// #1 define element structure in Type Section (Based on num_types)
class types{
public:
    int function_form;
    int num_param;
    int num_result;
    vector<int> param_type;
    vector<int> result_type;
};

// #2 define element structure in Import Section
class imports{
public:
    int module_len;
    string module_name;
    int function_len;
    string function_name;
    int kind;
    int signature_index;
};

// #3 define element structure in Function Section
class functions{
public:
    int func_signature_index;
};

// #4 define element structure in Table Section 
class tables{
public:
    int table_type;
    int table_limit_flag;
    int table_limit_init;
    int table_limit_max;
};

// #5 define element structure in Memory Section
class memories{
public:
    int mem_limit_flag;
    int mem_limit_init;
    int mem_limit_max;
};

// #6 define element structure in Global Section
class globals{

};

// #7 define element structure in Export Section
class exports{
public:
    int str_len;
    string export_name;
    int kind;
    int func_index;
};

// #8 define element structure in Start Section
/*class starts{

};*/

// #9 define element structure in Element Section
class elements{

};

// #10 define element structure in Code Section
class codes{
public:
    int func_body_size;
    int func_mem_loc;
};

// #11 define element structure in Data Section
class datas{
public:
    int memory_index;
    int data_segment_size;
    vector<char> data; 
};

#endif