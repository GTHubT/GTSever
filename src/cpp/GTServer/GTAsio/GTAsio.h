// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the GTASIO_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// GTASIO_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef GTASIO_EXPORTS
#define GTASIO_API __declspec(dllexport)
#else
#define GTASIO_API __declspec(dllimport)
#endif

// This class is exported from the GTAsio.dll
class GTASIO_API CGTAsio {
public:
	CGTAsio(void);
	// TODO: add your methods here.
};

extern GTASIO_API int nGTAsio;

GTASIO_API int fnGTAsio(void);
