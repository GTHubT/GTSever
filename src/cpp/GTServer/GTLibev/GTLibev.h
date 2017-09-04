// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the GTLIBEV_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// GTLIBEV_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef GTLIBEV_EXPORTS
#define GTLIBEV_API __declspec(dllexport)
#else
#define GTLIBEV_API __declspec(dllimport)
#endif

// This class is exported from the GTLibev.dll
class GTLIBEV_API CGTLibev {
public:
	CGTLibev(void);
	// TODO: add your methods here.
};

extern GTLIBEV_API int nGTLibev;

GTLIBEV_API int fnGTLibev(void);
