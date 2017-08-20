Please follow the coding style below:

1. Every identifier should be **Camel Case**. Except for:

  + Instruction handler function
  
  + Macros
  
2. Structure types should begin with **upper case**, and other names should begin with **lower case**

3. #include guards should be named as `<FILENAME WITHOUT .H>_DEF`

4. Header file should be postfixed `.h`

5. Format of macro names in Bincode.h:
  
  + `<TYPES>_<DESCRIPTION>`
  
  + Except for `i32`, `i64`, `f32`, `f64`, descriptions should begin with **upper case**
  
6. Indent with **a tab**, don't use spaces instead.

7. Bracket style:
````c
Foo():
  member0(0), member1(1)
{
}

void foo(){
  // Standalone block
  {
  }
  // Other block
  if(){
  }else{
  }
}
````

8. Don't use `using namespace`

9. Pointer and reference should be `int *ptr`, `int &ref`, except for:

  * Return type in the source file should use `int* Foo::foo()`
  
  * Unnamed parameter type should use `void (int&)`
  
