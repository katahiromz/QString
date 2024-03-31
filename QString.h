// QString Version 0.6 --- Quick String by katahiromz
// License: MIT
#pragma once

#ifdef _WIN32
    #include <windows.h>
#endif

#ifndef XMALLOC
    #ifdef _WIN32
        #define XMALLOC(x)      HeapAlloc(GetProcessHeap(), 0, (x))
        #define XREALLOC(x,y)   HeapReAlloc(GetProcessHeap(), 0, (x), (y))
        #define XFREE(x)        HeapFree(GetProcessHeap(), 0, (x))
    #else
        #define XMALLOC(x)      malloc(x)
        #define XREALLOC(x,y)   realloc((x), (y))
        #define XFREE(x)        free(x)
    #endif
#endif

#ifndef XTHROW
    #define XTHROW(x) /**/
#endif

#ifndef XNOEXCEPT
    #define XNOEXCEPT noexcept
#endif

#ifndef XMALLOC_THROWABLE
    #ifdef _WIN32
        #define XMALLOC_THROWABLE(x) \
            HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS	, (x))
    #else
        #define XMALLOC_THROWABLE XMALLOC
    #endif
#endif
#ifndef XREALLOC_THROWABLE
    #ifdef _WIN32
        #define XREALLOC_THROWABLE(x, y) \
            HeapReAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, (x), (y))
    #else
        #define XREALLOC_THROWABLE XREALLOC
    #endif
#endif

#include <emmintrin.h>
#include <immintrin.h>
#include <cstdint>
#include <cassert>
#include <algorithm>
#include <strsafe.h>

template <typename T_CHAR, size_t t_size>
struct QStringLiteral
{
    const T_CHAR *m_pszText;
    QStringLiteral(const T_CHAR (&szText)[t_size]) XNOEXCEPT : m_pszText(szText)
    {
        assert(szText[t_size - 1] == 0);
    }
    operator const T_CHAR *() const XNOEXCEPT { return m_pszText; }
    const T_CHAR& operator[](size_t index) const XNOEXCEPT { return m_pszText[index]; }
};

template <typename T_CHAR>
class QStringT
{
public:
    typedef T_CHAR value_type;
    typedef size_t size_type;
    typedef QStringT<T_CHAR> self_type;
    enum { SSO_MAX_SIZE = 60 }; // Short String Optimization

    T_CHAR *m_pszText;
    size_type m_nLength;
    size_type m_nCapacity;
    T_CHAR m_szText[SSO_MAX_SIZE];
    static const size_type npos = -1;

protected:
    inline size_type _length(const  char   *psz) const XNOEXCEPT { return strlen(psz); }
    inline size_type _length(const wchar_t *psz) const XNOEXCEPT { return wcslen(psz); }

    inline bool is_alloc() const XNOEXCEPT
    {
        return m_szText != m_pszText;
    }

    inline void _free() XNOEXCEPT
    {
        if (is_alloc())
            XFREE(m_pszText);
    }

    inline void _reset() XNOEXCEPT
    {
        m_pszText = m_szText;
        m_pszText[0] = 0;
        m_nLength = 0;
        m_nCapacity = SSO_MAX_SIZE;
    }

    inline void _resize_0(size_type newCapacity, bool alloc) XNOEXCEPT
    {
        const size_type newSize = newCapacity * sizeof(T_CHAR);
        _resize_0(newCapacity, alloc, newSize);
    }
    inline void _resize_0(size_type newCapacity, bool alloc, size_type newSize) XNOEXCEPT
    {
        T_CHAR *pszNew;
        if (alloc)
        {
            pszNew = (T_CHAR*)XREALLOC_THROWABLE(m_pszText, newSize);
        }
        else
        {
            pszNew = (T_CHAR*)XMALLOC_THROWABLE(newSize);
            memcpy(pszNew, m_szText, m_nLength * sizeof(T_CHAR));
        }

        m_pszText = pszNew;
        m_nCapacity = newCapacity;
    }
    inline void _resize_1(size_type newCapacity) XNOEXCEPT
    {
        _resize_1(newCapacity, is_alloc());
    }
    inline void _resize_1(size_type newCapacity, bool alloc) XNOEXCEPT
    {
        if (newCapacity <= m_nCapacity)
            return;

        _resize_0((newCapacity * 3) / 2, alloc);
    }

