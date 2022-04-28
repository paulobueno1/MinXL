#pragma once

#include "Common.hpp"

namespace xl
{
    class Variant;

    class String
    {
        friend class Variant;
    
    private:
        char16_t* m_String;

    public:
        String();
        String(const char16_t* str);
        String(const char* str);

        String(const String& other);
        String(String&& other);
        
        String& operator=(const String& other);
        String& operator=(String&& other);

        String(const std::string& str);
        String(const std::string&& str);
        String(const std::stringstream& str);

        String(const Variant& var);
        String(Variant&& var);

        bool operator==(const String& other);
        bool operator!=(const String& other);

        ~String();

    private:
        void Allocate(const char16_t* str);
        static void Deallocate(const char16_t* str);
    
    public:
        uint64_t Size() const;
        const char* Str() const;
        const char16_t* WStr() const;

        static bool Compare(const char16_t* lhs, const char16_t* rhs);
        static const char* WideToByte(const char16_t* str);
        static const char16_t* ByteToWide(const char* str);
    };
}