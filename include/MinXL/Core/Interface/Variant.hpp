#pragma once

#include "MinXL/Core/Types.hpp"


namespace mxl
{
    union VariantUnion
    {
        uint8_t         Byte;
        int16_t         Int16;
        int32_t         Int32;
        int64_t         Int64;
        float           Float;
        double          Double;
        char16_t*       String;
        ArrayBody*      Array;
        void*           Empty;
    };


    class Variant
    {
    private:
        Type::ID                _Type;
        uint8_t                 _ReservedMid[6];
        VariantUnion            _Value;
        uint8_t                 _ReservedEnd[8];


    public:
        Variant();
        
        // Variant <=> Variant

        Variant(const Variant& other);
        Variant(Variant&& other);
        Variant& operator=(const Variant& other);
        Variant& operator=(Variant&& other);

        // Variant <=> String

        Variant(const String& str);
        Variant(String&& str);
        Variant(const char* str);
        Variant(const char16_t* str);
        explicit operator String() &&;
        explicit operator String() const &;
        explicit operator String&();
        explicit operator const String&() const;


        // Variant <=> Array

        template <ArrayValue _Ty> Variant(const Array<_Ty>& array);
        template <ArrayValue _Ty> Variant(Array<_Ty>&& array);
        template <ArrayValue _Ty> explicit operator Array<_Ty>() &&;
        template <ArrayValue _Ty> explicit operator Array<_Ty>() const &;
        template <ArrayValue _Ty> explicit operator Array<_Ty>&();
        template <ArrayValue _Ty> explicit operator const Array<_Ty>&() const;

        // Variant <=> Numeric

        template <Numeric _Ty> Variant(_Ty value);
        template <Numeric _Ty> explicit operator _Ty&();
        template <Numeric _Ty> explicit operator const _Ty&() const;

        ~Variant();

    public:
        Type::ID    TypeID() const;
        bool        IsEmpty() const;
        bool        IsNumeric() const;
        bool        IsString() const;
        bool        IsDate() const;
        bool        IsArray() const;
        Type::ID    ArrayTypeID() const;
        bool        IsArrayOfTypeID(Type::ID type) const;
        bool        IsArrayOfTypeID(uint32_t type) const;


    private:
        void Deallocate();


    public:

        // Variant <> Variant operators

        Variant     operator+(const Variant& other) const;
        Variant     operator-(const Variant& other) const;
        Variant     operator*(const Variant& other) const;
        Variant     operator/(const Variant& other) const;
        Variant&    operator+=(const Variant& other);
        Variant&    operator-=(const Variant& other);
        Variant&    operator*=(const Variant& other);
        Variant&    operator/=(const Variant& other);
        bool        operator==(const Variant& other) const;
        bool        operator!=(const Variant& other) const;
        bool        operator<(const Variant& other) const;
        bool        operator<=(const Variant& other) const;
        bool        operator>(const Variant& other) const;
        bool        operator>=(const Variant& other) const;
        
        // Variant <> Primitive operators

        template<Numeric _Ty> Variant   operator+(const _Ty value) const;
        template<Numeric _Ty> Variant   operator-(const _Ty value) const;
        template<Numeric _Ty> Variant   operator*(const _Ty value) const;
        template<Numeric _Ty> Variant   operator/(const _Ty value) const;
        template<Numeric _Ty> Variant&  operator+=(const _Ty value);
        template<Numeric _Ty> Variant&  operator-=(const _Ty value);
        template<Numeric _Ty> Variant&  operator*=(const _Ty value);
        template<Numeric _Ty> Variant&  operator/=(const _Ty value);
        template<Numeric _Ty> bool      operator==(const _Ty value) const;
        template<Numeric _Ty> bool      operator!=(const _Ty value) const;
        template<Numeric _Ty> bool      operator<(const _Ty value) const;
        template<Numeric _Ty> bool      operator<=(const _Ty value) const;
        template<Numeric _Ty> bool      operator>(const _Ty value) const;
        template<Numeric _Ty> bool      operator>=(const _Ty value) const;

    };

    // Primitive <> Variant operators

    template<Numeric _Ty> _Ty  operator+(_Ty value, const Variant& var);
    template<Numeric _Ty> _Ty  operator-(_Ty value, const Variant& var);
    template<Numeric _Ty> _Ty  operator*(_Ty value, const Variant& var);
    template<Numeric _Ty> _Ty  operator/(_Ty value, const Variant& var);
    template<Numeric _Ty> _Ty& operator+=(_Ty value, const Variant& var);
    template<Numeric _Ty> _Ty& operator-=(_Ty value, const Variant& var);
    template<Numeric _Ty> _Ty& operator*=(_Ty value, const Variant& var);
    template<Numeric _Ty> _Ty& operator/=(_Ty value, const Variant& var);
    template<Numeric _Ty> bool operator==(const _Ty value, const Variant& var);
    template<Numeric _Ty> bool operator!=(const _Ty value, const Variant& var);
    template<Numeric _Ty> bool operator<(const _Ty value, const Variant& var);
    template<Numeric _Ty> bool operator<=(const _Ty value, const Variant& var);
    template<Numeric _Ty> bool operator>(const _Ty value, const Variant& var);
    template<Numeric _Ty> bool operator>=(const _Ty value, const Variant& var);


    std::ostream& operator<<(std::ostream &os, const Variant& var);
}