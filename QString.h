// QString --- Quick String by katahiromz
// License: MIT
#pragma once

#ifndef XMALLOC
#define XMALLOC(x)      malloc(x)
#define XREALLOC(x,y)   realloc(x, y)
#define XFREE(x)        free(x)
#endif

#ifndef XTHROW
    #define XTHROW(x) /**/
#endif

#include <emmintrin.h> // SSE2
#include <cassert>
#include <strsafe.h>

#ifndef _INC_WINDOWS
typedef size_t SIZE_T;
#endif

template <typename T_CHAR, SIZE_T t_size>
struct QStringLiteral
{
    enum : SIZE_T { m_size = t_size };
    const T_CHAR *m_pszText;
    QStringLiteral(const T_CHAR (&szText)[t_size]) noexcept : m_pszText(szText) { }
    operator const T_CHAR *() const noexcept { return m_pszText; }
    const T_CHAR& operator[](size_t index) const noexcept { return m_pszText[index]; }
};

template <typename T_CHAR>
class QStringT
{
public:
    typedef T_CHAR value_type;
    typedef SIZE_T size_type;
    typedef QStringT<T_CHAR> self_type;
    enum { SSO_MAX_SIZE = 60 }; // Short String Optimization

    T_CHAR *m_pszText;
    size_type m_nLength;
    size_type m_nCapacity;
    T_CHAR m_szText[SSO_MAX_SIZE];
    static const size_type npos = -1;

protected:
    inline size_type _length(const  char   *psz) const noexcept { return strlen(psz); }
    inline size_type _length(const wchar_t *psz) const noexcept { return wcslen(psz); }

    inline bool is_alloc() const noexcept
    {
        return m_szText != m_pszText;
    }

    inline void _free() noexcept
    {
        if (is_alloc())
            XFREE(m_pszText);
    }

    inline void _reset() noexcept
    {
        m_pszText = m_szText;
        m_pszText[0] = 0;
        m_nLength = 0;
        m_nCapacity = SSO_MAX_SIZE;
    }

    inline bool _resize_0(size_type newCapacity, bool alloc) noexcept
    {
        size_type newSize = newCapacity * sizeof(T_CHAR);
        return _resize_0(newCapacity, alloc, newSize);
    }
    bool _resize_0(size_type newCapacity, bool alloc, size_type newSize) noexcept
    {
        T_CHAR *pszNew;
        if (alloc)
        {
            pszNew = (T_CHAR*)XREALLOC(m_pszText, newSize);
            if (!pszNew)
            {
                XTHROW(1);
                return false;
            }
        }
        else
        {
            pszNew = (T_CHAR*)XMALLOC(newSize);
            if (!pszNew)
            {
                XTHROW(1);
                return false;
            }
            memcpy(pszNew, m_szText, m_nLength * sizeof(T_CHAR));
        }

        m_pszText = pszNew;
        m_nCapacity = newCapacity;
        return true;
    }
    inline bool _resize_1(size_type newCapacity) noexcept
    {
        return _resize_1(newCapacity, is_alloc());
    }
    inline bool _resize_1(size_type newCapacity, bool alloc) noexcept
    {
        if (newCapacity <= m_nCapacity)
            return true;

        return _resize_0(newCapacity * 2, alloc);
    }

    void _copy(const T_CHAR *pszText, size_type cchText) noexcept
    {
        if (!_resize_1(cchText + 1))
            return;

        memcpy(m_pszText, pszText, cchText * sizeof(T_CHAR));
        m_nLength = cchText;
        m_pszText[m_nLength] = 0;
    }

    void _insert_0(size_t index, const T_CHAR *pszText, size_type cchText) noexcept
    {
        if (!_resize_1(m_nLength + cchText + 1))
            return;

        memmove(&m_pszText[index + cchText], &m_pszText[index], (m_nLength - index) * sizeof(T_CHAR));
        memcpy(&m_pszText[index], pszText, cchText * sizeof(T_CHAR));
        m_nLength += cchText;
        m_pszText[m_nLength] = 0;
    }
    inline void _insert(size_t index, const T_CHAR *pszText, size_type cchText) noexcept
    {
        assert(index <= m_nLength);

        if (!cchText)
            return;

        _insert_0(index, pszText, cchText);
    }

    inline void _fill(size_type count, char ch) noexcept
    {
        memset(m_pszText, ch, count);
    }
    inline void _fill(size_type index, size_type count, char ch) noexcept
    {
        memset(&m_pszText[index], ch, count);
    }

