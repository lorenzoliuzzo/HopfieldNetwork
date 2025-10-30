#include <fstream>
#include <string>
#include <vector>


std::vector<std::vector<int>> read_pbm(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) throw std::runtime_error("Error: Could not open file '" + filename + "'.");
    
    std::string magic;
    file >> magic;
    if (magic != "P1") throw std::runtime_error("Error: Not a PBM file (P1 format)"); 

    std::string line;
    while (file.peek() == '#') std::getline(file, line);

    size_t width, height; 
    file >> width >> height;

    std::vector<std::vector<int>> pixels(height, std::vector<int>(width));
    for (size_t y = 0; y < height; ++y) 
        for (size_t x = 0; x < width; ++x) 
            file >> pixels[y][x];
        

    file.close();
    return pixels;
}

