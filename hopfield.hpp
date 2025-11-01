#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <Eigen/Dense>

#include "pattern.hpp"


class HopfieldNetwork {
public:
    
    void train(const std::vector<Pattern>& patterns, std::string learning_rule = "pinv") {
        if (patterns.size() > this->storage_capacity(patterns.front().size())) 
            throw std::runtime_error("Storage capacity exceeded! More neurons are needed in order to store this many patterns.");
        
        if (learning_rule == "hebb") 
            this->weights = this->hebb_weights(patterns);
        else if (learning_rule == "storkey")
            this->weights = this->storkey_weights(patterns); 
        else if (learning_rule == "pinv") 
            this->weights = this->pinv_weights(patterns);
        else    
            throw std::runtime_error(
                "There is no '" + learning_rule + "' learning rule in the current implementation. Consider using 'hebb', 'storkey' or 'pinv' learning rules."
            );
    }


    Pattern recall(const Pattern& input, size_t max_iters = 50, size_t stability_threshold = 2) const {
        Pattern current = input; 
        size_t n_neurons = input.size();

        std::vector<size_t> idxs(n_neurons);
        std::iota(idxs.begin(), idxs.end(), 0);

        std::random_device rd;
        std::mt19937 rng(rd());
        
        size_t stable_count = 0;
        for (size_t iter = 0; iter < max_iters; ++iter) {
            Pattern old = current;
            bool changed = false;

            std::shuffle(idxs.begin(), idxs.end(), rng);
            for (size_t i : idxs) {
                double sum = 0.0;
                for (size_t j = 0; j < n_neurons; ++j) 
                    sum += this->weights(i, j) * static_cast<int>(current.bits[j]);
                
                auto new_bit = (sum >= 0) ? SpinHalf::UP : SpinHalf::DOWN;
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

    Eigen::MatrixXd hebb_weights(const std::vector<Pattern>& patterns) const {
        size_t n_neurons = patterns.front().size(); 
        double inv_n_neurons = 1. / n_neurons; 

        Eigen::MatrixXd W = Eigen::MatrixXd::Zero(n_neurons, n_neurons);
        for (size_t i = 0; i < n_neurons; ++i) {
            for (size_t j = i + 1; j < n_neurons; ++j) {  
                double w_ij = 0.;
                for (const Pattern& p : patterns) 
                    w_ij += static_cast<int>(p.bits[i]) * static_cast<int>(p.bits[j]);
                w_ij *= inv_n_neurons;
                W(i, j) = W(j, i) = w_ij;                
            }
        }
        return W;
    }


    Eigen::MatrixXd storkey_weights(const std::vector<Pattern>& patterns) const {
        size_t n_neurons = patterns.front().size();
        double inv_n_neurons = 1. / n_neurons; 

        Eigen::MatrixXd W = Eigen::MatrixXd::Zero(n_neurons, n_neurons);
        for (const Pattern& p : patterns) {

            Eigen::VectorXd ξ(n_neurons);
            for (size_t i = 0; i < n_neurons; ++i) ξ(i) = static_cast<int>(p.bits[i]);
            
            for (size_t i = 0; i < n_neurons; ++i) {
                for (size_t j = 0; j < n_neurons; ++j) {
                    if (i == j) continue;  

                    double h_i = 0., h_j = 0.;
                    for (size_t k = 0; k < n_neurons; ++k) {
                        if (k != i && k != j) {
                            h_i += W(i, k) * ξ(k);
                            h_j += W(j, k) * ξ(k);
                        }
                    }
                    
                    W(i, j) += inv_n_neurons * (ξ(i) * ξ(j) - ξ(i) * h_j - h_i * ξ(j));
                }
            }
        }
        
        return W;
    }

    
    Eigen::MatrixXd pinv_weights(const std::vector<Pattern>& patterns) const {
        size_t n_patterns = patterns.size();
        size_t n_neurons = patterns.front().size();
        
        Eigen::MatrixXd X(n_neurons, n_patterns), X_T(n_patterns, n_neurons);
        for (size_t μ = 0; μ < n_patterns; ++μ) 
            for (size_t i = 0; i < n_neurons; ++i) 
                X(i, μ) = X_T(μ, i) = static_cast<int>(patterns[μ].bits[i]);
        
        return X * (X_T * X).inverse() * X_T;  
    }


    Eigen::MatrixXd weights;
}; 