    inline void _fill(size_type count, wchar_t ch) noexcept
    {
        _fill(0, count, ch);
    }
    void _fill(size_type index, size_type count, wchar_t ch) noexcept
    {
        count += index;
#ifdef NDEBUG
        __m128i xmm = _mm_set1_epi16(ch);
        for (; index + 16 <= count; index += 16)
        {
            _mm_store_si128((__m128i*)&m_pszText[index    ], xmm);
            _mm_store_si128((__m128i*)&m_pszText[index + 8], xmm);
        }
        for (; index + 8 <= count; index += 8)
        {
            _mm_store_si128((__m128i*)&m_pszText[index], xmm);
        }
#endif
        for (; index < count; ++index)
        {
            m_pszText[index] = ch;
        }
    }

    inline int _compare(const T_CHAR *psz, const T_CHAR *pszText, size_type cchText) const noexcept
    {
        return strncmp(psz, pszText, cchText);
    }
    inline int _compare(const T_CHAR *pszText, size_type cchText) const noexcept
    {
        return _compare(m_pszText, pszText, cchText);
    }

    inline int _icompare(const char *psz, const char *pszText, size_type cchText) const noexcept
    {
        return _strnicmp(psz, pszText, cchText);
    }
    inline int _icompare(const wchar_t *psz, const wchar_t *pszText, size_type cchText) const noexcept
    {
        return _wcsnicmp(psz, pszText, cchText);
    }
    inline int _icompare(const char *pszText, size_type cchText) const noexcept
    {
        return _icompare(m_pszText, pszText, cchText);
    }
    inline int _icompare(const wchar_t *pszText, size_type cchText) const noexcept
    {
        return _icompare(m_pszText, pszText, cchText);
    }

    inline size_type _format_v(const char *fmt, va_list va) noexcept
    {
        char szBuf[1024];
        StringCchVPrintfA(szBuf, _countof(szBuf), fmt, va);
        *this = szBuf;
        return strlen(szBuf);
    }
    inline size_type _format_v(const wchar_t *fmt, va_list va) noexcept
    {
        wchar_t szBuf[1024];
        StringCchVPrintfW(szBuf, _countof(szBuf), fmt, va);
        *this = szBuf;
        return wcslen(szBuf);
    }

    void _replace_0(size_type index, size_type count, const T_CHAR *pszText, size_type cchText) noexcept
    {
        memmove(&m_pszText[index + cchText], &m_pszText[index + count], (m_nLength - count) * sizeof(T_CHAR));
        memcpy(&m_pszText[index], pszText, cchText * sizeof(T_CHAR));
        m_nLength += cchText - count;
        m_pszText[m_nLength] = 0;
    }
    inline void _replace_1(size_type index, size_type count, const T_CHAR *pszText, size_type cchText) noexcept
    {
        assert(index <= m_nLength);
        assert(index + count <= m_nLength);

        if (count == cchText)
        {
            memcpy(&m_pszText[index], pszText, count * sizeof(T_CHAR));
            return;
        }

        if (cchText > count)
        {
            if (!_resize_1(m_nLength + (cchText - count) + 1))
                return;
        }

        _replace_0(index, count, pszText, cchText);
    }

public:
    inline QStringT() noexcept
        : m_pszText(m_szText)
        , m_nLength(0)
        , m_nCapacity(SSO_MAX_SIZE)
    {
        m_szText[0] = 0;
    }

    inline QStringT(const T_CHAR *pszText) noexcept
        : m_pszText(m_szText)
        , m_nLength(0)
        , m_nCapacity(SSO_MAX_SIZE)
    {
        _copy(pszText, _length(pszText));
    }

    inline QStringT(const T_CHAR *pch0, const T_CHAR *pch1) noexcept
        : m_pszText(m_szText)
        , m_nLength(0)
        , m_nCapacity(SSO_MAX_SIZE)
    {
        _copy(pch0, pch1 - pch0);
    }

    template <size_type t_size>
    inline QStringT(const QStringLiteral<T_CHAR, t_size>& literal) noexcept
        : m_pszText(m_szText)
        , m_nLength(0)
        , m_nCapacity(SSO_MAX_SIZE)
    {
        assert(literal[t_size - 1] == 0);
        _copy(literal, t_size - 1);
    }

    inline QStringT(const T_CHAR *pszText, size_type cchText) noexcept
        : m_pszText(m_szText)
        , m_nLength(0)
        , m_nCapacity(SSO_MAX_SIZE)
    {
        _copy(pszText, cchText);
    }

