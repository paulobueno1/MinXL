#pragma once

#include "MinXL/Core/Interface/Variant.hpp"

namespace mxl
{
    inline Variant::Variant(): _Type{Type::ID::Empty}, _Value{0}
    {
    }

    inline Variant::~Variant()
    {
        Deallocate();
    }

    inline Variant::Variant(const Variant& other)
    {
        // For Variants containing Arrays and Strings, we take advantage of the fact that these classes
        // already know how to construct themselves from a Variant; and Variant already knows
        // how to construct itselft from them.

        if (other.IsArray())
        {
            Variant temp;

            switch(other.ArrayType())
            {
                case Type::ID::Int16:   temp = Variant{Array<int16_t>{other}};  break;
                case Type::ID::Int32:   temp = Variant{Array<int32_t>{other}};  break;
                case Type::ID::Int64:   temp = Variant{Array<int64_t>{other}};  break;
                case Type::ID::Float:   temp = Variant{Array<float>  {other}};  break;
                case Type::ID::Double:  temp = Variant{Array<double> {other}};  break;
                case Type::ID::Variant: temp = Variant{Array<Variant>{other}};  break;

                default:
                {
                    MXL_THROW(
                        "Invalid attempt to construct Variant from another Variant that contains Array of invalid type"
                    );
                }
            }

            _Type               = temp._Type;
            _Value.Array        = temp._Value.Array;
            temp._Type          = Type::ID::Empty;
            temp._Value.Empty   = nullptr;
        }

        else if (other.IsString())
        {
            Variant temp{String{other}};
            
            _Type               = temp._Type;
            _Value.Array        = temp._Value.Array;
            temp._Type          = Type::ID::Empty;
            temp._Value.Empty   = nullptr;
        }

        else
        {
            _Type   = other._Type;
            _Value  = other._Value;
        }
    }

    inline Variant::Variant(Variant&& other)
    {
        _Type               = other._Type;
        _Value              = other._Value;
        other._Type         = Type::ID::Empty;
        other._Value.Empty  = nullptr;
    }

    inline Variant& Variant::operator=(const Variant& other)
    {
        if (this == &other)
            return *this;

        Deallocate();

        Variant temp{other};

        _Type               = temp._Type;
        _Value              = temp._Value;
        temp._Type          = Type::ID::Empty;
        temp._Value.Empty   = nullptr;

        return *this;
    }

    inline Variant& Variant::operator=(Variant&& other)
    {
        if (this == &other)
            return *this;

        Deallocate();

        _Type               = other._Type;
        _Value              = other._Value;
        other._Type         = Type::ID::Empty;
        other._Value.Empty  = nullptr;

        return *this;
    }


    inline Variant::Variant(const String& string)
    {
        String temp{string};

        if (auto buffer = static_cast<char16_t*>(temp))
        {
            _Type               = Type::ID::String;
            _Value.String       = buffer;
            temp._Container     = nullptr;
        }
    }

    inline Variant::Variant(String&& string)
    {
        if (auto buffer = static_cast<char16_t*>(string))
        {
            _Type               = Type::ID::String;
            _Value.String       = buffer;
            string._Container   = nullptr;
        }
    }

    inline Variant& Variant::operator=(const String& string)
    {
        Deallocate();

        Variant temp{string};
        _Type                   = temp._Type;
        _Value                  = temp._Value;
        temp._Type              = Type::ID::Empty;
        temp._Value.Empty       = nullptr;

        return *this;
    }
                                    

    inline Variant& Variant::operator=(String&& string)
    {
        Deallocate();

        Variant temp{string};
        _Type                   = temp._Type;
        _Value                  = temp._Value;
        temp._Type              = Type::ID::Empty;
        temp._Value.Empty       = nullptr;

        return *this;
    }

    inline void Variant::Deallocate()
    {
        if (IsArray())
            Array<Variant>::Deallocate(_Value.Array);

        else if (IsString())
            String::Deallocate(_Value.String);

        _Type               = Type::ID::Empty;
        _Value.Empty        = nullptr;
    }


    inline Variant Variant::operator+(const Variant other) const
    {
        if (IsNumeric())
        {
            switch (other._Type)
            {
                case Type::ID::Int16:     return operator+(other._Value.Int16);
                case Type::ID::Int32:     return operator+(other._Value.Int32);
                case Type::ID::Int64:     return operator+(other._Value.Int64);
                case Type::ID::Float:     return operator+(other._Value.Float);
                case Type::ID::Double:    return operator+(other._Value.Double);
                case Type::ID::Empty:     return *this;
                default: ;
            }
        }
        else if (IsEmpty())
        {
            return other;
        }

        MXL_THROW("Invalid attempt to perform addition between one or more non-numeric Variants");
    }


