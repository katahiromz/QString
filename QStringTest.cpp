#include <windows.h>
#include <stdio.h>
#include <string>
#include <cstddef>

HANDLE g_hHeap = GetProcessHeap();

inline LPVOID Alloc(SIZE_T size)
{
    return HeapAlloc(g_hHeap, 0, size);
}

inline void Free(LPVOID ptr)
{
    HeapFree(g_hHeap, 0, ptr);
}

inline LPVOID ReAlloc(LPVOID ptr, SIZE_T size)
{
    if (!ptr)
        return Alloc(size);

    return HeapReAlloc(g_hHeap, 0, ptr, size);
}

#define XMALLOC Alloc
#define XFREE Free
#define XREALLOC ReAlloc

#include "QString.h"

#define PRINT(str) printf("Line %d: " #str ": '%s', %d\n", __LINE__, str.c_str(), (int)str.size());

void TEST_QStringA(void)
{
    QStringA str1, str2;

    PRINT(str1);
    str1 += 'A';
    PRINT(str1);
    str1 += "BB";
    PRINT(str1);
    str1 += "CCC";
    PRINT(str1);
    str1 = "TEST";
    PRINT(str1);
    str1 = "ABC";
    PRINT(str1);
    str1 += "CDEF";
    PRINT(str1);
    str1.insert(0, "<<<");
    PRINT(str1);
    str1.insert(str1.size(), ">>>");
    PRINT(str1);
    str2 = std::move(str1);
    PRINT(str2);
    str2.erase(3, 7);
    PRINT(str2);
    str2.erase(3);
    PRINT(str2);
    str2.erase();
    PRINT(str2);
    str1 = "01234567";
    PRINT(str1);
    str1.replace(1, 3, "<><><>");
    PRINT(str1);
    str1.replace(1, 5, "##");
    PRINT(str1);
    str1.clear();
    PRINT(str1);
    str1 += "JJ";
    PRINT(str1);
    str1.resize(10, 'Z');
    PRINT(str1);
    str1.swap(str2);
    PRINT(str1);
    PRINT(str2);
    size_t index = str2.find("Z");
    printf("%d\n", (int)index);
}

void TEST_string(void)
{
    std::string str1, str2;

    PRINT(str1);
    str1 += 'A';
    PRINT(str1);
    str1 += "BB";
    PRINT(str1);
    str1 += "CCC";
    PRINT(str1);
    str1 = "TEST";
    PRINT(str1);
    str1 = "ABC";
    PRINT(str1);
    str1 += "CDEF";
    PRINT(str1);
    str1.insert(0, "<<<");
    PRINT(str1);
    str1.insert(str1.size(), ">>>");
    PRINT(str1);
    str2 = std::move(str1);
    PRINT(str2);
    str2.erase(3, 7);
    PRINT(str2);
    str2.erase(3);
    PRINT(str2);
    str2.erase();
    PRINT(str2);
    str1 = "01234567";
    PRINT(str1);
    str1.replace(1, 3, "<><><>");
    PRINT(str1);
    str1.replace(1, 5, "##");
    PRINT(str1);
    str1.clear();
    PRINT(str1);
    str1 += "JJ";
    PRINT(str1);
    str1.resize(10, 'Z');
    PRINT(str1);
    str1.swap(str2);
    PRINT(str1);
    PRINT(str2);
    size_t index = str2.find("Z");
    printf("%d\n", (int)index);
}

int main(void)
{
    DWORD dwTick0, dwTick1;

    puts("TEST_QStringA");
    TEST_QStringA();

    puts("TEST_string");
    TEST_string();

    return 0;
}
