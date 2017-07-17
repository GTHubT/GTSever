#include "GT_Util_CfgHelper.h"
#include "GT_Util_FileHelper.h"

#include <json/json.h>

namespace GT {

    namespace UTIL {

		std::string GT_Util_CfgHelper::cfg_content_ = "";
        GT_Util_CfgHelper::GT_Util_CfgHelper()
        {
        }


        GT_Util_CfgHelper::~GT_Util_CfgHelper()
        {
        }

		bool GT_Util_CfgHelper::LoadCfg(std::string cfgpath) {
			cfg_content_ = GT_Util_FileHelper::ReadFileContent(cfgpath);
			return true;
		}

		bool GT_Util_CfgHelper::ReadBoolCfg(std::string section, std::string key, bool defaultvalue) {
			return true;
		}

		double GT_Util_CfgHelper::ReadDoubleCfg(std::string section, std::string key, double defaultvalue) {
			return 0.0;
		}

		int GT_Util_CfgHelper::ReadIntCfg(std::string section, std::string key, int defaultvalue) {
			return 0;
		}

		std::string GT_Util_CfgHelper::ReadStringCfg(std::string section, std::string key, std::string defaultvalue) {
			return "";
		}
    }
}
