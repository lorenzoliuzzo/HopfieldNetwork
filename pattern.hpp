#include <vector>
#include <random>
#include <fstream>

#include "PBM.hpp"


enum STATE {UP = 1, DOWN = -1}; 
inline STATE flip(STATE s) { return (s == STATE::UP) ? STATE::DOWN : STATE::UP; }


struct Pattern {

    using pattern_t = std::vector<STATE>;


    Pattern(const pattern_t& p) : bits{p} {}


    inline size_t size() const { return this->bits.size(); }


    static Pattern from_pbm(const std::string& filename) {
        std::vector<std::vector<int>> img;
        try {
            img = read_pbm(filename);
        } catch (const std::exception& e) {
            std::cout << "Something bad happened while tring to read the pattern image from " << filename << ": '" << e.what() << "'\n";
            exit(-1);
        }

        size_t n_bits = img.size() * img.front().size(); 
        std::vector<int> flat_img; 
        flat_img.reserve(n_bits);
        for (const auto& row : img) 
            flat_img.insert(flat_img.end(), row.begin(), row.end());

        pattern_t bits(n_bits); 
        for (size_t i = 0; i < n_bits; ++i)
            bits[i] = flat_img[i] ? STATE::UP : STATE::DOWN;

        return bits;
    }


    void to_pbm(const std::string& filename, size_t width = 0, size_t height = 0) {
        if (width == 0 || height == 0) {
            size_t size = this->bits.size();
            width = static_cast<size_t>(std::sqrt(size));
            while (size % width != 0 && width > 1) --width;
            height = size / width;
            
            if (width * height != size) {
                width = static_cast<size_t>(std::sqrt(size));
                height = (size + width - 1) / width; 
            }
        }
        
        if (width * height != this->bits.size()) 
            throw std::invalid_argument("Width * Height must equal pattern size");
        
        std::ofstream file(filename);
        if (!file.is_open()) throw std::runtime_error("Cannot open file: " + filename);
        
        file << "P1\n";
        file << width << " " << height << "\n";
        
        for (size_t i = 0; i < height; ++i) {
            for (size_t j = 0; j < width; ++j) {
                file << (this->bits[i * width + j] == STATE::UP ? "1" : "0");
                if (j < width - 1) file << " ";
            }
            file << "\n";
        }
        
        if (!file.good()) throw std::runtime_error("Error writing to file: " + filename);
    }


    void display() const {
        size_t n_neurons = this->bits.size();
        size_t stride = std::sqrt(n_neurons); 
        auto separator = std::string(stride, '-'); 
        std::cout << separator << std::endl;
        for (size_t i = 0; i < n_neurons; ++i) {
            std::cout << ((this->bits[i] == UP) ? '#' : ' ');
            if ((i + 1) % stride == 0) std::cout << std::endl;
        }
        std::cout << separator << std::endl;
    }


    Pattern add_noise(float flip_pct) const {
        Pattern noisy_pattern = *this;
        if (flip_pct <= 0.) return noisy_pattern;
        if (flip_pct >= 1.) {
            for (STATE& value : noisy_pattern.bits) value = flip(value);
            return noisy_pattern;
        }
            
        size_t n_neurons = this->bits.size(); 
        size_t n_to_flip = static_cast<size_t>(n_neurons * flip_pct);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dist(0, n_neurons - 1);
        
        for (size_t i = 0; i < n_to_flip; ++i) {
            size_t idx = dist(gen);
            noisy_pattern.bits[idx] = flip(noisy_pattern.bits[idx]);
        }
        
        return noisy_pattern;
    }


    double error(const Pattern& true_pattern) const {
        if (true_pattern.bits.size() != this->bits.size())
            throw std::invalid_argument("Pattern dimensions must match");
        
        double total_error = 0.;
        int n_pixels = this->bits.size();
        for (int i = 0; i < n_pixels; ++i) {
            double diff = true_pattern.bits[i] - this->bits[i];
            total_error += diff * diff;
        }
        
        return total_error / n_pixels;
    }


    pattern_t bits; 

}; 