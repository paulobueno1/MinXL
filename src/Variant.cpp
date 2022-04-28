#include "Variant.hpp"

namespace xl
{
    Variant::Variant()
    {
    }

    Variant::~Variant()
    {
        Deallocate();
    }

    Variant::Variant(const Variant& other)
    {
        if (other.IsArray())
        {
            using Array = Array<int>;
            using SafeArray = COM::SAFEARRAY;

            vt = other.Type();
            
            if (auto pDest = static_cast<Array*>(std::malloc(sizeof(Array))))
            {
                auto pSource = reinterpret_cast<Array*>((char*)other.parray - SAFEARRAY_HIDDEN_HEADER);
                std::memcpy(pDest, pSource, sizeof(Array));

                parray = reinterpret_cast<SafeArray*>((char*)pDest + SAFEARRAY_HIDDEN_HEADER);

                uint64_t nBytes = pSource->Size() * pSource->ElementSize();
                if (auto pData = std::malloc(nBytes))
                {
                    parray->pvData = pData;
                    std::memcpy(parray->pvData, pSource->Data(), nBytes);
                }
                else
                {
                    Deallocate();
                }
            }
        }
        else if (other.IsString())
        {
            String temp(other);

            vt = other.Type();
            bstrVal = temp.m_String;
            temp.m_String = nullptr;
        }
        else
        {
            std::memcpy(this, &other, sizeof(other));
        }
    }

    Variant::Variant(Variant&& other)
    {
        std::memcpy(this, &other, sizeof(other));
        std::memset(&other, 0, sizeof(other));
    }

    Variant& Variant::operator=(const Variant& other)
    {
        if (this == &other)
            return *this;

        Deallocate();

        Variant temp(other);
        std::memcpy(this, &temp, sizeof(temp));
        std::memset(&temp, 0, sizeof(temp));
        
        return *this;
    }

    Variant& Variant::operator=(Variant&& other)
    {
        if (this == &other)
            return *this;

        Deallocate();

        std::memcpy(this, &other, sizeof(other));
        std::memset(&other, 0, sizeof(other));

        return *this;
    }

    Variant::Variant(const String& value) 
    {
        if (value.WStr())
        {
            String temp(value);

            vt = VT_BSTR;
            bstrVal = temp.m_String;
            temp.m_String = nullptr;
        }
        else
        {
            vt = VT_EMPTY;
            bstrVal = nullptr;
        }
    }

    Variant::Variant(String&& value) 
    {
        if (value.WStr())
        {
            vt = VT_BSTR;
            bstrVal = value.m_String;
            value.m_String = nullptr;
        }
        else
        {
            vt = VT_EMPTY;
            bstrVal = nullptr;
        }
    }

    Variant& Variant::operator=(const String& value) 
    {
        if (value.WStr())
        {
            Deallocate();
            String temp(value);

            vt = VT_BSTR;
            bstrVal = temp.m_String;
            temp.m_String = nullptr;
        }
        else
        {
            vt = VT_EMPTY;
            bstrVal = nullptr;
        }

        return *this;
    }

    Variant& Variant::operator=(String&& value) 
    {
        if (value.WStr())
        {
            Deallocate();
            vt = VT_BSTR;
            bstrVal = value.m_String;
            value.m_String = nullptr;
        }
        else
        {
            vt = VT_EMPTY;
            bstrVal = nullptr;
        }

        return *this;
    }

    template<typename _Ty> 
    Variant::Variant(const Array<_Ty>& value)
    {
        using Array = Array<_Ty>;
        using SafeArray = COM::SAFEARRAY;

        constexpr auto nDataType = COM::TypeConstant<_Ty>();
        if constexpr (nDataType != 0)
        {
            vt = VT_ARRAY | nDataType;

            if (auto p = static_cast<Array*>(std::malloc(sizeof(Array))))
            {
                Array temp(value);
                std::memcpy(p, &temp, sizeof(temp));
                std::memset(&temp, 0, sizeof(temp));

                parray = reinterpret_cast<SafeArray*>((char*)p + SAFEARRAY_HIDDEN_HEADER);
                return;
            }
        }

        vt = VT_EMPTY;
        parray = nullptr;
    }

