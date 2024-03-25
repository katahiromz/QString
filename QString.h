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
    inline size_type _length(const  CHAR *psz) const noexcept { return strlen(psz); }
    inline size_type _length(const WCHAR *psz) const noexcept { return wcslen(psz); }

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

    bool _resize(size_type nNewCapacity) noexcept
    {
        if (nNewCapacity <= m_nCapacity)
            return true;

        nNewCapacity = (nNewCapacity * 3) / 2;

        size_type nNewSize = nNewCapacity * sizeof(T_CHAR);

        bool alloc = is_alloc();
        T_CHAR *pszNew = (T_CHAR*)(alloc ? XREALLOC(m_pszText, nNewSize) : XMALLOC(nNewSize));
        if (!pszNew)
        {
            XTHROW(1);
            return false;
        }

        if (!alloc)
        {
            memcpy(pszNew, m_szText, m_nLength * sizeof(T_CHAR));
            pszNew[m_nLength] = 0;
        }

        m_pszText = pszNew;
        m_nCapacity = nNewCapacity;
        return true;
    }

    void _copy(const T_CHAR *pszText, size_type cchText) noexcept
    {
        if (!_resize(cchText + 1))
            return;

        memcpy(m_pszText, pszText, cchText * sizeof(T_CHAR));
        m_nLength = cchText;
        m_pszText[m_nLength] = 0;
    }

    void _concat(const T_CHAR *pszText, size_type cchText) noexcept
    {
        if (!cchText)
            return;

        if (!_resize(m_nLength + cchText + 1))
            return;

        memcpy(&m_pszText[m_nLength], pszText, cchText * sizeof(T_CHAR));
        m_nLength += cchText;
        m_pszText[m_nLength] = 0;
    }

    void _insert(size_t index, const T_CHAR *pszText, size_type cchText) noexcept
    {
        assert(index <= m_nLength);

        if (!cchText)
            return;

        if (!_resize(m_nLength + cchText + 1))
            return;

        memmove(&m_pszText[index + cchText], &m_pszText[index], (m_nLength - index) * sizeof(T_CHAR));
        memcpy(&m_pszText[index], pszText, cchText * sizeof(T_CHAR));
        m_nLength += cchText;
        m_pszText[m_nLength] = 0;
    }

    inline void _fill(size_type count, CHAR ch) noexcept
    {
        memset(m_pszText, ch, count);
    }
    inline void _fill(size_type index, size_type count, CHAR ch) noexcept
    {
        memset(&m_pszText[index], ch, count);
    }

    inline void _fill(size_type count, WCHAR ch) noexcept
    {
        _fill(0, count, ch);
    }
    void _fill(size_type index, size_type count, WCHAR ch) noexcept
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

    inline int _icompare(const CHAR *psz, const CHAR *pszText, size_type cchText) const noexcept
    {
        return _strnicmp(psz, pszText, cchText);
    }
    inline int _icompare(const WCHAR *psz, const WCHAR *pszText, size_type cchText) const noexcept
    {
        return _wcsnicmp(psz, pszText, cchText);
    }
    inline int _icompare(const CHAR *pszText, size_type cchText) const noexcept
    {
        return _icompare(m_pszText, pszText, cchText);
    }
    inline int _icompare(const WCHAR *pszText, size_type cchText) const noexcept
    {
        return _icompare(m_pszText, pszText, cchText);
    }

    inline size_type _format_v(const CHAR *fmt, va_list va) noexcept
    {
        CHAR szBuf[1024];
        StringCchVPrintfA(szBuf, _countof(szBuf), fmt, va);
        *this = szBuf;
        return strlen(szBuf);
    }
    inline size_type _format_v(const WCHAR *fmt, va_list va) noexcept
    {
        WCHAR szBuf[1024];
        StringCchVPrintfW(szBuf, _countof(szBuf), fmt, va);
        *this = szBuf;
        return wcslen(szBuf);
    }

