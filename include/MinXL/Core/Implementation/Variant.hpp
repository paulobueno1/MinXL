#pragma once

#include "MinXL/Core/Types.hpp"

#include "MinXL/Core/Interface/Array.hpp"
#include "MinXL/Core/Interface/String.hpp"
#include "MinXL/Core/Interface/Variant.hpp"
#include "Variant.hpp"


namespace mxl
{
    inline Variant::Variant(): _Type{Type::ID::Empty}, _Value{0}
    {
    }


    inline Variant::Variant(Variant&& other): _Type{other._Type}, _Value{other._Value}
    {
        std::memset(&other, 0, sizeof(Variant));
    }


    inline Variant::Variant(const Variant& other)
    {
        if (other.IsArray())
        {
            Variant temp;

            switch(other.ArrayTypeID())
            {
                case Type::ID::Int16:   temp = static_cast<const Array<int16_t>&>(other);  break;
                case Type::ID::Int32:   temp = static_cast<const Array<int32_t>&>(other);  break;
                case Type::ID::Int64:   temp = static_cast<const Array<int64_t>&>(other);  break;
                case Type::ID::Float:   temp = static_cast<const Array<float>&>(other);    break;
                case Type::ID::Double:  temp = static_cast<const Array<double>&>(other);   break;
                case Type::ID::Variant: temp = static_cast<const Array<Variant>&>(other);  break;

                default:
                {
                    MXL_THROW(
                        "Invalid attempt to construct Variant from another Variant that contains Array of invalid type"
                    );
                }
            }

            std::memcpy(this, &temp, sizeof(Variant));
            std::memset(&temp, 0, sizeof(Variant));
        }
        else if (other.IsString())
        {
            Variant temp{String{other}};

            std::memcpy(this, &temp, sizeof(Variant));
            std::memset(&temp, 0, sizeof(Variant));
        }
        else
        {
            std::memcpy(this, &other, sizeof(Variant));
        }
    }


    inline Variant& Variant::operator=(const Variant& other)
    {
        if (this == &other)
            return *this;

        Deallocate();

        Variant temp{other};

        std::memcpy(this, &temp, sizeof(Variant));
        std::memset(&temp, 0, sizeof(Variant));

        return *this;
    }


    inline Variant& Variant::operator=(Variant&& other)
    {
        if (this == &other)
            return *this;

        Deallocate();

        std::memcpy(this, &other, sizeof(Variant));
        std::memset(&other, 0, sizeof(Variant));

        return *this;
    }


    inline Variant::~Variant()
    {
        Deallocate();
    }


    //
    // Returns ID of the underlying data type.
    //
    inline Type::ID Variant::TypeID() const
    {
        return _Type;
    }


    inline bool Variant::IsEmpty() const
    {
        return _Type == Type::ID::Empty;
    }


    inline bool Variant::IsNumeric() const
    {
        return Type::IsNumericID(_Type);
    }


    inline bool Variant::IsString() const
    {
        return _Type == Type::ID::String;
    }


    inline bool Variant::IsDate() const
    {
        return _Type == Type::ID::Date;
    }


    inline bool Variant::IsArray() const
    {
        return (bool)(_Type & Type::ID::Array);
    }


    //
    // Returns ID of the data type contained in the underlying mxl::Array.
    //
    inline Type::ID Variant::ArrayTypeID() const
    {
        return IsArray() ? _Type ^ Type::ID::Array : Type::ID::Empty;
    }


    //
    // Checks if parameter matches ID of the data type contained in the underlying mxl::Array.
    //
    inline bool Variant::IsArrayOfTypeID(Type::ID id) const
    {
        return IsArray() && (id == (_Type ^ Type::ID::Array));
    }


    //
    // Checks if parameter matches ID of the data type contained in the underlying mxl::Array.
    //
    inline bool Variant::IsArrayOfTypeID(uint32_t id) const
    {
        return IsArray() && ((Type::ID)id == (_Type ^ Type::ID::Array));
    }