    inline Variant Variant::operator-(const Variant other) const
    {
        if (IsNumeric())
        {
            switch (other._Type)
            {
                case Type::ID::Int16:     return operator-(other._Value.Int16);
                case Type::ID::Int32:     return operator-(other._Value.Int32);
                case Type::ID::Int64:     return operator-(other._Value.Int64);
                case Type::ID::Float:     return operator-(other._Value.Float);
                case Type::ID::Double:    return operator-(other._Value.Double);
                case Type::ID::Empty:     return *this;
                default: ;
            }
        }
        else if (IsEmpty())
        {
            return other * -1;
        }

        MXL_THROW("Invalid attempt to perform subtraction between one or more non-numeric Variants");
    }


    inline Variant Variant::operator*(const Variant other) const
    {
        if (IsNumeric())
        {
            switch (other._Type)
            {
                case Type::ID::Int16:     return operator*(other._Value.Int16);
                case Type::ID::Int32:     return operator*(other._Value.Int32);
                case Type::ID::Int64:     return operator*(other._Value.Int64);
                case Type::ID::Float:     return operator*(other._Value.Float);
                case Type::ID::Double:    return operator*(other._Value.Double);
                case Type::ID::Empty:     return Variant{0.0};
                default: ;
            }
        }
        else if (IsEmpty())
        {
            return Variant{0.0};
        }

        MXL_THROW("Invalid attempt to perform multiplication between one or more non-numeric Variants");
    }


    inline Variant Variant::operator/(const Variant other) const
    {
        if (IsNumeric())
        {
            switch (other._Type)
            {
                case Type::ID::Int16:     return operator/(other._Value.Int16);
                case Type::ID::Int32:     return operator/(other._Value.Int32);
                case Type::ID::Int64:     return operator/(other._Value.Int64);
                case Type::ID::Float:     return operator/(other._Value.Float);
                case Type::ID::Double:    return operator/(other._Value.Double);
                case Type::ID::Empty:     MXL_THROW("Division by empty Variant");
                default: ;
            }
        }
        else if (IsEmpty())
        {
            return Variant{0.0};
        }

        MXL_THROW("Invalid attempt to perform division between one or more non-numeric Variants");
    }


    inline Variant& Variant::operator+=(const Variant& other)
    {
        if (IsNumeric())
        {
            switch (other._Type)
            {
                case Type::ID::Int16:    operator+=(other._Value.Int16);    return *this;
                case Type::ID::Int32:    operator+=(other._Value.Int32);    return *this;
                case Type::ID::Int64:    operator+=(other._Value.Int64);    return *this;
                case Type::ID::Float:    operator+=(other._Value.Float);    return *this;
                case Type::ID::Double:   operator+=(other._Value.Double);   return *this;
                case Type::ID::Empty:                                       return *this;
                default: ;
            }
        }
        else if (IsEmpty())
        {
            *this = other;
            return *this;
        }

        MXL_THROW("Invalid attempt to perform assign-addition between one or more non-numeric Variants");
    }


    inline Variant& Variant::operator-=(const Variant& other)
    {
        if (IsNumeric())
        {
            switch (other._Type)
            {
                case Type::ID::Int16:    operator-=(other._Value.Int16);    return *this;
                case Type::ID::Int32:    operator-=(other._Value.Int32);    return *this;
                case Type::ID::Int64:    operator-=(other._Value.Int64);    return *this;
                case Type::ID::Float:    operator-=(other._Value.Float);    return *this;
                case Type::ID::Double:   operator-=(other._Value.Double);   return *this;
                case Type::ID::Empty:                                       return *this;
                default: ;
            }
        }
        else if (IsEmpty())
        {
            *this = other * -1;
            return *this;
        }

        MXL_THROW("Invalid attempt to perform assign-subtraction between one or more non-numeric Variants");
    }


