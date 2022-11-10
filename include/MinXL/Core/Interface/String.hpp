#pragma once

#include "Core/Common.hpp"
#include "Core/Types.hpp"


namespace mxl
{
    struct StringHeader
    {
        uint8_t  Reserved[4];
        uint32_t Size;
    };

    struct StringContainer
    {
        StringHeader    Head;
        char16_t        Buffer[2];
    };


    class String
    {
        friend class Variant;
        
    public:
        using Container = StringContainer;

    private:
        Container* _Container{nullptr};

    public:
        String();
        String(const char16_t* str);
        String(const char* str);
        String(const String& other);
        String(const std::string& str);
        String(const std::string_view& str);
        String(const std::stringstream& str);

        String(String&& other);
        
        String& operator=(const String& other);
        String& operator=(String&& other);

        String(const Variant& var);
        String(Variant&& var);
        String& operator=(const Variant& other);
        String& operator=(Variant&& other);

        bool operator==(const String& other);
        bool operator!=(const String& other);

        inline operator char16_t*() const { return _Container ? _Container->Buffer : nullptr; }

        ~String();

    private:
        void                    Allocate(const char16_t* str);
        void                    Allocate(const String& str);
        static void             Deallocate(char16_t* str);
        static void             Deallocate(String& str);

    public:
        uint64_t                Size() const;
        const char*             CStr() const;
        const char16_t*         WStr() const;

        static bool             Compare(const char16_t* lhs, const char16_t* rhs);
        static const char*      Str16to8(const char16_t* str);
        static const char16_t*  Str8to16(const char* str);
    };
}