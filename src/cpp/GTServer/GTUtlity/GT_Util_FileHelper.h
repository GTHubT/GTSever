#ifndef GT_UTIL_FILEHELPER_h_
#define GT_UTIL_FILEHELPER_h_

namespace GT {

    namespace UTIL {

        class GT_Util_FileHelper
        {
        public:
            GT_Util_FileHelper();
            ~GT_Util_FileHelper();

        public:
            template<class T>
            static T ReadFileContent(T path);
        };
    }
}

#endif //GT_UTIL_FILEHELPER_h_