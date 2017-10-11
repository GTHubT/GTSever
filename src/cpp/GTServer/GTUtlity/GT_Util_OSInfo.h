#ifndef GT_UTIL_OSINFO_H_
#define GT_UTIL_OSINFO_H_

#ifdef _WIN
#include <Windows.h>
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")
#endif

#ifdef LINUX
#include <sys/sysinfo.h>
#include <stdlib.h>
#endif

#include <string>
namespace GT {
    namespace UTIL {

        class GT_Util_OSInfo
        {
        public:
            static int GetCPUNum();
			static std::string GetCurrentFolder();
            static int GetRandomInt();
			static long long  Win_GetCurrentMemorySize();
			static void Try2CollectProcessMem();
        private:

        };
    }
}

#endif // GT_UTIL_OSINFO_H_
