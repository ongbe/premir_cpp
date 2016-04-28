//
// Created by ilya on 4/11/16.
//

#ifndef SRC_ML_DECISIONTREE_DECISIONTREE_H
#define SRC_ML_DECISIONTREE_DECISIONTREE_H

#include <cstdio>

#include "utils/CArrayList.h"

namespace ml {
	namespace decisiontree {

		struct TreeItem {

		};

		template<typename In>
		class DecisionTree {

			void buildTree(utils::CArrayList<In> *items) {

				// 1. split result -> matchedItems, notMatchedItems, rule

				size_t itemsSize = items->size();
				for (size_t i = 0; i < itemsSize; ++i) {
					In &itemA = (*items)[i];

				}

				// match mutual items
	            for (size_t i = 0; i < items->size(); ++i) {
	            	In &itemA = (*items)[i];

	                for (size_t j = 0; j < items->size(); ++j) {
	                    if (i == j) {
	                        break;
	                    }

	                    In &itemB = (*items)[i];

	                    // match itemA with itemB

	                }
	            }
			}

		};
	}
}

#endif
