#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.h>
#include <iostream>

int main() {
    cl_uint numPlatforms;
    clGetPlatformIDs(0, NULL, &numPlatforms);
    std::cout << "Number of platforms: " << numPlatforms << std::endl;

    cl_platform_id* platforms = new cl_platform_id[numPlatforms];
    clGetPlatformIDs(numPlatforms, platforms, NULL);

    for (cl_uint i = 0; i < numPlatforms; ++i) {
        char platformName[128];
        clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(platformName), platformName, NULL);
        std::cout << "Platform " << i << ": " << platformName << std::endl;
    }

    delete[] platforms;
    return 0;
}