    inline QStringT(const self_type& str) noexcept
        : m_pszText(m_szText)
        , m_nLength(0)
        , m_nCapacity(SSO_MAX_SIZE)
    {
        _copy(str.m_pszText, str.m_nLength);
    }

    inline QStringT(self_type&& str) noexcept
    {
        bool alloc = str.is_alloc();
        memcpy(this, &str, sizeof(*this));
        if (!alloc)
            m_pszText = m_szText;
        str._reset();
    }

    inline QStringT(size_type count, T_CHAR ch) noexcept
        : m_pszText(m_szText)
        , m_nLength(0)
        , m_nCapacity(SSO_MAX_SIZE)
    {
        if (!count)
            return;

        if (!_resize_1(count + 1))
            return;

        _fill(count, ch);
        m_pszText[count] = 0;
    }

    inline ~QStringT() noexcept
    {
        _free();
    }

    inline       T_CHAR& operator[](size_type index)       noexcept { return m_pszText[index]; }
    inline const T_CHAR& operator[](size_type index) const noexcept { return m_pszText[index]; }

    inline T_CHAR *data()                  noexcept { return m_pszText; }
    inline const T_CHAR *c_str() const     noexcept { return m_pszText; }
    inline operator const T_CHAR *() const noexcept { return m_pszText; }
    inline size_type size() const          noexcept { return m_nLength; }
    inline bool empty() const              noexcept { return !size();   }

    inline void clear() noexcept
    {
        _free();
        _reset();
    }

    inline QStringT& operator=(const T_CHAR *pszText) noexcept
    {
        assign(pszText);
        return *this;
    }
    inline QStringT& operator=(const self_type& str) noexcept
    {
        if (this != &str)
            assign(str);
        return *this;
    }
    inline QStringT& operator=(self_type&& str) noexcept
    {
        if (this == &str)
            return *this;

        assign(str);
        return *this;
    }
    template <size_type t_size>
    inline QStringT& operator=(const QStringLiteral<T_CHAR, t_size>& literal) noexcept
    {
        assert(literal[t_size - 1] == 0);
        assign(literal);
        return *this;
    }

    inline void assign(const T_CHAR *pszText) noexcept
    {
        _copy(pszText, _length(pszText));
    }
    inline void assign(const T_CHAR *pch0, const T_CHAR *pch1) noexcept
    {
        _copy(pch0, pch1 - pch0);
    }
    inline void assign(const self_type& str) noexcept
    {
        assert(this != &str);
        _copy(str.m_pszText, str.m_nLength);
    }
    inline void assign(self_type&& str) noexcept
    {
        assert(this != &str);
        _free();
        bool alloc = str.is_alloc();
        memcpy(this, &str, sizeof(*this));
        if (!alloc)
            m_pszText = m_szText;
        str._reset();
    }
    inline void assign(size_type count, T_CHAR ch) noexcept
    {
        if (!_resize_1(count + 1))
            return;

        _fill(count, ch);
        m_pszText[count] = 0;
    }
    template <size_type t_size>
    inline void assign(const QStringLiteral<T_CHAR, t_size>& literal) noexcept
    {
        assert(literal[t_size - 1] == 0);
        _copy(literal, t_size - 1);
    }

    inline void operator+=(T_CHAR ch) noexcept
    {
        size_type newLength = m_nLength + 1;
        if (!_resize_1(newLength + 1))
            return;

        m_pszText[m_nLength] = ch;
        m_nLength = newLength;
        m_pszText[m_nLength] = 0;
    }
    inline void operator+=(const T_CHAR *pszText) noexcept
    {
        append(pszText);
    }
    inline void operator+=(const self_type& str) noexcept
    {
        append(str.m_pszText, str.m_nLength);
    }
    template <size_type t_size>
    inline void operator+=(const QStringLiteral<T_CHAR, t_size>& literal) noexcept
    {
        assert(literal[t_size - 1] == 0);
        append(literal, t_size - 1);
    }

    inline void append(size_type count, T_CHAR ch) noexcept
    {
        self_type str(count, ch);
        *this += str;
    }
    inline void append(const T_CHAR *pszText) noexcept
    {
        size_type cchText = _length(pszText);
        size_type newLength = m_nLength + cchText;

        if (!_resize_1(newLength + 1))
            return;

        memcpy(&m_pszText[m_nLength], pszText, cchText * sizeof(T_CHAR));
        m_nLength = newLength;
        m_pszText[m_nLength] = 0;
    }
    inline void append(const T_CHAR *pszText, size_type cchText) noexcept
    {
        size_type newLength = m_nLength + cchText;
        if (!_resize_1(newLength + 1))
            return;

        memcpy(&m_pszText[m_nLength], pszText, cchText * sizeof(T_CHAR));
        m_nLength = newLength;
        m_pszText[m_nLength] = 0;
    }
    inline void append(const self_type& str) noexcept
    {
        append(str.m_pszText, str.m_nLength);
    }

