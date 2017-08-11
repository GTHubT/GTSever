#include "GT_Util_OSInfo.h"
#include <random>
#ifdef _WIN
#include <Windows.h>
#endif

namespace GT {

    namespace UTIL {

        int GT_Util_OSInfo::GetCPUNum() {
            int cpu_num = 0;

#ifdef _WIN
            SYSTEM_INFO si;
            GetSystemInfo(&si);
            cpu_num = si.dwNumberOfProcessors;
#endif
            return cpu_num;
        }

		std::string GT_Util_OSInfo::GetCurrentFolder() {
			std::string path_;
#ifdef _WIN
			char path[MAX_PATH];
			GetModuleFileNameA(NULL, path, MAX_PATH);
			path_ = std::string(path);
			path_ = path_.substr(0,path_.rfind("\\")) + "\\";
#endif
			return path_;
		}

        int GT_Util_OSInfo::GetRandomInt() {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(1,10);
            return dist(gen);
        }

    }
}
