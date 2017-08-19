#ifndef GT_NET_RESOURCE_MANAGER_
#define GT_NET_RESOURCE_MANAGER_

#include <unordered_set>

namespace GT {
    namespace NET {

		struct select_buffer {
			int buffer_len;
			char* data;
			select_buffer(int buffer_size) {
				buffer_len = buffer_size;
				data = new char[buffer_len];
			}
			~select_buffer() {
				if (!data) {
					delete[] data;
				}
				buffer_len = 0;
			}
		};

        class GT_Select_Resource_Manager
        {
        public:
            ~GT_Select_Resource_Manager();

			static GT_Select_Resource_Manager& GetInstance();
			select_buffer* GetSelectBuffer();
			void	ReleaseSelectBuffer(select_buffer*);
			void	AllocatCache();
			void	CleanCache();

		private:
			GT_Select_Resource_Manager();
			
		private:
			std::unordered_set<select_buffer*> buffer_cache_;
			std::unordered_set<select_buffer*> inuse_buffer_cache_;
        };
    }
}


#endif // GT_NET_RESOURCE_MANAGER_