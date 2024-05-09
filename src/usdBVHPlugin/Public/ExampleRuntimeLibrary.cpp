#include "ExampleRuntimeLibrary.h"
#include "Private.h"
#include <iostream>

PUBLIC_API void ExampleRuntimeLibrary::hello()
{
    std::cout << "Hello from ExampleRuntimeLibrary" << std::endl;
    hello_private();
}
