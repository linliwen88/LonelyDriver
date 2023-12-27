// TestPhysX.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#define __SNIPPET_HELLO__  // comment this to run the vehicle drive snippet

#ifdef __SNIPPET_HELLO__
extern int snippetHelloWorldMain();
#else
extern int snippetVehicleMain(int argc, const char* const* argv);
#endif


int main(int argc, const char* const* argv)
{
#ifdef __SNIPPET_HELLO__
    return snippetHelloWorldMain();
#else
    return snippetVehicleMain(argc, argv);
#endif // __SNIPPET_HELLO__
}
