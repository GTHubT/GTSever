#ifndef GT_UTIL_OSINFO_H_
#define GT_UTIL_OSINFO_H_

#include <string>
namespace GT {
    namespace UTIL {

        class GT_Util_OSInfo
        {
        public:
            static int GetCPUNum();
			static std::string GetCurrentFolder();
        };
    }
}

#endif // GT_UTIL_OSINFO_H_