    void _copy(const T_CHAR *pszText, size_type cchText) XNOEXCEPT
    {
        _resize_1(cchText + 1);
        memcpy(m_pszText, pszText, cchText * sizeof(T_CHAR));
        m_nLength = cchText;
        m_pszText[m_nLength] = 0;
    }

    void _insert_0(size_t index, const T_CHAR *pszText, size_type cchText) XNOEXCEPT
    {
        _resize_1(m_nLength + cchText + 1);
        memmove(&m_pszText[index + cchText], &m_pszText[index], (m_nLength - index) * sizeof(T_CHAR));
        memcpy(&m_pszText[index], pszText, cchText * sizeof(T_CHAR));
        m_nLength += cchText;
        m_pszText[m_nLength] = 0;
    }
    inline void _insert(size_t index, const T_CHAR *pszText, size_type cchText) XNOEXCEPT
    {
        assert(index <= m_nLength);

        if (!cchText)
            return;

        _insert_0(index, pszText, cchText);
    }

    inline int _compare(const char *psz, const T_CHAR *pszText, size_type cchText) const XNOEXCEPT
    {
        return strncmp(psz, pszText, cchText);
    }
    inline int _compare(const wchar_t *psz, const T_CHAR *pszText, size_type cchText) const XNOEXCEPT
    {
        return wcsncmp(psz, pszText, cchText);
    }
    inline int _compare(const T_CHAR *pszText, size_type cchText) const XNOEXCEPT
    {
        return _compare(m_pszText, pszText, cchText);
    }

    inline int _icompare(const char *psz, const char *pszText, size_type cchText) const XNOEXCEPT
    {
        return _strnicmp(psz, pszText, cchText);
    }
    inline int _icompare(const wchar_t *psz, const wchar_t *pszText, size_type cchText) const XNOEXCEPT
    {
        return _wcsnicmp(psz, pszText, cchText);
    }
    inline int _icompare(const char *pszText, size_type cchText) const XNOEXCEPT
    {
        return _icompare(m_pszText, pszText, cchText);
    }
    inline int _icompare(const wchar_t *pszText, size_type cchText) const XNOEXCEPT
    {
        return _icompare(m_pszText, pszText, cchText);
    }

    inline size_type _format_v(const char *fmt, va_list va) XNOEXCEPT
    {
        char szBuf[1024];
        StringCchVPrintfA(szBuf, _countof(szBuf), fmt, va);
        *this = szBuf;
        return strlen(szBuf);
    }
    inline size_type _format_v(const wchar_t *fmt, va_list va) XNOEXCEPT
    {
        wchar_t szBuf[1024];
        StringCchVPrintfW(szBuf, _countof(szBuf), fmt, va);
        *this = szBuf;
        return wcslen(szBuf);
    }

    void _replace_0(size_type index, size_type count, const T_CHAR *pszText, size_type cchText) XNOEXCEPT
    {
        memmove(&m_pszText[index + cchText], &m_pszText[index + count], (m_nLength - count) * sizeof(T_CHAR));
        memcpy(&m_pszText[index], pszText, cchText * sizeof(T_CHAR));
        m_nLength += cchText - count;
        m_pszText[m_nLength] = 0;
    }
    inline void _replace_1(size_type index, size_type count, const T_CHAR *pszText, size_type cchText) XNOEXCEPT
    {
        assert(index <= m_nLength);
        assert(index + count <= m_nLength);

        if (count == cchText)
        {
            memcpy(&m_pszText[index], pszText, count * sizeof(T_CHAR));
            return;
        }

        if (cchText > count)
            _resize_1(m_nLength + (cchText - count) + 1);

        _replace_0(index, count, pszText, cchText);
    }

