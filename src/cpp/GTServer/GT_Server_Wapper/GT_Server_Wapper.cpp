// GT_Server_Wapper.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GTUtlity/GT_Util_CmdParser.h"
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


int main(int argc, const char* argv[])
{
	const HMENU hMenu = GetSystemMenu(GetConsoleWindow(), FALSE); // disable close button
	DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);

	::SetConsoleCtrlHandler(NULL, true); // ignore CTRL+C https://docs.microsoft.com/en-us/windows/console/setconsolectrlhandler
	::GenerateConsoleCtrlEvent(0, 0);

    GT::UTIL::GT_Util_CmdParser cmdparser;
    cmdparser.ParserCmd(argc, argv);

	HANDLE m_event = ::CreateEvent(NULL, true, false, GLOBAL_EVENT_NAME); /* create global event for exit service */
	if (m_event == INVALID_HANDLE_VALUE) {
		printf("create global event failed!");
		return 0;
	}

	GTSERVER.Initialize();
	GTSERVER.StartGTService();

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

	GTSERVER.StopGTService();

    return 0;
}