    template<typename _Ty>
    Variant::Variant(Array<_Ty>&& value)
    {
        using Array = Array<_Ty>;
        using SafeArray = COM::SAFEARRAY;

        constexpr auto nDataType = COM::TypeConstant<_Ty>();
        if constexpr (nDataType != 0)
        {
            vt = VT_ARRAY | nDataType;

            if (auto p = static_cast<Array*>(std::malloc(sizeof(Array))))
            {
                std::memcpy(p, &value, sizeof(value));
                std::memset(&value, 0, sizeof(value));

                parray = reinterpret_cast<SafeArray*>((char*)p + SAFEARRAY_HIDDEN_HEADER);
                return;
            }
        }

        parray = nullptr;
        vt = VT_EMPTY;
    }

    template<typename _Ty> 
    Variant& Variant::operator=(const Array<_Ty>& value)
    {
        using Array = Array<_Ty>;
        using SafeArray = COM::SAFEARRAY;

        constexpr auto nDataType = COM::TypeConstant<_Ty>();
        if constexpr (nDataType != 0)
        {
            Deallocate();
            vt = VT_ARRAY | nDataType;

            if (auto p = static_cast<Array*>(std::malloc(sizeof(Array))))
            {
                Array temp(value);
                std::memcpy(p, &temp, sizeof(temp));
                std::memset(&temp, 0, sizeof(temp));

                parray = reinterpret_cast<SafeArray*>((char*)p + SAFEARRAY_HIDDEN_HEADER);
                return *this;
            }
        }

        parray = nullptr;
        vt = VT_EMPTY;
        return *this;
    }

    template<typename _Ty> 
    Variant& Variant::operator=(Array<_Ty>&& value)
    {
        using Array = Array<_Ty>;
        using SafeArray = COM::SAFEARRAY;

        constexpr auto nDataType = COM::TypeConstant<_Ty>();
        if constexpr (nDataType != 0)
        {
            Deallocate();
            vt = VT_ARRAY | nDataType;

            if (auto p = static_cast<Array*>(std::malloc(sizeof(Array))))
            {
                std::memcpy(p, &value, sizeof(value));
                std::memset(&value, 0, sizeof(value));

                parray = reinterpret_cast<SafeArray*>((char*)p + SAFEARRAY_HIDDEN_HEADER);
                return *this;
            }
        }

        parray = nullptr;
        vt = VT_EMPTY;
        return *this;
    }

    uint16_t Variant::Type() const
    {
        return vt;
    }

    bool Variant::IsNumeric() const
    {   
        return COM::IsTypeNumeric(vt);
    }

    bool Variant::IsString() const
    {   
        return vt == VT_BSTR;
    }

    bool Variant::IsDate() const
    {   
        return vt == VT_DATE;
    }

    bool Variant::IsArray() const
    {   
        return vt & VT_ARRAY;
    }

    //
    // Checks if array is of given type.
    //
    bool Variant::IsArrayOf(uint16_t type) const
    {
        if (IsArray())
            return type == (vt ^ VT_ARRAY);

        return false;
    }

    //
    // Returns type constant of underlying array.
    //
    uint16_t Variant::IsArrayOf() const
    {
        if (IsArray())
            return vt ^ VT_ARRAY;

        return VT_EMPTY;
    }

    bool Variant::IsEmpty() const
    {
        return vt == VT_EMPTY;
    }

    void Variant::Deallocate()
    {
        if (IsArray() && parray)
            Array<int>::Deallocate(parray);

        else if (IsString())
            String::Deallocate(bstrVal);
        
        std::memset(this, 0, sizeof(*this));
    }

    Variant::Variant(short value)                  { vt = VT_SHORT;    iVal = value;     }
    Variant::Variant(int value)                    { vt = VT_INTEGER;  intVal = value;   }
    Variant::Variant(long value)                   { vt = VT_LONG;     llVal = value;    }
    Variant::Variant(long long value)              { vt = VT_LONG;     llVal = value;    }
    Variant::Variant(float value)                  { vt = VT_FLOAT;    fltVal = value;   }
    Variant::Variant(double value)                 { vt = VT_DOUBLE;   dblVal = value;   }

    Variant& Variant::operator=(short value)       { vt = VT_SHORT;    iVal = value;    return *this; }
    Variant& Variant::operator=(int value)         { vt = VT_INTEGER;  intVal = value;  return *this; }
    Variant& Variant::operator=(long value)        { vt = VT_LONG;     llVal = value;   return *this; }
    Variant& Variant::operator=(long long value)   { vt = VT_LONG;     llVal = value;   return *this; }
    Variant& Variant::operator=(float value)       { vt = VT_FLOAT;    fltVal = value;  return *this; }
    Variant& Variant::operator=(double value)      { vt = VT_DOUBLE;   dblVal = value;  return *this; }

