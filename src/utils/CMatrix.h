#ifndef SRC_UTILS_CMATRIX_H_
#define SRC_UTILS_CMATRIX_H_

#include "CArrayList.h"

namespace utils {

/**
 * Реализация матрицы на основе CArrayList и следовательно предназначенной для
 * хранения объектов простых типов.
 */
template<typename T>
class CMatrix {
public:

	CMatrix(size_t _rowSize, size_t _colSize) :
			rowSize(_rowSize), colSize(_colSize) {
		matrix = new CArrayList<T>(rowSize * colSize);
	}

	CMatrix() = delete;
	CMatrix(const CMatrix<T>&) = delete;
	CMatrix<T>& operator=(const CMatrix<T>&) = delete;// assign operator
	CMatrix<T>& operator=(CMatrix<T> &&) = delete; // replacement operator

	~CMatrix() {
		delete matrix;
	}

	struct Coordinate {
		size_t r; // row
		size_t c; // col
	};

	// iterators
	class SliceIterator {
	public:
		SliceIterator(T* _arr, size_t _start, size_t _step, size_t _index) :
				arr(_arr), start(_start), step(_step), pos(_start), //
				index(_index) {
		}
		bool operator!=(const SliceIterator& other) const {
			return index != other.index;
		}
		T& operator*() const {
			return arr[pos];
		}
		const SliceIterator& operator++() {
			++index;
			pos = start + index * step;
			return *this;
		}

	private:
		size_t start;
		size_t step;
		size_t index;
		size_t pos;
		T* arr;
	};

	class ColIterator {
	public:
		ColIterator(T* _arr, size_t _rsize, size_t _csize, size_t _index) :
				arr(_arr), rsize(_rsize), csize(_csize), //
				index(_index) {
		}
		SliceIterator begin() const {
			return SliceIterator(arr, index, csize, 0);
		}
		SliceIterator end() const {
			return SliceIterator(arr, 0, 0, rsize);
		}

	private:
		size_t rsize;
		size_t csize;
		size_t index;
		T* arr;
	};

	class RowIterator {
	public:
		RowIterator(T* _arr, size_t _rsize, size_t _csize, size_t _index) :
				arr(_arr), rsize(_rsize), csize(_csize), //
				index(_index) {
		}
		SliceIterator begin() const {
			return SliceIterator(arr, index * csize, 1, 0);
		}
		SliceIterator end() const {
			return SliceIterator(arr, 0, 0, csize);
		}

	private:
		size_t rsize;
		size_t csize;
		size_t index;
		T* arr;
	};
	///////////////////////////////

	ColIterator col(size_t i) {
		return ColIterator(matrix, rowSize, colSize, i);
	}

	RowIterator row(size_t i) {
		return RowIterator(matrix, rowSize, colSize, i);
	}

	T& operator()(size_t r, size_t c) {
		return (*matrix)[r*colSize + c];
	}

	Coordinate getColIndex(size_t index) const {
		size_t r = index / colSize;
		size_t c = index - r*colSize;
		return {r, c};
	}

	CArrayList<T>* getMatrix() {
		return matrix;
	}

	void push(size_t r, size_t c, const T& value) {
		(*this)(r, c);
	}

private:
	CArrayList<T>* matrix;
	size_t rowSize;
	size_t colSize;
};

}

#endif /* SRC_UTILS_CMATRIX_H_ */
