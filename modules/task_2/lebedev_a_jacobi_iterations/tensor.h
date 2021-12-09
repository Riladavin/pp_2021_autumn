// Copyright 2021 Lebedev Alexey
#ifndef MODULES_TASK_2_LEBEDEV_A_JACOBI_ITERATIONS_TENSOR_H_
#define MODULES_TASK_2_LEBEDEV_A_JACOBI_ITERATIONS_TENSOR_H_

#include <memory>
#include <vector>
#include <numeric>
#include <stdexcept>

template <typename T>
class Tensor
{
private:
	std::shared_ptr<T[]> data;
	std::vector<size_t> shape;
	std::vector<size_t> strides;
	size_t size;
    const size_t prod(std::vector<size_t>::const_iterator begin, std::vector<size_t>::const_iterator end) {
        return std::accumulate(begin, end, 1, std::multiplies<size_t>{});
    }

public:
    Tensor(const std::vector<size_t> _shape): shape(_shape) {
        size = prod(shape.begin(), shape.end());
	    data = std::shared_ptr<T[]>(new T[size], std::default_delete<T[]>{});
	    for (size_t i = 1; i < shape.size(); i++) {
		    strides.push_back(prod(shape.begin() + i, shape.end()));
	    }
	    strides.push_back(1);
    }
    Tensor(const Tensor<T>& t) = default;
    ~Tensor() = default;

    T* get_data() const {
        return data.get();
    };

    size_t get_size() const {
        return size;
    };

    std::vector<size_t> get_shape() const {
        return shape;
    };

    std::vector<size_t> get_strides() const {
        return strides;
    };

	T& operator [] (const size_t offset) {
		return data.get()[offset];
	}

	T operator [] (const size_t offset) const {
		return data.get()[offset];
	}
};


template<typename T>
Tensor<T> operator + (const Tensor<T>& t1, const Tensor<T> t2) {
	if (t1.get_shape() != t2.get_shape()) {
		throw std::logic_error("Tensor shapes must be equal for sum operation!");
	}
	Tensor<T> sum(t1.get_shape());
	for (size_t i = 0; i < t1.get_size(); i++) {
		sum[i] = t1[i] + t2[i];
	}
	return sum;
}


template<typename T>
Tensor<T> matmul2D(const Tensor<T>& t1, const Tensor<T>& t2) {
	std::vector<size_t> t1_shape(t1.get_shape()), t2_shape(t2.get_shape());
	if (t1_shape.size() != 2 || t2_shape.size() != 2) {
		throw std::length_error("matmul2D expects two-dimensional tensors.");
	}

	if (t1_shape[1] != t2_shape[0]) {
		throw std::logic_error("Incorrect shapes");
	}

	Tensor<T> result({ t1_shape[0], t2_shape[1] });
	std::vector<size_t> t1_strides(t1.get_strides()), t2_strides(t2.get_strides()), result_strides(result.get_strides());

	for (size_t h1 = 0; h1 < t1_shape[0]; h1++) {
		for (size_t w2 = 0; w2 < t2_shape[1]; w2++) {
			T local_sum = 0;
			for (size_t w1 = 0, h2 = 0; w1 < t1_shape[1]; w1++, h2++) {
				local_sum += t1[h1 * t1_strides[0] + w1 * t1_strides[1]] * t2[h2 * t2_strides[0] + w2 * t2_strides[1]];
			}
			result[h1 * result_strides[0] + w2 * result_strides[1]] = local_sum;
		}
	}

    return result;
}


#endif  // MODULES_TASK_2_LEBEDEV_A_JACOBI_ITERATIONS_TENSOR_H_