    Variant::operator short()
    {
        switch (vt)
        {
            case VT_SHORT:      return (short)iVal;
            case VT_INTEGER:    return (short)intVal;
            case VT_LONG:       return (short)llVal;
            case VT_FLOAT:      return (short)fltVal;
            case VT_DOUBLE:     return (short)dblVal;
            case VT_EMPTY:      return 0;
        }

        XL_THROW("Invalid attempt to cast non-numeric xl::Variant into short");
    }

    Variant::operator int()
    {
        switch (vt)
        {
            case VT_SHORT:      return (int)iVal;
            case VT_INTEGER:    return (int)intVal;
            case VT_LONG:       return (int)llVal;
            case VT_FLOAT:      return (int)fltVal;
            case VT_DOUBLE:     return (int)dblVal;
            case VT_EMPTY:      return 0;
        }

        XL_THROW("Invalid attempt to cast non-numeric xl::Variant into int");
    }

    Variant::operator long()
    {
        switch (vt)
        {
            case VT_SHORT:      return (long)iVal;
            case VT_INTEGER:    return (long)intVal;
            case VT_LONG:       return (long)llVal;
            case VT_FLOAT:      return (long)fltVal;
            case VT_DOUBLE:     return (long)dblVal;
            case VT_EMPTY:      return 0l;
        }

        XL_THROW("Invalid attempt to cast non-numeric xl::Variant into long");
    }

    Variant::operator float()
    {
        switch (vt)
        {
            case VT_SHORT:      return (float)iVal;
            case VT_INTEGER:    return (float)intVal;
            case VT_LONG:       return (float)llVal;
            case VT_FLOAT:      return (float)fltVal;
            case VT_DOUBLE:     return (float)dblVal;
            case VT_EMPTY:      return 0.0f;
        }

        XL_THROW("Invalid attempt to cast non-numeric xl::Variant into float");
    }

    Variant::operator double()
    {
        switch (vt)
        {
            case VT_SHORT:      return (double)iVal;
            case VT_INTEGER:    return (double)intVal;
            case VT_LONG:       return (double)llVal;
            case VT_FLOAT:      return (double)fltVal;
            case VT_DOUBLE:     return (double)dblVal;
            case VT_EMPTY:      return 0.0;
        }

        XL_THROW("Invalid attempt to cast non-numeric xl::Variant into double");
    }

    template<typename _Ty>
    Variant::operator xl::Array<_Ty>&()
    {
        constexpr auto nTargetType = COM::TypeConstant<_Ty>();

        if (nTargetType == IsArrayOf())
        {
            return *reinterpret_cast<xl::Array<_Ty>*>((char*)parray - SAFEARRAY_HIDDEN_HEADER);
        }
        else
        {
            XL_THROW("Invalid attempt to cast xl::Variant into xl::Array reference; type mismatch detected");
        }
        
    }

    template<typename _Ty>
    Variant Variant::operator+(const _Ty& value) const
    {
        constexpr bool bIsValueNumeric = COM::IsTypeNumeric(COM::TypeConstant<_Ty>());

        if constexpr (bIsValueNumeric)
        {
            switch (vt)
            {
                case VT_SHORT:      return Variant((short)  (iVal   + value));
                case VT_INTEGER:    return Variant((int)    (intVal + value));
                case VT_LONG:       return Variant((long)   (llVal  + value));
                case VT_FLOAT:      return Variant((float)  (fltVal + value));
                case VT_DOUBLE:     return Variant((double) (dblVal + value));
                case VT_EMPTY:      return Variant(value);
            }
        }

        if constexpr (!bIsValueNumeric)
            XL_THROW("Invalid attempt to add xl::Variant to non-numeric type");
        else
            XL_THROW("Invalid attempt to add non-numeric xl::Variant");
    }

