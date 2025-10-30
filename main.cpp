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

    for (auto p : digit_patterns) {        
        auto noisy_p = p.add_noise(0.5); 
        auto noisy_E = net.energy(noisy_p); 

        std::cout << "Initial energy: " << noisy_E << std::endl; 
        noisy_p.display(); 

        auto recalled_p = net.recall(noisy_p); 
        auto recalled_E = net.energy(recalled_p); 
        auto err = recalled_p.error(p); 

        recalled_p.display();         
        std::cout << "Final energy: " << recalled_E << std::endl; 
        std::cout << "Error: " << err * 100 << " %" << std::endl << std::endl; 
    }
    
    return 0; 
}   