#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <cassert>
#include <cstring>
#include "QString.h"

int main(void)
{
    QStringA str1("Hello");
    assert(str1.size() == 5);
    assert(strcmp(str1.c_str(), "Hello") == 0);

    QStringA str2(str1);
    assert(str1 == str2);

    str2.clear();
    assert(str2.empty());

    str2 = "World";
    assert(str2.size() == 5);
    assert(strcmp(str2.c_str(), "World") == 0);

    str2 += '!';
    assert(str2.size() == 6);
    assert(strcmp(str2.c_str(), "World!") == 0);

    str2.insert(5, " Hello");
    assert(strcmp(str2.c_str(), "World Hello!") == 0);

    str2.erase(5, 6);
    assert(strcmp(str2.c_str(), "World!") == 0);

    str2.replace(0, 5, "Goodbye");
    assert(strcmp(str2.c_str(), "Goodbye!") == 0);

    assert(str1.compare("Hello") == 0);
    assert(str1.compare("World") < 0);
    assert(str1.compare("Apple") > 0);

    assert(str1.icompare("hello") == 0);
    assert(str1.icompare("WORLD") < 0);
    assert(str1.icompare("apple") > 0);

    assert(str1.find('e') == 1);
    assert(str1.find("lo") == 3);
    assert(str1.rfind('l') == 3);
    assert(str1.rfind("el") == 1);
    assert(str1.ifind('E') == 1);
    assert(str1.ifind("LO") == 3);

    QStringA sub = str1.substr(1, 3);
    assert(strcmp(sub.c_str(), "ell") == 0);

    QStringA fmt;
    fmt.format("%s, %d", "Number", 42);
    assert(strcmp(fmt.c_str(), "Number, 42") == 0);

    std::cout << "All tests passed!" << std::endl;

    return 0;
}
