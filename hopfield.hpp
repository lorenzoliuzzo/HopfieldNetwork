#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <Eigen/Dense>

#include "pattern.hpp"


class HopfieldNetwork {
public:
    
    void train(const std::vector<Pattern>& patterns, std::string rule) {
        if (patterns.size() > this->storage_capacity(patterns.front().size())) 
            throw std::runtime_error("Storage capacity exceeded! More neurons are needed in order to store this many patterns.");
        
        if (rule == "hebb") 
            this->weights = this->hebb_weights(patterns);
        else if (rule == "pinv") 
            this->weights = this->pinv_weights(patterns);
        else    
            throw std::runtime_error("No rule found.");
    }


    Pattern recall(const Pattern& input, size_t max_iters = 50, size_t stability_threshold = 2) const {
        Pattern current = input; 
        size_t n_neurons = input.size();

        std::vector<size_t> indices(n_neurons);
        std::iota(indices.begin(), indices.end(), 0);

        std::random_device rd;
        std::mt19937 rng(rd());
        
        size_t stable_count = 0;
        for (size_t iter = 0; iter < max_iters; ++iter) {
            Pattern old = current;
            bool changed = false;

            std::shuffle(indices.begin(), indices.end(), rng);
            for (size_t i : indices) {
                double sum = 0.0;
                for (size_t j = 0; j < n_neurons; ++j) 
                    sum += this->weights(i, j) * static_cast<int>(current.bits[j]);
                
                STATE new_bit = (sum >= 0) ? UP : DOWN;
                if (new_bit != current.bits[i]) {
                    current.bits[i] = new_bit;
                    changed = true;
                }
            }

            if (!changed) {
                stable_count++;
                if (stable_count >= stability_threshold) {
                    std::cout << "Converged after " << iter + 1 << " iterations\n"; 
                    break;
                }
            } else {
                stable_count = 0;
            }
        }

        return current; 
    }


    double energy(const Pattern& pattern) const {
        double E = 0.0;
        for (size_t i = 0; i < pattern.size(); ++i)
            for (size_t j = 0; j < pattern.size(); ++j)
                E -= this->weights(i, j) * static_cast<int>(pattern.bits[i]) * static_cast<int>(pattern.bits[j]);
        return E;
    }


    static inline size_t storage_capacity(size_t n_neurons) {
        return static_cast<size_t>(0.5 * n_neurons / std::log2(n_neurons));
    }


private:

    Eigen::MatrixXd hebb_weights(const std::vector<Pattern>& patterns) {
        size_t n_patterns = patterns.size(); 
        size_t n_neurons = patterns.front().size(); 

        Eigen::MatrixXd W(n_neurons, n_neurons);        
        for (size_t i = 0; i < n_neurons; ++i) {
            W(i, i) = 0.; 
            for (size_t j = i + 1; j < n_neurons; ++j) {  
                double w_ij = 0.0;
                for (const Pattern& p : patterns) 
                    w_ij += int(p.bits[i]) * int(p.bits[j]);
                w_ij /= n_neurons;
                W(i, j) = W(j, i) = w_ij;                
            }
        }
        return W;
    }

    Eigen::MatrixXd pinv_weights(const std::vector<Pattern>& patterns) {
        size_t n_neurons = patterns.front().size();
        size_t n_patterns = patterns.size();
        
        Eigen::MatrixXd X(n_neurons, n_patterns);
        Eigen::MatrixXd X_T(n_patterns, n_neurons);
        for (size_t μ = 0; μ < n_patterns; ++μ) 
            for (size_t i = 0; i < n_neurons; ++i) 
                X(i, μ) = X_T(μ, i) = patterns[μ].bits[i];
        
        return X * (X_T * X).inverse() * X_T;  
    }


    Eigen::MatrixXd weights;
}; 