    inline Variant& Variant::operator*=(const Variant& other)
    {
        if (IsNumeric())
        {
            switch (other._Type)
            {
                case Type::ID::Int16:    operator*=(other._Value.Int16);    return *this;
                case Type::ID::Int32:    operator*=(other._Value.Int32);    return *this;
                case Type::ID::Int64:    operator*=(other._Value.Int64);    return *this;
                case Type::ID::Float:    operator*=(other._Value.Float);    return *this;
                case Type::ID::Double:   operator*=(other._Value.Double);   return *this;
                case Type::ID::Empty:    operator*=(0.0);                   return *this;
                default: ;
            }
        }
        else if (IsEmpty())
        {
            *this = Variant{0.0};
            return *this;
        }

        MXL_THROW("Invalid attempt to perform assign-multiplication between one or more non-numeric Variants");
    }


    inline Variant& Variant::operator/=(const Variant& other)
    {
        if (IsNumeric())
        {
            switch (other._Type)
            {
                case Type::ID::Int16:    operator/=(other._Value.Int16);    return *this;
                case Type::ID::Int32:    operator/=(other._Value.Int32);    return *this;
                case Type::ID::Int64:    operator/=(other._Value.Int64);    return *this;
                case Type::ID::Float:    operator/=(other._Value.Float);    return *this;
                case Type::ID::Double:   operator/=(other._Value.Double);   return *this;
                case Type::ID::Empty:    MXL_THROW("Assign-division by empty Variant");
                default: ;
            }
        }
        else if (IsEmpty())
        {
            *this = Variant{0.0};
            return *this;
        }

        MXL_THROW("Invalid attempt to perform assign-division between one or more non-numeric Variants");
    }


    template<Numeric _Ty>
    inline Variant::Variant(const _Ty value)
    {
        _Type = Type::GetID<_Ty>();

        if constexpr (Type::IsSame<_Ty, int16_t>)   _Value.Int16  = value;
        if constexpr (Type::IsSame<_Ty, int32_t>)   _Value.Int32  = value;
        if constexpr (Type::IsSame<_Ty, int64_t>)   _Value.Int64  = value;
        if constexpr (Type::IsSame<_Ty, float>)     _Value.Float  = value;
        if constexpr (Type::IsSame<_Ty, double>)    _Value.Double = value;
    }


    template<Numeric _Ty>
    inline Variant& Variant::operator=(const _Ty value)
    {
        Deallocate();

        _Type = Type::GetID<_Ty>();

        if constexpr (Type::IsSame<_Ty, int16_t>)   _Value.Int16  = value;
        if constexpr (Type::IsSame<_Ty, int32_t>)   _Value.Int32  = value;
        if constexpr (Type::IsSame<_Ty, int64_t>)   _Value.Int64  = value;
        if constexpr (Type::IsSame<_Ty, float>)     _Value.Float  = value;
        if constexpr (Type::IsSame<_Ty, double>)    _Value.Double = value;

        return *this;
    }


    template <ArrayValue _Ty>
    Variant::Variant(const Array<_Ty>& array)
    {
        constexpr auto typeID = Type::GetID<_Ty>();

        if constexpr (typeID != Type::ID::Empty)
        {
            if (auto dst = static_cast<Array<_Ty>*>(std::malloc(sizeof(Array<_Ty>))))
            {
                *dst = array;

                _Type           = Type::ID::Array | typeID;
                _Value.Array    = reinterpret_cast<ArrayBody*>((char*)dst + sizeof(ArrayHeader));
            }
            else
            {
                MXL_THROW("Allocation failed when constructing Variant from Array");
            }
        }
        else
        {
            MXL_THROW("Invalid attempt to construct Variant from Array containing unsupported data type");
        }
    }


    template <ArrayValue _Ty>
    Variant::Variant(Array<_Ty>&& array)
    {
        constexpr auto typeID = Type::GetID<_Ty>();

        if constexpr (typeID != Type::ID::Empty)
        {
            if (auto dst = static_cast<Array<_Ty>*>(std::malloc(sizeof(Array<_Ty>))))
            {
                *dst = std::move(array);

                _Type           = Type::ID::Array | typeID;
                _Value.Array    = reinterpret_cast<ArrayBody*>((char*)dst + sizeof(ArrayHeader));
                array._Header   = ArrayHeader{};
                array._Body     = ArrayBody{};
            }
            else
            {
                MXL_THROW("Allocation failed when move-constructing Variant from Array");
            }
        }
        else
        {
            MXL_THROW("Invalid attempt to move-construct Variant from Array containing unsupported data type");
        }
    }


    template <ArrayValue _Ty>
    Variant& Variant::operator=(const Array<_Ty>& array)
    {
        Deallocate();

        Variant temp{array};
        _Type                   = temp._Type;
        _Value                  = temp._Value;
        temp._Type              = Type::ID::Empty;
        temp._Value.Empty       = nullptr;

        return *this;
    }


