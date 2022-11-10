#pragma once

#include "Core/Common.hpp"
#include "Core/Types.hpp"


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


    template<ArrayValue _Ty> class Array
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
        Array(const std::vector<_Ty>& vec);
        Array(const std::initializer_list<_Ty>&& vec);

        ~Array();

    public:
        const _Ty&  operator[](const uint64_t index) const;
        _Ty&        operator[](const uint64_t index);
        const _Ty&  operator()(const uint64_t row, const uint64_t col) const;
        _Ty&        operator()(const uint64_t row, const uint64_t col);

    public:
        auto Type() const         { return _Header.Type;                                                      }
        auto Size() const         { return _Body.Columns.ElementCount * _Body.Rows.ElementCount;              }
        auto Data() const         { return static_cast<_Ty*>(_Body.Data);                                     }
        auto Rows() const         { return _Body.Rows.ElementCount;                                           }
        auto Columns() const      { return _Body.Columns.ElementCount;                                        }
        auto ElementSize() const  { return _Body.ElementSize;                                                 }
        auto Column(uint64_t col) { return std::make_pair(&operator()(0, col), &operator()(Rows(), col));    }

        void Resize(const uint64_t rows, const uint64_t cols);

    private:
        bool Allocate(const uint64_t rows, const uint64_t cols);
        static void Deallocate(ArrayBody* array);
    };

    template <ArrayValue _Ty> auto begin   (Array<_Ty>& arr);
    template <ArrayValue _Ty> auto end     (Array<_Ty>& arr);
    template <ArrayValue _Ty> auto cbegin  (const Array<_Ty>& arr);
    template <ArrayValue _Ty> auto cend    (const Array<_Ty>& arr);
}
