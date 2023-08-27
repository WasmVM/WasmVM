Type
=====

Entities for represent values or objects used within WasmVM

Types out of WebAssembly specification (extended) are denoted as **[ext]**

Fundamental types
-----------------

Represent basic values or date references

.. cpp:type:: i32_t = int32_t

    32-bit signed integer

.. cpp:type:: i64_t = int64_t

    64-bit signed integer

.. cpp:type:: f32_t = float

    32-bit (single precision) floating-point number

.. cpp:type:: f64_t = double

    64-bit (double precision) floating-point number

.. cpp:type:: funcref_t = std::optional<index_t>

    Nullable reference of function

.. cpp:type:: externref_t = void*

    Nullable reference of external objects

.. cpp:type:: i8_t = int8_t 

    **[ext]** 8-bit signed integer

.. cpp:type:: u8_t = uint8_t 

    **[ext]** 8-bit unsigned integer

.. cpp:type:: i16_t = int16_t 

    **[ext]** 16-bit signed integer

.. cpp:type:: u16_t = uint16_t 

    **[ext]** 16-bit unsigned integer

.. cpp:type:: u32_t = uint32_t 

    **[ext]** 32-bit unsigned integer

.. cpp:type:: u64_t = uint64_t 

    **[ext]** 64-bit unsigned integer

.. cpp:type:: byte_t = std::byte 

    **[ext]** Single byte

.. cpp:type:: index_t = u32_t

    **[ext]** Index of objects

.. cpp:type:: offset_t = u64_t

    **[ext]** Offset of objects

.. cpp:type:: align_t = uint8_t

    **[ext]** Memory alignment

Constants
---------

.. cpp:var:: constexpr u64_t page_size = 65536

    Memory page size

.. cpp:var:: constexpr index_t index_npos = -1

    **[ext]** Undefined (non-existing) index

Enumerations
------------

.. cpp:enum:: ValueType

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

.. cpp:enum:: RefType

    .. cpp:enumerator:: funcref
        
        Denote function reference value
    
    .. cpp:enumerator:: externref
        
        Denote external object reference value

Object types
------------

.. cpp:type:: Value = std::variant<i32_t, i64_t, f32_t, f64_t, funcref_t, externref_t>

    General type of value

.. cpp:struct:: Limits

    Limits of ranged objects or values

    .. cpp:member:: offset_t min

        Minimum value

    .. cpp:member:: std::optional<offset_t> max

        **[Optional]** Maximum value

.. cpp:struct:: FuncType

    Declarational type (signature) of function

    .. cpp:member:: std::vector<ValueType> params

        Value types of function parameters

    .. cpp:member:: std::vector<ValueType> results

        Value types of function results (return values)

.. cpp:type:: MemType = Limits

    Declarational type (signature) of memory

.. cpp:struct:: TableType

    Declarational type (signature) of table

    .. cpp:member:: Limits Limits

        Limits of table

    .. cpp:member:: RefType reftype

        Specify the reference type of table elements

.. cpp:struct:: GlobalType

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

.. cpp:struct:: ExternVal

    Declarational type (signature) of external values or objects

    .. cpp:enumerator:: Func

        Indicate the external value is function

    .. cpp:enumerator:: Mem

        Indicate the external value is memory
        
    .. cpp:enumerator:: Table

        Indicate the external value is table

    .. cpp:enumerator:: Global

        Indicate the external value is global
    
    .. cpp:member:: type

    .. cpp:member:: index_t addr

        Specify the type of global value