Type
=====

Entities for represent values or objects used within WasmVM

Types out of WebAssembly specification (extended) are denoted as **[ext]**

Fundamental types
-----------------

Represent basic values or date references

.. cpp:type:: WasmVM::i32_t = int32_t

    32-bit signed integer

.. cpp:type:: WasmVM::i64_t = int64_t

    64-bit signed integer

.. cpp:type:: WasmVM::f32_t = float

    32-bit (single precision) floating-point number

.. cpp:type:: WasmVM::f64_t = double

    64-bit (double precision) floating-point number

.. cpp:type:: WasmVM::funcref_t = std::optional<index_t>

    Nullable reference of function

.. cpp:type:: WasmVM::externref_t = void*

    Nullable reference of external objects

.. cpp:type:: WasmVM::i8_t = int8_t 

    **[ext]** 8-bit signed integer

.. cpp:type:: WasmVM::u8_t = uint8_t 

    **[ext]** 8-bit unsigned integer

.. cpp:type:: WasmVM::i16_t = int16_t 

    **[ext]** 16-bit signed integer

.. cpp:type:: WasmVM::u16_t = uint16_t 

    **[ext]** 16-bit unsigned integer

.. cpp:type:: WasmVM::u32_t = uint32_t 

    **[ext]** 32-bit unsigned integer

.. cpp:type:: WasmVM::u64_t = uint64_t 

    **[ext]** 64-bit unsigned integer

.. cpp:type:: WasmVM::byte_t = std::byte 

    **[ext]** Single byte

.. cpp:type:: WasmVM::index_t = u32_t

    **[ext]** Index of objects

.. cpp:type:: WasmVM::offset_t = u64_t

    **[ext]** Offset of objects

.. cpp:type:: WasmVM::align_t = uint8_t

    **[ext]** Memory alignment

Constants
---------

.. cpp:var:: constexpr u64_t WasmVM::page_size = 65536

    Memory page size

.. cpp:var:: constexpr index_t WasmVM::index_npos = -1

    **[ext]** Undefined (non-existing) index

Enumerations
------------

.. cpp:enum:: WasmVM::ValueType

    .. cpp:enumerator:: i32
        
        Denote i32_t value
    
    .. cpp:enumerator:: i64
        
        Denote i64_t value
    
    .. cpp:enumerator:: f32
        
        Denote f32_t value
    
    .. cpp:enumerator:: f64
        
        Denote f64_t value
    
    .. cpp:enumerator:: funcref
        
        Denote funcref_t value
    
    .. cpp:enumerator:: externref
        
        Denote externref_t value

.. cpp:enum:: WasmVM::RefType

    .. cpp:enumerator:: funcref
        
        Denote function reference value
    
    .. cpp:enumerator:: externref
        
        Denote external object reference value

Object types
------------

.. cpp:type:: WasmVM::Value = std::variant<i32_t, i64_t, f32_t, f64_t, funcref_t, externref_t>

    General type of value

.. cpp:type:: WasmVM::Ref = std::variant<funcref_t, externref_t>

    Reference type

.. cpp:struct:: WasmVM::Limits

    Limits of ranged objects or values

    .. cpp:member:: offset_t min

        Minimum value

    .. cpp:member:: std::optional<offset_t> max

        **[Optional]** Maximum value

.. cpp:struct:: WasmVM::FuncType

    Declarational type (signature) of function

    .. cpp:member:: std::vector<ValueType> params

        Value types of function parameters

    .. cpp:member:: std::vector<ValueType> results

        Value types of function results (return values)

.. cpp:type:: WasmVM::MemType = Limits

    Declarational type (signature) of memory

.. cpp:struct:: WasmVM::TableType

    Declarational type (signature) of table

    .. cpp:member:: Limits Limits

        Limits of table

    .. cpp:member:: RefType reftype

        Specify the reference type of table elements

.. cpp:struct:: WasmVM::GlobalType

    Declarational type (signature) of global

    .. cpp:enum:: Mut 

        Declarational type for denoting mutability of the global

        .. cpp:enumerator:: constant

            The global value is immutable

        .. cpp:enumerator:: variable

            The global value is variable

    .. cpp:member:: Mut mut

        Specify the mutability

    .. cpp:member:: ValueType type

        Specify the type of global value

.. cpp:struct:: WasmVM::ExternVal

    Declarational type (signature) of external values or objects

    .. cpp:enum:: ExternType 

    .. cpp:enumerator:: Func

        Indicate the external value is function

    .. cpp:enumerator:: Mem

        Indicate the external value is memory
        
    .. cpp:enumerator:: Table

        Indicate the external value is table

    .. cpp:enumerator:: Global

        Indicate the external value is global
    
    .. cpp:member:: ExternType type

    .. cpp:member:: index_t addr

        Specify the type of global value