    inline void insert(size_type index, size_type count, T_CHAR ch) noexcept
    {
        self_type str(count, ch);
        insert(index, str);
    }
    inline void insert(size_type index, const T_CHAR *psz) noexcept
    {
        insert(index, psz, _length(psz));
    }
    inline void insert(size_type index, const T_CHAR *pszText, size_type cchText) noexcept
    {
        _insert(index, pszText, cchText);
    }
    inline void insert(size_type index, const self_type& str) noexcept
    {
        _insert(index, str.m_pszText, str.m_nLength);
    }

    inline void erase() noexcept
    {
        clear();
    }
    inline void erase(size_type index) noexcept
    {
        if (index < m_nLength)
            m_nLength = index;
        m_pszText[m_nLength] = 0;
    }
    inline void erase(size_type index, size_type count) noexcept
    {
        if (!count)
            return;

        if (count == npos)
            count = m_nLength - index;

        assert(index <= m_nLength);
        assert(index + count <= m_nLength);
        memmove(&m_pszText[index], &m_pszText[index + count], count * sizeof(T_CHAR));
        m_nLength -= count;
        m_pszText[m_nLength] = 0;
    }

    inline void replace(size_type index, size_type count, const T_CHAR *pszText) noexcept
    {
        replace(index, count, pszText, _length(pszText));
    }
    inline void replace(size_type index, size_type count, const self_type& str) noexcept
    {
        replace(index, count, str.m_pszText, str.m_nLength);
    }
    inline void replace(size_type index, size_type count, const T_CHAR *pszText, size_type cchText) noexcept
    {
        _replace_1(index, count, pszText, cchText);
    }

    inline void resize(size_type length) noexcept
    {
        if (!_resize_1(length + 1))
            return;
        m_nLength = length;
        m_pszText[m_nLength] = 0;
    }
    void resize(size_type length, T_CHAR ch) noexcept
    {
        size_type len = m_nLength;
        if (!_resize_1(length + 1))
            return;
        if (length > len)
            _fill(len, length - len, ch);
        m_nLength = length;
        m_pszText[m_nLength] = 0;
    }

    inline void reserve(size_type length)
    {
        _resize_0(length + 1, is_alloc());
    }

    inline int compare(const T_CHAR *psz) const noexcept
    {
        return _compare(psz, _length(psz));
    }
    inline int compare(const self_type& str) const noexcept
    {
        return _compare(str.m_pszText, str.m_nLength);
    }

    inline int icompare(const T_CHAR *psz) const noexcept
    {
        return _icompare(psz, _length(psz));
    }
    inline int icompare(const self_type& str) const noexcept
    {
        return _icompare(str.m_pszText, str.m_nLength);
    }

    inline bool operator==(const T_CHAR *psz) const noexcept
    {
        return compare(psz) == 0;
    }
    inline bool operator!=(const T_CHAR *psz) const noexcept
    {
        return compare(psz) != 0;
    }
    inline bool operator<(const T_CHAR *psz) const noexcept
    {
        return compare(psz) < 0;
    }
    inline bool operator>(const T_CHAR *psz) const noexcept
    {
        return compare(psz) > 0;
    }
    inline bool operator<=(const T_CHAR *psz) const noexcept
    {
        return compare(psz) <= 0;
    }
    inline bool operator>=(const T_CHAR *psz) const noexcept
    {
        return compare(psz) >= 0;
    }

    inline bool operator==(const self_type& str) const noexcept
    {
        return compare(str) == 0;
    }
    inline bool operator!=(const self_type& str) const noexcept
    {
        return compare(str) != 0;
    }
    inline bool operator<(const self_type& str) const noexcept
    {
        return compare(str) < 0;
    }
    inline bool operator>(const self_type& str) const noexcept
    {
        return compare(str) > 0;
    }
    inline bool operator<=(const self_type& str) const noexcept
    {
        return compare(str) <= 0;
    }
    inline bool operator>=(const self_type& str) const noexcept
    {
        return compare(str) >= 0;
    }

    void swap(self_type& str) noexcept
    {
        self_type tmp = std::move(*this);
        *this = std::move(str);
        str = std::move(tmp);
    }

