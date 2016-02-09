#ifndef SRC_UTILS_HASH_HASHENGINE_H
#define SRC_UTILS_HASH_HASHENGINE_H

#include <cstdio>

namespace utils {
    namespace hash {

        template<typename T>
        class HashEngine {
        public:

            virtual size_t hashCode(const T& value) const = 0;

            size_t setIndexSize(size_t _indexSize) {
                indexSize = _indexSize;
            }

        protected:
            size_t indexSize;

        };
    }
}


#endif