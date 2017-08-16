#include <Value.h>

Value::Value(std::int32_t value) {
    type = i32;
    data.i32 = value;
}
Value::Value(std::int64_t value){
    type = i64;
    data.i64 = value;
}
Value::Value(float value){
    type = f32;
    data.f32 = value;
}
Value::Value(double value){
    type = f64;
    data.f64 = value;
}
Value::Value(const Value &value){
    type = value.type;
    if(type == i32 || type == f32){
        data.i32 = value.data.i32;
    }else{
        data.i64 = value.data.i64;
    }
}
Value& Value::operator=(Value val){
    type = val.type;
    if(type == i32 || type == f32){
        data.i32 = val.data.i32;
    }else{
        data.i64 = val.data.i64;
    }
    return *this;
}

bool FuncType::operator==(const FuncType &ty){
    if(ty.paramTypes.size() != paramTypes.size()){
        return false;
    }
    for(std::size_t i = 0; i < paramTypes.size(); ++i){
        if(paramTypes.at(i) != ty.paramTypes.at(i)){
            return false;
        }
    }
    if(ty.resultTypes.size() != resultTypes.size()){
        return false;
    }
    for(std::size_t i = 0; i < resultTypes.size(); ++i){
        if(resultTypes.at(i) != ty.resultTypes.at(i)){
            return false;
        }
    }
    return true;
}
bool FuncType::operator!=(const FuncType &ty){
    return !(*this == ty);
}