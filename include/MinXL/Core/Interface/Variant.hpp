#pragma once

#include "MinXL/Core/Common.hpp"
#include "MinXL/Core/Types.hpp"


namespace mxl
{
    union VariantValue
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
        friend class String;

    public:
        using ValueType = VariantValue;

        Variant();
        ~Variant();

    private:
        Type::ID                _Type{Type::ID::Empty};
        uint8_t                 _ReservedMid[6];
        ValueType               _Value{0};
        uint8_t                 _ReservedEnd[8];

    public:
        inline auto Type()       const { return _Type;                           }
        inline bool IsEmpty()    const { return _Type == Type::ID::Empty;        }
        inline bool IsNumeric()  const { return Type::IsNumeric(_Type);          }
        inline bool IsString()   const { return _Type == Type::ID::String;       }
        inline bool IsDate()     const { return _Type == Type::ID::Date;         }
        inline bool IsArray()    const { return (bool)(_Type & Type::ID::Array); }

        inline const ValueType& Value() const
        {
            return _Value;
        }

        inline ValueType& Value()
        {
            return _Value;
        }

        // Direct numeric accessors

        inline auto& Int16()                { if (_Type == Type::ID::Int16)     return _Value.Int16;    MXL_THROW("Invalid direct access; Variant is not of type Int16");  }
        inline auto& Int32()                { if (_Type == Type::ID::Int32)     return _Value.Int32;    MXL_THROW("Invalid direct access; Variant is not of type Int32");  }
        inline auto& Int64()                { if (_Type == Type::ID::Int64)     return _Value.Int64;    MXL_THROW("Invalid direct access; Variant is not of type Int64");  }
        inline auto& Float()                { if (_Type == Type::ID::Float)     return _Value.Float;    MXL_THROW("Invalid direct access; Variant is not of type Float");  }
        inline auto& Double()               { if (_Type == Type::ID::Double)    return _Value.Double;   MXL_THROW("Invalid direct access; Variant is not of type Double"); }

        inline const auto& Int16() const    { if (_Type == Type::ID::Int16)     return _Value.Int16;    MXL_THROW("Invalid direct access; Variant is not of type Int16");  }
        inline const auto& Int32() const    { if (_Type == Type::ID::Int32)     return _Value.Int32;    MXL_THROW("Invalid direct access; Variant is not of type Int32");  }
        inline const auto& Int64() const    { if (_Type == Type::ID::Int64)     return _Value.Int64;    MXL_THROW("Invalid direct access; Variant is not of type Int64");  }
        inline const auto& Float() const    { if (_Type == Type::ID::Float)     return _Value.Float;    MXL_THROW("Invalid direct access; Variant is not of type Float");  }
        inline const auto& Double() const   { if (_Type == Type::ID::Double)    return _Value.Double;   MXL_THROW("Invalid direct access; Variant is not of type Double"); }

        Type::ID ArrayType() const
        {
            return IsArray() ? _Type ^ Type::ID::Array : Type::ID::Empty;
        }

        bool IsArrayOfType(Type::ID type) const
        {
            return IsArray() && (type == (_Type ^ Type::ID::Array));
        }

        bool IsArrayOfType(uint32_t type) const
        {
            return IsArray() && ((Type::ID)type == (_Type ^ Type::ID::Array));
        }

    private:
        void Deallocate();

    public:
        // Copy/Move Construction/Assignment

        Variant(const Variant& other);
        Variant(Variant&& other);
        Variant& operator=(const Variant& other);
        Variant& operator=(Variant&& other);

        // Conversion from/to String + String operations

        Variant(const String& string);
        Variant(String&& string);
        Variant& operator=(const String& string);
        Variant& operator=(String&& string);

        // Conversion from/to Array

        template<ArrayValue _Ty> Variant(const Array<_Ty>& array);
        template<ArrayValue _Ty> Variant(Array<_Ty>&& array);
        template<ArrayValue _Ty> Variant& operator=(const Array<_Ty>& array);
        template<ArrayValue _Ty> Variant& operator=(Array<_Ty>&& array);

        // Conversion from/to primitives

        template<Numeric _Ty> Variant(const _Ty value);
        template<Numeric _Ty> Variant& operator=(const _Ty value);
        template<Numeric _Ty> operator _Ty() const;

        // Math operations with Variants

        Variant  operator+(const Variant other) const;
        Variant  operator-(const Variant other) const;
        Variant  operator*(const Variant other) const;
        Variant  operator/(const Variant other) const;
        Variant& operator+=(const Variant& other);
        Variant& operator-=(const Variant& other);
        Variant& operator*=(const Variant& other);
        Variant& operator/=(const Variant& other);

        // Math operations with numeric types
        
        template<Numeric _Ty> Variant   operator+(const _Ty value) const;
        template<Numeric _Ty> Variant   operator-(const _Ty value) const;
        template<Numeric _Ty> Variant   operator*(const _Ty value) const;
        template<Numeric _Ty> Variant   operator/(const _Ty value) const;
        template<Numeric _Ty> Variant&  operator+=(const _Ty value);
        template<Numeric _Ty> Variant&  operator-=(const _Ty value);
        template<Numeric _Ty> Variant&  operator*=(const _Ty value);
        template<Numeric _Ty> Variant&  operator/=(const _Ty value);
    };

    template<Numeric _Ty> _Ty  operator+(_Ty value, const Variant& var);
    template<Numeric _Ty> _Ty  operator-(_Ty value, const Variant& var);
    template<Numeric _Ty> _Ty  operator*(_Ty value, const Variant& var);
    template<Numeric _Ty> _Ty  operator/(_Ty value, const Variant& var);
    template<Numeric _Ty> _Ty& operator+=(_Ty value, const Variant& var);
    template<Numeric _Ty> _Ty& operator-=(_Ty value, const Variant& var);
    template<Numeric _Ty> _Ty& operator*=(_Ty value, const Variant& var);
    template<Numeric _Ty> _Ty& operator/=(_Ty value, const Variant& var);

}

std::ostream& operator<<(std::ostream &os, const mxl::Variant& var);