    inline self_type substr() const noexcept
    {
        return *this;
    }
    inline self_type substr(size_type index) const noexcept
    {
        assert(index <= m_nLength);
        return self_type(&m_pszText[index], m_nLength - index);
    }
    inline self_type substr(size_type index, size_type count) const noexcept
    {
        assert(index <= m_nLength);
        assert(index + count <= m_nLength);
        return self_type(&m_pszText[index], count);
    }

    inline size_type format(const T_CHAR *fmt, ...) noexcept
    {
        va_list va;
        va_start(va, fmt);
        size_type ret = _format_v(fmt, va);
        va_end(va);
        return ret;
    }

    inline size_type find(T_CHAR ch, size_type index = 0) const noexcept
    {
        if (index >= m_nLength)
            return npos;
        for (; index < m_nLength; ++index)
        {
            if (m_pszText[index] == ch)
                return index;
        }
        return npos;
    }
    inline size_type find(const T_CHAR *psz, size_type index = 0) const noexcept
    {
        return find(psz, index, _length(psz));
    }
    size_type find(const T_CHAR *pszText, size_type index, size_type cchText) const noexcept
    {
        if (index >= m_nLength || cchText == 0 || cchText > m_nLength - index)
            return npos;

        const T_CHAR *str = &m_pszText[index];
        const T_CHAR *end = &m_pszText[m_nLength - cchText + 1];

        while (str < end) {
            const T_CHAR *p = &pszText[cchText - 1];
            const T_CHAR *s = &str[cchText - 1];
            while (p >= pszText) {
                if (*s != *p) {
                    break;
                }
                --s;
                --p;
            }
            if (p < pszText)
                return str - m_pszText;

            size_type skip = 1;
            for (size_type i = 0; i < cchText - 1; ++i) {
                if (pszText[i] == *s) {
                    skip = cchText - i - 1;
                    break;
                }
            }
            str += skip;
        }

        return npos;
    }
    inline size_type find(const self_type& str, size_type index = 0) const noexcept
    {
        return find(str.m_pszText, index, str.m_nLength);
    }

    inline size_type rfind(T_CHAR ch, size_type index = 0) const noexcept
    {
        if (index >= m_nLength)
            return npos;

        for (size_type rindex = m_nLength; rindex > 0; )
        {
            --rindex;
            if (m_pszText[rindex] == ch)
                return rindex;
        }
        return npos;
    }
    inline size_type rfind(const T_CHAR *psz, size_type index = 0) const noexcept
    {
        return rfind(psz, index, _length(psz));
    }
    size_type rfind(const T_CHAR *pszText, size_type index, size_type cchText) const noexcept
    {
        if (index >= m_nLength || cchText == 0 || cchText > m_nLength - index)
            return npos;

        const T_CHAR* str = &m_pszText[m_nLength - cchText];
        const T_CHAR* end = &m_pszText[index + cchText];

        while (str >= m_pszText) {
            const T_CHAR* p = &pszText[cchText - 1];
            const T_CHAR* s = &str[cchText - 1];

            while (p >= pszText) {
                if (*s != *p)
                    break;
                --s;
                --p;
            }

            if (p < pszText)
                return str - m_pszText;

            const T_CHAR* q = p;
            while (q >= pszText && *q != *s) {
                --q;
            }

            str -= p - q;
        }

        return npos;
    }
    inline size_type rfind(const self_type& str, size_type index = 0) const noexcept
    {
        return rfind(str.m_pszText, index, str.m_nLength);
    }

    size_type ifind(T_CHAR ch, size_type index = 0) const noexcept
    {
        if (index >= m_nLength)
            return npos;
        for (; index < m_nLength; ++index)
        {
            if (_icompare(&m_pszText[index], &ch, 1) == 0)
                return index;
        }
        return npos;
    }
    inline size_type ifind(const T_CHAR *psz, size_type index = 0) const noexcept
    {
        return ifind(psz, index, _length(psz));
    }
    size_type ifind(const T_CHAR *pszText, size_type index, size_type cchText) const noexcept
    {
        for (; index < m_nLength; ++index)
        {
            if (index + cchText > m_nLength)
                break;
            if (_icompare(&m_pszText[index], pszText, cchText) == 0)
                return index;
        }
        return npos;
    }
    inline size_type ifind(const self_type& str, size_type index = 0) const noexcept
    {
        return ifind(str.m_pszText, index, str.m_nLength);
    }
};

typedef QStringT<char>    QStringA;
typedef QStringT<wchar_t> QStringW;
#ifdef _INC_WINDOWS
    typedef QStringT<TCHAR> QString;
#else
    typedef QStringA QString;
#endif
