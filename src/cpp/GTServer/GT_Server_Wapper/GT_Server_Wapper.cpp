// GT_Server_Wapper.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GTUtlity/GT_Util_CmdParser.h"
#include "GTUtlity/GT_Util_OSInfo.h"
#include "GT_Module_Wrapper.h"

#include <iostream>
#include <Windows.h>

#ifndef GTSERVER
#define GTSERVER GT::MODULE::GT_Module_Wrapper::GetInstance()
#endif

#ifndef GLOBAL_EVENT_NAME	
#define GLOBAL_EVENT_NAME	L"Global\\EVENT_GTService_Global_Exit_Event"
#endif

void CmdHelper() {
    std::wcout << L"-type [IOCP|Select] default is IOCP" << std::endl;
}

void CreateAndWaitEvent() {
    HANDLE m_event = ::CreateEvent(NULL, true, false, GLOBAL_EVENT_NAME); /* create global event for exit service */
    if (m_event == INVALID_HANDLE_VALUE) {
        printf("create global event failed!");
        return;
    }
    
    while (1) {

        DWORD retcode = ::WaitForSingleObject(m_event, INFINITE);
        if (retcode == WAIT_OBJECT_0) {
            char c = '0';
            std::cout << "do you mean to end the GT Service? (y/n) ";
            std::cin >> c;
            if (c == 'y') {
                ::CloseHandle(m_event);
                break;
            }
            ::ResetEvent(m_event);
        }
    }
}


int main(int argc, const char* argv[])
{
	const HMENU hMenu = GetSystemMenu(GetConsoleWindow(), FALSE); // disable close button
	DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);

	::SetConsoleCtrlHandler(NULL, true); // ignore CTRL+C https://docs.microsoft.com/en-us/windows/console/setconsolectrlhandler
	::GenerateConsoleCtrlEvent(0, 0);

    GT::UTIL::GT_Util_CmdParser cmdparser;
    cmdparser.ParserCmd(argc, argv);

    if (cmdparser.IsCmdExists("type")) {
        std::string value = cmdparser.GetCmdValue("type");
        if (value == "IOCP") {
            GTSERVER.SetModuleType(GT::MODULE::GT_IOCP);
        }
        else if (value == "Select") {
            GTSERVER.SetModuleType(GT::MODULE::GT_Selete);
        }
        else {
            CmdHelper();
        }
    }


	std::string config_path = GT::UTIL::GT_Util_OSInfo::GetCurrentFolder() + "GTServer.cfg";
	GTSERVER.InitLogService(config_path);

	bool ret = GTSERVER.Initialize();
	if (!ret) {
		printf("GT Service Init Failed! ");
        GTSERVER.ExitGTService();
		return 0;
	}
	ret = GTSERVER.StartGTService();
	if (!ret) {
        GTSERVER.ExitGTService();
		printf("GT Service Start Failed! ");
		return 0;
	}

    CreateAndWaitEvent();
	GTSERVER.ExitGTService();

    return 0;
}

