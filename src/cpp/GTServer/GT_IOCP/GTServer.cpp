// GTServer.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "GTServer.h"


//// This is an example of an exported variable
//GTSERVER_API int nGTServer=0;
//
//// This is an example of an exported function.
//GTSERVER_API int fnGTServer(void)
//{
//    return 42;
//}
//
//// This is the constructor of a class that has been exported.
//// see GTServer.h for the class definition
//CGTServer::CGTServer()
//{
//    return;
//}


GTSERVER_API GT_ERROR_CODE GTServer_Initialize(std::string cfg_path) {
    return GT_ERROR_SUCCESS;
}
