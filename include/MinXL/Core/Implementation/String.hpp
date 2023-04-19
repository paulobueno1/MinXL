#pragma once

#include "MinXL/Core/Types.hpp"

#include "MinXL/Core/Interface/String.hpp"
#include "MinXL/Core/Interface/Variant.hpp"


namespace mxl
{
    inline String::String(): _Buffer{nullptr}
    {
    }


    inline String::String(const char16_t* str)
    {
        Allocate(str);
    }


    inline String::String(StringContainer* str): _Buffer{str->Buffer}
    {
    }


    inline String::String(const char* str): String(Char8to16(str).get())
    {
    }


    inline String::String(const String& other): String(other.Buffer())
    {
    }


    inline String::String(String&& other): _Buffer{other._Buffer}
    {
        std::memset(&other, 0, sizeof(String));
    }


    inline String& String::operator=(const String& other)
    {
        if (this == &other)
            return *this;

        Deallocate(Buffer());
        Allocate(other.Buffer());
        
        return *this;
    }


    inline String& String::operator=(String&& other)
    {
        if (this == &other)
            return *this;

        Deallocate(Buffer());

        std::memcpy(this, &other, sizeof(String));
        std::memset(&other, 0, sizeof(String));
        
        return *this;
    }


    inline String::String(const Variant& var): String(var.operator const String&())
    {
    }


    inline String::String(Variant&& var): String(std::move(var).operator String())
    {
    }


    inline String::~String() 
    {
        Deallocate(Buffer());
    }


    inline bool String::operator==(const String& other) const
    {
        return Compare(Buffer(), other.Buffer());
    }


    inline bool String::operator!=(const String& other) const
    {
        return !Compare(Buffer(), other.Buffer());
    }


    inline uint64_t String::Size() const
    {
        return _Buffer ? Container()->Header.Size : 0;
    }


    inline char16_t* String::Buffer() const
    {
        return _Buffer;
    }


    inline std::unique_ptr<char[]> String::CStr() const
    {
        return Char16to8(Buffer());
    }


    inline StringContainer* String::Container() const
    {
        return reinterpret_cast<StringContainer*>(
            (std::byte*)_Buffer - sizeof(StringHeader)
        );
    }


    inline void String::Allocate(const char16_t* str)
    {   
        if (!str)
        {
            _Buffer = nullptr;
        }
        else
        {
            const char16_t* end = str;
            
            // Count characters, including null-termination
            while (*end++);

            const uint64_t length = end - str;

            uint64_t allocSize = sizeof(StringHeader) + length * sizeof(char16_t);

            // Always allocate in blocks of 16 bytes
            if (allocSize % 16 > 0)
                allocSize += (16 - allocSize % 16);

            if (auto container = static_cast<StringContainer*>(std::malloc(allocSize)))
            {
                container->Header.Size = (length - 1);
                std::copy(str, str + length, container->Buffer);
                
                _Buffer = container->Buffer;
            }
            else
            {
                MXL_THROW("Dynamic allocation failed.");
            }
        }
    }


    inline void String::Deallocate(char16_t* str)
    {
        if (str)
            std::free((std::byte*)str - sizeof(StringHeader));
    }


    inline bool String::Compare(const char16_t* lhs, const char16_t* rhs)
    {
        if (lhs == nullptr && rhs == nullptr)
            return true;
        else if (lhs == nullptr || rhs == nullptr)
            return false;
        
        while(*lhs && *rhs)
        {
            if (*lhs != *rhs)
            {
                return false;
            }
            else
            {
                lhs++;
                rhs++;
                continue;
            }
        }

        if (*lhs == *rhs)
            return true;
        
        else
            return false;
    }


    inline std::unique_ptr<char[]> String::Char16to8(const char16_t* str)
    {
        if (!(*str))
            return std::unique_ptr<char[]>(new char[1]{'\0'});

        const char16_t* end = str;

        while (*end++);

        int n = end - str;
        auto converted = std::unique_ptr<char[]>(new char[n]{'\0'});

        for (int c = 0; c < n - 1; c++)
            converted[c] = (char)(str[c]);

        return converted;
    }


    inline std::unique_ptr<char16_t[]> String::Char8to16(const char* str)
    {
        if (!(*str))
        {
            return std::unique_ptr<char16_t[]>(new char16_t[1]{'\0'});
        }

        const char* end = str;

        while (*end++);

        int n = end - str;
        auto converted = std::unique_ptr<char16_t[]>(new char16_t[n]{'\0'});

        for (int c = 0; c < n - 1; c++)
            converted[c] = (char16_t)(str[c]);

        return converted;
    }

    inline std::ostream& operator<<(std::ostream &os, const String& str)
    {
        return os << str.CStr();
    }
}