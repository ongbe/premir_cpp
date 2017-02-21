#include "cache/str-intern.h"

namespace cache {
	
	/* initialize global string pool hashmap */
	static LinkedHashMap<pool_entry> interningMap(0);
	
    const void *memintern(const void *data, size_t len) {

		//if (!map.tablesize)
			//hashmap_init(&map, (hashmap_cmp_fn) pool_entry_cmp, 0);

		/* lookup interned string in pool */
		pool_entry key(memhash(data, len)); 
		key.len = len;
		//char dp[255];
		//char* dp2 = dp;
		//dp = (char*) data;	
		//std::memcpy(key.data, data, 1);
		//key.data = (unsigned char *)data;
		//unsigned char *dpp = &(key.data[0]);
		//key.data[0] = data;
		pool_entry* e = interningMap.getEntry(&key);
		if (!e) {
			/* not found: create it */
			/* allocate memory sizeof struct + data length */
			size_t amount = sizeof(pool_entry) + len;
			e = (pool_entry*)std::calloc(1, amount);
			if (e == nullptr) {
				throw std::runtime_error(std::strerror(errno));
			}
			/* copy data to the end of struct */
			std::memcpy(e->data, data, len);
			e->ent.hash = key.ent.hash;
			e->ent.next = nullptr;
			e->len = len;
			interningMap.add(e);
		}
		return e->data;
	}

	const char *strintern(const char *str) {
		return (char*) memintern(str, strlen(str));
	}

}


