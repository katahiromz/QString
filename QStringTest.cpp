#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <cassert>
#include <cstring>
#include <chrono>
#include "QString.h"

const int ITERATIONS = 100000000;
const char* pattern = "test";

void speed_test_CStringA(void)
{
    QStringA qstr;
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < ITERATIONS; ++i) {
        qstr += pattern;
    }
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "QStringA concatenation time: " << duration << " ms" << std::endl;

    start = std::chrono::steady_clock::now();
    for (int i = 0; i < ITERATIONS; ++i) {
        size_t index = qstr.find('t');
    }
    end = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "QStringA find time: " << duration << " ms" << std::endl;
}

void speed_test_string(void)
{
    std::string stdstr;
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < ITERATIONS; ++i) {
        stdstr += pattern;
    }
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "std::string concatenation time: " << duration << " ms" << std::endl;

    start = std::chrono::steady_clock::now();
    for (int i = 0; i < ITERATIONS; ++i) {
        size_t index = stdstr.find('t');
    }
    end = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "std::string find time: " << duration << " ms" << std::endl;
}

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

    speed_test_CStringA();
    speed_test_string();

    std::cout << "All tests passed!" << std::endl;

    return 0;
}
