#pragma once

#include "Common.hpp"

namespace mxl
{
    class Variant;
    class String;
    struct StringContainer;

    namespace Type
    {
        namespace Detail
        {
            template<typename _T1, typename _T2>
            struct IsSameType {
                static constexpr bool value = std::is_same_v<
                    std::remove_const_t<_T1>, std::remove_const_t<_T2>
                >;
            };
        };

        // Checks if two types are equal. Ignores const.
        template <typename _T1, typename _T2> 
        inline constexpr bool IsSame = Detail::IsSameType<_T1, _T2>::value;
    };

    // Supported numeric types
    template <typename _Ty> concept Numeric = 
        Type::IsSame<_Ty, int16_t>
        || Type::IsSame<_Ty, int16_t>
        || Type::IsSame<_Ty, int32_t>
        || Type::IsSame<_Ty, int64_t>
        || Type::IsSame<_Ty, float>
        || Type::IsSame<_Ty, double>;

    // Valid Array value type (Supported numeric types + Variant)
    template <typename _Ty> concept ArrayValue = Numeric<_Ty> || Type::IsSame<_Ty, Variant>;
    template <ArrayValue _Ty> class Array;
    struct ArrayHeader;
    struct ArrayBody;

    namespace Type
    {
        // Type numeric identifiers
        // https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-oaut/3fe7db9f-5803-4dc4-9d14-5425d3f5461f

        enum class ID: uint16_t
        {
            Empty       = 0x0000,
            Byte        = 0x0011,
            Bool        = 0x000B,
            Int16       = 0x0002,
            Int32       = 0x0003,
            Int64       = 0x0014,
            Float       = 0x0004,
            Double      = 0x0005,
            Date        = 0x0007,
            String      = 0x0008,
            Variant     = 0x000C,
            Array       = 0x2000,
            Range       = 0x0009,
            Error       = 0x000A,
        };

        // Enable bitmasking for Type::ID

        inline constexpr ID operator&(ID rhs, ID lhs)   { return (Type::ID)((uint16_t)(rhs) & (uint16_t)(lhs)); }
        inline constexpr ID operator^(ID rhs, ID lhs)   { return (Type::ID)((uint16_t)(rhs) ^ (uint16_t)(lhs)); }
        inline constexpr ID operator|(ID rhs, ID lhs)   { return (Type::ID)((uint16_t)(rhs) | (uint16_t)(lhs)); }
        inline constexpr ID operator&=(ID& rhs, ID lhs) { rhs = rhs & lhs; return rhs; }
        inline constexpr ID operator^=(ID& rhs, ID lhs) { rhs = rhs ^ lhs; return rhs; }
        inline constexpr ID operator|=(ID& rhs, ID lhs) { rhs = rhs | lhs; return rhs; }
        inline constexpr ID operator~(ID id)            { return (Type::ID)(~(uint16_t)(id)); }

        inline constexpr bool IsNumeric(ID id)
        {
            switch (id)
            {
                case ID::Int16:
                case ID::Int32:
                case ID::Int64:
                case ID::Float:
                case ID::Double:
                    return true;
                default:
                    return false;
            }
        }

        template <typename _Ty>
        inline consteval ID GetID()
        {
            if constexpr (IsSame<_Ty, short> || IsSame<_Ty, int16_t>)
                return ID::Int16;
            if constexpr (IsSame<_Ty, int> || IsSame<_Ty, int32_t>)
                return ID::Int32;
            if constexpr (IsSame<_Ty, long> || IsSame<_Ty, long long> || IsSame<_Ty, int64_t>)
                return ID::Int64;
            if constexpr (IsSame<_Ty, float>)
                return ID::Float; 
            if constexpr (IsSame<_Ty, double>)
                return ID::Double;
            if constexpr (IsSame<_Ty, String>)
                return ID::String;
            if constexpr (IsSame<_Ty, Variant>)
                return ID::Variant;

            return ID::Empty;
        }

        // Translates Excel date to Tuple containing Year, Month and Day
        inline constexpr std::tuple<uint16_t, uint8_t, uint8_t> DeserializeDate(double date)
        {
            // I have no idea how this does what it does, but it works and it's fast.
            // Credits: https://www.codeproject.com/Articles/2750/Excel-Serial-Date-to-Day-Month-Year-and-Vice-Versa

            int l           = date + 68569 + 2415019;
            int n           = int(( 4 * l ) / 146097);
            l               = l - int(( 146097 * n + 3 ) / 4);
            int i           = int(( 4000 * ( l + 1 ) ) / 1461001);
            l               = l - int(( 1461 * i ) / 4) + 31;
            int j           = int(( 80 * l ) / 2447);
            uint8_t day     = l - int(( 2447 * j ) / 80);
            l               = int(j / 11);
            uint8_t month   = j + 2 - ( 12 * l );
            uint16_t year   = 100 * ( n - 49 ) + i + l;

            return {year, month, day};
        }
    };
};