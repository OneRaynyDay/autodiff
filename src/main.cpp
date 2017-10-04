#include "utils.h"
#include <vector>
#include <fstream>
#include <memory>
#include <iostream>
using et::var;

template<typename M>
M load_csv (const std::string & path, bool verbose = false) {
    using namespace Eigen;
    std::ifstream indata;
    indata.open(path);
    std::string line;
    std::vector<double> values;
    uint rows = 0;
    while (std::getline(indata, line)) {
        std::stringstream lineStream(line);
        std::string cell;
        while (std::getline(lineStream, cell, ',')) {
            if(verbose)
                std::cout << cell << std::endl;
            
            double val = std::stod(cell);
            values.push_back(val);
        }
        ++rows;
    }
    return Map<const Matrix<typename M::Scalar, M::RowsAtCompileTime, M::ColsAtCompileTime, RowMajor>>(values.data(), rows, values.size()/rows);
}

// Returns a one-hot encoding representation
// Assumes the matrix is in the shape (Nx1)
MatrixXd one_hot(const MatrixXd& m){
    int max_num = 0;
    for(int i = 0; i < m.rows(); i++){
       max_num = std::max(double(max_num), double(m(i,0))); 
    }
    MatrixXd onehot = MatrixXd::Zero(m.rows(), max_num);
    for(int i = 0; i < m.rows(); i++){
        onehot(i, m(i,0)-1) = 1;
    }
    return onehot;
}

int main(){
    const float learning_rate = 0.01;
    const int iterations = 1000;
    MatrixXd A = load_csv<MatrixXd>("iris.csv");

    var X(A.leftCols(4)), 
        y(A.rightCols(1)), 
        w(MatrixXd::Zero(4,1));
    
    std::cout << "X : " << X.getValue() << std::endl;
    std::cout << "y : " << y.getValue() << std::endl;
    std::cout << "w : " << w.getValue() << std::endl;

    std::unordered_map<var, MatrixXd> m = {
        { w, zeros_like(w) },
    };
    
    // Setting up the equation
    var sigm = 1 / (1 + exp(-1 * et::dot(X, w)));
    var loss = -1 * (dot(transpose(y), log(sigm)) + dot(transpose(1-y), log(1-sigm)));
    for(int i = 0; i < iterations; i++){
        std::cout << "LOSS : " << et::eval(loss, true) << std::endl;
        et::back(loss, m, {et::back_flags::const_qualify});
        w.setValue( w.getValue().array() - learning_rate * m[w].array() );
    }

    // std::cout << y.getValue() << std::endl;
    // std::cout << sigm.getValue() << std::endl;
    MatrixXd ans(y.getValue().rows(), 2);
    ans.col(0) << VectorXd(y.getValue());
    ans.col(1) << VectorXd(sigm.getValue());
    std::cout << "PREDICTION : " << ans << std::endl;
}
