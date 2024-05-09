#include "ExampleRuntimeLibrary.h"
#include "Private.h"

int main(int, char*[])
{
    ExampleRuntimeLibrary::hello();
    ExampleRuntimeLibrary::hello_private();
    return 0;
}
