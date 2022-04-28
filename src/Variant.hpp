#pragma once

#include "Common.hpp"
#include "COM.hpp"

namespace xl
{
    // ------------------------------------------------------------------------------------------------
    // C++ wrapper to COM's original VARIANT struct. MinXL's official and safest way of exchanging 
    // data between C++ and Excel.
    //
    // - Copy and move constructible from/into xl::String and xl::Array;
    // - Has type-safe arithmetic operator overloads, including compound assignement;
    // - Castable to built-in numeric types;
    // - Castable into xl::Array reference. Useful to temporarily treat a variant as an array without
    //   necessarily copying data
    // ------------------------------------------------------------------------------------------------
    class Variant: public COM::VARIANT
    {
    public:
        Variant();
        ~Variant();

        Variant(const Variant& other);
        Variant(Variant&& other);
        Variant& operator=(const Variant& other);
        Variant& operator=(Variant&& other);

        Variant(const String& value);
        Variant(String&& value);
        Variant& operator=(const String& other);
        Variant& operator=(String&& other);

        template<typename _Ty> Variant(const Array<_Ty>& value);
        template<typename _Ty> Variant(Array<_Ty>&& value);
        template<typename _Ty> Variant& operator=(const Array<_Ty>& value);
        template<typename _Ty> Variant& operator=(Array<_Ty>&& value);

    public:
        uint16_t  Type() const;
        bool      IsNumeric() const;
        bool      IsString() const;
        bool      IsDate() const;
        bool      IsArray() const;
        bool      IsEmpty() const;
        bool      IsArrayOf(uint16_t type) const;
        uint16_t  IsArrayOf() const;
    
    private:
        void      Deallocate();

    public:
        Variant(const short value);
        Variant(const int value);
        Variant(const long value);
        Variant(const long long value);
        Variant(const float value);
        Variant(const double value);

        Variant& operator=(const short value);
        Variant& operator=(const int value);
        Variant& operator=(const long value);
        Variant& operator=(const long long value);
        Variant& operator=(const float value);
        Variant& operator=(const double value);
    
        operator short();
        operator int();
        operator long();
        operator float();
        operator double();

        template<typename _Ty> operator Array<_Ty>&();

    public:
        template<typename _Ty> Variant operator+(const _Ty& value) const;
        template<typename _Ty> Variant operator-(const _Ty& value) const;
        template<typename _Ty> Variant operator*(const _Ty& value) const;
        template<typename _Ty> Variant operator/(const _Ty& value) const;

        template<typename _Ty> Variant& operator+=(const _Ty& value);
        template<typename _Ty> Variant& operator-=(const _Ty& value);
        template<typename _Ty> Variant& operator*=(const _Ty& value);
        template<typename _Ty> Variant& operator/=(const _Ty& value);
    };

    std::ostream& operator<<(std::ostream &os, const Variant& v);
}