#include "Private.h"
#include <iostream>

PUBLIC_API void ExampleRuntimeLibrary::hello_private()
{
    std::cerr << "Hello from ExampleRuntimeLibrary (private)" << std::endl;
}
