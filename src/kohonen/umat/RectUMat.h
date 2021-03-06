#ifndef SRC_KOHONEN_UMAT_RECTUMAT_H
#define SRC_KOHONEN_UMAT_RECTUMAT_H

#include <cstdio>
#include <cmath>
#include <iostream>

#include "UMat.h"
#include "utils/R3DMatrix.h"

namespace kohonen {
    namespace umat {

        class RectUMat : public UMat {
        public:

            typedef utils::RMatrix<models::NeuronInfo, double> SomCodes;
            typedef utils::RMatrix<models::NeuronInfo, double> UMatCodes;
            typedef utils::R3DMatrix<bool, models::NeuronInfo, double> SOMMatrix;

            RectUMat(size_t _xdim, size_t _ydim, size_t _dim) :
                    UMat(_xdim, _ydim, _dim) {
            }

            void buildUMatrix() {
                SOMMatrix &mvalue = (*UMat::somMatrix);
                UMatCodes &uvalue = (*UMat::uMatrix);

                size_t xdim = UMat::xdim;
                size_t ydim = UMat::ydim;
                size_t dim = UMat::dim;
                size_t uxdim = UMat::uxdim;
                size_t uydim = UMat::uydim;

                for (size_t j = 0; j < ydim; ++j) {
                    for (size_t i = 0; i < xdim; ++i) {
                        double dx = 0, dy = 0, dz1 = 0, dz2 = 0;
                        long count = 0, bx = 0, by = 0, bz = 0;


                        for (size_t k = 0; k < dim; k++) {

                            if (i < (xdim - 1)) {
                                double temp = (mvalue[i][j][k] - mvalue[i + 1][j][k]);
                                dx += temp * temp;
                                bx = 1;
                            }
                            if (j < (ydim - 1)) {
                                double temp = (mvalue[i][j][k] - mvalue[i][j + 1][k]);
                                dy += temp * temp;
                                by = 1;
                            }

                            if (j < (ydim - 1) && i < (xdim - 1)) {
                                double temp = (mvalue[i][j][k] - mvalue[i + 1][j + 1][k]);
                                dz1 += temp * temp;
                                temp = (mvalue[i][j + 1][k] - mvalue[i + 1][j][k]);
                                dz2 += temp * temp;
                                bz = 1;
                            }
                        }
                        double dz = (sqrt(dz1) / std::sqrt((double) 2.0) + std::sqrt(dz2) / std::sqrt((double) 2.0)) / 2;
                        if (bx) {
                            uvalue[2 * i + 1][2 * j] = std::sqrt(dx);
                        }
                        if (by) {
                            uvalue[2 * i][2 * j + 1] = std::sqrt(dy);
                        }
                        if (bz) {
                            uvalue[2 * i + 1][2 * j + 1] = dz;
                        }

                    }
                }

                /* medians of the 4-neighborhood */
                sort::HeapSort<double> hsort;
                double medtable[4];
                for (size_t j = 0; j < uydim; j += 2) {
                    for (size_t i = 0; i < uxdim; i += 2) {
                        if (i > 0 && j > 0 && i < (uxdim - 1) && j < (uydim - 1)) {
                            /* in the middle of the map */
                            medtable[0] = uvalue[i - 1][j];
                            medtable[1] = uvalue[i + 1][j];
                            medtable[2] = uvalue[i][j - 1];
                            medtable[3] = uvalue[i][j + 1];
                            hsort.sort(medtable, 6);
//                            std::qsort((void *) medtable, 4, sizeof(*medtable), compar);
                            /* Actually mean of two median values */
                            uvalue[i][j] = (medtable[1] + medtable[2]) / 2.0;
                        } else if (j == 0 && i > 0 && i < uxdim - 1) {
                            /* in the upper edge */
                            medtable[0] = uvalue[i - 1][j];
                            medtable[1] = uvalue[i + 1][j];
                            medtable[2] = uvalue[i][j + 1];
                            hsort.sort(medtable, 3);
//                            std::qsort((void *) medtable, 3, sizeof(*medtable), compar);
                            uvalue[i][j] = medtable[1];
                        } else if (j == (uydim - 1) && i > 0 && i < (uxdim - 1)) {
                            /* in the lower edge */
                            medtable[0] = uvalue[i - 1][j];
                            medtable[1] = uvalue[i + 1][j];
                            medtable[2] = uvalue[i][j - 1];
                            hsort.sort(medtable, 3);
//                            std::qsort((void *) medtable, 3, sizeof(*medtable), UMat::compar);
                            uvalue[i][j] = medtable[1];
                        } else if (i == 0 && j > 0 && j < (uydim - 1)) {
                            /* in the left edge */
                            medtable[0] = uvalue[i + 1][j];
                            medtable[1] = uvalue[i][j - 1];
                            medtable[2] = uvalue[i][j + 1];
                            hsort.sort(medtable, 3);
//                            qsort((void *) medtable, 3, sizeof(*medtable), UMat::compar);
                            uvalue[i][j] = medtable[1];
                        } else if (i == (uxdim - 1) && j > 0 && j < (uydim - 1)) {
                            /* in the right edge */
                            medtable[0] = uvalue[i - 1][j];
                            medtable[1] = uvalue[i][j - 1];
                            medtable[2] = uvalue[i][j + 1];
                            hsort.sort(medtable, 3);
//                            std::qsort((void *) medtable, 3, sizeof(*medtable), UMat::compar);
                            uvalue[i][j] = medtable[1];
                        } else if (i == 0 && j == 0)
                            /* the upper left-hand corner */
                            uvalue[i][j] = (uvalue[i + 1][j] + uvalue[i][j + 1]) / 2.0;
                        else if (i == (uxdim - 1) && j == 0) {
                            /* the upper right-hand corner */
                            uvalue[i][j] = (uvalue[i - 1][j] + uvalue[i][j + 1]) / 2.0;
                        } else if (i == 0 && j == (uydim - 1)) {
                            /* the lower left-hand corner */
                            uvalue[i][j] = (uvalue[i + 1][j] + uvalue[i][j - 1]) / 2.0;
                        } else if (i == (uxdim - 1) && j == (uydim - 1)) {
                            /* the lower right-hand corner */
                            uvalue[i][j] = (uvalue[i - 1][j] + uvalue[i][j - 1]) / 2.0;
                        }
                    }
                }

                UMat::scaleUMatrix();
            }

