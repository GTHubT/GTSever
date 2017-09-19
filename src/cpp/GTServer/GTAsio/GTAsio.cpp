// GTAsio.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "GTAsio.h"


// This is an example of an exported variable
GTASIO_API int nGTAsio=0;

// This is an example of an exported function.
GTASIO_API int fnGTAsio(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
// see GTAsio.h for the class definition
CGTAsio::CGTAsio()
{
    return;
}
