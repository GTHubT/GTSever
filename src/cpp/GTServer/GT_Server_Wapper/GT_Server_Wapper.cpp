// GT_Server_Wapper.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GTUtlity/GT_Util_CmdParser.h"


int main(int argc, const char* argv[])
{
    GT::UTIL::GT_Util_CmdParser cmdparser;
    cmdparser.ParserCmd(argc, argv);
    if (cmdparser.IsCmdExists("-type")) {
        std::string cmd = cmdparser.GetCmdValue("-type");
        printf("cmd = %s", cmd.c_str());
    }
    return 0;
}

