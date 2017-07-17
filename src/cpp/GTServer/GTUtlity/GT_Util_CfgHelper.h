#ifndef GT_UTIL_CFGHELPER_H_
#define GT_UTIL_CFGHELPER_H_

#include <string>
namespace GT {

    namespace UTIL {

        class GT_Util_CfgHelper
        {
        public:
            GT_Util_CfgHelper();
            ~GT_Util_CfgHelper();

        public:
            static bool         LoadCfg(std::string cfgpath);
            static bool         ReadBoolCfg(std::string section, std::string key, bool defaultvalue);
            static int          ReadIntCfg(std::string section, std::string key, int defaultvalue);
            static double       ReadDoubleCfg(std::string section, std::string key, double defaultvalue);
            static std::string  ReadStringCfg(std::string section, std::string key, std::string defaultvalue);
        };

#define GT_READ_CFG_BOOL        GT_Util_CfgHelper::ReadBoolCfg
#define GT_READ_CFG_INT         GT_Util_CfgHelper::ReadIntCfg
#define GT_READ_CFG_DOUBLE      GT_Util_CfgHelper::ReadDoubleCfg
#define GT_READ_CFG_STRING      GT_Util_CfgHelper::ReadStringCfg
    }
}
#endif //GT_UTIL_CFGHELPER_H_

