#include "Tests.h"
#include "ParseBVH.h"

int main(int, char*[])
{
    CALL_TEST_FIXTURE(ParseTests);
    CALL_TEST_FIXTURE(ParseBVHTests);
    return 0;
}
