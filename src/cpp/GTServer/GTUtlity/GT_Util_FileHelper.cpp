#include "GT_Util_FileHelper.h"

#include <fstream>

namespace GT {

    namespace UTIL {

        GT_Util_FileHelper::GT_Util_FileHelper()
        {
        }


        GT_Util_FileHelper::~GT_Util_FileHelper()
        {
        }

        std::string GT_Util_FileHelper::ReadFileContent(std::string path) {
            std::fstream fs(path.c_str(), std::ios::ate | std::ios::in);
            size_t len = fs.tellg();
            if (fs.good() && len > 0) {
                char* content = new char[len];
                fs.seekg(0, std::ios::beg);
                fs.read(content, len);
                fs.close();
                return std::string(content, len);
            }
            return "";
        }

    }
}