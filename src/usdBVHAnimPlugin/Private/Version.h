#pragma once

namespace usdBVHPlugin
{
    // Concatenate the separate version components that have been defined by
    // the build system into a single version string
    #define STR(x) #x
    char const * const c_ProjectVersion = STR(PROJECT_VERSION_MAJOR) "." STR(PROJECT_VERSION_MINOR) "." STR(PROJECT_VERSION_PATCH);
}