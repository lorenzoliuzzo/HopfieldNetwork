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

    double noise_pct = 0.2; 

    for (size_t i = 0; i < 10; ++i) {
        auto p = digit_patterns[i];     

        auto noisy_p = p.add_noise(noise_pct);
        noisy_p.display(); 

        std::cout << "Initial energy: " << net.energy(noisy_p) << std::endl; 

        auto recalled_p = net.recall(noisy_p); 
        recalled_p.display();       

        std::cout << "Final energy: " << net.energy(recalled_p) << std::endl; 

        std::cout << "Error: " << recalled_p.error(p) * 100 << " %" << std::endl << std::endl; 
    
        std::string number_path = "digits/N" + std::to_string(i);
        noisy_p.to_pbm(number_path + "_noisy.pbm"); 
        recalled_p.to_pbm(number_path + "_recalled.pbm");  
    }

    
    return 0; 
}   