#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <cassert>
#include <cstring>
#include <chrono>
#include "QString.h"

const int ITERATIONS = 100000000;

template <typename T_FN>
void speed_test(const char *name, T_FN fn)
{
    auto start = std::chrono::steady_clock::now();
    fn();
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << name << " time: " << duration << " ms" << std::endl;
}

void speed_test_CStringA(void)
{
    QStringA str;

    speed_test("CStringA concatnate", [&]() {
        for (int i = 0; i < ITERATIONS; ++i) {
            str += "test";
        }
    });

    str.clear();
    speed_test("QStringA concatnate 2", [&]() {
        QStringA pattern = "test";
        for (int i = 0; i < ITERATIONS; ++i) {
            str += pattern;
        }
    });

    speed_test("CStringA find 1", [&]() {
        size_t index = 0;
        for (int i = 0; i < ITERATIONS; ++i) {
            index += str.find('t');
        }
        printf("index: %d\n", (int)index);
    });

    speed_test("CStringA find 2", [&]() {
        size_t index = 0;
        for (int i = 0; i < ITERATIONS; ++i) {
            index += str.find("te");
        }
        printf("index: %d\n", (int)index);
    });

#if 0
    speed_test("QStringA string literal", [&]() {
        for (int i = 0; i < ITERATIONS; ++i) {
            QStringA str(QStringLiteral("asdflkajsldflajsdkfjalskdfajlkslalkaksjldfkjalsjdfkajslfjdaslkfdjasldf"));
            str += QStringLiteral("asdflkajsldflajsdkfjalskdfajlkslalkaksjldfkjalsjdfkajslfjdaslkfdjasldf");
        }
    });
#endif
}

void speed_test_string(void)
{
    std::string str;

    speed_test("std::string concatnate", [&]() {
        for (int i = 0; i < ITERATIONS; ++i) {
            str += "test";
        }
    });

    str.clear();
    speed_test("std::string concatnate 2", [&]() {
        std::string pattern = "test";
        for (int i = 0; i < ITERATIONS; ++i) {
            str += pattern;
        }
    });

    speed_test("std::string find 1", [&]() {
        size_t index = 0;
        for (int i = 0; i < ITERATIONS; ++i) {
            index += str.find('t');
        }
        printf("index: %d\n", (int)index);
    });

    speed_test("std::string find 2", [&]() {
        size_t index = 0;
        for (int i = 0; i < ITERATIONS; ++i) {
            index += str.find("te");
        }
        printf("index: %d\n", (int)index);
    });

#if 0
    speed_test("std::string string literal", [&]() {
        for (int i = 0; i < ITERATIONS; ++i) {
            std::string str("asdflkajsldflajsdkfjalskdfajlkslalkaksjldfkjalsjdfkajslfjdaslkfdjasldf");
            str += "asdflkajsldflajsdkfjalskdfajlkslalkaksjldfkjalsjdfkajslfjdaslkfdjasldf";
        }
    });
#endif
}

void test_QStringA(void)
{
    std::cout << "test_QStringA" << std::endl;

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
    assert(str1.find("llo") == 2);
    assert(str1.find("LLO") == str1.npos);
    assert(str1.ifind("LLO") == 2);
    assert(str1.rfind("l") == 3);

    QStringA sub = str1.substr(1, 3);
    assert(strcmp(sub.c_str(), "ell") == 0);

    QStringA fmt;
    fmt.format("%s, %d", "Number", 42);
    assert(strcmp(fmt.c_str(), "Number, 42") == 0);
}

void test_string(void)
{
    std::cout << "test_string" << std::endl;

    std::string str1("Hello");
    assert(str1.size() == 5);
    assert(strcmp(str1.c_str(), "Hello") == 0);

    std::string str2(str1);
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

    //assert(str1.icompare("hello") == 0);
    //assert(str1.icompare("WORLD") < 0);
    //assert(str1.icompare("apple") > 0);

    assert(str1.find('e') == 1);
    assert(str1.find("lo") == 3);
    assert(str1.rfind('l') == 3);
    assert(str1.rfind("el") == 1);
    //assert(str1.ifind('E') == 1);
    //assert(str1.ifind("LO") == 3);
    assert(str1.find("llo") == 2);
    assert(str1.find("LLO") == str1.npos);
    //assert(str1.ifind("LLO") == 2);
    assert(str1.rfind("l") == 3);

    std::string sub = str1.substr(1, 3);
    assert(strcmp(sub.c_str(), "ell") == 0);

    //std::string fmt;
    //fmt.format("%s, %d", "Number", 42);
    //assert(strcmp(fmt.c_str(), "Number, 42") == 0);
}

int main(void)
{
    test_QStringA();
    test_string();

    speed_test_CStringA();
    speed_test_string();

    std::cout << "All tests passed!" << std::endl;

    return 0;
}
