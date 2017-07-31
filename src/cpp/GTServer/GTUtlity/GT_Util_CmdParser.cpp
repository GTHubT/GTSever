#include "GT_Util_CmdParser.h"

#include <algorithm>
namespace GT {
    namespace UTIL {


        GT_Util_CmdParser::GT_Util_CmdParser()
        {
            cmd_vector_.clear();
        }


        GT_Util_CmdParser::~GT_Util_CmdParser()
        {
        }


        void GT_Util_CmdParser::ParserCmd(int argc, const char* argv[]) {
            if (argc <= 0) {
                return;
            }

            for (int i = 1; i < argc; i++) {
                cmd_vector_.push_back(CmdPreProcess_(std::string(argv[i])));
            }
        }

         bool GT_Util_CmdParser::IsCmdExists(std::string cmd) {
			 bool is_exist = false;
			 for_each(cmd_vector_.begin(), cmd_vector_.end(), [&](std::string iter)->void {if (iter == cmd) is_exist = true; });
             return is_exist;
        }

         std::string GT_Util_CmdParser::GetCmdValue(std::string cmd) {
			 std::string value = "";
             if (IsCmdExists(cmd)) {
				 for (std::vector < std::string >::iterator iter = cmd_vector_.begin(); iter != cmd_vector_.end(); iter++) {
					 if (*iter == cmd && ((iter++) != cmd_vector_.end())) {
						 value = *iter;
						 break;
					 }
				 }
             }
			 return value;
         }

		 std::string GT_Util_CmdParser::CmdPreProcess_(std::string cmd) {
			 std::string cmd_aft = "";
			 do {
				 if (cmd[0] == '-' && cmd[1] == '-') {
					 cmd_aft = cmd.substr(2);
					 break;
				 }
				 else if (cmd[0] == '-') {
					 cmd_aft = cmd.substr(1);
					 break;
				 }
				 else {
					 cmd_aft = cmd;
				 }
			 } while (0);
			 return cmd_aft;
		 }
    }
}
