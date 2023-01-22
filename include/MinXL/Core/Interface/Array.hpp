#pragma once

#include "MinXL/Core/Types.hpp"


namespace mxl
{
    enum class ArrayFeatures: uint16_t
    {
        HasVarType      = 0x00000080,
        ArrayOfStrings  = 0x00000100,
        ArrayOfVariants = 0x00000800
    };

    struct ArrayBound
    {
        uint32_t    ElementCount;
        int32_t     LowerBound;
    };

    struct ArrayHeader
    {
        uint8_t     Reserved[12];
        uint32_t    Type;
    };

    struct ArrayBody
    {
        uint16_t    Dims;
        uint16_t    Features;
        uint32_t    ElementSize;
        uint32_t    Locks;
        void*       Data;
        ArrayBound  Columns;
        ArrayBound  Rows;
    };


    template<ArrayValue _Ty = Variant> class Array
    {
        friend class Variant;

    private:
        ArrayHeader _Header;
        ArrayBody   _Body;

    public:
        using ValueType = _Ty;

        Array();
        Array(const uint64_t rows, const uint64_t cols);

        Array(const Array<_Ty>& other);
        Array(Array<_Ty>&& other);
        Array<_Ty>& operator=(const Array<_Ty>& other);
        Array<_Ty>& operator=(Array<_Ty>&& other);

        Array(const Variant& var);
        Array(Variant&& var);

        ~Array();

    public:
        const _Ty&  operator[](const uint64_t index) const;
        _Ty&        operator[](const uint64_t index);
        const _Ty&  operator()(const uint64_t row, const uint64_t col) const;
        _Ty&        operator()(const uint64_t row, const uint64_t col);

    public:

        inline auto Size() const         { return _Body.Columns.ElementCount * _Body.Rows.ElementCount;              }
        inline auto Data() const         { return static_cast<_Ty*>(_Body.Data);                                     }
        inline auto Rows() const         { return _Body.Rows.ElementCount;                                           }
        inline auto Columns() const      { return _Body.Columns.ElementCount;                                        }
        inline auto ElementSize() const  { return _Body.ElementSize;                                                 }
        inline auto Column(uint64_t col) { return std::make_pair(&operator()(0, col), &operator()(Rows(), col));     }

        // Test
        void Resize(const uint64_t rows, const uint64_t cols);

    private:
        bool Allocate(const uint64_t rows, const uint64_t cols);
        static void Deallocate(ArrayBody* array);
    };


    //
    // Tuples are simply a one-dimension mxl::Array<mxl::Variant>.
    // They only exist for convenience, allowing the creation of inline arrays
    // without the cumbersomeness of populating the array item by item.
    // mxl::Tuple can be used in every way you would use mxl::Array.
    //
    // Example:
    // >>> auto tp = mxl::Tuple{1.2, 2, "3"};
    //
    template<ArrayValue _Ty = Variant>
    class Tuple: public Array<_Ty>
    {
    public:
        Tuple(std::initializer_list<_Ty> args);

        template<Castable<_Ty>... _Ts>
        Tuple(_Ts... args);
    };
}