    inline void _fill_n(char *first, size_type count, char value) XNOEXCEPT
    {
#ifdef _WIN32
        FillMemory(first, count, value);
#else
        memset(first, value, count);
#endif
    }
    inline void _fill_n(wchar_t *first, size_type count, wchar_t value) XNOEXCEPT
    {
        wchar_t *end = first + count;
        while (first < end)
            *first++ = value;
    }

public:
    inline QStringT() XNOEXCEPT
        : m_pszText(m_szText)
        , m_nLength(0)
        , m_nCapacity(SSO_MAX_SIZE)
    {
        m_szText[0] = 0;
    }
    inline QStringT(const T_CHAR *pszText) XNOEXCEPT
        : m_pszText(m_szText)
        , m_nLength(0)
        , m_nCapacity(SSO_MAX_SIZE)
    {
        _copy(pszText, _length(pszText));
    }
    inline QStringT(const T_CHAR *pch0, const T_CHAR *pch1) XNOEXCEPT
        : m_pszText(m_szText)
        , m_nLength(0)
        , m_nCapacity(SSO_MAX_SIZE)
    {
        _copy(pch0, pch1 - pch0);
    }
    inline QStringT(const T_CHAR *pszText, size_type cchText) XNOEXCEPT
        : m_pszText(m_szText)
        , m_nLength(0)
        , m_nCapacity(SSO_MAX_SIZE)
    {
        _copy(pszText, cchText);
    }
    inline QStringT(const self_type& str) XNOEXCEPT
        : m_pszText(m_szText)
        , m_nLength(0)
        , m_nCapacity(SSO_MAX_SIZE)
    {
        _copy(str.m_pszText, str.m_nLength);
    }
    inline QStringT(self_type&& str) XNOEXCEPT
    {
        bool alloc = str.is_alloc();
        memcpy(this, &str, sizeof(*this));
        if (!alloc)
            m_pszText = m_szText;
        str._reset();
    }
    inline QStringT(size_type count, T_CHAR ch) XNOEXCEPT
        : m_pszText(m_szText)
        , m_nLength(0)
        , m_nCapacity(SSO_MAX_SIZE)
    {
        if (!count)
            return;

        _resize_1(count + 1);
        _fill_n(m_pszText, count, ch);
        m_nLength = count;
        m_pszText[m_nLength] = 0;
    }
    inline QStringT(std::initializer_list<T_CHAR> initList) XNOEXCEPT
        : m_pszText(m_szText)
        , m_nLength(0)
        , m_nCapacity(SSO_MAX_SIZE)
    {
        for (const T_CHAR* it = initList.begin(); it != initList.end(); ++it) {
            operator+=(*it);
        }
    }
    template <size_type t_size>
    inline QStringT(const QStringLiteral<T_CHAR, t_size>& literal) XNOEXCEPT
        : m_pszText(m_szText)
        , m_nLength(0)
        , m_nCapacity(SSO_MAX_SIZE)
    {
        _copy(literal, t_size - 1);
    }

    inline ~QStringT() XNOEXCEPT
    {
        _free();
    }

    inline       T_CHAR& operator[](size_type index)       XNOEXCEPT { return m_pszText[index]; }
    inline const T_CHAR& operator[](size_type index) const XNOEXCEPT { return m_pszText[index]; }

    inline       T_CHAR *data()       XNOEXCEPT { return m_pszText; }
    inline const T_CHAR *data() const XNOEXCEPT { return m_pszText; }
    inline const T_CHAR *c_str() const     XNOEXCEPT { return m_pszText; }
    //inline operator const T_CHAR *() const XNOEXCEPT { return m_pszText; }
    inline size_type size() const          XNOEXCEPT { return m_nLength; }
    inline bool empty() const              XNOEXCEPT { return !size();   }

    inline void clear() XNOEXCEPT
    {
        m_pszText[0] = 0;
        m_nLength = 0;
    }

    inline QStringT& operator=(const T_CHAR *pszText) XNOEXCEPT
    {
        assign(pszText);
        return *this;
    }
    inline QStringT& operator=(const self_type& str) XNOEXCEPT
    {
        if (this != &str)
            assign(str);
        return *this;
    }
    inline QStringT& operator=(self_type&& str) XNOEXCEPT
    {
        if (this == &str)
            return *this;

        assign(str);
        return *this;
    }
    template <size_type t_size>
    inline QStringT& operator=(const QStringLiteral<T_CHAR, t_size>& literal) XNOEXCEPT
    {
        assign(literal);
        return *this;
    }