    inline Variant::Variant(const String& str)
    {
        String temp = str;

        _Type = Type::ID::String;
        _Value.String = temp.Buffer();

        std::memset(&temp, 0, sizeof(String));

        if (!_Value.String)
            std::memset(this, 0, sizeof(Variant));
    }


    inline Variant::Variant(String&& str): _Type{Type::ID::String}, _Value{.String = str.Buffer()}
    {
        std::memset(&str, 0, sizeof(String));

        if (!_Value.String)
            std::memset(this, 0, sizeof(Variant));
    }


    inline Variant::Variant(const char* str): Variant(String{str})
    {
    }


    inline Variant::Variant(const char16_t* str): Variant(String{str})
    {
    }


    //
    // Performs a "self-move" into a new mxl::String and returns it.
    //
    inline Variant::operator String() &&
    {
        String str = std::move(operator String&());

        std::memset(this, 0, sizeof(String));

        return str;
    }


    //
    // Copies underlying mxl::String into a new one and returns it.
    //
    inline Variant::operator String() const &
    {
        return operator const String&();
    }


    //
    // Exposes a reference to the underlying mxl::String.
    //
    inline Variant::operator String&()
    {
        if (IsString())
            return reinterpret_cast<String&>(_Value.String);

        MXL_THROW("Invalid conversion; Variant is not a String");
    }


    //
    // Exposes a const reference to the underlying mxl::String.
    //
    inline Variant::operator const String&() const
    {
        return static_cast<const String&>(
            const_cast<Variant*>(this)->operator String&()
        );
    }


    template <ArrayValue _Ty>
    inline Variant::Variant(const Array<_Ty>& array)
    {
        constexpr auto size = sizeof(Array<_Ty>);

        Array<_Ty> temp = array;

        if (auto ptr = static_cast<Array<_Ty>*>(std::malloc(size)))
        {
            std::memcpy(ptr, &temp, size);
            std::memset(&temp, 0, size);

            _Type = Type::GetID<Array<_Ty>>();
            _Value.Array = &ptr->_Body;
        }
        else
            MXL_THROW("Dynamic allocation failed.");
    }


    template <ArrayValue _Ty>
    inline Variant::Variant(Array<_Ty>&& array)
    {
        constexpr auto size = sizeof(Array<_Ty>);
        
        if (auto ptr = static_cast<Array<_Ty>*>(std::malloc(size)))
        {
            std::memcpy(ptr, &array, size);
            std::memset(&array, 0, size);

            _Type = Type::GetID<Array<_Ty>>();
            _Value.Array = &ptr->_Body;
        }
        else
            MXL_THROW("Dynamic allocation failed.");
    }


    //
    // Performs a "self-move" into a new mxl::Array and returns it.
    //
    template <ArrayValue _Ty>
    inline Variant::operator Array<_Ty>() &&
    {
        Array<_Ty> array = std::move(operator Array<_Ty>&());

        std::memset(this, 0, sizeof(Variant));

        return array;
    }


    //
    // Copies underlying mxl::Array into a new one and returns it.
    //
    template <ArrayValue _Ty>
    inline Variant::operator Array<_Ty>() const &
    {
        return operator const Array<_Ty>&();
    }


    //
    // Exposes a reference to the underlying mxl::Array.
    //
    template <ArrayValue _Ty>
    inline Variant::operator Array<_Ty>&()
    {
        if (IsArray())
        {
            if (IsArrayOfTypeID(Type::GetID<typename Array<_Ty>::ValueType>()))
            {
                return *reinterpret_cast<Array<_Ty>*>(
                    (std::byte*)_Value.Array - sizeof(ArrayHeader)
                );
            }

            MXL_THROW("Invalid conversion; Variant contains Array of different type");
        }
        
        MXL_THROW("Invalid conversion; Variant is not of type Array");
    }


    //
    // Exposes a const reference to the underlying mxl::Array.
    //
    template <ArrayValue _Ty>
    inline Variant::operator const Array<_Ty>&() const
    {
        return static_cast<const Array<_Ty>&>(
            const_cast<Variant*>(this)->operator Array<_Ty>&()
        );
    }