            void averageUMatrix() {
//                SOMMatrix &mvalue = (*UMat::somMatrix);
                UMatCodes &uvalue = (*UMat::uMatrix);

                size_t xdim = UMat::xdim;
                size_t ydim = UMat::ydim;
                size_t dim = UMat::dim;
                size_t uxdim = UMat::uxdim;
                size_t uydim = UMat::uydim;

                UMatCodes umat2(uxdim, uydim); // temp

                for (size_t j = 0; j < uydim; j++) {
                    for (size_t i = 0; i < uxdim; i++) {
                        if (i && j && (j < (uydim - 1)) && (i < (uxdim - 1))) {
                            /* Non borders */
                            umat2[i][j] = ((uvalue[i][j - 1] + uvalue[i - 1][j] + uvalue[i][j] + uvalue[i + 1][j] +
                                            uvalue[i][j + 1]) / 5.0);
                        } else if (i && (i < (uxdim - 1)) && !j) {
                            /* West brdr*/
                            umat2[i][j] = ((uvalue[i - 1][j] + uvalue[i][j] + uvalue[i + 1][j] + uvalue[i][j + 1]) /
                                           4.0);
                        } else if (!i && j && (j < (uydim - 1))) {
                            /*north*/
                            umat2[i][j] = ((uvalue[i][j - 1] + uvalue[i][j] + uvalue[i + 1][j] + uvalue[i][j + 1]) /
                                           4.0);
                        } else if (i && (i < (uxdim - 1)) && (j == (uydim - 1))) {
                            /* south */
                            umat2[i][j] = ((uvalue[i][j - 1] + uvalue[i - 1][j] + uvalue[i][j] + uvalue[i + 1][j]) /
                                           4.0);
                        } else if (j && (j < (uydim - 1)) && (i == (uxdim - 1))) {
                            /* east*/
                            umat2[i][j] = ((uvalue[i][j - 1] + uvalue[i - 1][j] + uvalue[i][j] + uvalue[i][j + 1]) /
                                           4.0);
                        }
                    }
                }

                /*corners*/
                umat2[0][uydim - 1] = (uvalue[1][uydim - 1] + uvalue[0][uydim - 1] + uvalue[0][uydim - 2]) / 3.0;
                umat2[uxdim - 1][uydim - 1] =
                        (uvalue[uxdim - 2][uydim - 1] + uvalue[uxdim - 1][uydim - 1] + uvalue[uxdim - 1][uydim - 2]) /
                        3.0;
                umat2[uxdim - 1][0] = (uvalue[uxdim - 2][0] + uvalue[uxdim - 1][0] + uvalue[uxdim - 1][1]) / 3.0;
                umat2[0][0] = (uvalue[1][0] + uvalue[0][1] + uvalue[0][0]) / 3.0;

                for (size_t j = 0; j < uydim; j++) {
                    for (size_t i = 0; i < uxdim; i++) {
                        uvalue[i][j] = umat2[i][j];
                    }
                }
            }

