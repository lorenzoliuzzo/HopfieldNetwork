#include <vector>
#include <random>
#include <fstream>

#include "PBM.hpp"


enum SpinHalf {UP = 1, DOWN = -1}; 

inline SpinHalf flip(const SpinHalf& state) { 
    return (state == SpinHalf::UP) ? SpinHalf::DOWN : SpinHalf::UP; 
}


struct Pattern {

    Pattern(const std::vector<SpinHalf>& p) {
        this->width = this->height = std::sqrt(p.size());
        this->bits = p; 
    }

    Pattern(const std::vector<SpinHalf>& p, size_t width, size_t height) {
        if (width * height != p.size()) 
            throw std::invalid_argument("Width * Height must equal pattern size");

        this->width = width;
        this->height = height;
        this->bits = p; 
    }

    inline size_t size() const { return this->bits.size(); }


    static Pattern from_pbm(const std::string& filename) {
        try {
            PBM pbm = read_pbm(filename);
            size_t n_bits = pbm.width * pbm.height; 
            std::vector<SpinHalf> bits(n_bits); 
            std::transform(pbm.bits.begin(), pbm.bits.end(), bits.begin(),
                [](int bit) { return bit ? SpinHalf::UP : SpinHalf::DOWN; }
            );
            return Pattern(bits, pbm.width, pbm.height);

        } catch (const std::exception& e) {
            std::cout << "Something bad happened while tring to read the pattern image from " << filename << ": '" << e.what() << "'\n";
            exit(-1);
        }
    }


    void to_pbm(const std::string& filename) {        
        std::ofstream file(filename);
        if (!file.is_open()) throw std::runtime_error("Cannot open file: " + filename);
        
        file << "P1\n";
        file << this->width << " " << this->height << "\n";
        
        for (size_t i = 0; i < this->height; ++i) {
            for (size_t j = 0; j < this->width; ++j) {
                file << (this->bits[i * this->width + j] == SpinHalf::UP ? "1" : "0");
                if (j < this->width - 1) file << " ";
            }
            file << "\n";
        }
        
        if (!file.good()) throw std::runtime_error("Error writing to file: " + filename);
    }


    void display() const {
        auto separator = std::string(this->width, '-'); 
        std::cout << separator << std::endl;
        for (size_t i = 0; i < this->bits.size(); ++i) {
            std::cout << ((this->bits[i] == SpinHalf::UP) ? '#' : ' ');
            if ((i + 1) % this->width == 0) std::cout << std::endl;
        }
        std::cout << separator << std::endl;
    }


    Pattern add_noise(double flip_pct) const {           
        size_t n_neurons = this->bits.size(); 
        size_t n_to_flip = static_cast<size_t>(n_neurons * flip_pct);

        std::vector<size_t> idxs(n_neurons), chosen_idxs(n_to_flip);
        std::iota(idxs.begin(), idxs.end(), 0);

        std::ranges::sample(
            idxs.begin(), idxs.end(), 
            std::back_inserter(chosen_idxs), 
            n_to_flip,
            std::mt19937{std::random_device{}()}
        ); 

        Pattern noisy = *this;
        for (size_t idx : chosen_idxs) noisy.bits[idx] = flip(noisy.bits[idx]);
        return noisy;
    }


    double error(const Pattern& true_pattern) const {
        if (true_pattern.bits.size() != this->bits.size())
            throw std::invalid_argument("Pattern dimensions must match");
        
        size_t errors = 0;
        size_t n_pixels = this->bits.size();
        for (size_t i = 0; i < n_pixels; ++i) {
            if (this->bits[i] != true_pattern.bits[i])
                errors += 1;
        }
        
        return static_cast<double>(errors) / n_pixels;
    }


    size_t width, height; 
    std::vector<SpinHalf> bits; 

}; 