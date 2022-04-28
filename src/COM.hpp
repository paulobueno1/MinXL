#pragma once

#include "Common.hpp"

// ------------------------------------------------------------------------------------------------
// VARIANT, SAFEARRAY and BSTR were ported from directly from Windows. The implementation style is 
// similar to the original for comparability with available documentation, except for typedefs such
// as  DWORD for 32-bit integers.
// 
// Big thanks to Olorin for setting me on the right track early on
// https://stackoverflow.com/questions/35940136/bstrs-and-variants-under-mac-os-x
// ------------------------------------------------------------------------------------------------


// Variant data type constants
// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-oaut/3fe7db9f-5803-4dc4-9d14-5425d3f5461f

#define VT_EMPTY        0x0000
#define VT_NULL         0x0001
#define VT_I2           0x0002
#define VT_I4           0x0003
#define VT_R4           0x0004
#define VT_R8           0x0005
#define VT_CY           0x0006
#define VT_DATE         0x0007
#define VT_BSTR         0x0008
#define VT_DISPATCH     0x0009
#define VT_ERROR        0x000A
#define VT_BOOL         0x000B
#define VT_VARIANT      0x000C
#define VT_UNKNOWN      0x000D
#define VT_DECIMAL      0x000E
#define VT_I1           0x0010
#define VT_UI1          0x0011
#define VT_UI2          0x0012
#define VT_UI4          0x0013
#define VT_I8           0x0014
#define VT_UI8          0x0015
#define VT_INT          0x0016
#define VT_UINT         0x0017
#define VT_VOID         0x0018
#define VT_HRESULT      0x0019
#define VT_PTR          0x001A
#define VT_SAFEARRAY    0x001B
#define VT_CARRAY       0x001C
#define VT_USERDEFINED  0x001D
#define VT_LPSTR        0x001E
#define VT_LPWSTR       0x001F
#define VT_RECORD       0x0024
#define VT_INT_PTR      0x0025
#define VT_UINT_PTR     0x0026
#define VT_ARRAY        0x2000
#define VT_BYREF        0x4000

// MinXL only
#define VT_SHORT        VT_I2
#define VT_INTEGER      VT_I4
#define VT_LONG         VT_I8
#define VT_FLOAT        VT_R4
#define VT_DOUBLE       VT_R8
#define VT_STRING       VT_BSTR


// SafeArray Feature constants
// https://docs.microsoft.com/en-us/windows/win32/api/oaidl/ns-oaidl-safearray

#define FADF_AUTO               0x00000001
#define FADF_STATIC             0x00000002
#define FADF_EMBEDDED           0x00000004
#define FADF_FIXEDSIZE          0x00000010
#define FADF_RECORD             0x00000020
#define FADF_HAVEIID            0x00000040
#define FADF_HAVEVARTYPE        0x00000080
#define FADF_BSTR               0x00000100
#define FADF_UNKNOWN            0x00000200
#define FADF_DISPATCH           0x00000400
#define FADF_VARIANT            0x00000800
#define FADF_RESERVED           0x0000F008 


// Return and error constants
#define DISP_E_ARRAYISLOCKED    0x8002000D
#define E_INVALIDARG            0x80070057
#define E_POINTER               0x80004003
#define E_OUTOFMEMORY           0x8007000E
#define S_OK                    0x00000000


// Finally, the source of a lot of headache.........
#define BSTR_BUCKET_SIZE 16
#define SAFEARRAY_HIDDEN_HEADER 16


namespace xl
{
    namespace COM
    {
        // Renamed from CY for clarity 
        union CURRENCY 
        {
            struct 
            { 
                uint32_t Lo; 
                int32_t Hi; 
            };

            int64_t int64;
        };

        struct DECIMAL
        {
            uint16_t wReserved;

            union
            {
                struct   
                {
                    uint8_t scale;
                    uint8_t sign;
                };
                uint16_t signscale;
            };
            uint32_t Hi32;

            union
            {
                struct
                {
                    uint32_t Lo32;
                    uint32_t Mid32;
                };
                uint64_t Lo64;
            };
        };


        // https://docs.microsoft.com/en-us/windows/win32/api/wtypes/ns-wtypes-cy-r1
        // https://docs.microsoft.com/en-us/windows/win32/api/wtypes/ns-wtypes-decimal-r1
        struct BSTR
        {
            uint32_t pad;
            uint32_t size;
            union 
            {
                char16_t ptr[1];
                char16_t str[1];
                uint32_t dwptr[1];
            };
        };
        

        // https://docs.microsoft.com/en-us/windows/win32/api/oaidl/ns-oaidl-safearray
        struct SAFEARRAYBOUND
        {
            uint32_t    cElements;
            int32_t     lLbound;
        };

        struct SAFEARRAY
        {
            uint16_t            cDims;
            uint16_t            fFeatures;
            uint32_t            cbElements;
            uint32_t            cLocks;
            void*               pvData;
            SAFEARRAYBOUND      rgsabound[1];
        };