    inline void assign(const T_CHAR *pszText) XNOEXCEPT
    {
        _copy(pszText, _length(pszText));
    }
    inline void assign(const T_CHAR *pszText, size_type cchText) XNOEXCEPT
    {
        _copy(pszText, cchText);
    }
    inline void assign(const T_CHAR *pch0, const T_CHAR *pch1) XNOEXCEPT
    {
        _copy(pch0, pch1 - pch0);
    }
    inline void assign(const self_type& str) XNOEXCEPT
    {
        assert(this != &str);
        _copy(str.m_pszText, str.m_nLength);
    }
    inline void assign(self_type&& str) XNOEXCEPT
    {
        assert(this != &str);
        _free();
        bool alloc = str.is_alloc();
        memcpy(this, &str, sizeof(*this));
        if (!alloc)
            m_pszText = m_szText;
        str._reset();
    }
    inline void assign(size_type count, T_CHAR ch) XNOEXCEPT
    {
        _resize_1(count + 1);
        _fill_n(m_pszText, count, ch);
        m_nLength = count;
        m_pszText[m_nLength] = 0;
    }
    template <size_type t_size>
    inline void assign(const QStringLiteral<T_CHAR, t_size>& literal) XNOEXCEPT
    {
        _copy(literal, t_size - 1);
    }

    inline void operator+=(T_CHAR ch) XNOEXCEPT
    {
        size_type newLength = m_nLength + 1;
        _resize_1(newLength + 1);
        m_pszText[m_nLength] = ch;
        m_nLength = newLength;
        m_pszText[m_nLength] = 0;
    }
    inline void operator+=(const T_CHAR *pszText) XNOEXCEPT
    {
        append(pszText);
    }
    inline void operator+=(const self_type& str) XNOEXCEPT
    {
        size_type newLength = m_nLength + str.m_nLength;
        _resize_1(newLength + 1);
        memcpy(&m_pszText[m_nLength], str.m_pszText, str.m_nLength * sizeof(T_CHAR));
        m_nLength = newLength;
        m_pszText[m_nLength] = 0;
    }
    template <size_type t_size>
    inline void operator+=(const QStringLiteral<T_CHAR, t_size>& literal) XNOEXCEPT
    {
        append(literal, t_size - 1);
    }

    inline void append(size_type count, T_CHAR ch) XNOEXCEPT
    {
        self_type str(count, ch);
        append(str.m_pszText, count);
    }
    inline void append(const T_CHAR *pszText) XNOEXCEPT
    {
        size_type cchText = _length(pszText);
        size_type newLength = m_nLength + cchText;

        _resize_1(newLength + 1);
        memcpy(&m_pszText[m_nLength], pszText, cchText * sizeof(T_CHAR));
        m_nLength = newLength;
        m_pszText[m_nLength] = 0;
    }
    inline void append(const T_CHAR *pszText, size_type cchText) XNOEXCEPT
    {
        size_type newLength = m_nLength + cchText;
        _resize_1(newLength + 1);
        memcpy(&m_pszText[m_nLength], pszText, cchText * sizeof(T_CHAR));
        m_nLength = newLength;
        m_pszText[m_nLength] = 0;
    }
    inline void append(const T_CHAR *it0, const T_CHAR *it1) XNOEXCEPT
    {
        append(it0, it1 - it0);
    }
    inline void append(const self_type& str) XNOEXCEPT
    {
        size_type newLength = m_nLength + str.m_nLength;
        _resize_1(newLength + 1);
        memcpy(&m_pszText[m_nLength], str.m_pszText, str.m_nLength * sizeof(T_CHAR));
        m_nLength = newLength;
        m_pszText[m_nLength] = 0;
    }

    inline void insert(size_type index, size_type count, T_CHAR ch) XNOEXCEPT
    {
        self_type str(count, ch);
        insert(index, str);
    }
    inline void insert(size_type index, const T_CHAR *psz) XNOEXCEPT
    {
        insert(index, psz, _length(psz));
    }
    inline void insert(size_type index, const T_CHAR *pszText, size_type cchText) XNOEXCEPT
    {
        _insert(index, pszText, cchText);
    }
    inline void insert(size_type index, const self_type& str) XNOEXCEPT
    {
        _insert(index, str.m_pszText, str.m_nLength);
    }

    inline void erase() XNOEXCEPT
    {
        clear();
    }
    inline void erase(size_type index) XNOEXCEPT
    {
        if (index < m_nLength)
            m_nLength = index;
        m_pszText[m_nLength] = 0;
    }
    inline void erase(size_type index, size_type count) XNOEXCEPT
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

    inline void replace(size_type index, size_type count, const T_CHAR *pszText) XNOEXCEPT
    {
        replace(index, count, pszText, _length(pszText));
    }
    inline void replace(size_type index, size_type count, const self_type& str) XNOEXCEPT
    {
        replace(index, count, str.m_pszText, str.m_nLength);
    }
    inline void replace(size_type index, size_type count, const T_CHAR *pszText, size_type cchText) XNOEXCEPT
    {
        _replace_1(index, count, pszText, cchText);
    }

