#include "GT_util_osinfo.h"

#ifdef _WIN
#include <Windows.h>
#endif

namespace GT {

    namespace UTIL {

        int GT_util_osinfo::GetCPUNum() {
            int cpu_num = 0;

#ifdef _WIN
            SYSTEM_INFO si;
            GetSystemInfo(&si);
            cpu_num = si.dwNumberOfProcessors;
#endif

            return cpu_num;
        }

    }
}