    template <ArrayValue _Ty>
    Variant& Variant::operator=(Array<_Ty>&& array)
    {
        Deallocate();

        Variant temp{array};
        _Type                   = temp._Type;
        _Value                  = temp._Value;
        temp._Type              = Type::ID::Empty;
        temp._Value.Empty       = nullptr;

        return *this;
    }


    template<Numeric _Ty>
    inline Variant::operator _Ty() const
    {
        switch (_Type)
        {
            case Type::ID::Int16:     return (_Ty) _Value.Int16     ;
            case Type::ID::Int32:     return (_Ty) _Value.Int32     ;
            case Type::ID::Int64:     return (_Ty) _Value.Int64     ;
            case Type::ID::Float:     return (_Ty) _Value.Float     ;
            case Type::ID::Double:    return (_Ty) _Value.Double    ;
            case Type::ID::Empty:     return _Ty{0}                 ;
            default: ;
        }

        MXL_THROW("Invalid attempt to cast non-numeric Variant into numeric type");
    }


    template<Numeric _Ty>
    inline Variant Variant::operator+(const _Ty value) const
    {
        switch (_Type)
        {
            case Type::ID::Int16:     return Variant{(int16_t)    (_Value.Int16  + value) };
            case Type::ID::Int32:     return Variant{(int32_t)    (_Value.Int32  + value) };
            case Type::ID::Int64:     return Variant{(int64_t)    (_Value.Int64  + value) };
            case Type::ID::Float:     return Variant{(float)      (_Value.Float  + value) };
            case Type::ID::Double:    return Variant{(double)     (_Value.Double + value) };
            case Type::ID::Empty:     return Variant{value};
            default: ;
        }

        MXL_THROW("Invalid attempt to perform addition with a non-numeric Variant");
    }


    template<Numeric _Ty>
    inline Variant Variant::operator-(const _Ty value) const
    {
        switch (_Type)
        {
            case Type::ID::Int16:     return Variant{(int16_t)    (_Value.Int16  - value) };
            case Type::ID::Int32:     return Variant{(int32_t)    (_Value.Int32  - value) };
            case Type::ID::Int64:     return Variant{(int64_t)    (_Value.Int64  - value) };
            case Type::ID::Float:     return Variant{(float)      (_Value.Float  - value) };
            case Type::ID::Double:    return Variant{(double)     (_Value.Double - value) };
            case Type::ID::Empty:     return Variant{-value};
            default: ;
        }

        MXL_THROW("Invalid attempt to perform subtraction with a non-numeric Variant");
    }


    template<Numeric _Ty>
    inline Variant Variant::operator*(const _Ty value) const
    {
        switch (_Type)
        {
            case Type::ID::Int16:     return Variant{(int16_t)    (_Value.Int16  * value) };
            case Type::ID::Int32:     return Variant{(int32_t)    (_Value.Int32  * value) };
            case Type::ID::Int64:     return Variant{(int64_t)    (_Value.Int64  * value) };
            case Type::ID::Float:     return Variant{(float)      (_Value.Float  * value) };
            case Type::ID::Double:    return Variant{(double)     (_Value.Double * value) };
            case Type::ID::Empty:     return Variant{0.0};
            default: ;
        }

        MXL_THROW("Invalid attempt to perform multiplication with a non-numeric Variant");
    }


    template<Numeric _Ty>
    inline Variant Variant::operator/(const _Ty value) const
    {
        switch (_Type)
        {
            case Type::ID::Int16:     return Variant{(int16_t)    (_Value.Int16  / value) };
            case Type::ID::Int32:     return Variant{(int32_t)    (_Value.Int32  / value) };
            case Type::ID::Int64:     return Variant{(int64_t)    (_Value.Int64  / value) };
            case Type::ID::Float:     return Variant{(float)      (_Value.Float  / value) };
            case Type::ID::Double:    return Variant{(double)     (_Value.Double / value) };
            case Type::ID::Empty:     return Variant{0.0};
            default: ;
        }

        MXL_THROW("Invalid attempt to perform division with a non-numeric Variant");
    }