    inline void resize(size_type length) XNOEXCEPT
    {
        _resize_1(length + 1);
        m_nLength = length;
        m_pszText[m_nLength] = 0;
    }
    inline void resize(size_type length, T_CHAR ch) XNOEXCEPT
    {
        if (length > m_nLength)
        {
            const size_type oldlen = m_nLength;
            if (m_nCapacity < length + 1)
                _resize_0(length + 1, is_alloc());

            _fill_n(&m_pszText[oldlen], length - oldlen, ch);
        }
        m_nLength = length;
        m_pszText[m_nLength] = 0;
    }

    inline void reserve(size_type length)
    {
        _resize_0(length + 1, is_alloc());
    }

    inline int compare(const T_CHAR *psz) const XNOEXCEPT
    {
        return _compare(psz, _length(psz));
    }
    inline int compare(const self_type& str) const XNOEXCEPT
    {
        return _compare(str.m_pszText, str.m_nLength);
    }

    inline int icompare(const T_CHAR *psz) const XNOEXCEPT
    {
        return _icompare(psz, _length(psz));
    }
    inline int icompare(const self_type& str) const XNOEXCEPT
    {
        return _icompare(str.m_pszText, str.m_nLength);
    }

    inline bool operator==(const T_CHAR *psz) const XNOEXCEPT
    {
        return compare(psz) == 0;
    }
    inline bool operator!=(const T_CHAR *psz) const XNOEXCEPT
    {
        return compare(psz) != 0;
    }
    inline bool operator<(const T_CHAR *psz) const XNOEXCEPT
    {
        return compare(psz) < 0;
    }
    inline bool operator>(const T_CHAR *psz) const XNOEXCEPT
    {
        return compare(psz) > 0;
    }
    inline bool operator<=(const T_CHAR *psz) const XNOEXCEPT
    {
        return compare(psz) <= 0;
    }
    inline bool operator>=(const T_CHAR *psz) const XNOEXCEPT
    {
        return compare(psz) >= 0;
    }

    inline bool operator==(const self_type& str) const XNOEXCEPT
    {
        return compare(str) == 0;
    }
    inline bool operator!=(const self_type& str) const XNOEXCEPT
    {
        return compare(str) != 0;
    }
    inline bool operator<(const self_type& str) const XNOEXCEPT
    {
        return compare(str) < 0;
    }
    inline bool operator>(const self_type& str) const XNOEXCEPT
    {
        return compare(str) > 0;
    }
    inline bool operator<=(const self_type& str) const XNOEXCEPT
    {
        return compare(str) <= 0;
    }
    inline bool operator>=(const self_type& str) const XNOEXCEPT
    {
        return compare(str) >= 0;
    }

    void swap(self_type& str) XNOEXCEPT
    {
        self_type tmp = std::move(*this);
        *this = std::move(str);
        str = std::move(tmp);
    }

    inline self_type substr() const XNOEXCEPT
    {
        return *this;
    }
    inline self_type substr(size_type index) const XNOEXCEPT
    {
        assert(index <= m_nLength);
        return self_type(&m_pszText[index], m_nLength - index);
    }
    inline self_type substr(size_type index, size_type count) const XNOEXCEPT
    {
        assert(index <= m_nLength);
        assert(index + count <= m_nLength);
        return self_type(&m_pszText[index], count);
    }

    inline size_type format(const T_CHAR *fmt, ...) XNOEXCEPT
    {
        va_list va;
        va_start(va, fmt);
        size_type ret = _format_v(fmt, va);
        va_end(va);
        return ret;
    }

    inline size_type find(T_CHAR ch, size_type index = 0) const XNOEXCEPT
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
    inline size_type find(const T_CHAR *psz, size_type index = 0) const XNOEXCEPT
    {
        return find(psz, index, _length(psz));
    }
    size_type find(const T_CHAR *pszText, size_type index, size_type cchText) const XNOEXCEPT
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
    inline size_type find(const self_type& str, size_type index = 0) const XNOEXCEPT
    {
        return find(str.m_pszText, index, str.m_nLength);
    }

