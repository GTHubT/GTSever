#include "GT_Util_CfgHelper.h"
#include "GT_Util_FileHelper.h"
#include "GT_Util_GlogWrapper.h"


namespace GT {

    namespace UTIL {

		std::string GT_Util_CfgHelper::cfg_content_ = "";
		Json::Value GT_Util_CfgHelper::cfg_root_;
		bool GT_Util_CfgHelper::cfg_load_ = false;

        GT_Util_CfgHelper::GT_Util_CfgHelper()
        {
        }


        GT_Util_CfgHelper::~GT_Util_CfgHelper()
        {
        }

		bool GT_Util_CfgHelper::LoadCfg(std::string cfgpath) {
			if (cfg_load_)
				return cfg_load_;

			cfg_content_ = GT_Util_FileHelper::ReadFileContent(cfgpath);
			if (!cfg_content_.empty()) {
				Json::Reader reader;
				bool ret = reader.parse(cfg_content_, cfg_root_);
				if (!ret)
					printf("CFG parser failed!");
				else
					cfg_load_ = true;
				return ret;
			}
			printf("CFG file not found or the file is empty!\n");
			return !cfg_content_.empty();
		}

		bool GT_Util_CfgHelper::ReadBoolCfg(std::string section, std::string key, bool defaultvalue) {
			if (cfg_root_[section][key].isBool()) {
				return cfg_root_[section][key].asBool();
			}
			return defaultvalue;
		}

		double GT_Util_CfgHelper::ReadDoubleCfg(std::string section, std::string key, double defaultvalue) {
			if (cfg_root_[section][key].isDouble()) {
				return cfg_root_[section][key].asDouble();
			}
			return defaultvalue;
		}

		int GT_Util_CfgHelper::ReadIntCfg(std::string section, std::string key, int defaultvalue) {
			if (cfg_root_[section][key].isInt()) {
				return cfg_root_[section][key].asInt();
			}
			return defaultvalue;
		}

		std::string GT_Util_CfgHelper::ReadStringCfg(std::string section, std::string key, std::string defaultvalue) {
			if (cfg_root_[section][key].isString()) {
				return cfg_root_[section][key].asString();
			}
			return defaultvalue;
		}
    }
}
