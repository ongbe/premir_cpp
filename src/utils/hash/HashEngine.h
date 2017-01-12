#ifndef SRC_UTILS_HASH_HASHENGINE_H
#define SRC_UTILS_HASH_HASHENGINE_H

#include <cstdio>
#include <cstring>

namespace utils {
    namespace hash {

        template<typename T>
        class HashEngine {
        public:

            HashEngine(size_t _indexSize=DEFAULT_TABLE_SIZE) :
			   	indexSize(_indexSize) {
            }

            void setIndexSize(size_t _indexSize) {
                indexSize = _indexSize;
            }

			size_t charsHash(const char* str, const size_t& len) const {
				return DEKHash(str, len);
			}

            size_t hashIndex(const T& value) const {
			   return hashCode(value)%indexSize;
			}

        protected:
            size_t indexSize;

			virtual size_t hashCode(const T& value) const = 0;

			/**
			 * An algorithm proposed by Donald E. Knuth in The Art Of Computer Programming Volume 3,
			 * under the topic of sorting and search chapter 6.4.
			 */
			size_t DEKHash(const char* str, const size_t& len) const {
				size_t hash = len;
				for(size_t i = 0; i < len; str++, i++) {
					hash = ((hash << 5) ^ (hash >> 27)) ^ (*str);
				}
				return hash;
			}
			
        public:
            // наилучшее случайное распределние занений хэш функции получается,
            // если размер таблицы индексов равен простому числу
            const static size_t DEFAULT_TABLE_SIZE = 1021;

        };
    }
}


#endif
