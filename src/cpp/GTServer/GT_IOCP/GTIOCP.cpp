// GTServer.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "GTIOCP.h"


#ifndef GTSERVER_MANAGER
#define GTSERVER_MANAGER	GT::NET::GT_ServerManager::GetInstance()
#endif // !GTIOCP


GTSERVER_API GT_ERROR_CODE GTIOCP_InitLogService(std::string cfg_path) {
	GT_TRACE_FUNCTION;
	GT_LOG_INFO("Initiaize GT Server log service, config file path = " << cfg_path.c_str());
	GTSERVER_MANAGER.InitLogAndCfgSrvice(cfg_path);
	return GT_ERROR_SUCCESS;
}


GTSERVER_API GT_ERROR_CODE GTIOCP_Initialize() {
	GT_TRACE_FUNCTION;
	GT_ERROR_CODE retcode = GT_ERROR_FAILED;
	bool ret = GTSERVER_MANAGER.Initialize();
	ret ? retcode = GT_ERROR_SUCCESS : ret = GT_ERROR_SUCCESS;
    return retcode;
}


GTSERVER_API GT_ERROR_CODE GTIOCP_StartService() {
	GT_TRACE_FUNCTION;
	GT_LOG_INFO("Now start GT Server service...");
	GT_ERROR_CODE retcode = GT_ERROR_FAILED;
	GTSERVER_MANAGER.GTStartService();
	return GT_ERROR_SUCCESS;
}


GTSERVER_API GT_ERROR_CODE GTIOCP_RefisterEventCallBack(IO_EVENT_TYPE type, GT::NET::CallBackFunc call_back) {
	GT_TRACE_FUNCTION;
	GT_LOG_INFO("Register IO Event worker, Event type = " << type);

	switch (type)
	{
	case IO_EVENT_READ:
		GTSERVER_MANAGER.RegisterServerCallBack(IO_EVENT_READ, call_back);
		GT_LOG_INFO("Register Read Event success!");
		break;
	case IO_EVENT_WRITE:
		GTSERVER_MANAGER.RegisterServerCallBack(IO_EVENT_WRITE, call_back);
		GT_LOG_INFO("Register Write Event success!");
		break;
	default:
		GT_LOG_INFO("illegal IO Event call back!");
		break;
	}

	return GT_ERROR_SUCCESS;
}


GTSERVER_API void GTIOCP_WriteMessage(PULONG_PTR completion_key, char* data, size_t len) {
	GT_TRACE_FUNCTION;
	GT_LOG_INFO("Write Message, Message len = " << len << ", Message data = " << data);
	GTSERVER_MANAGER.PostWriteEvent(completion_key, data, len);
}


GTSERVER_API GT_ERROR_CODE GTIOCP_UnRegisterEventCallBack(IO_EVENT_TYPE type) {
	GT_TRACE_FUNCTION;
	GT_LOG_INFO("unregister io event call back, io event type = " << type);

	switch (type)
	{
	case IO_EVENT_READ:
		GTSERVER_MANAGER.UnRegisterServerCallBack(IO_EVENT_READ);
		GT_LOG_INFO("UnRegister Read Event success!");
		break;
	case IO_EVENT_WRITE:
		GTSERVER_MANAGER.UnRegisterServerCallBack(IO_EVENT_WRITE);
		GT_LOG_INFO("UnRegister Write Event success!");
		break;
	default:
		GT_LOG_INFO("illegal IO Event call back!");
		break;
	}

	return GT_ERROR_SUCCESS;
}


GTSERVER_API GT_ERROR_CODE GTIOCP_Uninitialize() {
	GT_TRACE_FUNCTION;
	GT_LOG_INFO("GT Server stopping...");
	GTSERVER_MANAGER.StopService();
	return GT_ERROR_SUCCESS;
}