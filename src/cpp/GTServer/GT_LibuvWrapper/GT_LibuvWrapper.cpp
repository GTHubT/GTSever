// GT_LibuvWrapper.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "GT_LibuvWrapper.h"


// This is an example of an exported variable
GT_LIBUVWRAPPER_API int nGT_LibuvWrapper=0;

// This is an example of an exported function.
GT_LIBUVWRAPPER_API int fnGT_LibuvWrapper(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
// see GT_LibuvWrapper.h for the class definition
CGT_LibuvWrapper::CGT_LibuvWrapper()
{
    return;
}
