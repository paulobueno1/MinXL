#pragma once

#include "Common.hpp"
#include "COM.hpp"

namespace xl
{
    template<class Array> class ArrayIterator
    {
    public:
        using ValueType = typename Array::ValueType;
        using PointerType = ValueType*;
        using ReferenceType = ValueType&;

    private:
        PointerType m_Ptr;
    
    public:
        explicit ArrayIterator(PointerType ptr) noexcept : m_Ptr(ptr) {}
    
        ArrayIterator& operator++()                 { m_Ptr++;    return *this;                    }
        ArrayIterator& operator--()                 { m_Ptr--;    return *this;                    }

        ArrayIterator& operator++(int)              { auto it = *this;    ++(*this);    return it; }
        ArrayIterator& operator--(int)              { auto it = *this;    --(*this);    return it; }

        long operator-(ArrayIterator& other)        { return m_Ptr - other.m_Ptr;                  }

        bool operator==(const ArrayIterator& other) const { return m_Ptr == other.m_Ptr;           }
        bool operator!=(const ArrayIterator& other) const { return m_Ptr != other.m_Ptr;           }

        ReferenceType   operator[](int index)       { return *(m_Ptr + index);                     }
        ReferenceType   operator*()                 { return *m_Ptr;                               }
        PointerType     operator->()                { return m_Ptr;                                }
    };

    struct ArrayBound
    {
        uint32_t m_ElementCount;
        int32_t m_LowerBound;
    };


    // ------------------------------------------------------------------------------------------------
    // C++ implementation of COM's C-style SAFEARRAY.
    // 
    // Has to be converted into xl::Variant to be returned to Excel (not really, but it's safer and 
    // completely inexpensive to do so).
    // 
    // The class' memory layout is explicit in this implementation, without any obscure allocation,
    // hidden byte headers, etc. The allocators, however, have to utilize malloc/free
    // instead of new/delete for compatibility with Excel; not doing so WILL result in a runtime
    // crash.
    // ------------------------------------------------------------------------------------------------
    template<typename _Ty> class Array
    {
        friend class Variant;

    private:
        uint8_t     m_Reserved[12] = {0};
        uint32_t    m_Type;
        uint16_t    m_Dims;
        uint16_t    m_Features;
        uint32_t    m_ElementSize;
        uint32_t    m_Locks;
        _Ty*        m_Data = nullptr;
        ArrayBound  m_Columns;
        ArrayBound  m_Rows;
    
    public:
        Array();
        Array(const Array<_Ty>& other);
        Array(Array<_Ty>&& other);

        Array(const uint64_t rows, const uint64_t cols);

        Array<_Ty>& operator=(const Array<_Ty>& other);
        Array<_Ty>& operator=(Array<_Ty>&& other);

        Array(const Variant& var);
        Array(Variant&& var);

        Array(const std::vector<_Ty>& vec);
        Array(const std::vector<_Ty>&& vec);

        Array(const std::vector<std::string>& vec);
        Array(const std::vector<std::string>&& vec);

        ~Array();
    
    private:
        bool        Allocate(const uint64_t rows, const uint64_t cols);
        static void Deallocate(COM::SAFEARRAY* array);

    public:
        const _Ty&  operator[](const uint64_t index) const;
        _Ty&        operator[](const uint64_t index);
        const _Ty&  operator()(const uint64_t row, const uint64_t col) const;
        _Ty&        operator()(const uint64_t row, const uint64_t col);

    public:
        void        Resize(const uint64_t rows, const uint64_t cols);
        void        Print() const;

    public:
        inline _Ty*     Data() const        { return m_Data;                                            }
        inline uint64_t Rows() const        { return m_Rows.m_ElementCount;                             }
        inline uint64_t Cols() const        { return m_Columns.m_ElementCount;                          }
        inline uint64_t Size() const        { return m_Columns.m_ElementCount * m_Rows.m_ElementCount;  }
        inline uint16_t Type() const        { return m_Type;                                            }
        inline uint32_t ElementSize() const { return m_ElementSize;                                     }
        inline bool     IsValid() const     { return m_Data;                                            }

    public:
        using ValueType = _Ty;
        using Iterator = ArrayIterator<Array<_Ty>>;
        using ConstIterator = ArrayIterator<const Array<_Ty>>;
        using RangeIterator = std::pair<typename Array<_Ty>::Iterator, typename Array<_Ty>::Iterator>;

        inline Iterator       begin()               { return Iterator(m_Data);                          }
        inline Iterator       end()                 { return Iterator(m_Data + Size());                 }
        inline ConstIterator  cbegin() const        { return ConstIterator(m_Data);                     }
        inline ConstIterator  cend() const          { return ConstIterator(m_Data + Size());            }
        
        inline RangeIterator column(uint64_t col)
        {
            return {Iterator{&(operator()(0, col))}, Iterator{&(operator()(Rows(), col))}}; 
        }

        inline RangeIterator subset(uint64_t col_1, uint64_t col_n)
        {
            return {Iterator{&(operator()(0, col_1))}, Iterator{&(operator()(Rows(), col_n))}}; 
        }
    };
}