#include "Tests.h"

int main(int, char*[])
{
    CALL_TEST_FIXTURE(ParseTests);
    CALL_TEST_FIXTURE(ParseBVHTests);
    CALL_TEST_FIXTURE(USDTests);
    return 0;
}
