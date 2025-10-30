#include <iostream>
#include <vector>

#include "hopfield.hpp"


std::vector<Pattern> load_digits() {
    std::vector<Pattern> digit_patterns;
    for (size_t i = 0; i < 10; ++i) {
        auto file = "digits/N" + std::to_string(i) + ".pbm"; 
        Pattern p = Pattern::from_pbm(file); 
        digit_patterns.push_back(p);
    }
    return digit_patterns;
}


int main() {
    
    HopfieldNetwork net; 

    auto pinv_flag = true; 
    auto training_rule = pinv_flag ? "pinv" : "hebb"; 

    auto digit_patterns = load_digits(); 
    net.train(digit_patterns, training_rule);

    for (size_t i = 0; i < 10; ++i) {
        Pattern p = digit_patterns[i]; 
        
        auto noisy_p = p.add_noise(0.5); 
        noisy_p.display(); 

        auto file = "digits/N" + std::to_string(i) + "_noisy.pbm"; 
        noisy_p.to_pbm(file); 

        std::cout << "Initial energy: " << net.energy(noisy_p) << std::endl; 

        auto recalled_p = net.recall(noisy_p); 
        recalled_p.display();       

        std::cout << "Final energy: " << net.energy(recalled_p) << std::endl; 
        std::cout << "Error: " << recalled_p.error(p) * 100 << " %" << std::endl << std::endl; 
    }
    
    return 0; 
}   