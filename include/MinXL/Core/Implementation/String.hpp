#pragma once

#include "Core/Interface/String.hpp"
#include "Core/Interface/Variant.hpp"

namespace mxl
{
    inline String::String(): _Container{nullptr}
    {
    }

    
    inline String::String(const char16_t* str)
    {
        Allocate(str);
    }

    
    inline String::String(const char* str)     
    {
        Allocate(Str8to16(str));
    }

    
    inline String::String(const String& other)
    {
        Allocate(static_cast<char16_t*>(other));
    }

    
    inline String::String(String&& other)
    {
        if (static_cast<char16_t*>(other))
        {
            _Container = other._Container;
            other._Container = nullptr;
        }
    }

    
    inline String& String::operator=(const String& other)
    {
        Deallocate(*this);

        if (auto buffer = static_cast<char16_t*>(other))
            Allocate(buffer);
        
        return *this;
    }

    
    inline String& String::operator=(String&& other)
    {
        if (other._Container)
        {
            _Container = other._Container;
            other._Container = nullptr;
        }
        else
            _Container = nullptr;
        
        return *this;
    }

    
    inline String::String(const Variant& var)
    {
        auto& value = var.Value();

        if (var.IsString())
            Allocate(value.String);
        else
            _Container = nullptr;
    }

    
    inline String::String(Variant&& var)
    {
        auto& value = var.Value();

        if (var.IsString())
        {
            _Container = reinterpret_cast<Container*>(
                (char*)value.String - sizeof(StringHeader)
            );

            var._Type = Type::ID::Empty;
            var._Value.String = nullptr;
        }
        else
            _Container = nullptr;
    }

    
    inline String& String::operator=(const Variant& var)
    {
        auto& value = var.Value();

        if (var.IsString())
            Allocate(value.String);
        else
            _Container = nullptr;
        
        return *this;
    }

    
    inline String& String::operator=(Variant&& var)
    {
        auto& value = var.Value();

        if (var.IsString())
        {
            _Container = reinterpret_cast<Container*>(
                (char*)value.String - sizeof(StringHeader)
            );

            var._Type = Type::ID::Empty;
            var._Value.String = nullptr;
        }
        else
            _Container = nullptr;
        
        return *this;
    }

    
    inline String::String(const std::string_view& str)
    {
        Allocate(Str8to16(str.data()));
    }

    
    inline String::String(const std::string& str)
    {
        Allocate(Str8to16(str.c_str()));
    }

    
    inline String::String(const std::stringstream& str)
    {
        Allocate(Str8to16(str.str().c_str()));
    }

    
    inline String::~String() 
    {
        Deallocate(*this);
    }

    
    inline bool String::operator==(const String& other)
    {
        return Compare(
            static_cast<char16_t*>(*this), static_cast<char16_t*>(other)
        );
    }

    
    inline bool String::operator!=(const String& other)
    {
        return !Compare(
            static_cast<char16_t*>(*this), static_cast<char16_t*>(other)
        );
    }

    
    inline void String::Allocate(const char16_t* str)
    {   
        if (!str)
        {
            _Container = nullptr;
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

            if ((_Container = static_cast<Container*>(std::malloc(allocSize))))
            {
                _Container->Head.Size = (length - 1) * sizeof(char16_t);
                std::copy(str, str + length, _Container->Buffer);
            }
            else
                MXL_THROW("Could not allocate String buffer");
        }
    }

    
    inline void String::Allocate(const String& str)
    {
        if (auto buffer = static_cast<char16_t*>(str))
            Allocate(buffer);
    }

    
    inline void String::Deallocate(char16_t* str)
    {
        if (auto container = reinterpret_cast<Container*>((char*)str - sizeof(StringHeader)))
        {
            std::free(container);
        }
    }

    
    inline void String::Deallocate(String& str)
    {
        if (str._Container)
        {
            std::free(str._Container);
            str._Container = nullptr;
        }
    }

    
    inline uint64_t String::Size() const
    {
        return _Container ? _Container->Head.Size / sizeof(char16_t) : 0;
    }

    
    inline const char* String::CStr() const
    {
        return Str16to8(_Container->Buffer);
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

    
    inline const char* String::Str16to8(const char16_t* str)
    {
        if (!(*str))
            return new char[1]{'\0'};

        const char16_t* end = str;

        while (*end++);

        int n = end - str;
        char* converted = new char[n];

        for (int c = 0; c < n - 1; c++)
            converted[c] = (char)(str[c]);

        converted[n - 1] = '\0';
        return converted;
    }

    
    inline const char16_t* String::Str8to16(const char* str)
    {
        if (!(*str))
        {
            return new char16_t[1]{'\0'};
        }

        const char* end = str;

        while (*end++);

        int n = end - str;
        char16_t* converted = new char16_t[n];

        for (int c = 0; c < n - 1; c++)
            converted[c] = (char16_t)(str[c]);

        converted[n - 1] = '\0';
        return converted;
    }
}