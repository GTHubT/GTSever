// GT_Server_Wapper.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GTUtlity/GT_Util_CmdParser.h"


int main(int argc, const char* argv[])
{
    GT::UTIL::GT_Util_CmdParser cmdparser;
    cmdparser.ParserCmd(argc, argv);
    return 0;
}

