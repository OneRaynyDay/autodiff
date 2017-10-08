#include "utils.h"
#include <vector>
#include <fstream>
#include <memory>
#include <iostream>
#include <chrono>
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
    MatrixXd onehot = MatrixXd::Zero(m.rows(), max_num+1);
    for(int i = 0; i < m.rows(); i++){
        onehot(i, m(i,0)) = 1;
    }

    return onehot;
}

int main(){
    auto start = std::chrono::steady_clock::now();

    // TIMING START
    const float learning_rate = 0.001;
    const float scale_init = 0.01;
    const int iterations = 10000;
    MatrixXd A = load_csv<MatrixXd>("iris.csv");

    var X(A.leftCols(4)), 
        y(one_hot(A.rightCols(1))), 
        w1(MatrixXd::Random(4,10)*scale_init),
        w2(MatrixXd::Random(10,3)*scale_init);
    
    std::cout << "X : " << X.getValue() << std::endl;
    std::cout << "y : " << y.getValue() << std::endl;
    std::cout << "w1 : " << w1.getValue() << std::endl;
    std::cout << "w2 : " << w2.getValue() << std::endl;

    std::unordered_map<var, MatrixXd> m = {
        { w1, zeros_like(w1) },
        { w2, zeros_like(w2) },
    };

    // Setting up the equation
    var sigm1 = 1 / (1 + exp(-1 * dot(X, w1)));
    var sigm2 = 1 / (1 + exp(-1 * dot(sigm1, w2)));
    var loss = sum(-1 * (y * log(sigm2) + (1-y) * log(1-sigm2)));


    std::vector<var> leaves = {w1, w2};
    et::expression exp(loss);
    std::unordered_set<var> leaf_set = exp.findNonConsts(leaves);

    for(int i = 0; i < iterations; i++){
        std::cout << "LOSS : " << et::eval(loss, true) << std::endl;
        // et::back(exp, m , leaf_set);
        et::back(loss, m);
        w1.setValue( w1.getValue().array() - learning_rate * m[w1].array() );
        w2.setValue( w2.getValue().array() - learning_rate * m[w2].array() );
    }
    // TIMING END
    
    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    
    MatrixXd ans(y.getValue().rows(), 6);
    ans.leftCols(3) << y.getValue();
    ans.rightCols(3) << sigm2.getValue();
    std::cout << "PREDICTION : " << ans << std::endl;
    std::cout << "Time elapsed : " << 
        std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;
}
