#ifndef GT_NET_SELECT_WRAPPER_
#define GT_NET_SELECT_WRAPPER_

#include <string>

#include "GT_Select_Core.h"
#include "GT_Definition.h"

namespace GT {
    namespace NET {

        class GT_Select_Wrapper
        {
        public:
            GT_Select_Wrapper();
            ~GT_Select_Wrapper();

		public:
			bool	Initialize(std::string cfg_path);
			bool	Finalize();

			void	RegisterSelectCallBack();
			void	RegisterCallBack(gt_event_callback cb, EVENT_TYPE type);
			void	StartService();
			void	DispatchEvent(EVENT_TYPE type, intptr_t, char*, int);
		private:
			gt_event_callback read_cb;
			gt_event_callback write_cb;
			gt_event_callback select_cb;
		private:
			GT_Select_Core	select_core_;
        };
    }
}

#endif

