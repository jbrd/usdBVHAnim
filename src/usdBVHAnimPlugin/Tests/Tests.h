#pragma once
#include <cstdio>

#define BEGIN_TEST_FIXTURE(name) \
    void Fixture_##name()        \
    {

#define END_TEST_FIXTURE() \
    }

#define CALL_TEST_FIXTURE(name)   \
    extern void Fixture_##name(); \
    Fixture_##name();

#define TEST(name) \
    printf("Running '%s'...\n", #name);

#define TEST_REQUIRE(exp)                                                 \
    if (!(exp)) {                                                         \
        printf("\t%s:%d - test '%s' failed\n", __FILE__, __LINE__, #exp); \
        exit(1);                                                          \
    }