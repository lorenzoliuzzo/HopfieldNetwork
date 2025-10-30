CXX := g++
CXXFLAGS := -std=c++23 -O2 -Wall -Wextra
EIGEN_PATH := /usr/local/include/eigen-5.0.0

TARGET := main
SOURCES := main.cpp
HEADERS := hopfield.hpp pattern.hpp PBM.hpp

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -I$(EIGEN_PATH) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

convert:
	bash digits/pbm2png.sh

clean:
	rm -f $(TARGET)

clean-images:
	rm -f digits/*_noisy.png
	rm -f digits/*_recalled.png

.PHONY: run clean clean-images convert