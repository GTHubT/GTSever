#ifndef GT_UTIL_OSINFO_H_
#define GT_UTIL_OSINFO_H_

namespace GT {
    namespace UTIL {

        class GT_util_osinfo
        {
        public:
            GT_util_osinfo();
            ~GT_util_osinfo();

        public:
            static int GetCPUNum();
        };
    }
}

#endif // GT_UTIL_OSINFO_H_