    template<>
    Variant Variant::operator+(const Variant& other) const
    {
        if (this->IsNumeric())
        {
            switch (other.vt)
            {
                case VT_SHORT:      return Variant::operator+(other.iVal);
                case VT_INTEGER:    return Variant::operator+(other.lVal);
                case VT_LONG:       return Variant::operator+(other.llVal);
                case VT_FLOAT:      return Variant::operator+(other.fltVal);
                case VT_DOUBLE:     return Variant::operator+(other.dblVal);
                case VT_EMPTY:      return *this;
            }
        }
        else if (this->IsEmpty())
        {
            return other;
        }

        XL_THROW("Invalid attempt to add non-numeric xl::Variant(s)");
    }

    template<typename _Ty>
    Variant Variant::operator-(const _Ty& value) const
    {
        constexpr bool bIsValueNumeric = COM::IsTypeNumeric(COM::TypeConstant<_Ty>());

        if constexpr (bIsValueNumeric)
        {
            switch (vt)
            {
                case VT_SHORT:      return Variant((short)  (iVal   - value ));
                case VT_INTEGER:    return Variant((int)    (intVal - value ));
                case VT_LONG:       return Variant((long)   (llVal  - value ));
                case VT_FLOAT:      return Variant((float)  (fltVal - value ));
                case VT_DOUBLE:     return Variant((double) (dblVal - value ));
                case VT_EMPTY:      return Variant(-value);
            }
        }

        if constexpr (!bIsValueNumeric)
            XL_THROW("Invalid attempt to subtract xl::Variant to non-numeric type");
        else
            XL_THROW("Invalid attempt to subtract non-numeric xl::Variant");
    }

    template<>
    Variant Variant::operator-(const Variant& other) const
    {
        if (this->IsNumeric())
        {
            switch (other.vt)
            {
                case VT_SHORT:      return Variant::operator-(other.iVal);
                case VT_INTEGER:    return Variant::operator-(other.lVal);
                case VT_LONG:       return Variant::operator-(other.llVal);
                case VT_FLOAT:      return Variant::operator-(other.fltVal);
                case VT_DOUBLE:     return Variant::operator-(other.dblVal);
                case VT_EMPTY:      return *this;
            }
        }
        else if (this->IsEmpty())
        {
            return Variant(other) *= -1;
        }

        XL_THROW("Invalid attempt to subtract non-numeric xl::Variant(s)");
    }

    template<typename _Ty>
    Variant Variant::operator*(const _Ty& value) const
    {
        constexpr bool bIsValueNumeric = COM::IsTypeNumeric(COM::TypeConstant<_Ty>());

        if constexpr (bIsValueNumeric)
        {
            switch (vt)
            {
                case VT_SHORT:      return Variant((short)  (iVal   * value ));
                case VT_INTEGER:    return Variant((int)    (intVal * value ));
                case VT_LONG:       return Variant((long)   (llVal  * value ));
                case VT_FLOAT:      return Variant((float)  (fltVal * value ));
                case VT_DOUBLE:     return Variant((double) (dblVal * value ));
                case VT_EMPTY:      return Variant(0);
            }
        }

        if constexpr (!bIsValueNumeric)
            XL_THROW("Invalid attempt to multiply xl::Variant to non-numeric type");
        else
            XL_THROW("Invalid attempt to multiply non-numeric xl::Variant");
    }

    template<>
    Variant Variant::operator*(const Variant& other) const
    {
        if (this->IsNumeric())
        {
            switch (other.vt)
            {
                case VT_SHORT:      return Variant::operator*(other.iVal);
                case VT_INTEGER:    return Variant::operator*(other.lVal);
                case VT_LONG:       return Variant::operator*(other.llVal);
                case VT_FLOAT:      return Variant::operator*(other.fltVal);
                case VT_DOUBLE:     return Variant::operator*(other.dblVal);
                case VT_EMPTY:      return Variant(0);
            }
        }
        else if (this->IsEmpty())
        {
            return Variant(0);
        }

        XL_THROW("Invalid attempt to multiply non-numeric xl::Variant(s)");
    }

    template<typename _Ty>
    Variant Variant::operator/(const _Ty& value) const
    {
        constexpr bool bIsValueNumeric = COM::IsTypeNumeric(COM::TypeConstant<_Ty>());

        if constexpr (bIsValueNumeric)
        {
            switch (vt)
            {
                case VT_SHORT:      return Variant((short)  (iVal   / value ));
                case VT_INTEGER:    return Variant((int)    (intVal / value ));
                case VT_LONG:       return Variant((long)   (llVal  / value ));
                case VT_FLOAT:      return Variant((float)  (fltVal / value ));
                case VT_DOUBLE:     return Variant((double) (dblVal / value ));
                case VT_EMPTY:      return Variant(0 / value);
            }
        }

        if constexpr (!bIsValueNumeric)
            XL_THROW("Invalid attempt to divide xl::Variant to non-numeric type");
        else
            XL_THROW("Invalid attempt to divide non-numeric xl::Variant");
    }

