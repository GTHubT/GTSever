#include "GT_Util_CmdParser.h"

namespace GT {
    namespace UTIL {


        GT_Util_CmdParser::GT_Util_CmdParser()
        {
            cmd_set_.clear();
        }


        GT_Util_CmdParser::~GT_Util_CmdParser()
        {
        }


        void GT_Util_CmdParser::ParserCmd(int argc, const char* argv[]) {
            if (argc <= 0) {
                return;
            }

            for (int i = 0; i < argc; i++) {
                cmd_set_.insert(std::string(argv[i]));
            }
        }

         bool GT_Util_CmdParser::IsCmdExists(std::string cmd) {
             return cmd_set_.find(cmd) != cmd_set_.end();
        }

         std::string GT_Util_CmdParser::GetCmdValue(std::string cmd) {
             if (IsCmdExists(cmd)) {
                 std::deque<std::string>::iterator iter = cmd_set_.find(cmd);
                 return *(++iter);
             }
             else {
                 return "";
             }
         }

    }
}
