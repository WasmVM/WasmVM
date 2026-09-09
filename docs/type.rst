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

.. cpp:struct:: WasmVM::ValueType

    A number type, or a reference type ``(ref null? heaptype)``. The heap
    type is abstract or names a type index; the legacy spellings are the
    nullable abstract forms, so ``funcref`` is ``(ref null func)``.

    .. cpp:member:: Kind kind

        Which number type, or which abstract heap type, or
        :cpp:enumerator:`ref` for a concrete one.

    .. cpp:member:: bool nullable

        Meaningful for the reference kinds.

    .. cpp:member:: index_t typeidx

        Meaningful when ``kind`` is :cpp:enumerator:`ref`.

    .. cpp:function:: static ValueType concrete(index_t typeidx, bool nullable)

        ``(ref $t)`` / ``(ref null $t)``

    .. cpp:function:: bool is_num() const

    .. cpp:function:: bool is_ref() const

    .. cpp:function:: bool is_concrete() const

    .. cpp:function:: bool is_bottom() const

.. cpp:enum:: WasmVM::ValueType::Kind

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

    .. cpp:enumerator:: i31ref

        Denote i31ref_t value (GC proposal)

    .. cpp:enumerator:: exnref

        Denote exnref_t value (exception-handling proposal)

    .. cpp:enumerator:: structref

        Any struct (GC proposal)

    .. cpp:enumerator:: arrayref

        Any array (GC proposal)

    .. cpp:enumerator:: anyref

        Anything in the internal hierarchy (GC proposal)

    .. cpp:enumerator:: eqref

        Anything comparable with ``ref.eq`` (GC proposal)

    .. cpp:enumerator:: nullref

        The bottom of the internal hierarchy: only null

    .. cpp:enumerator:: nullfuncref

        The bottom of the function hierarchy: only null

    .. cpp:enumerator:: nullexternref

        The bottom of the external hierarchy: only null

    .. cpp:enumerator:: ref

        A concrete reference, whose ``typeidx`` names the type

.. cpp:struct:: WasmVM::GcObject

    Base of everything a guest allocates on the garbage-collected heap: a
    struct, an array, an exception payload. Reference counted, with a cycle
    collector on top so that a cycle through a mutable reference field is
    reclaimed as well.

.. cpp:class:: template<typename T> WasmVM::GcPtr

    A counted handle on a :cpp:struct:`WasmVM::GcObject`, null when it holds
    nothing. Shaped like the ``std::shared_ptr`` it replaces.

.. cpp:function:: std::size_t WasmVM::gc_collect()

    Collect now, and return how many objects were freed. Collection also
    happens on its own as candidates gather, so an embedder need not call
    this.

.. cpp:function:: std::size_t WasmVM::gc_live_count()

    How many objects the guest has allocated and not yet had reclaimed.

.. cpp:type:: WasmVM::structref_t = GcPtr<StructObj>

    A reference to a struct on the heap; null when the handle is

.. cpp:type:: WasmVM::arrayref_t = GcPtr<ArrayObj>

    A reference to an array on the heap; null when the handle is

.. cpp:struct:: WasmVM::StructObj

    .. cpp:member:: index_t typeidx

        The type this struct was allocated from

    .. cpp:member:: std::vector<Value> fields

        One per field, in declaration order. A packed field is held as an
        ``i32`` already truncated to its width.

.. cpp:struct:: WasmVM::ArrayObj

    .. cpp:member:: index_t typeidx

    .. cpp:member:: std::vector<Value> elems

.. cpp:enum:: WasmVM::RefType

    .. cpp:enumerator:: funcref
        
        Denote function reference value
    
    .. cpp:enumerator:: externref
        
        Denote external object reference value

Object types
------------

.. cpp:type:: WasmVM::Value = std::variant<i32_t, i64_t, f32_t, f64_t, funcref_t, externref_t, i31ref_t, exnref_t, structref_t, arrayref_t>

    General type of value

.. cpp:type:: WasmVM::Ref = std::variant<funcref_t, externref_t>

    Reference type

.. cpp:struct:: WasmVM::Limits

    Limits of ranged objects or values

    .. cpp:member:: offset_t min

        Minimum value

    .. cpp:member:: std::optional<offset_t> max

        **[Optional]** Maximum value

    .. cpp:member:: bool is64 = false

        **[ext]** When ``true``, the memory or table uses a 64-bit address
        space (``memory64`` proposal). Defaults to ``false`` (32-bit).

.. cpp:struct:: WasmVM::FuncType

    Declarational type (signature) of function

    .. cpp:member:: std::vector<ValueType> params

        Value types of function parameters

    .. cpp:member:: std::vector<ValueType> results

        Value types of function results (return values)

.. cpp:enum-class:: WasmVM::PackedType

    Packed storage of a struct field or array element (GC proposal). Never a
    value type: a packed field is read and written through ``i32``.

    .. cpp:enumerator:: i8

    .. cpp:enumerator:: i16

.. cpp:type:: WasmVM::StorageType = std::variant<ValueType, PackedType>

    Storage type of a struct field or array element

.. cpp:struct:: WasmVM::FieldType

    Type of a struct field or of an array's elements

    .. cpp:member:: StorageType storage

    .. cpp:member:: bool mut = false

        Whether ``struct.set`` / ``array.set`` may write it

.. cpp:struct:: WasmVM::StructType

    .. cpp:member:: std::vector<FieldType> fields

.. cpp:struct:: WasmVM::ArrayType

    .. cpp:member:: FieldType field

.. cpp:type:: WasmVM::CompType = std::variant<FuncType, StructType, ArrayType>

    Composite type: what a type section entry defines

.. cpp:struct:: WasmVM::SubType

    One entry of the type section: a composite type that may name a
    supertype and may sit inside a recursion group. A plain function type
    is a ``SubType`` that is ``final`` with no ``supers``.

    .. cpp:member:: bool final = true

    .. cpp:member:: std::vector<index_t> supers

        Supertype indices, as written. The spec allows at most one; the
        validator rejects more.

    .. cpp:member:: CompType comp

    .. cpp:function:: bool is_func() const

    .. cpp:function:: bool is_struct() const

    .. cpp:function:: bool is_array() const

    .. cpp:function:: const FuncType& func() const

        The function type, for the places a type index can only ever name a
        function's. Throws :cpp:struct:`WasmVM::Exception::Exception` when
        the entry is a struct or array type.

.. cpp:struct:: WasmVM::RecGroup

    An explicit ``(rec ...)`` group: ``count`` consecutive entries of
    :cpp:member:`WasmVM::WasmModule::types` starting at ``first``.

    .. cpp:member:: index_t first

    .. cpp:member:: index_t count

.. cpp:type:: WasmVM::MemType = Limits

    Declarational type (signature) of memory

.. cpp:struct:: WasmVM::TableType

    Declarational type (signature) of table

    .. cpp:member:: Limits limits

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