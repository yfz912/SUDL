#ifndef _UTIL_H
#define _UTIL_H

#include <iostream>
#include <vector>
#include <math.h>
#include "matrix.h"
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <google/protobuf/io/coded_stream.h>  
#include <google/protobuf/io/zero_copy_stream_impl.h>  
#include <google/protobuf/text_format.h> 

using google::protobuf::io::FileInputStream;
using google::protobuf::Message;

namespace sub_dl {

template <typename T>
bool read_proto_from_txt_file(const char* filename, T* proto) {  
	int fd = open(filename, O_RDONLY);  
	FileInputStream* input = new FileInputStream(fd);  
	bool success = google::protobuf::TextFormat::Parse(input, proto);  
	delete input;  
	close(fd);  
	return success;  
}  

template <typename T>
static int merge(const Matrix<T>& output_val) {
    int val = 0;
    int d = 1;
    for (int i = 8; i >= 1; i--) {
        val += int(output_val[i - 1][0] + 0.5) * pow(2, i - 1);
    }
    return val;
}

template <typename T>
static std::string merge(const Matrix<T>& output_val, int wordseg) {
    std::string ret = "";
    for (int i = 0; i < output_val._x_dim; i++) {
        int d = 1;
        T val = 0.0;
        for (int j = 0; j < output_val._y_dim; j++) {
            if (val < output_val[i][j]) {
                val = output_val[i][j];
                d = j + 1;
            }
        }
        ret += char('0' + d);
        ret += "_";
    }
    return ret;
}

template <typename T>
static void gradient_clip(Matrix<T>& matrix, double clip_gra) {    
    T tot = 0.0;
    for (size_t i = 0; i < matrix._x_dim; i++) {
        for (size_t j = 0; j < matrix._y_dim; j++) {
            tot += (matrix[i][j] * matrix[i][j]);
        }
    }
    T val = sqrt(tot);
    if (val > clip_gra) {
        T scale = clip_gra / val;
        for (int i = 0; i < matrix._x_dim; i++) {
            for (int j = 0; j < matrix._y_dim; j++) {
                matrix[i][j] *= scale;
            }
        }
    }
}

template <typename T>
static T tanh(T x) {
    if (x > 400) {
        return 1.0;
    }
    return (exp(x) - exp(-x)) / (exp(x) + exp(-x));
}

template <typename T>
static T sigmoid(T x) {
    return 1.0 / (1 + exp(-x));
}

template <typename T>
T sigmoid_diff(T x) {
    return x * (1 - x);
}

template <typename T>
Matrix<T> tanh_m(const Matrix<T>& matrix) {
    Matrix<T> ret_val(matrix._x_dim, matrix._y_dim);
    for (size_t i = 0; i < matrix._x_dim; i++) {
        for (size_t j = 0; j < matrix._y_dim; j++) {
            ret_val[i][j] = tanh(matrix[i][j]);
        }
    }
    return ret_val;
}

template <typename T>
Matrix<T> tanh_m_diff(const Matrix<T>& matrix) {
    Matrix<T> ret_val(matrix._x_dim, matrix._y_dim);
    for (size_t i = 0; i < matrix._x_dim; i++) {
        for (size_t j = 0; j < matrix._y_dim; j++) {
            ret_val[i][j] = 1 - matrix[i][j] * matrix[i][j];
        }
    }
    return ret_val;
}

template <typename T>
Matrix<T> sigmoid_m(const Matrix<T>& matrix) {
    Matrix<T> ret_val(matrix._x_dim, matrix._y_dim);
    for (size_t i = 0; i < matrix._x_dim; i++) {
        for (size_t j = 0; j < matrix._y_dim; j++) {
            ret_val[i][j] = sigmoid(matrix[i][j]);
        }
    }
    return ret_val;
}

template <typename T>
Matrix<T> sigmoid_m_diff(const Matrix<T>& matrix) {
    Matrix<T> ret_val(matrix._x_dim, matrix._y_dim);
    for (size_t i = 0; i < matrix._x_dim; i++) {
        for (size_t j = 0; j < matrix._y_dim; j++) {
            ret_val[i][j] = matrix[i][j] * (1 - matrix[i][j]);
        }
    }
    return ret_val;
}

template <typename T>
Matrix<T> exp_m(const Matrix<T>& matrix) {
    Matrix<T> ret_val(matrix._x_dim, matrix._y_dim);
    for (size_t i = 0; i < matrix._x_dim; i++) {
        for (size_t j = 0; j < matrix._y_dim; j++) {
            ret_val[i][j] = exp(matrix[i][j]);
        }
    }
    return ret_val;
}

template <typename T>
Matrix<T> log_m(const Matrix<T>& matrix) {
    Matrix<T> ret_val(matrix._x_dim, matrix._y_dim);
    for (size_t i = 0; i < matrix._x_dim; i++) {
        for (size_t j = 0; j < matrix._y_dim; j++) {
            ret_val[i][j] = log(matrix[i][j]);
        }
    }
    return ret_val;
}

static void split(const std::string& str, 
        const std::string& delim,
        std::vector<std::string>& ret) {
    if (str.size() <= 0 || delim.size() <= 0) {
        ret.clear();
        return;
    }
    ret.clear();
    int last = 0;
    int index = str.find_first_of(delim, last);
    while (index != -1) {
        ret.push_back(str.substr(last, index - last));
        last = index + 1;
        index = str.find_first_of(delim, last);
    }
    if (index == -1 && str[str.size() - 1] != '\t') {
        ret.push_back(str.substr(last, str.size() - last));
    }
}

static void label_encode(const matrix_double& label_id_vec,
        matrix_double& label, int label_dim) {
    if (label_id_vec._x_dim == 0 || label_id_vec._y_dim == 0) {
        std::cerr << "Error when label encode!" << std::endl;
        exit(1);
    }
    label.resize(label_id_vec._x_dim, label_dim);
    for (int i = 0; i < label_id_vec._x_dim; i++) {
        label[i][int(label_id_vec[i][0])] = 1;
    }
}

}

#endif
