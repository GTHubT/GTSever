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

// This class is exported from the GTServer.dll
class GTSERVER_API CGTServer {
public:
	CGTServer(void);
	// TODO: add your methods here.
};

extern GTSERVER_API int nGTServer;

GTSERVER_API int fnGTServer(void);