    inline size_type rfind(T_CHAR ch, size_type index = 0) const XNOEXCEPT
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
    inline size_type rfind(const T_CHAR *psz, size_type index = 0) const XNOEXCEPT
    {
        return rfind(psz, index, _length(psz));
    }
    size_type rfind(const T_CHAR *pszText, size_type index, size_type cchText) const XNOEXCEPT
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
    inline size_type rfind(const self_type& str, size_type index = 0) const XNOEXCEPT
    {
        return rfind(str.m_pszText, index, str.m_nLength);
    }

    size_type ifind(T_CHAR ch, size_type index = 0) const XNOEXCEPT
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
    inline size_type ifind(const T_CHAR *psz, size_type index = 0) const XNOEXCEPT
    {
        return ifind(psz, index, _length(psz));
    }
    size_type ifind(const T_CHAR *pszText, size_type index, size_type cchText) const XNOEXCEPT
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
    inline size_type ifind(const self_type& str, size_type index = 0) const XNOEXCEPT
    {
        return ifind(str.m_pszText, index, str.m_nLength);
    }

    inline size_type find_first_of(T_CHAR ch, size_type index = 0) const XNOEXCEPT
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
    inline size_type find_first_of(const T_CHAR *pszText, size_type index = 0) const XNOEXCEPT
    {
        if (index >= m_nLength || *pszText == 0)
            return npos;

        const T_CHAR *end = &m_pszText[m_nLength];
        for (const T_CHAR *ptr = &m_pszText[index]; ptr < end; ++ptr)
        {
            for (const T_CHAR *pch = pszText; *pch; ++pch)
            {
                if (*ptr == *pch)
                    return ptr - m_pszText;
            }
        }
        return npos;
    }
    inline size_type find_first_of(const T_CHAR *pszText, size_type index, size_type cchText) const XNOEXCEPT
    {
        if (index >= m_nLength || cchText == 0)
            return npos;

        const T_CHAR *end = &m_pszText[m_nLength];
        const T_CHAR *pchTextEnd = &pszText[cchText];
        for (const T_CHAR *ptr = &m_pszText[index]; ptr < end; ++ptr)
        {
            for (const T_CHAR *pch = pszText; pch < pchTextEnd; ++pch)
            {
                if (*ptr == *pch)
                    return ptr - m_pszText;
            }
        }
        return npos;
    }
    inline size_type find_first_of(const self_type& str, size_type index = 0) const XNOEXCEPT
    {
        return find_first_of(str.m_pszText, index, str.m_nLength);
    }

