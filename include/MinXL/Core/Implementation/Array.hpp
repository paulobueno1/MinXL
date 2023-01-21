#pragma once

#include "MinXL/Core/Interface/Array.hpp"

namespace mxl
{
    template<ArrayValue _Ty>
    inline Array<_Ty>::Array()
    {
    }


    template<ArrayValue _Ty>
    inline Array<_Ty>::Array(const uint64_t rows, const uint64_t cols)
    {
        Allocate(rows, cols);
    }


    template<ArrayValue _Ty>
    inline Array<_Ty>::Array(const Array<_Ty>& other)
    {
        if (Allocate(other.Rows(), other.Columns()))
            std::copy(cbegin(other), cend(other), begin(*this));
    }


    template<ArrayValue _Ty>
    inline Array<_Ty>::Array(Array<_Ty>&& other)
    {
        _Header         = other._Header;
        _Body           = other._Body;
        other._Header   = ArrayHeader{};
        other._Body     = ArrayBody{};
    }


    template<ArrayValue _Ty>
    inline Array<_Ty>& Array<_Ty>::operator=(const Array<_Ty>& other)
    {
        if (Allocate(other.Rows(), other.Columns()))
            std::copy(cbegin(other), cend(other), begin(*this));

        return *this;
    }


    template<ArrayValue _Ty>
    inline Array<_Ty>& Array<_Ty>::operator=(Array<_Ty>&& other)
    {
        _Header         = other._Header;
        _Body           = other._Body;
        other._Header   = ArrayHeader{};
        other._Body     = ArrayBody{};

        return *this;
    }


    template<ArrayValue _Ty>
    inline Array<_Ty>::Array(const Variant& var)
    {
        const auto value = var.Value();

        if (var.IsArray() && value.Array)
        {
            if (value.Array->Data)
            {
                const uint64_t cols = value.Array->Columns.ElementCount;
                const uint64_t rows = value.Array->Rows.ElementCount;
                constexpr auto type = Type::GetID<_Ty>();

                if (var.IsArrayOfType(type))
                {
                    auto src  = reinterpret_cast<Array*>((char*)value.Array - sizeof(ArrayHeader));

                    if (Allocate(rows, cols))
                        std::copy(cbegin(*src), cend(*src), begin(*this));
                }
                else
                {
                    MXL_THROW("Invalid attempt to copy-construct Array from Variant; type mismatch detected");
                }
            }
        }
        else
        {
            MXL_THROW("Invalid attempt to copy-construct Array from Variant; passed Variant is not an array");
        }
    }


    template<ArrayValue _Ty>
    inline Array<_Ty>::Array(Variant&& var)
    {
        auto value = var.Value();

        if (var.IsArray() && value.Array)
        {
            constexpr auto type = Type::GetID<_Ty>();

            if (var.IsArrayOfType(type))
            {
                auto src = reinterpret_cast<Array*>((char*)value.Array - sizeof(ArrayHeader));

                _Header         = src->_Header;
                _Body           = src->_Body;
                src->_Header    = ArrayHeader{};
                src->_Body      = ArrayBody{};
            }
            else
            {
                MXL_THROW("Invalid attempt to move-construct Array from Variant; type mismatch");
            }
        }
        else
        {
            MXL_THROW("Invalid attempt to move-construct Array from Variant; passed Variant is not an array");
        }
    }


    template<ArrayValue _Ty>
    inline Array<_Ty>::Array(const std::vector<_Ty>& vec)
    {
        if (Allocate(vec.size(), 1))
            std::copy(vec.cbegin(), vec.cend(), begin(*this));
    }


    template<ArrayValue _Ty>
    inline Array<_Ty>::Array(const std::initializer_list<_Ty>&& vec)
    {
        if (Allocate(vec.size(), 1))
            std::copy(vec.begin(), vec.end(), begin(*this));
    }


    template<ArrayValue _Ty>
    inline Array<_Ty>::~Array()
    {
        if (_Body.Data)
            std::free(_Body.Data);
        
        _Header = ArrayHeader{};
        _Body   = ArrayBody{};
    }


