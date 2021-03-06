#ifndef SRC_KOHONEN_NEIGHADAP_NEIGHBORADAPTATION_H
#define SRC_KOHONEN_NEIGHADAP_NEIGHBORADAPTATION_H

#include <cstdio>
#include <cmath>

#include "utils/RMatrix.h"
#include "models/models.h"

namespace kohonen {
    namespace neighadap {

        /**
         * Функция подстройки соседей.
         *
         * Функция сосседства может иметь либо форму пузырька (bubble) либо
         * гауссовскую форму.
         */
        class NeighborAdaptation {
        public:

            NeighborAdaptation(size_t _xdim, size_t _ydim) : xdim(_xdim), ydim(_ydim) {
            }

            virtual void adaptation(utils::RMatrix<models::NeuronInfo, double> *somCodes,
                                    models::DataSample *inSampleRow, long bx, long by, double radius, double alpha) = 0;

            void recalculateCodeVector(double *codeVector, models::DataSample *inSampleRow, size_t dim, double alpha) {
                for (size_t i = 0; i < dim; ++i) {
                    // TODO: ignore skipped vector components
                    if (!inSampleRow[i].skipped) {
                        codeVector[i] += alpha * (inSampleRow[i].value - codeVector[i]);
                    }
                }
            }

        protected:
            size_t xdim;
            size_t ydim;

        };
    }
}

#endif
