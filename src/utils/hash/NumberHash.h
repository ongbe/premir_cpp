//
// Created by ilya on 1/6/16.
// hash code for types:
// 1) integer value
// 2) float number value if float range number is not defined
//

#ifndef SRC_UTILS_HASH_NUMBERHASH_H
#define SRC_UTILS_HASH_NUMBERHASH_H

#include <cstdio>

#include "HashEngine.h"

namespace utils {
    namespace hash {

        template<typename T>
        class NumberHash : public HashEngine<T> {
        public:
            NumberHash(size_t _indexSize) : indexSize(_indexSize) {
            }

            size_t setIndexSize(size_t _indexSize) {
                indexSize = _indexSize;
            }

            size_t hashCode(const T& value) const {
                return ((size_t)value) % indexSize;
            }

        private:
            size_t indexSize;
        };
    }
}


#endif
