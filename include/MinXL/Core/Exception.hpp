#pragma once

#include <string_view>

// Stringiy
#define MXL_STRINGIFY(x) #x
#define MXL_STRINGIFY_EXPR(x) MXL_STRINGIFY(x)

// Location within source code (without file full path)
#define MXL_WHERE Detail::StripPath("file " __FILE__ ", line " MXL_STRINGIFY_EXPR(__LINE__))

// Exception throw
#define MXL_THROW(msg) throw mxl::Exception{msg, MXL_WHERE}


namespace mxl
{
    namespace Detail
    {
        consteval auto StripPath(std::string_view path)
        {
            return path.substr(path.find("src/") + sizeof("src/") - 1).data();
        }
    }


    class Exception: public std::exception
    {
        char* m_What;

    public:
        Exception(const char* message, const char* file_line)
        {
            auto nLength = std::strlen(message) + std::strlen(file_line) + 32;
            m_What = new char[nLength];

            std::snprintf(m_What, nLength, "[MinXL] Exception: %s (at %s)", message, file_line);
        }

        const char* what() const noexcept override
        {
            return m_What;
        }
    };
}