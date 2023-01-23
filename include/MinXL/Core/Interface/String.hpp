#pragma once

#include "MinXL/Core/Types.hpp"


namespace mxl
{
    struct StringHeader
    {
        uint8_t  Reserved[4];
        uint32_t Size;
    };


    struct StringContainer
    {
        StringHeader Header;
        char16_t Buffer[2];
    };


    class String
    {
        friend class Variant;

    private:
        char16_t* _Buffer;


    public:
        String();
        String(const String& other);
        String(String&& other);
        String(const char16_t* str);
        String(const char* str);
        String(const Variant& var);
        String(Variant&& var);

        String& operator=(const String& other);
        String& operator=(String&& other);

        ~String();

    private:
        String(StringContainer* str);
        StringContainer*        Container() const;


    public:
        uint64_t                Size() const;
        char16_t*               Buffer() const;
        std::unique_ptr<char[]> CStr() const;

        bool                    operator==(const String& other) const;
        bool                    operator!=(const String& other) const;

    private:
        static bool Compare(const char16_t* lhs, const char16_t* rhs);
        static std::unique_ptr<char[]> Char16to8(const char16_t* str);
        static std::unique_ptr<char16_t[]> Char8to16(const char* str);

    private:
        void                    Allocate(const char16_t* str);
        static void             Deallocate(char16_t* str);
    };


    std::ostream& operator<<(std::ostream &os, const String& str);
}