    template <Numeric _Ty>
    inline Variant::Variant(_Ty value): _Type{Type::GetID<_Ty>()}
    {
        if constexpr (Type::IsSame<_Ty, int16_t>)   _Value.Int16  = value;
        if constexpr (Type::IsSame<_Ty, int32_t>)   _Value.Int32  = value;
        if constexpr (Type::IsSame<_Ty, int64_t>)   _Value.Int64  = value;
        if constexpr (Type::IsSame<_Ty, float>)     _Value.Float  = value;
        if constexpr (Type::IsSame<_Ty, double>)    _Value.Double = value;
        if constexpr (Type::IsSame<_Ty, char16_t*>) _Value.String = value;
    }


    //
    // Exposes a reference to the underlying numeric value.
    //
    template <Numeric _Ty>
    inline Variant::operator _Ty&()
    {
        if (IsNumeric())
        {
            if (TypeID() == Type::GetID<_Ty>())
            {
                if constexpr (Type::IsSame<_Ty, int16_t>)    return _Value.Int16;
                if constexpr (Type::IsSame<_Ty, int64_t>)    return _Value.Int64;
                if constexpr (Type::IsSame<_Ty, int32_t>)    return _Value.Int32;
                if constexpr (Type::IsSame<_Ty, float>)      return _Value.Float;
                if constexpr (Type::IsSame<_Ty, double>)     return _Value.Double;
            }
            
            MXL_THROW("Invalid access by reference; Variant is of different numeric type");        
        }

        MXL_THROW("Invalid access by reference; Variant is not Numeric");
    }


    //
    // Exposes a const reference to the underlying numeric value.
    //
    template <Numeric _Ty>
    inline Variant::operator const _Ty&() const
    {
        return static_cast<const _Ty&>(
            const_cast<Variant*>(this)->operator _Ty&()
        );
    }


    //
    // Frees owned resources.
    //
    inline void Variant::Deallocate()
    {
        if (IsArray())
            Array<Variant>::Deallocate(_Value.Array);

        else if (IsString())
            String::Deallocate(_Value.String);

        _Type               = Type::ID::Empty;
        _Value.Empty        = nullptr;
    }


    inline Variant Variant::operator+(const Variant& other) const
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


    inline Variant Variant::operator-(const Variant& other) const
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


    inline Variant Variant::operator*(const Variant& other) const
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


    inline Variant Variant::operator/(const Variant& other) const
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


    inline bool Variant::operator==(const Variant& other) const
    {
        if (IsEmpty() && other.IsEmpty())
        {
            return true;
        }
        else if (IsNumeric() && other.IsNumeric())
        {
            switch (other._Type)
            {
                case Type::ID::Int16:    return operator==(other._Value.Int16);
                case Type::ID::Int32:    return operator==(other._Value.Int32);
                case Type::ID::Int64:    return operator==(other._Value.Int64);
                case Type::ID::Float:    return operator==(other._Value.Float);
                case Type::ID::Double:   return operator==(other._Value.Double);
                default: ;
            }
        }
        else if (IsString() && other.IsString())
        {
            return static_cast<const String&>(*this) == static_cast<const String&>(other);
        }

        MXL_THROW("Invalid attempt to perform comparison between incompatible Variants");
    }


    inline bool Variant::operator!=(const Variant& other) const
    {
        return !operator==(other);
    }


    inline bool Variant::operator<(const Variant& other) const
    {
        if (IsEmpty() && other.IsEmpty())
        {
            return true;
        }
        else if (IsNumeric() && other.IsNumeric())
        {
            switch (other._Type)
            {
                case Type::ID::Int16:    return operator<(other._Value.Int16);
                case Type::ID::Int32:    return operator<(other._Value.Int32);
                case Type::ID::Int64:    return operator<(other._Value.Int64);
                case Type::ID::Float:    return operator<(other._Value.Float);
                case Type::ID::Double:   return operator<(other._Value.Double);
                default: ;
            }
        }
        else if (IsString() && other.IsString())
        {
            return static_cast<const String&>(*this).Size() < static_cast<const String&>(other).Size();
        }

        MXL_THROW("Invalid attempt to perform comparison between incompatible Variants");
    }


