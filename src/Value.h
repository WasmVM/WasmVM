#ifndef VALUE_DEF
#define VALUE_DEF

#include <cstdint>
#include <vector>

enum ValType { i32, i64, f32, f64 };

class Value {
public:
    Value(std::int32_t = 0);
    Value(std::int64_t);
    Value(float);
    Value(double);
    Value(const Value&);
    Value &operator=(Value);

    ValType type;
    union {
        std::int32_t i32;
        std::int64_t i64;
        float   f32;
        double  f64;
    } data;
};

enum ExternType { func, table, mem, global };

class ExternVal {
public:
    ExternVal(ExternType type);
    ExternType type;
    uint32_t addr;
};

class FuncType{
public:
    std::vector<ValType> paramTypes;
    std::vector<ValType> resultTypes;
};

class Func{
public:
    std::uint32_t typeidx;
    std::vector<ValType> localTypes;
    std::vector<char> body;
};

#endif