#include "Array.hpp"

namespace xl
{
    template<typename _Ty> 
    Array<_Ty>::Array()
    {
        Allocate(0, 0);
    }

    template<typename _Ty> 
    Array<_Ty>::Array(const uint64_t rows, const uint64_t cols)
    {
        Allocate(rows, cols);
    }

    template<typename _Ty>
    Array<_Ty>::Array(const Array<_Ty>& other)
    {
        if (Allocate(other.Rows(), other.Cols()))
            std::copy(other.cbegin(), other.cend(), this->begin());
    }

    template<typename _Ty>
    Array<_Ty>::Array(Array<_Ty>&& other)
    {
        std::memcpy(this, &other, sizeof(other));
        std::memset(&other, 0, sizeof(other));
    }

    template<typename _Ty>
    Array<_Ty>& Array<_Ty>::operator=(const Array<_Ty>& other)
    {
        if (Allocate(other.Rows(), other.Cols()))
            std::copy(other.cbegin(), other.cend(), this->begin());

        return *this;
    }

    template<typename _Ty>
    Array<_Ty>& Array<_Ty>::operator=(Array<_Ty>&& other)
    {
        std::memcpy(this, &other, sizeof(other));
        std::memset(&other, 0, sizeof(other));

        return *this;
    }

    template<typename _Ty>
    Array<_Ty>::Array(const Variant& var)
    {
        if (var.IsArray() && var.parray)
        {
            if (var.parray->pvData)
            {
                const uint64_t nCols  = var.parray->rgsabound[0].cElements;
                const uint64_t nRows  = var.parray->rgsabound[1].cElements;
                constexpr auto nType  = COM::TypeConstant<_Ty>();
                const _Ty*     pOther = static_cast<_Ty*>(var.parray->pvData);

                if (var.IsArrayOf(nType))
                {
                    if (Allocate(nRows, nCols))
                        std::memcpy(m_Data, pOther, nRows * nCols);
                }
            else
                XL_THROW("Attempt to convert xl::Variant to xl::Array failed; type mismatch detected");
        }
        else
            XL_THROW("Attempl to convert xl::Variant to xl::Array failed; received Variant is not an array");
        }
    }

    template<typename _Ty>
    Array<_Ty>::Array(Variant&& var)
    {
        using Array = Array<_Ty>;

        if (var.IsArray() && var.parray)
        {
            constexpr auto nType = COM::TypeConstant<_Ty>();

            if (var.IsArrayOf(nType))
            {
                auto p = reinterpret_cast<Array*>((char*)var.parray - SAFEARRAY_HIDDEN_HEADER);

                std::memcpy(this, p, sizeof(Array));
                std::memset(&var, 0, sizeof(var));
            }
            else
                XL_THROW("Attempt to convert xl::Variant to xl::Array failed; type mismatch detected");
        }
        else
            XL_THROW("Attempl to convert xl::Variant to xl::Array failed; received Variant is not an array");
    }

    template<typename _Ty>
    Array<_Ty>::Array(const std::vector<_Ty>& vec)
    {
        if (Allocate(vec.size(), 1))
            std::copy(vec.begin(), vec.end(), this->begin());
    }

    template<typename _Ty>
    Array<_Ty>::Array(const std::vector<_Ty>&& vec)
    {
        if (Allocate(vec.size(), 1))
            std::copy(vec.begin(), vec.end(), this->begin());
    }

    template<>
    Array<Variant>::Array(const std::vector<std::string>& vec)
    {
        if (Allocate(vec.size(), 1))
            for (uint64_t i = 0; i < vec.size(); i++)
                m_Data[i] = xl::Variant(xl::String(vec[i]));
    }

    template<>
    Array<Variant>::Array(const std::vector<std::string>&& vec)
    {
        if (Allocate(vec.size(), 1))
            for (uint64_t i = 0; i < vec.size(); i++)
                m_Data[i] = xl::Variant(xl::String(vec[i]));
    }

