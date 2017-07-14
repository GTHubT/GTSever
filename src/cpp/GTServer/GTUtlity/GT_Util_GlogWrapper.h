#ifndef GT_UTIL_LOG_WRAPPER_
#define GT_UTIL_LOG_WRAPPER_



#ifndef GLOG_NO_ABBREVIATED_SEVERITIES
#define GLOG_NO_ABBREVIATED_SEVERITIES
#endif
#include <glog/logging.h>

namespace GT {
    
    namespace UTIL {

        class GT_Util_GlogWrapper
        {
        public:
            ~GT_Util_GlogWrapper();

        private:
            GT_Util_GlogWrapper();

        };
    }
}


#endif //GT_UTIL_LOG_WRAPPER_