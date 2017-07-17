#ifndef GT_UTIL_FILEHELPER_h_
#define GT_UTIL_FILEHELPER_h_

#include <string>
namespace GT {

    namespace UTIL {

        class GT_Util_FileHelper
        {
        public:
            GT_Util_FileHelper();
            ~GT_Util_FileHelper();

        public:
            static std::string ReadFileContent(std::string path);
        };
    }
}

#endif //GT_UTIL_FILEHELPER_h_