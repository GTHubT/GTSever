// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the GTSERVER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// GTSERVER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef GTSERVER_EXPORTS
#define GTSERVER_API __declspec(dllexport)
#else
#define GTSERVER_API __declspec(dllimport)
#endif


#include "GT_Definition.h"
#include "GT_ServerManager.h"
#include "GTUtlity/GT_Util_GlogWrapper.h"

#include <string>

//GTSERVER_API GT_ERROR_CODE GTIOCP_InitLogService(std::string cfg_path);

GTSERVER_API GT_ERROR_CODE GTIOCP_Initialize(std::string cfg_path);

GTSERVER_API GT_ERROR_CODE GTIOCP_StartService();

GTSERVER_API GT_ERROR_CODE GTIOCP_Uninitialize();

GTSERVER_API GT_ERROR_CODE GTIOCP_RegisterEventCallBack(IO_EVENT_TYPE, GT::NET::CallBackFunc call_back);

GTSERVER_API GT_ERROR_CODE GTIOCP_UnRegisterEventCallBack(IO_EVENT_TYPE);

GTSERVER_API void GTIOCP_WriteMessage(PULONG_PTR, char*, size_t);



