// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the GT_LIBUVWRAPPER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// GT_LIBUVWRAPPER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef GT_LIBUVWRAPPER_EXPORTS
#define GT_LIBUVWRAPPER_API __declspec(dllexport)
#else
#define GT_LIBUVWRAPPER_API __declspec(dllimport)
#endif

// This class is exported from the GT_LibuvWrapper.dll
class GT_LIBUVWRAPPER_API CGT_LibuvWrapper {
public:
	CGT_LibuvWrapper(void);
	// TODO: add your methods here.
};

extern GT_LIBUVWRAPPER_API int nGT_LibuvWrapper;

GT_LIBUVWRAPPER_API int fnGT_LibuvWrapper(void);
