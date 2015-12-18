#include "matrix_utils_test.h"

void test_multiply_matrix() {

//    size_t nSize = 3;
    utils::CMatrix<int> matrixA(4, 3);

    // set values to A
    int row0[] = {0, 1, 2};
    matrixA.writeRow(0, row0);
    int row1[]  = {3, 4, 5};
    matrixA.writeRow(1, row1);
    int row2[]  = {6, 7, 8};
    matrixA.writeRow(2, row2);
    int row3[]  = {9, 10, 11};
    matrixA.writeRow(3, row3);

    utils::CMatrix<int> matrixB(3, 4);
    int rowB0[] = {0, 1, 2, 3};
    matrixB.writeRow(0, rowB0);
    int rowB1[] = {4, 5, 6, 7};
    matrixB.writeRow(1, rowB1);
    int rowB2[] = {8, 9, 10, 11};
    matrixB.writeRow(2, rowB2);

    utils::CMatrix<int> *matrixC = utils::multiplyMatrix(matrixA, matrixB, 1, 1,
                                                         1, 1, 2, 2, 2, 2);

    utils::CMatrix<int> matrixCheck(2, 2);
    int rowCheck0[] = {65, 74};
    matrixCheck.writeRow(0, rowCheck0);
    int rowCheck1[] = {107, 122};
    matrixCheck.writeRow(1, rowCheck1);

    assert(matrixCheck == (*matrixC));


    delete matrixC; // do not remember delete resource
}

void matrix_utils_test() {
    suite("matrix_utils");
    test(multiply_matrix);
}
