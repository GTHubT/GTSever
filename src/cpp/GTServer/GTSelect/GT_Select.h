// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the GT_SELECT_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// GT_SELECT_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef GT_SELECT_EXPORTS
#define GT_SELECT_API __declspec(dllexport)
#else
#define GT_SELECT_API __declspec(dllimport)
#endif

#include "GT_Definition.h"

#include <string>

#ifdef __cplusplus
extern "C"
#endif
GT_SELECT_API bool	GTSelect_Initialize(std::string cfg_path);

#ifdef __cplusplus
extern "C"
#endif
GT_SELECT_API void	GTSelect_StartGTService();

#ifdef __cplusplus
extern "C"
#endif
GT_SELECT_API bool	GTSelect_Finalize();

#ifdef __cplusplus
extern "C"
#endif
GT_SELECT_API void	GTSelect_RegisterCallBack(EVENT_TYPE, gt_event_callback);

#ifdef __cplusplus
extern "C"
#endif
GT_SELECT_API void	GTSelect_UnRegisterCallBack(EVENT_TYPE);
