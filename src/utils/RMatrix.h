/**
 * Simple matrix for store values as MatrixRow*
 */

#ifndef SRC_UTILS_RMATRIX_H
#define SRC_UTILS_RMATRIX_H

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>

#include <exception>
#include <stdexcept>
#include <iostream>

namespace utils {

    template<typename S, typename T>
    class RMatrix {
    public:

        struct Row {
            S data; // произвольные данные строки
            T* points; // массив однотипных данных строки
        };

        RMatrix(const size_t &_rowSize, const size_t &_colSize) :
                rowSize(_rowSize), colSize(_colSize), matrix(nullptr),
                tTypeSizeof(sizeof(T)), rTypeSizeof(sizeof(Row)),
                sTypeSizeof(sizeof(S)) {
            if (rowSize > 0) {
                initializeRowMemory(rowSize);
                size_t cAmount = tTypeSizeof * colSize;
                for (size_t r = 0; r < rowSize; ++r) {
                    T *pointsArray = (T *) std::malloc(cAmount);
                    if (pointsArray == NULL) {
                        throw std::runtime_error(std::strerror(errno));
                    }
                    matrix[r].points = pointsArray;
                }
            }
        }

        RMatrix() = delete; // disable default constructor
        RMatrix<S, T>(const RMatrix<S, T> &) = delete; // disable copy constructor
        RMatrix<S, T> &operator=(const RMatrix<S, T> &) = delete; // assign operator
        RMatrix<S, T> &operator=(RMatrix<S, T> &&) = delete; // replacement operator

        ~RMatrix() {
            if (matrix) {
                // remove all row array
                for (size_t r = 0; r < rowSize; ++r) {
                    std::free(matrix[r].points);
                    // set to null each array pointer
                    matrix[r].points = nullptr;
                }
                std::free(matrix);
                matrix = nullptr;
                rowSize = 0;
                colSize = 0;
            }
        }

        T &operator()(const size_t &r, const size_t &c) const {
            return matrix[r].points[c];
        }

        T* operator[](const size_t &r) const {
            return matrix[r].points;
        }

        bool operator==(const RMatrix<S, T> &other) const {
            if (other.getRowSize() != rowSize) {
                return false;
            }
            if (other.getColSize() != colSize) {
                return false;
            }
            for (size_t r = 0; r < rowSize; ++r) {
                // проверка объекта с произвольными данными
                if(matrix[r].data != other.getRow(r).data) {
                    return false;
                }
                for (size_t c = 0; c < colSize; ++c) {
                    if (matrix[r].points[c] != other(r, c)) {
                        return false;
                    } // compare each element
                }
            }
            return true;
        }

        bool operator!=(const RMatrix<T> &other) const {
            return !((*this) == other);
        }

        bool equalsWithError(const RMatrix<T> &other,
                             const double &error) const {
            if (other.getRowSize() != rowSize) {
                return false;
            }
            if (other.getColSize() != colSize) {
                return false;
            }
            for (size_t r = 0; r < rowSize; ++r) {
                if(matrix[r].data != other.getRow(r).data) {
                    return false;
                }
                for (size_t c = 0; c < colSize; ++c) {
                    const T &a = (*this)(r, c);
                    const T &b = other(r, c);

                    // compare each element as range b-error <= a <= b+error
                    if (!(((b - error) <= a) && (a <= (b + error)))) {
                        return false;
                    }
                }
            }
            return true;
        }

        size_t getRowSize() const {
            return rowSize;
        }

        size_t getColSize() const {
            return colSize;
        }

        Row* getMatrix() const {
            return matrix;
        }

        Row& getRow(const size_t &r) const {
            return matrix[r];
        }

        T* getRowPoints(const size_t &r) const {
            return matrix[r].points;
        }

        void writeRow(size_t rowIndex, const Row& value) {
            if (!(rowIndex < rowSize)) {
                // re-initialize row memory
                size_t newRowSize = rowIndex + 1;
                initializeRowMemory(newRowSize);


                size_t cAmount = tTypeSizeof * colSize;
                for (size_t r = rowSize; r < newRowSize; ++r) {
                    T *pointsArray = (T *) std::malloc(cAmount);
                    if (pointsArray == NULL) {
                        throw std::runtime_error(std::strerror(errno));
                    }
                    matrix[r].points = pointsArray;
                }
                rowSize = newRowSize;
            }

            copyRow(matrix[rowIndex], value);
        }

        void pushRow(const Row& value) {
            writeRow(rowSize, value);
        }

        void swapRows(size_t r1, size_t r2) {
            // copy r1 row to temp
            T *temp = *(matrix + r1);
            // copy r2 row to r1
            *(matrix + r1) = *(matrix + r2);
            // copy temp to r2
            *(matrix + r2) = temp;
        }

        SMatrix<T> *createClone(size_t startRow, size_t startCol,
                                size_t sizeRow, size_t sizeCol) const {
            SMatrix<T> *clone = new SMatrix<T>(sizeRow, sizeCol);
            for (size_t r = 0; r < sizeRow; ++r) {
                clone->writeRow(r, matrix[r + startRow] + startCol);
            }
            return clone;
        }

        SMatrix<T> *createClone(size_t startRow, size_t startCol) const {
            return createClone(startRow, startCol, rowSize - startRow,
                               colSize - startCol);
        }

        SMatrix<T> *createClone() const {
            return createClone(0, 0, rowSize, colSize);
        }

        /**
         * Скопировать значения в матрицу destination
         */
        void copyTo(SMatrix<T>& destination) const {
            for (size_t r = 0; r < rowSize; ++r) {
                destination.writeRow(r, matrix[r]);
            }
        }

        void print() const {
            std::cout << "Matrix["<<rowSize<<"x"<<colSize<<"]"<< std::endl;
            for (size_t r = 0; r < rowSize; ++r) {
                for (size_t c = 0; c < colSize; ++c) {
                    std::cout << matrix[r][c] << ", ";
                }
                std::cout << std::endl;
            }
        }

    private:
        Row *matrix;
        size_t rowSize;
        size_t colSize;
        size_t tTypeSizeof; // saved value sizeof T
        size_t sTypeSizeof; // saved value sizeof S
        size_t rTypeSizeof; // saved value sizeof Row

        void initializeRowMemory(size_t _rowSize) {
            size_t rAmount = rTypeSizeof * _rowSize;
            Row *newMatrix;
            if (matrix) {
                newMatrix = (Row *) std::realloc(matrix, rAmount);
            } else {
                newMatrix = (Row *) std::malloc(rAmount);
            }

            if (newMatrix == NULL) {
                throw std::runtime_error(std::strerror(errno));
            }
            matrix = newMatrix;
        }

        void copyRow(Row& dest, const Row& src) {
            // copy data
            memcpy(&(dest.data), &(src.data), sTypeSizeof);
            // copy points array
            memcpy(dest.points, src.points, colSize * tTypeSizeof);
        }
    };
}


#endif