    template<Numeric _Ty>
    inline Variant& Variant::operator+=(const _Ty value)
    {
        switch (_Type)
        {
            case Type::ID::Int16:     _Value.Int16    += value; return *this;
            case Type::ID::Int32:     _Value.Int32    += value; return *this;
            case Type::ID::Int64:     _Value.Int64    += value; return *this;
            case Type::ID::Float:     _Value.Float    += value; return *this;
            case Type::ID::Double:    _Value.Double   += value; return *this;
            case Type::ID::Empty:     *this = Variant{value};   return *this;
            default: ;
        }

        MXL_THROW("Invalid attempt to perform assign-addition with a non-numeric Variant");
    }


    template<Numeric _Ty>
    inline Variant& Variant::operator-=(const _Ty value)
    {
        switch (_Type)
        {
            case Type::ID::Int16:     _Value.Int16    -= value; return *this;
            case Type::ID::Int32:     _Value.Int32    -= value; return *this;
            case Type::ID::Int64:     _Value.Int64    -= value; return *this;
            case Type::ID::Float:     _Value.Float    -= value; return *this;
            case Type::ID::Double:    _Value.Double   -= value; return *this;
            case Type::ID::Empty:     *this = Variant{-value};  return *this;
            default: ;
        }

        MXL_THROW("Invalid attempt to perform assign-subtraction with a non-numeric Variant");
    }


    template<Numeric _Ty>
    inline Variant& Variant::operator*=(const _Ty value)
    {
        switch (_Type)
        {
            case Type::ID::Int16:     _Value.Int16    *= value; return *this;
            case Type::ID::Int32:     _Value.Int32    *= value; return *this;
            case Type::ID::Int64:     _Value.Int64    *= value; return *this;
            case Type::ID::Float:     _Value.Float    *= value; return *this;
            case Type::ID::Double:    _Value.Double   *= value; return *this;
            case Type::ID::Empty:     *this = Variant{0.0};     return *this;
            default: ;
        }

        MXL_THROW("Invalid attempt to perform assign-multiplication with a non-numeric Variant");
    }


    template<Numeric _Ty>
    inline Variant& Variant::operator/=(const _Ty value)
    {
        switch (_Type)
        {
            case Type::ID::Int16:     _Value.Int16    /= value; return *this;
            case Type::ID::Int32:     _Value.Int32    /= value; return *this;
            case Type::ID::Int64:     _Value.Int64    /= value; return *this;
            case Type::ID::Float:     _Value.Float    /= value; return *this;
            case Type::ID::Double:    _Value.Double   /= value; return *this;
            case Type::ID::Empty:     *this = Variant{0.0};     return *this;
            default: ;
        }

        MXL_THROW("Invalid attempt to perform assign-division with a non-numeric Variant");
    }

    template<Numeric _Ty>
    inline _Ty operator+(_Ty value, const Variant& var)
    {
        return value + static_cast<_Ty>(var);
    }

    template<Numeric _Ty>
    inline _Ty operator-(_Ty value, const Variant& var)
    {
        return value + static_cast<_Ty>(var);
    }

    template<Numeric _Ty>
    inline _Ty operator*(_Ty value, const Variant& var)
    {
        return value * static_cast<_Ty>(var);
    }

    template<Numeric _Ty>
    inline _Ty operator/(_Ty value, const Variant& var)
    {
        return value / static_cast<_Ty>(var);
    }

    template<Numeric _Ty>
    inline _Ty& operator+=(_Ty& value, const Variant& var)
    {
        return (value += static_cast<_Ty>(var));
    }

    template<Numeric _Ty>
    inline _Ty& operator-=(_Ty& value, const Variant& var)
    {
        return (value -= static_cast<_Ty>(var));
    }

    template<Numeric _Ty>
    inline _Ty& operator*=(_Ty& value, const Variant& var)
    {
        return (value *= static_cast<_Ty>(var));
    }

    template<Numeric _Ty>
    inline _Ty& operator/=(_Ty& value, const Variant& var)
    {
        return (value /= static_cast<_Ty>(var));
    }
}


inline std::ostream& operator<<(std::ostream &os, const mxl::Variant& var)
{
    switch (var.Type())
    {
        case mxl::Type::ID::Int16:       return os << var.Int16();
        case mxl::Type::ID::Int32:       return os << var.Int32();
        case mxl::Type::ID::Int64:       return os << var.Int64();
        case mxl::Type::ID::Float:       return os << var.Float();
        case mxl::Type::ID::Double:      return os << var.Double();
        case mxl::Type::ID::String:      return os << mxl::String::Str16to8(var.Value().String);
        default: ;
    }

    return os;
}