    template<>
    Variant Variant::operator/(const Variant& other) const
    {
        if (this->IsNumeric())
        {
            switch (other.vt)
            {
                case VT_SHORT:      return Variant::operator/(other.iVal);
                case VT_INTEGER:    return Variant::operator/(other.lVal);
                case VT_LONG:       return Variant::operator/(other.llVal);
                case VT_FLOAT:      return Variant::operator/(other.fltVal);
                case VT_DOUBLE:     return Variant::operator/(other.dblVal);
                case VT_EMPTY:      XL_THROW("Division by empty variant");
            }
        }
        else if (this->IsEmpty())
        {
            return Variant(0);
        }

        XL_THROW("Invalid attempt to divide non-numeric xl::Variant(s)");
    }

    template<typename _Ty>
    Variant& Variant::operator+=(const _Ty& value)
    {
        constexpr bool bIsValueNumeric = COM::IsTypeNumeric(COM::TypeConstant<_Ty>());

        if constexpr (bIsValueNumeric)
        {
            switch (vt)
            {
                case VT_SHORT:      iVal   += value;        return *this;
                case VT_INTEGER:    intVal += value;        return *this;
                case VT_LONG:       llVal  += value;        return *this;
                case VT_FLOAT:      fltVal += value;        return *this;
                case VT_DOUBLE:     dblVal += value;        return *this;
                case VT_EMPTY:      *this = Variant{value}; return *this;
            }
        }

        if constexpr (!bIsValueNumeric)
            XL_THROW("Invalid attempt to add xl::Variant to non-numeric type");
        else
            XL_THROW("Invalid attempt to add non-numeric xl::Variant");
    }

    template<>
    Variant& Variant::operator+=(const Variant& other)
    {
        if (this->IsNumeric())
        {
            switch (other.vt)
            {
                case VT_SHORT:      Variant::operator+=(other.iVal);     return *this;
                case VT_INTEGER:    Variant::operator+=(other.lVal);     return *this;
                case VT_LONG:       Variant::operator+=(other.llVal);    return *this;
                case VT_FLOAT:      Variant::operator+=(other.fltVal);   return *this;
                case VT_DOUBLE:     Variant::operator+=(other.dblVal);   return *this;
                case VT_EMPTY:                                           return *this;
            }
        }
        else if (this->IsEmpty())
        {
            *this = other;
            return *this;
        }

        XL_THROW("Invalid attempt to add non-numeric xl::Variant(s)");
    }

    template<typename _Ty>
    Variant& Variant::operator-=(const _Ty& value)
    {
        constexpr bool bIsValueNumeric = COM::IsTypeNumeric(COM::TypeConstant<_Ty>());

        if constexpr (bIsValueNumeric)
        {
            switch (vt)
            {
                case VT_SHORT:   iVal   -= value;         return *this;
                case VT_INTEGER: intVal -= value;         return *this;
                case VT_LONG:    llVal  -= value;         return *this;
                case VT_FLOAT:   fltVal -= value;         return *this;
                case VT_DOUBLE:  dblVal -= value;         return *this;
                case VT_EMPTY:   *this = Variant{-value}; return *this;
            }
        }

        if constexpr (!bIsValueNumeric)
            XL_THROW("Invalid attempt to subtract xl::Variant to non-numeric type");
        else
            XL_THROW("Invalid attempt to subtract non-numeric xl::Variant");
    }

    template<>
    Variant& Variant::operator-=(const Variant& other)
    {
        if (this->IsNumeric())
        {
            switch (other.vt)
            {
                case VT_SHORT:      Variant::operator-=(other.iVal);     return *this;
                case VT_INTEGER:    Variant::operator-=(other.lVal);     return *this;
                case VT_LONG:       Variant::operator-=(other.llVal);    return *this;
                case VT_FLOAT:      Variant::operator-=(other.fltVal);   return *this;
                case VT_DOUBLE:     Variant::operator-=(other.dblVal);   return *this;
                case VT_EMPTY:                                           return *this;
            }
        }
        else if (this->IsEmpty())
        {
            *this = other;
            return *this;
        }

        XL_THROW("Invalid attempt to subtract non-numeric xl::Variant(s)");
    }

