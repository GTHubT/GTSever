// GTLibev.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "GTLibev.h"


// This is an example of an exported variable
GTLIBEV_API int nGTLibev=0;

// This is an example of an exported function.
GTLIBEV_API int fnGTLibev(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
// see GTLibev.h for the class definition
CGTLibev::CGTLibev()
{
    return;
}
