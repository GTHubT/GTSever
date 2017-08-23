#include "GT_Select_Resource_Manager.h"
#include "GTUtlity/GT_Util_GlogWrapper.h"
#include "GTUtlity/GT_Util_CfgHelper.h"

namespace GT {
    namespace NET {

        GT_Select_Resource_Manager::GT_Select_Resource_Manager()
        {
			buffer_cache_.clear();
        }


        GT_Select_Resource_Manager::~GT_Select_Resource_Manager()
        {
        }

		GT_Select_Resource_Manager& GT_Select_Resource_Manager::GetInstance() {
			static GT_Select_Resource_Manager re_instance;
			return re_instance;
		}

		void GT_Select_Resource_Manager::AllocatCache() {
			GT_TRACE_FUNCTION;
			int allocat_size = GT_READ_CFG_INT("select_control", "buffer_cache_size", 100);
			int buffer_size = GT_READ_CFG_INT("select_control", "buffer_size", 2048);

			for (int i = 0; i < allocat_size; i++) {
				//select_buffer* temp_ptr = new select_buffer(buffer_size);
				buffer_cache_.insert(new select_buffer(buffer_size));
			}
		}

		select_buffer* GT_Select_Resource_Manager::GetSelectBuffer() {
			if (buffer_cache_.empty()) {
				AllocatCache();
			}

			select_buffer* bu = *(buffer_cache_.begin());
			inuse_buffer_cache_.insert(bu);
			buffer_cache_.erase(buffer_cache_.begin());
			return bu;
		}

		void GT_Select_Resource_Manager::ReleaseSelectBuffer(select_buffer* bu) {
			auto& iter = inuse_buffer_cache_.find(bu);
			if (iter != inuse_buffer_cache_.end()) {
				delete *iter;
			}
			inuse_buffer_cache_.erase(iter);
		}

		void GT_Select_Resource_Manager::CleanCache() {
			for (auto& iter : buffer_cache_) {
				delete iter;
			}
			for (auto& iter : inuse_buffer_cache_) {
				delete iter;
			}
			buffer_cache_.clear();
			inuse_buffer_cache_.clear();
		}

    }
}