public:
    QStringT() noexcept
        : m_pszText(m_szText)
        , m_nLength(0)
        , m_nCapacity(SSO_MAX_SIZE)
    {
        m_szText[0] = 0;
    }

    QStringT(const T_CHAR *pszText) noexcept
        : m_pszText(m_szText)
        , m_nLength(0)
        , m_nCapacity(SSO_MAX_SIZE)
    {
        _copy(pszText, _length(pszText));
    }

    QStringT(const T_CHAR *pch0, const T_CHAR *pch1) noexcept
        : m_pszText(m_szText)
        , m_nLength(0)
        , m_nCapacity(SSO_MAX_SIZE)
    {
        _copy(pch0, pch1 - pch0);
    }

    template <size_type t_size>
    QStringT(const QStringLiteral<T_CHAR, t_size>& literal) noexcept
        : m_pszText(m_szText)
        , m_nLength(0)
        , m_nCapacity(SSO_MAX_SIZE)
    {
        assert(literal[t_size - 1] == 0);
        _copy(literal, t_size - 1);
    }

    QStringT(const T_CHAR *pszText, size_type cchText) noexcept
        : m_pszText(m_szText)
        , m_nLength(0)
        , m_nCapacity(SSO_MAX_SIZE)
    {
        _copy(pszText, cchText);
    }

    QStringT(self_type&& str) noexcept
    {
        bool alloc = str.is_alloc();
        memcpy(this, &str, sizeof(*this));
        if (!alloc)
            m_pszText = m_szText;
        str._reset();
    }

    QStringT(size_type count, T_CHAR ch) noexcept
        : m_pszText(m_szText)
        , m_nLength(0)
        , m_nCapacity(SSO_MAX_SIZE)
    {
        if (!count)
            return;

        if (!_resize(count + 1))
            return;

        _fill(count, ch);
        m_pszText[count] = 0;
    }

    ~QStringT() noexcept
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

    QStringT& operator=(const T_CHAR *pszText) noexcept
    {
        assign(pszText);
        return *this;
    }

    QStringT& operator=(const self_type& str) noexcept
    {
        if (this == &str)
            return *this;

        assign(str);
        return *this;
    }

    template <size_type t_size>
    QStringT& operator=(const QStringLiteral<T_CHAR, t_size>& literal) noexcept
    {
        assert(literal[t_size - 1] == 0);
        assign(literal);
        return *this;
    }

    void assign(const T_CHAR *pszText) noexcept
    {
        _copy(pszText, _length(pszText));
    }
    void assign(const T_CHAR *pch0, const T_CHAR *pch1) noexcept
    {
        _copy(pch0, pch1 - pch0);
    }
    void assign(const self_type& str) noexcept
    {
        assert(this != &str);
        _copy(str.m_pszText, str.m_nLength);
    }
    template <size_type t_size>
    void assign(const QStringLiteral<T_CHAR, t_size>& literal) noexcept
    {
        assert(literal[t_size - 1] == 0);
        _copy(literal, t_size - 1);
    }
    void assign(self_type&& str) noexcept
    {
        assert(this != &str);
        _free();
        bool alloc = str.is_alloc();
        memcpy(this, &str, sizeof(*this));
        if (!alloc)
            m_pszText = m_szText;
        str._reset();
    }
    void assign(size_type count, T_CHAR ch) noexcept
    {
        if (!_resize(count + 1))
            return;

        _fill(count, ch);
        m_pszText[count] = 0;
    }

    QStringT& operator=(self_type&& str) noexcept
    {
        if (this == &str)
            return *this;

        assign(str);
        return *this;
    }

    QStringT& operator+=(T_CHAR ch) noexcept
    {
        _concat(&ch, 1);
        return *this;
    }

    QStringT& operator+=(const T_CHAR *pszText) noexcept
    {
        _concat(pszText, _length(pszText));
        return *this;
    }

    QStringT& operator+=(const self_type& str) noexcept
    {
        _concat(str.m_pszText, str.m_nLength);
        return *this;
    }

    template <size_type t_size>
    QStringT& operator+=(const QStringLiteral<T_CHAR, t_size>& literal) noexcept
    {
        assert(literal[t_size - 1] == 0);
        _concat(literal, t_size - 1);
        return *this;
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

    inline void append(size_type count, T_CHAR ch) noexcept
    {
        if (!count)
            return;
        self_type str(count, ch);
        _concat(str);
    }
    inline void append(const T_CHAR *pszText) noexcept
    {
        _concat(pszText, _length(pszText));
    }
    inline void append(const T_CHAR *pszText, size_type cchText) noexcept
    {
        _concat(pszText, cchText);
    }
    inline void append(const self_type& str) noexcept
    {
        _concat(str.m_pszText, str.m_nLength);
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

    void replace(size_type index, size_type count, const T_CHAR *pszText) noexcept
    {
        replace(index, count, pszText, _length(pszText));
    }
    void replace(size_type index, size_type count, const self_type& str) noexcept
    {
        replace(index, count, str.m_pszText, str.m_nLength);
    }
    void replace(size_type index, size_type count, const T_CHAR *pszText, size_type cchText) noexcept
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
            if (!_resize(m_nLength + (cchText - count) + 1))
                return;
        }

        memmove(&m_pszText[index + cchText], &m_pszText[index + count], (m_nLength - count) * sizeof(T_CHAR));
        memcpy(&m_pszText[index], pszText, cchText * sizeof(T_CHAR));
        m_nLength += cchText - count;
        m_pszText[m_nLength] = 0;
    }

    void resize(size_type length) noexcept
    {
        if (!_resize(length + 1))
            return;
        m_nLength = length;
        m_pszText[m_nLength] = 0;
    }
    void resize(size_type length, T_CHAR ch) noexcept
    {
        size_type len = m_nLength;
        if (!_resize(length + 1))
            return;
        if (length > len)
            _fill(len, length - len, ch);
        m_nLength = length;
        m_pszText[m_nLength] = 0;
    }

    void reserve(size_type length)
    {
        _resize(length + 1);
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

    self_type substr() const noexcept
    {
        return *this;
    }
    self_type substr(size_type index) const noexcept
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
        return find(&ch, index, 1);
    }
    inline size_type find(const T_CHAR *psz, size_type index = 0) const noexcept
    {
        return find(psz, index, _length(psz));
    }
    inline size_type find(const T_CHAR *pszText, size_type index, size_type cchText) const noexcept
    {
        for (; index + cchText < m_nLength; ++index)
        {
            if (_compare(&m_pszText[index], pszText, cchText) == 0)
                return index;
        }
        return npos;
    }
    inline size_type find(const self_type& str, size_type index = 0) const noexcept
    {
        return find(str.m_pszText, index, str.m_nLength);
    }

    inline size_type rfind(T_CHAR ch, size_type index = 0) const noexcept
    {
        return rfind(&ch, index, 1);
    }
    inline size_type rfind(const T_CHAR *psz, size_type index = 0) const noexcept
    {
        return rfind(psz, index, _length(psz));
    }
    inline size_type rfind(const T_CHAR *pszText, size_type index, size_type cchText) const noexcept
    {
        if (m_nLength < cchText)
            return npos;
        for (size_type rindex = m_nLength - cchText; rindex < m_nLength; --rindex)
        {
            if (_compare(&m_pszText[rindex], pszText, cchText) == 0)
                return rindex;
        }
        return npos;
    }
    inline size_type rfind(const self_type& str, size_type index = 0) const noexcept
    {
        return rfind(str.m_pszText, index, str.m_nLength);
    }

    inline size_type ifind(T_CHAR ch, size_type index = 0) const noexcept
    {
        return ifind(&ch, index, 1);
    }
    inline size_type ifind(const T_CHAR *psz, size_type index = 0) const noexcept
    {
        return ifind(psz, index, _length(psz));
    }
    inline size_type ifind(const T_CHAR *pszText, size_type index, size_type cchText) const noexcept
    {
        for (; index + cchText < m_nLength; ++index)
        {
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

typedef QStringT<CHAR>  QStringA;
typedef QStringT<WCHAR> QStringW;
typedef QStringT<TCHAR> QString;