            void medianUMatrix() {
                SOMMatrix &mvalue = (*UMat::somMatrix);
                UMatCodes &uvalue = (*UMat::uMatrix);

                size_t xdim = UMat::xdim;
                size_t ydim = UMat::ydim;
                size_t dim = UMat::dim;
                size_t uxdim = UMat::uxdim;
                size_t uydim = UMat::uydim;

                UMatCodes umat2(uxdim, uydim); // temp

                for (size_t j = 0; j < uydim; j++) {
                    for (size_t i = 0; i < uxdim; i++) {
                        if(i&&j&&(j<(uydim-1))&&(i<uxdim-1)) {
                            uvalue[i][j]=(median5(uvalue[i][j-1], uvalue[i-1][j], uvalue[i][j], uvalue[i+1][j],
                                                  uvalue[i][j+1]));
                        } else if(i && (i<uxdim-1) && !j) {
                            uvalue[i][j]=(median4(uvalue[i-1][j], uvalue[i][j], uvalue[i+1][j], uvalue[i][j+1]));
                        } else if (!i && j&& (j<uydim-1)) {
                            uvalue[i][j]=(median4(uvalue[i][j-1], uvalue[i][j], uvalue[i+1][j], uvalue[i][j+1]));
                        } else if ( i && (i < (uxdim-1))&& (j==(uydim-1))) {
                            uvalue[i][j]=(median4(uvalue[i][j-1], uvalue[i-1][j], uvalue[i][j], uvalue[i+1][j]));
                        } else if ( j && (j < (uydim-1))&& (i==(uxdim-1))) {
                            uvalue[i][j]=(median5(uvalue[i][j-1], uvalue[i-1][j], uvalue[i-1][j], uvalue[i][j],
                                                  uvalue[i][j+1]));
                        }
                    }
                }

                // corners
                uvalue[0][uydim-1]=median3(uvalue[1][uydim-1], uvalue[0][uydim-1], uvalue[0][uydim-2]);
                uvalue[uxdim-1][uydim-1]=median3(uvalue[uxdim-2][uydim-1], uvalue[uxdim-1][uydim-1], uvalue[uxdim-1][uydim-2]);
                uvalue[uxdim-1][0]=median3(uvalue[uxdim-2][0], uvalue[uxdim-1][0], uvalue[uxdim-1][1]);
                uvalue[0][0]=median3(uvalue[1][0], uvalue[0][1], uvalue[0][0]);

                for (size_t j = 0; j < uydim; j++) {
                    for (size_t i = 0; i < uxdim; i++) {
                        uvalue[i][j] = umat2[i][j];
                    }
                }
            }

        };
    }
}


#endif