    template<ArrayValue _Ty> 
    inline bool Array<_Ty>::Allocate(const uint64_t rows, const uint64_t cols)
    {
        constexpr auto type = Type::GetID<_Ty>();
        
        static_assert(
            type != Type::ID::Empty, "Attempt to instantiate Array with unsupported type."
        );

        if (auto buffer = std::calloc(rows * cols, sizeof(_Ty)))
        {
            _Header.Type                = (uint32_t)type;
            _Body.Dims                  = 2;
            _Body.Features              = (uint16_t)ArrayFeatures::HasVarType;
            _Body.ElementSize           = sizeof(_Ty);
            _Body.Locks                 = 0;
            _Body.Data                  = buffer;
            _Body.Columns.ElementCount  = cols;
            _Body.Columns.LowerBound    = 1;
            _Body.Rows.ElementCount     = rows;
            _Body.Rows.LowerBound       = 1;

            switch (type)
            {
                case Type::ID::String:
                    _Body.Features |= (uint16_t)ArrayFeatures::ArrayOfStrings;
                    break;

                case Type::ID::Variant:
                    _Body.Features |= (uint16_t)ArrayFeatures::ArrayOfVariants;
                    break;

                default:
                    break;
            }

            return true;
        }
        else
        {
            _Header = ArrayHeader{};
            _Body   = ArrayBody{};

            return false;
        }
    }


    template<ArrayValue _Ty>
    inline void Array<_Ty>::Deallocate(ArrayBody* array)
    {
        auto ptr = reinterpret_cast<Array*>((char*)array - sizeof(ArrayHeader));

        if (ptr)
        {
            if (ptr->_Body.Data)
                std::free(ptr->_Body.Data);

            std::free(ptr);
        }
    }


    template<ArrayValue _Ty>
    inline const _Ty& Array<_Ty>::operator[](const uint64_t index) const
    {
        return Data()[index];
    }


    template<ArrayValue _Ty>
    inline _Ty& Array<_Ty>::operator[](const uint64_t index)
    {
        return Data()[index];
    }


    template<ArrayValue _Ty>
    inline const _Ty& Array<_Ty>::operator()(const uint64_t row, const uint64_t col) const
    {
        return Data()[row + col * Rows()];
    }


    template<ArrayValue _Ty>
    inline _Ty& Array<_Ty>::operator()(const uint64_t row, const uint64_t col)
    {
        return Data()[row + col * Rows()];
    }


    template<ArrayValue _Ty>
    inline void Array<_Ty>::Resize(const uint64_t rows, const uint64_t cols)
    {
        const uint64_t oldCols = _Body.Columns.ElementCount;
        const uint64_t oldRows = _Body.Rows.ElementCount;

        if (oldCols == cols && oldRows == rows)
            return;

        auto oldPtr = Data();

        if (auto newPtr = static_cast<_Ty*>(calloc(rows * cols, sizeof(_Ty))))
        {
            const auto newCols = std::min(oldCols, cols);
            const auto newRows = std::min(oldRows, rows);

            // Copy each column from old buffer to new
            for (uint64_t c = 0; c < newCols; c++)
            {
                auto oldColBegin    = &operator()(0, c);
                auto oldColEnd      = &operator()(newRows, c);
                auto newColBegin    = &newPtr[c * rows];

                std::copy(oldColBegin, oldColEnd, newColBegin);
            }

            _Body.Data = newPtr;
            _Body.Columns.ElementCount = cols;
            _Body.Rows.ElementCount = rows;

            std::free(oldPtr);
        }
    }


    template <ArrayValue _Ty>
    inline auto begin(Array<_Ty>& arr)
    {
        return arr.Data();
    }


    template <ArrayValue _Ty>
    inline auto end(Array<_Ty>& arr)
    { 
        return arr.Data() + arr.Size();
    }


    template <ArrayValue _Ty>
    inline auto cbegin(const Array<_Ty>& arr)
    { 
        return arr.Data();
    }


    template <ArrayValue _Ty>
    inline auto cend(const Array<_Ty>& arr)
    { 
        return arr.Data() + arr.Size();
    }
}

