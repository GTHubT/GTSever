// GT_Libevent.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "GT_Libevent.h"


// This is an example of an exported variable
GT_LIBEVENT_API int nGT_Libevent=0;

// This is an example of an exported function.
GT_LIBEVENT_API int fnGT_Libevent(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
// see GT_Libevent.h for the class definition
CGT_Libevent::CGT_Libevent()
{
    return;
}
