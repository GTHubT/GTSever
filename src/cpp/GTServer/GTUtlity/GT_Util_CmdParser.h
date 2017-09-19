#ifndef GT_UTIL_CMDPARSER_H_
#define GT_UTIL_CMDPARSER_H_

#include <vector>
#include <string>

namespace GT {
    namespace UTIL {

        class GT_Util_CmdParser
        {
        public:
            GT_Util_CmdParser();
            ~GT_Util_CmdParser();

            void ParserCmd(int argc, const char* argv[]);
            bool IsCmdExists(std::string cmd);
            std::string GetCmdValue(std::string cmd);

		private:
			std::string CmdPreProcess_(std::string cmd);
        private:
            std::vector<std::string> cmd_vector_;

        };
    }
}


#endif // 