    template<typename _Ty>
    Array<_Ty>::~Array()
    {
        if (m_Data)
            std::free(m_Data);
        
        std::memset(this, 0, sizeof(*this));
    }

    template<typename _Ty> 
    bool Array<_Ty>::Allocate(const uint64_t rows, const uint64_t cols)
    {
        constexpr auto nType = COM::TypeConstant<_Ty>();
        static_assert(nType, "Attempt to instantiate xl::Array with unsupported type.");

        m_Type = nType;
        m_Dims = 2;
        m_Locks = 0;
        m_ElementSize = sizeof(_Ty);
        m_Features = FADF_HAVEVARTYPE;
        m_Columns.m_ElementCount = cols;
        m_Columns.m_LowerBound = 1;
        m_Rows.m_ElementCount = rows;
        m_Rows.m_LowerBound = 1;

        switch (nType)
        {
            case VT_BSTR:     m_Features |= FADF_BSTR;       break;
            case VT_VARIANT:  m_Features |= FADF_VARIANT;    break;
            case VT_DISPATCH: m_Features |= FADF_DISPATCH;   break; /* (for when we port IDispatch) */
            default:                                         break;
        }

        if (auto p = static_cast<_Ty*>(std::calloc(rows * cols, sizeof(_Ty))))
        {
            m_Data = p;
            return true;
        }
        else
        {
            std::memset(this, 0, sizeof(*this));
            return false;
        }
    }

    template<typename _Ty>
    void Array<_Ty>::Deallocate(COM::SAFEARRAY* array)
    {
        if (array)
        {
            if (array->pvData)
                std::free(array->pvData);

            std::free((char*)array - SAFEARRAY_HIDDEN_HEADER);
        }
    }

    template<typename _Ty>
    const _Ty& Array<_Ty>::operator[](const uint64_t index) const
    {
        return m_Data[index];
    }

    template<typename _Ty>
    _Ty& Array<_Ty>::operator[](const uint64_t index)
    {
        return m_Data[index];
    }

    template<typename _Ty>
    const _Ty& Array<_Ty>::operator()(const uint64_t row, const uint64_t col) const
    {
        return m_Data[row + col * Rows()];
    }

    template<typename _Ty>
    _Ty& Array<_Ty>::operator()(const uint64_t row, const uint64_t col)
    {
        return m_Data[row + col * Rows()];
    }

    template<typename _Ty>
    void Array<_Ty>::Resize(const uint64_t rows, const uint64_t cols)
    {
        const uint64_t nPrevCols = m_Columns.m_ElementCount;
        const uint64_t nPrevRows = m_Rows.m_ElementCount;

        if (nPrevCols == cols && nPrevRows == rows)
            return;

        auto ptrPrev = m_Data;

        if (auto ptrNew = static_cast<_Ty*>(calloc(rows * cols, sizeof(_Ty))))
        {
            const auto nMinCols = std::min(nPrevCols, cols);
            const auto nMinRows = std::min(nPrevRows, rows);

            for (uint64_t c = 0; c < nMinCols; c++)
                std::memcpy(&ptrNew[c * rows], &ptrPrev[c * nPrevRows], nMinRows * sizeof(_Ty));

            m_Data = ptrNew;
            m_Columns.m_ElementCount = cols;
            m_Rows.m_ElementCount = rows;

            std::free(ptrPrev);
        }
    }

    template<typename _Ty>
    void Array<_Ty>::Print() const
    {
        for (uint64_t r = 0; r < Rows(); r++)
        {
            for (uint64_t c = 0; c < Cols(); c++)
                std::cout << (*this)(r, c) << ' ';

            std::cout << '\n';
        }
    }

    // Explicit class instantiations to avoid linking errors.
    template class Array<short>;
    template class Array<int>;
    template class Array<long>;
    template class Array<long long>;
    template class Array<float>;
    template class Array<double>;
    template class Array<Variant>;
}