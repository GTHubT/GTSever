#ifndef GT_NET_SERVER_MANAGER_H_
#define GT_NET_SERVER_MANAGER_H_

/***********************************************************************************************/
/* this class serve as the only interface for dll interface call IOCP Service and other module */
/***********************************************************************************************/

namespace GT {

    namespace NET {

        class GT_ServerManager
        {
        public:
            GT_ServerManager();
            ~GT_ServerManager();
        };
    }
}

#endif //GT_NET_SERVER_MANAGER_H_