compile: main.cpp hopfield.hpp pattern.hpp PBM.hpp
	g++ -I/usr/local/include/eigen-5.0.0 -std=c++23 -O2 main.cpp -o main

run: main
	./main