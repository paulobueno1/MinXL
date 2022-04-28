#include "String.hpp"

namespace xl
{
    String::String() : m_String(nullptr) 
    {
    }

    String::String(const char16_t* str)     
    {
        Allocate(str);
    }

    String::String(const char* str)     
    {
        Allocate(ByteToWide(str));
    }

    String::String(const String& other)
    {
        if (other.m_String)
            Allocate(other.WStr());
        
        else
            m_String = nullptr;
    }

    String::String(String&& other)
    {
        if (other.m_String)
        {
            m_String = other.m_String;
            other.m_String = nullptr;
        }
        else
            m_String = nullptr;
    }

    String& String::operator=(const String& other)
    {
        if (other.m_String)
            Allocate(other.WStr());
        else
            m_String = nullptr;
        
        return *this;
    }
    
    String& String::operator=(String&& other)
    {
        if (other.m_String)
        {
            this->m_String = other.m_String;
            other.m_String = nullptr;
        }
        else
            this->m_String = nullptr;
        
        return *this;
    }

    String::String(const std::string& str)
    {
        Allocate(ByteToWide(str.c_str()));
    }

    String::String(const std::string&& str)
    {
        Allocate(ByteToWide(str.c_str()));
    }

    String::String(const std::stringstream& str)
    {
        Allocate(ByteToWide(str.str().c_str()));
    }

    String::String(const Variant& var)
    {
        if (var.Type() != VT_BSTR)
        {
            m_String = nullptr;
        }
        else if (!var.bstrVal)
        {
            m_String = nullptr;
        }
        else
        {
            Allocate(var.bstrVal);
        }
    }

    String::String(Variant&& var)
    {
        if (var.Type() != VT_BSTR)
        {
            m_String = nullptr;
        }
        else if (!var.bstrVal)
        {
            m_String = nullptr;
        }
        else
        {
            m_String = var.bstrVal;
            std::memset(&var, 0, sizeof(var));
        }
    }
    
    String::~String() 
    {
        Deallocate(m_String);
    }

    bool String::operator==(const String& other)
    {
        return String::Compare(m_String, other.m_String);
    }

    bool String::operator!=(const String& other)
    {
        return !String::Compare(m_String, other.m_String);
    }

    void String::Allocate(const char16_t* str)
    {   
        if (!str)
            m_String = nullptr;
        else
        {
            const char16_t* s = str;
            while (*s) s++;
            const uint64_t nLength = s - str;
            const uint64_t nByteSize = nLength * sizeof(char16_t);

            // Bit witchcraft ported from WINE
            // Always allocates memory in 16-byte blocks
            uint64_t nAllocSize = (offsetof(COM::BSTR, ptr[nByteSize + sizeof(char16_t)]) + (BSTR_BUCKET_SIZE - 1)) & ~(BSTR_BUCKET_SIZE - 1);

            if (auto pBstr = static_cast<COM::BSTR*>(std::calloc(nAllocSize, 1)))
            {
                std::memcpy(pBstr->str, str, nByteSize);
                pBstr->str[nByteSize] = '\0';
                pBstr->size           = nByteSize;
                m_String              = pBstr->str;
            }
            else
                std::memset(pBstr->str, '\0', nLength + 1);
        }
    }

    void String::Deallocate(const char16_t* str)
    {
        if (str)
            std::free((char*)str - offsetof(COM::BSTR, ptr));
    }

    uint64_t String::Size() const
    {
        return ((uint32_t*)m_String)[-1] / sizeof(char16_t);
    }

    const char16_t* String::WStr() const
    {
        return m_String;
    }

    const char* String::Str() const
    {
        return WideToByte(m_String);
    }

    bool String::Compare(const char16_t* lhs, const char16_t* rhs)
    {
        const char16_t* pThis = lhs;
        const char16_t* pThat = rhs;

        if (pThis == nullptr && pThat == nullptr)
            return true;
        else if (pThis == nullptr || pThat == nullptr)
            return false;
        
        while(*pThis && *pThat)
        {
            if (*pThis != *pThat)
            {
                return false;
            }
            else
            {
                pThis++;
                pThat++;
                continue;
            }
        }

        if (*pThis == *pThat)
            return true;
        
        else
            return false;
    }

    const char* String::WideToByte(const char16_t* str)
    {
        if (!(*str))
        {
            auto empty = new char[1];
            empty[0] = '\0';
            
            return empty;
        }

        const char16_t* pCounter = str;

        while (*(++pCounter));

        int n = pCounter - str;
        char* converted = new char[n + 1];

        for (int c = 0; c < n; c++)
            converted[c] = (char)(str[c]);

        converted[n] = '\0';
        return converted;
    }

    const char16_t* String::ByteToWide(const char* str)
    {
        if (!(*str))
        {
            auto empty = new char16_t[1];
            empty[0] = '\0';
            
            return empty;
        }

        const char* pCounter = str;

        while (*(++pCounter));

        int n = pCounter - str;
        char16_t* converted = new char16_t[n + 1];

        for (int c = 0; c < n; c++)
            converted[c] = (char16_t)(str[c]);

        converted[n] = '\0';
        return converted;
    }
}