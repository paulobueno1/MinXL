#pragma once

#include <string>

#ifdef XL_DEBUG
    #define XL_STRINGIFY(x) #x
    #define XL_STRINGIFY_EXPR(x) XL_STRINGIFY(x)
    #define XL_WHERE xl::Utils::StripPath(__FILE__ ", line " XL_STRINGIFY_EXPR(__LINE__))
    #define XL_THROW(msg) throw xl::Exception(msg, XL_WHERE)
#else
    #define XL_THROW(msg) throw xl::Exception(msg)
#endif


namespace xl
{
    namespace Utils
    {
        constexpr auto StripPath(const char* path)
        {
            auto file = path;
            while (*path)
                if (*path++ == '/')
                    file = path;

            return file;
        }
    }

    class Exception : public std::exception
    {
        std::string m_What;

    public:
        Exception(const char* message, [[maybe_unused]] const char* where = nullptr)
        {
            m_What = std::string("[MinXL] Exception: ") + message;
            
            #ifdef XL_DEBUG
                m_What = m_What + "(at " + where + ")";
            #endif
        }

        const char* what() const noexcept override
        {
            return m_What.c_str();
        }
    };
}