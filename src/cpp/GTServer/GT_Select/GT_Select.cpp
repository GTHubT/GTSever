// GT_Select.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "GT_Select.h"


// This is an example of an exported variable
GT_SELECT_API int nGT_Select=0;

// This is an example of an exported function.
GT_SELECT_API int fnGT_Select(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
// see GT_Select.h for the class definition
CGT_Select::CGT_Select()
{
    return;
}