        // https://docs.microsoft.com/en-us/windows/win32/api/oaidl/ns-oaidl-variant
        struct VARIANT
        {
            VARIANT()
                : vt(0)
                , wReserved1(0)
                , wReserved2(0)
                , wReserved3(0)
                , pvRecord(0)
                , pRecInfo(0) // Initializing gratest union member to ensure all bits are set to zero.
            {
            }

            union
            {
                struct
                {
                    uint16_t vt;
                    uint16_t wReserved1;
                    uint16_t wReserved2;
                    uint16_t wReserved3;

                    union
                    {
                        int64_t         llVal;
                        int32_t         lVal;
                        uint8_t         bVal;
                        int16_t         iVal;
                        float           fltVal;
                        double          dblVal;
                        int16_t         boolVal;

                        int32_t         scode;
                        CURRENCY        cyVal;
                        double          date;
                        char16_t*       bstrVal;

                        // IUnknown
                        void*           punkVal;
                        void**          ppunkVal;
                        
                        // IDispatch
                        void*           pdispVal;
                        void**          ppdispVal; 

                        SAFEARRAY*      parray;
                        uint8_t*        pbVal;
                        int16_t*        piVal;
                        int32_t*        plVal;
                        int64_t*        pllVal;
                        float*          pfltVal;
                        double*         pdblVal;
                        int16_t*        pboolVal;
                        bool*           pbool;
                        int32_t*        pscode;
                        CURRENCY*       pcyVal;
                        double*         pdate;
                        char16_t**      pbstrVal;

                        SAFEARRAY**     pparray; 
                        VARIANT*        pvarVal;
                        void*           byref;
                        int8_t          cVal;
                        uint16_t        uiVal;
                        uint32_t        ulVal;
                        uint64_t        ullVal;
                        int32_t         intVal;
                        uint32_t        uintVal;
                        DECIMAL*        pdecVal;
                        int8_t*         pcVal;
                        uint16_t*       puiVal;
                        uint32_t*       pulVal;
                        uint64_t*       pullVal;
                        int32_t*        pintVal;
                        uint32_t*       puintVal;
                        
                        struct
                        {
                            void* pvRecord;
                            void* pRecInfo;
                        };
                    };
                };
                DECIMAL decVal;
            };
        };


        // ------------------------------------------------------------------------------------------------
        // Returns the size in bytes of a certain type constant
        // ------------------------------------------------------------------------------------------------
        constexpr uint16_t TypeSize(uint16_t type_constant)
        {
            switch (type_constant)
            {
                case VT_I1:
                case VT_UI1:      return sizeof(uint8_t);
                case VT_BOOL:
                case VT_I2:
                case VT_UI2:      return sizeof(int16_t);
                case VT_I4:
                case VT_UI4:
                case VT_R4:
                case VT_INT:
                case VT_UINT:
                case VT_ERROR:    return sizeof(int32_t);
                case VT_R8:
                case VT_I8:
                case VT_UI8:
                case VT_DATE:     return sizeof(int64_t);
                case VT_INT_PTR:
                case VT_UINT_PTR: 
                case VT_BSTR:
                case VT_DISPATCH:
                case VT_UNKNOWN:  return sizeof(void**);
                case VT_CY:       return sizeof(COM::CURRENCY);
                case VT_DECIMAL:  return sizeof(COM::DECIMAL);
                case VT_VARIANT:  return 24;
                case VT_RECORD:   return 32;
            }
            return 0;
        }


        // ------------------------------------------------------------------------------------------------
        // Helper function that converts C++ types to COM constants at compile time.
        // Usage: TypeConstant<double>() returns VT_DOUBLE.
        // ------------------------------------------------------------------------------------------------
        template<typename _Ty> constexpr uint16_t TypeConstant()
        {
            if constexpr 
            (
                std::is_same<_Ty, short>::value ||
                std::is_same<_Ty, int16_t>::value
            )                                                       return VT_SHORT;

            if constexpr 
            (
                std::is_same<_Ty, int>::value ||
                std::is_same<_Ty, int32_t>::value
            )                                                       return VT_INTEGER;

            if constexpr 
            (
                std::is_same<_Ty, long>::value ||
                std::is_same<_Ty, long long>::value ||
                std::is_same<_Ty, int64_t>::value
            )                                                       return VT_LONG; 

            if constexpr (std::is_same<_Ty, float>::value)          return VT_FLOAT; 
            if constexpr (std::is_same<_Ty, double>::value)         return VT_DOUBLE;
            if constexpr (std::is_same<_Ty, xl::String>::value)     return VT_BSTR;
            if constexpr (std::is_same<_Ty, xl::Variant>::value)    return VT_VARIANT;

            return VT_EMPTY;
        }


        constexpr bool IsTypeNumeric(uint16_t type_constant)
        {
            switch (type_constant)
            {
                case VT_SHORT:
                case VT_INTEGER:
                case VT_LONG:
                case VT_FLOAT:
                case VT_DOUBLE: return true;
                default:        return false;
            }
        }
    }
}