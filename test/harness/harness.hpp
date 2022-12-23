#ifndef WASMVM_TEST_HARNESS_DEF
#define WASMVM_TEST_HARNESS_DEF

#include <iostream>
#include <exception>

#define SUITE_Begin(SUITE_NAME) \
    int main(void){ \
        int total_num = 0, passed_num = 0; \
        const char * SUITE_Name = # SUITE_NAME;

#define SUITE_End \
        return total_num - passed_num; \
    }

#define TEST_Success(STMTS) \
    {total_num += 1; \
    try { \
        STMTS \
        passed_num += 1; \
        std::cerr << SUITE_Name << "(" << __LINE__ << "): [Passed]" << std::endl; \
    } catch (const std::exception& e) { \
        std::cerr << SUITE_Name << "(" << __LINE__ << "): [Failed]" << std::endl; \
        std::cerr << e.what(); \
    }}

#define TEST_Error(EXCEPTION_TYPE, STMTS) \
    total_num += 1; \
    try { \
        STMTS \
        std::cerr << SUITE_Name << "(" << __LINE__ << "): [Failed]" << std::endl; \
    } catch (const EXCEPTION_TYPE &) { \
        passed_num += 1; \
        std::cerr << SUITE_Name << "(" << __LINE__ << "): [Passed]" << std::endl; \
    }

#endif