    inline bool Variant::operator<=(const Variant& other) const
    {
        if (IsEmpty() && other.IsEmpty())
        {
            return true;
        }
        else if (IsNumeric() && other.IsNumeric())
        {
            switch (other._Type)
            {
                case Type::ID::Int16:    return operator<=(other._Value.Int16);
                case Type::ID::Int32:    return operator<=(other._Value.Int32);
                case Type::ID::Int64:    return operator<=(other._Value.Int64);
                case Type::ID::Float:    return operator<=(other._Value.Float);
                case Type::ID::Double:   return operator<=(other._Value.Double);
                default: ;
            }
        }
        else if (IsString() && other.IsString())
        {
            return static_cast<const String&>(*this).Size() <= static_cast<const String&>(other).Size();
        }

        MXL_THROW("Invalid attempt to perform comparison between incompatible Variants");
    }


    inline bool Variant::operator>(const Variant& other) const
    {
        return !operator<=(other);
    }


    inline bool Variant::operator>=(const Variant& other) const
    {
        if (IsEmpty() && other.IsEmpty())
        {
            return true;
        }
        else if (IsNumeric() && other.IsNumeric())
        {
            switch (other._Type)
            {
                case Type::ID::Int16:    return operator>=(other._Value.Int16);
                case Type::ID::Int32:    return operator>=(other._Value.Int32);
                case Type::ID::Int64:    return operator>=(other._Value.Int64);
                case Type::ID::Float:    return operator>=(other._Value.Float);
                case Type::ID::Double:   return operator>=(other._Value.Double);
                default: ;
            }
        }
        else if (IsString() && other.IsString())
        {
            return static_cast<const String&>(*this).Size() >= static_cast<const String&>(other).Size();
        }

        MXL_THROW("Invalid attempt to perform comparison between incompatible Variants");
    }


    inline Variant Variant::operator-() const
    {
        if (IsNumeric())
        {
            auto ret = *this;
            ret *= -1;
            return ret;
        }
        else if (IsEmpty())
        {
            return Variant{};
        }

        MXL_THROW("Invalid attempt to change signal of non-numeric Variant");
    }


    inline Variant& Variant::operator++()
    {
        if (IsNumeric())
        {
            switch (_Type)
            {
                case Type::ID::Int16:     _Value.Int16++;
                case Type::ID::Int32:     _Value.Int32++;
                case Type::ID::Int64:     _Value.Int64++;
                case Type::ID::Float:     _Value.Float++;
                case Type::ID::Double:    _Value.Double++;
                case Type::ID::Empty:     *this = Variant{0} + 1;
                default: ;
            }
        }
        else
        {
            MXL_THROW("Invalid attempt to pre-increment non-numeric Variant");
        }

        return *this;
    }


    inline Variant Variant::operator++(int)
    {
        auto temp = *this;
        operator++();
        return temp;
    }


    inline Variant& Variant::operator--()
    {
        if (IsNumeric())
        {
            switch (_Type)
            {
                case Type::ID::Int16:     _Value.Int16--;
                case Type::ID::Int32:     _Value.Int32--;
                case Type::ID::Int64:     _Value.Int64--;
                case Type::ID::Float:     _Value.Float--;
                case Type::ID::Double:    _Value.Double--;
                case Type::ID::Empty:     *this = Variant{0} - 1;
                default: ;
            }
        }
        else
        {
            MXL_THROW("Invalid attempt to pre-decrement non-numeric Variant");
        }

        return *this;
    }