    template<typename _Ty>
    Variant& Variant::operator*=(const _Ty& value)
    {
        constexpr bool bIsValueNumeric = COM::IsTypeNumeric(COM::TypeConstant<_Ty>());

        if constexpr (bIsValueNumeric)
        {
            switch (vt)
            {
                case VT_SHORT:      iVal   *= value;         return *this;
                case VT_INTEGER:    intVal *= value;         return *this;
                case VT_LONG:       llVal  *= value;         return *this;
                case VT_FLOAT:      fltVal *= value;         return *this;
                case VT_DOUBLE:     dblVal *= value;         return *this;
                case VT_EMPTY:      *this = Variant{0};      return *this;
            }
        }

        if constexpr (!bIsValueNumeric)
            XL_THROW("Invalid attempt to multiply xl::Variant to non-numeric type");
        else
            XL_THROW("Invalid attempt to multiply non-numeric xl::Variant");
    }

    template<>
    Variant& Variant::operator*=(const Variant& other)
    {
        if (this->IsNumeric())
        {
            switch (other.vt)
            {
                case VT_SHORT:      Variant::operator*=(other.iVal);     return *this;
                case VT_INTEGER:    Variant::operator*=(other.lVal);     return *this;
                case VT_LONG:       Variant::operator*=(other.llVal);    return *this;
                case VT_FLOAT:      Variant::operator*=(other.fltVal);   return *this;
                case VT_DOUBLE:     Variant::operator*=(other.dblVal);   return *this;
                case VT_EMPTY:      Variant::operator*=(0);              return *this;
            }
        }
        else if (this->IsEmpty())
        {
            *this = Variant{0};
            return *this;
        }

        XL_THROW("Invalid attempt to multiply non-numeric xl::Variant(s)");
    }

    template<typename _Ty>
    Variant& Variant::operator/=(const _Ty& value)
    {
        constexpr bool bIsValueNumeric = COM::IsTypeNumeric(COM::TypeConstant<_Ty>());

        if constexpr (bIsValueNumeric)
        {
            switch (vt)
            {
                case VT_SHORT:      iVal   /= value;    return *this;
                case VT_INTEGER:    intVal /= value;    return *this;
                case VT_LONG:       llVal  /= value;    return *this;
                case VT_FLOAT:      fltVal /= value;    return *this;
                case VT_DOUBLE:     dblVal /= value;    return *this;
                case VT_EMPTY:      *this = Variant{0}; return *this;
            }
        }

        if constexpr (!bIsValueNumeric)
            XL_THROW("Invalid attempt to divide xl::Variant to non-numeric type");
        else
            XL_THROW("Invalid attempt to divide non-numeric xl::Variant");
    }

    template<>
    Variant& Variant::operator/=(const Variant& other)
    {
        if (this->IsNumeric())
        {
            switch (other.vt)
            {
                case VT_SHORT:      Variant::operator/=(other.iVal);     return *this;
                case VT_INTEGER:    Variant::operator/=(other.lVal);     return *this;
                case VT_LONG:       Variant::operator/=(other.llVal);    return *this;
                case VT_FLOAT:      Variant::operator/=(other.fltVal);   return *this;
                case VT_DOUBLE:     Variant::operator/=(other.dblVal);   return *this;
            }
        }
        else if (this->IsEmpty())
        {
            *this = Variant{0};
            return *this;
        }

        XL_THROW("Invalid attempt to divide non-numeric xl::Variant(s)");
    }

    std::ostream& operator<<(std::ostream &os, const Variant& v) 
    {
        switch (v.Type())
        {
            case VT_SHORT:      return os << v.iVal;
            case VT_INTEGER:    return os << v.intVal;
            case VT_LONG:       return os << v.llVal;
            case VT_FLOAT:      return os << v.fltVal;
            case VT_DOUBLE:     return os << v.dblVal;
            case VT_STRING:     return os << String::WideToByte(v.bstrVal);
        }

        return os;
    }