    inline size_type find_first_not_of(T_CHAR ch, size_type index = 0) const XNOEXCEPT
    {
        if (index >= m_nLength)
            return npos;
        for (; index < m_nLength; ++index)
        {
            if (m_pszText[index] != ch)
                return index;
        }
        return npos;
    }
    inline size_type find_first_not_of(const T_CHAR *pszText, size_type index = 0) const XNOEXCEPT
    {
        if (index >= m_nLength)
            return npos;

        const T_CHAR *end = &m_pszText[m_nLength];
        for (const T_CHAR *ptr = &m_pszText[index]; ptr < end; ++ptr)
        {
            bool found = false;
            for (const T_CHAR *pch = pszText; *pch; ++pch)
            {
                if (*ptr == *pch)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                return ptr - m_pszText;
        }
        return npos;
    }
    inline size_type find_first_not_of(const T_CHAR *pszText, size_type index, size_type cchText) const XNOEXCEPT
    {
        if (index >= m_nLength)
            return npos;

        const T_CHAR *end = &m_pszText[m_nLength];
        const T_CHAR *pchTextEnd = &pszText[cchText];
        for (const T_CHAR *ptr = &m_pszText[index]; ptr < end; ++ptr)
        {
            bool found = false;
            for (const T_CHAR *pch = pszText; pch < pchTextEnd; ++pch)
            {
                if (*ptr == *pch)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                return ptr - m_pszText;
        }
        return npos;
    }
    inline size_type find_first_not_of(const self_type& str, size_type index = 0) const XNOEXCEPT
    {
        return find_first_not_of(str.m_pszText, index, str.m_nLength);
    }

    inline size_type find_last_of(T_CHAR ch, size_type index = npos) const XNOEXCEPT
    {
        if (m_nLength == 0)
            return npos;
        if (index >= m_nLength)
            index = m_nLength - 1;
        for (size_type i = index; i != npos; --i)
        {
            if (m_pszText[i] == ch)
                return i;
        }
        return npos;
    }
    inline size_type find_last_of(const T_CHAR *pszText, size_type index = npos) const XNOEXCEPT
    {
        if (m_nLength == 0 || *pszText == 0)
            return npos;

        if (index >= m_nLength)
            index = m_nLength - 1;

        const T_CHAR *start = m_pszText;
        const T_CHAR *end = &m_pszText[index];
        for (const T_CHAR *ptr = end; ptr >= start; --ptr)
        {
            for (const T_CHAR *pch = pszText; *pch; ++pch)
            {
                if (*ptr == *pch)
                    return ptr - start;
            }
        }
        return npos;
    }
    inline size_type find_last_of(const T_CHAR *pszText, size_type index, size_type cchText) const XNOEXCEPT
    {
        if (m_nLength == 0 || cchText == 0)
            return npos;

        if (index >= m_nLength)
            index = m_nLength - 1;

        const T_CHAR *start = m_pszText;
        const T_CHAR *end = &m_pszText[index];
        const T_CHAR *pchTextEnd = &pszText[cchText];
        for (const T_CHAR *ptr = end; ptr >= start; --ptr)
        {
            for (const T_CHAR *pch = pszText; pch < pchTextEnd; ++pch)
            {
                if (*ptr == *pch)
                    return ptr - start;
            }
        }
        return npos;
    }
    inline size_type find_last_of(const self_type& str, size_type index = npos) const XNOEXCEPT
    {
        return find_last_of(str.m_pszText, index, str.m_nLength);
    }

    inline size_type find_last_not_of(T_CHAR ch, size_type index = npos) const XNOEXCEPT
    {
        if (m_nLength == 0)
            return npos;
        if (index >= m_nLength)
            index = m_nLength - 1;
        for (size_type i = index; i != npos; --i)
        {
            if (m_pszText[i] != ch)
                return i;
        }
        return npos;
    }
    inline size_type find_last_not_of(const T_CHAR *pszText, size_type index = npos) const XNOEXCEPT
    {
        if (m_nLength == 0 || *pszText == 0)
            return npos;

        if (index >= m_nLength)
            index = m_nLength - 1;

        const T_CHAR *start = m_pszText;
        const T_CHAR *end = &m_pszText[index];
        for (const T_CHAR *ptr = end; ptr >= start; --ptr)
        {
            bool found = false;
            for (const T_CHAR *pch = pszText; *pch; ++pch)
            {
                if (*ptr == *pch)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                return ptr - start;
        }
        return npos;
    }
    inline size_type find_last_not_of(const T_CHAR *pszText, size_type index, size_type cchText) const XNOEXCEPT
    {
        if (m_nLength == 0 || cchText == 0)
            return npos;

        if (index >= m_nLength)
            index = m_nLength - 1;

        const T_CHAR *start = m_pszText;
        const T_CHAR *end = &m_pszText[index];
        const T_CHAR *pchTextEnd = &pszText[cchText];
        for (const T_CHAR *ptr = end; ptr >= start; --ptr)
        {
            bool found = false;
            for (const T_CHAR *pch = pszText; pch < pchTextEnd; ++pch)
            {
                if (*ptr == *pch)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                return ptr - start;
        }
        return npos;
    }
    inline size_type find_last_not_of(const self_type& str, size_type index = npos) const XNOEXCEPT
    {
        return find_last_not_of(str.m_pszText, index, str.m_nLength);
    }

          T_CHAR* begin()       { return m_pszText; }
    const T_CHAR* begin() const { return m_pszText; }
          T_CHAR* end()       { return &m_pszText[m_nLength]; }
    const T_CHAR* end() const { return &m_pszText[m_nLength]; }

    void push_back(T_CHAR ch)
    {
        *this += ch;
    }

    size_t hash() const
    {
        const uint8_t *pb = (const uint8_t *)m_pszText;
        size_t cb = m_nLength * sizeof(T_CHAR);
        size_t result = 0xDEADFACE + cb;

        for (size_t cdw = cb / sizeof(uint32_t); cdw > 0; cdw--)
        {
            result ^= *(const uint32_t *)pb;
            pb += 4;
        }

        if ((cb & 3) >= sizeof(uint16_t))
        {
            result ^= *(const uint16_t *)pb;
            pb += 2;
        }

        if (cb & 1)
            result ^= *pb;

        return result;
    }
}; // class QStringT

template <typename T_CHAR>
inline QStringT<T_CHAR> operator+(T_CHAR ch, const QStringT<T_CHAR>& str)
{
    QStringT<T_CHAR> ret;
    ret += ch;
    ret += str;
    return ret;
}
template <typename T_CHAR>
inline QStringT<T_CHAR> operator+(const T_CHAR *psz, const QStringT<T_CHAR>& str)
{
    QStringT<T_CHAR> ret;
    ret += psz;
    ret += str;
    return ret;
}
template <typename T_CHAR>
inline QStringT<T_CHAR> operator+(const QStringT<T_CHAR>& str, T_CHAR ch)
{
    QStringT<T_CHAR> ret;
    ret += str;
    ret += ch;
    return ret;
}
template <typename T_CHAR>
inline QStringT<T_CHAR> operator+(const QStringT<T_CHAR>& str, const T_CHAR *psz)
{
    QStringT<T_CHAR> ret;
    ret += str;
    ret += psz;
    return ret;
}
template <typename T_CHAR>
inline QStringT<T_CHAR> operator+(const QStringT<T_CHAR>& str1, const QStringT<T_CHAR>& str2)
{
    QStringT<T_CHAR> ret;
    ret.reserve(str1.size() + str2.size());
    ret += str1;
    ret += str2;
    return ret;
}

typedef QStringT<char>    QStringA;
typedef QStringT<wchar_t> QStringW;

namespace detail
{
    template <typename T_STRING, typename T_NUMBER>
    void to_QString(T_STRING& str, T_NUMBER number)
    {
        auto psz = str.data();
        auto pch = psz;

        do
        {
            *pch++ = typename T_STRING::value_type('0' + (number % 10));
            number /= 10;
        } while (number > 0);
        *pch = 0;

        str.m_nLength = pch - psz;
        assert(str.m_nLength < QStringA::SSO_MAX_SIZE);
    }
}

template <typename T_NUMBER, typename std::enable_if<std::is_signed<T_NUMBER>::value, int>::type = 0>
QStringA to_QStringA(T_NUMBER number)
{
    if (number == 0)
        return "0";

    bool minus = (number < 0);
    if (minus)
        number = -number;

    QStringA ret;
    detail::to_QString(ret, number);

    if (minus)
        ret += '-';

    std::reverse(ret.begin(), ret.end());
    return ret;
}

template <typename T_NUMBER, typename std::enable_if<std::is_unsigned<T_NUMBER>::value, int>::type = 0>
QStringA to_QStringA(T_NUMBER number)
{
    if (number == 0)
        return "0";

    QStringA ret;
    detail::to_QString(ret, number);

    std::reverse(ret.begin(), ret.end());
    return ret;
}

template <typename T_NUMBER, typename std::enable_if<std::is_signed<T_NUMBER>::value, int>::type = 0>
QStringW to_QStringW(T_NUMBER number)
{
    if (number == 0)
        return L"0";

    bool minus = (number < 0);
    if (minus)
        number = -number;

    QStringW ret;
    detail::to_QString(ret, number);

    if (minus)
        ret += L'-';

    std::reverse(ret.begin(), ret.end());
    return ret;
}

template <typename T_NUMBER, typename std::enable_if<std::is_unsigned<T_NUMBER>::value, int>::type = 0>
QStringW to_QStringW(T_NUMBER number)
{
    if (number == 0)
        return L"0";

    QStringW ret;
    detail::to_QString(ret, number);

    std::reverse(ret.begin(), ret.end());
    return ret;
}

template <>
inline QStringA to_QStringA(double number)
{
    char buf[256];
    StringCbPrintfA(buf, sizeof(buf), "%lf", number);
    return buf;
}

template <>
inline QStringW to_QStringW(double number)
{
    wchar_t buf[256];
    StringCbPrintfW(buf, sizeof(buf), L"%lf", number);
    return buf;
}

#ifdef UNICODE
    using QString = QStringW;

    template <typename T_NUMBER>
    using to_QString = decltype(to_QStringW<T_NUMBER>);
#else
    using QString = QStringA;

    template <typename T_NUMBER>
    using to_QString = decltype(to_QStringA<T_NUMBER>);
#endif

namespace std
{
    template<>
    class hash<QStringA>
    {
    public:
        inline size_t operator()(const QStringA& str) const
        {
            return str.hash();
        }
    };
    template<>
    class hash<QStringW>
    {
    public:
        inline size_t operator()(const QStringW& str) const
        {
            return str.hash();
        }
    };
} // namespace std