    inline Variant Variant::operator--(int)
    {
        auto temp = *this;
        operator--();
        return temp;
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
    inline bool Variant::operator==(const _Ty value) const
    {
        switch (_Type)
        {
            case Type::ID::Int16:     return _Value.Int16  == value;
            case Type::ID::Int32:     return _Value.Int32  == value;
            case Type::ID::Int64:     return _Value.Int64  == value;
            case Type::ID::Float:     return _Value.Float  == value;
            case Type::ID::Double:    return _Value.Double == value;
            default: ;
        }

        MXL_THROW("Invalid attempt to perform comparison between a number and a non-numeric Variant");
    }


    template<Numeric _Ty>
    inline bool Variant::operator!=(const _Ty value) const
    {
        return !operator==(value);
    }


    template<Numeric _Ty>
    inline bool Variant::operator<(const _Ty value) const
    {
        switch (_Type)
        {
            case Type::ID::Int16:     return _Value.Int16  < value;
            case Type::ID::Int32:     return _Value.Int32  < value;
            case Type::ID::Int64:     return _Value.Int64  < value;
            case Type::ID::Float:     return _Value.Float  < value;
            case Type::ID::Double:    return _Value.Double < value;
            default: ;
        }

        MXL_THROW("Invalid attempt to perform comparison between a number and a non-numeric Variant");
    }


    template<Numeric _Ty>
    inline bool Variant::operator<=(const _Ty value) const
    {
        switch (_Type)
        {
            case Type::ID::Int16:     return _Value.Int16  < value;
            case Type::ID::Int32:     return _Value.Int32  < value;
            case Type::ID::Int64:     return _Value.Int64  < value;
            case Type::ID::Float:     return _Value.Float  < value;
            case Type::ID::Double:    return _Value.Double < value;
            default: ;
        }

        MXL_THROW("Invalid attempt to perform comparison between a number and a non-numeric Variant");
    }


    template<Numeric _Ty>
    inline bool Variant::operator>(const _Ty value) const
    {
        return !operator<=(value);
    }


    template<Numeric _Ty>
    inline bool Variant::operator>=(const _Ty value) const
    {
        switch (_Type)
        {
            case Type::ID::Int16:     return _Value.Int16  >= value;
            case Type::ID::Int32:     return _Value.Int32  >= value;
            case Type::ID::Int64:     return _Value.Int64  >= value;
            case Type::ID::Float:     return _Value.Float  >= value;
            case Type::ID::Double:    return _Value.Double >= value;
            default: ;
        }

        MXL_THROW("Invalid attempt to perform comparison between a number and a non-numeric Variant");
    }


    template<Numeric _Ty>
    inline _Ty operator+(const _Ty value, const Variant& var)
    {
        return value + static_cast<_Ty>(var);
    }


    template<Numeric _Ty>
    inline _Ty operator-(const _Ty value, const Variant& var)
    {
        return value + static_cast<_Ty>(var);
    }


    template<Numeric _Ty>
    inline _Ty operator*(const _Ty value, const Variant& var)
    {
        return value * static_cast<_Ty>(var);
    }


    template<Numeric _Ty>
    inline _Ty operator/(const _Ty value, const Variant& var)
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


    template<Numeric _Ty>
    inline bool operator==(_Ty value, const Variant& var)
    {
        return (value == static_cast<_Ty>(var));
    }


    template<Numeric _Ty>
    inline bool operator!=(_Ty value, const Variant& var)
    {
        return (value != static_cast<_Ty>(var));
    }


    template<Numeric _Ty>
    inline bool operator<(_Ty value, const Variant& var)
    {
        return (value < static_cast<_Ty>(var));
    }


    template<Numeric _Ty>
    inline bool operator<=(_Ty value, const Variant& var)
    {
        return (value <= static_cast<_Ty>(var));
    }


    template<Numeric _Ty>
    inline bool operator>(_Ty value, const Variant& var)
    {
        return (value > static_cast<_Ty>(var));
    }


    template<Numeric _Ty>
    inline bool operator>=(_Ty value, const Variant& var)
    {
        return (value >= static_cast<_Ty>(var));
    }


    inline std::ostream& operator<<(std::ostream &os, const Variant& var)
    {
        switch (var.TypeID())
        {
            case Type::ID::Int16:       return os << static_cast<const int16_t&>(var);
            case Type::ID::Int32:       return os << static_cast<const int32_t&>(var);
            case Type::ID::Int64:       return os << static_cast<const int64_t&>(var);
            case Type::ID::Float:       return os << static_cast<const float&>(var);
            case Type::ID::Double:      return os << static_cast<const double&>(var);
            case Type::ID::String:      return os << static_cast<const String&>(var);
            case Type::ID::Empty:       return os << "Empty";
            default: ;
        }

        return os;
    }
}