    // Explicit instantiations
    template Variant::Variant(const Array<short>& value);
    template Variant::Variant(const Array<int>& value);
    template Variant::Variant(const Array<long>& value);
    template Variant::Variant(const Array<long long>& value);
    template Variant::Variant(const Array<float>& value);
    template Variant::Variant(const Array<double>& value);
    template Variant::Variant(const Array<Variant>& value);

    template Variant::Variant(Array<short>&& value);
    template Variant::Variant(Array<int>&& value);
    template Variant::Variant(Array<long>&& value);
    template Variant::Variant(Array<long long>&& value);
    template Variant::Variant(Array<float>&& value);
    template Variant::Variant(Array<double>&& value);
    template Variant::Variant(Array<Variant>&& value);

    template Variant::operator xl::Array<short>&();
    template Variant::operator xl::Array<int>&();
    template Variant::operator xl::Array<long>&();
    template Variant::operator xl::Array<long long>&();
    template Variant::operator xl::Array<float>&();
    template Variant::operator xl::Array<double>&();
    template Variant::operator xl::Array<Variant>&();

    template Variant& Variant::operator=(const Array<short>& value);
    template Variant& Variant::operator=(const Array<int>& value);
    template Variant& Variant::operator=(const Array<long>& value);
    template Variant& Variant::operator=(const Array<long long>& value);
    template Variant& Variant::operator=(const Array<float>& value);
    template Variant& Variant::operator=(const Array<double>& value);
    template Variant& Variant::operator=(const Array<Variant>& value);

    template Variant& Variant::operator=(Array<short>&& value);
    template Variant& Variant::operator=(Array<int>&& value);
    template Variant& Variant::operator=(Array<long>&& value);
    template Variant& Variant::operator=(Array<long long>&& value);
    template Variant& Variant::operator=(Array<float>&& value);
    template Variant& Variant::operator=(Array<double>&& value);
    template Variant& Variant::operator=(Array<Variant>&& value);

    template Variant Variant::operator+(const short& value) const;
    template Variant Variant::operator+(const int& value) const;
    template Variant Variant::operator+(const long& value) const;
    template Variant Variant::operator+(const long long& value) const;
    template Variant Variant::operator+(const float& value) const;
    template Variant Variant::operator+(const double& value) const;

    template Variant Variant::operator-(const short& value) const;
    template Variant Variant::operator-(const int& value) const;
    template Variant Variant::operator-(const long& value) const;
    template Variant Variant::operator-(const long long& value) const;
    template Variant Variant::operator-(const float& value) const;
    template Variant Variant::operator-(const double& value) const;

    template Variant Variant::operator*(const short& value) const;
    template Variant Variant::operator*(const int& value) const;
    template Variant Variant::operator*(const long& value) const;
    template Variant Variant::operator*(const long long& value) const;
    template Variant Variant::operator*(const float& value) const;
    template Variant Variant::operator*(const double& value) const;

    template Variant Variant::operator/(const short& value) const;
    template Variant Variant::operator/(const int& value) const;
    template Variant Variant::operator/(const long& value) const;
    template Variant Variant::operator/(const long long& value) const;
    template Variant Variant::operator/(const float& value) const;
    template Variant Variant::operator/(const double& value) const;

    template Variant& Variant::operator+=(const short& value);
    template Variant& Variant::operator+=(const int& value);
    template Variant& Variant::operator+=(const long& value);
    template Variant& Variant::operator+=(const long long& value);
    template Variant& Variant::operator+=(const float& value);
    template Variant& Variant::operator+=(const double& value);

    template Variant& Variant::operator-=(const short& value);
    template Variant& Variant::operator-=(const int& value);
    template Variant& Variant::operator-=(const long& value);
    template Variant& Variant::operator-=(const long long& value);
    template Variant& Variant::operator-=(const float& value);
    template Variant& Variant::operator-=(const double& value);

    template Variant& Variant::operator*=(const short& value);
    template Variant& Variant::operator*=(const int& value);
    template Variant& Variant::operator*=(const long& value);
    template Variant& Variant::operator*=(const long long& value);
    template Variant& Variant::operator*=(const float& value);
    template Variant& Variant::operator*=(const double& value);
    
    template Variant& Variant::operator/=(const short& value);
    template Variant& Variant::operator/=(const int& value);
    template Variant& Variant::operator/=(const long& value);
    template Variant& Variant::operator/=(const long long& value);
    template Variant& Variant::operator/=(const float& value);
    template Variant& Variant::operator/=(const double& value);
}