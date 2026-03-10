#pragma once
#include <fstream>
#include <vector>
#include <string>

inline void writePPM(const std::string& filename, int width, int height, const std::vector<int>& pixels) {
    std::ofstream out(filename);
    out << "P3\n" << width << " " << height << "\n255\n";

    for (int i = 0; i < width * height; i++) {
    int R = pixels[i * 3 + 0];
    int G = pixels[i * 3 + 1];
    int B = pixels[i * 3 + 2];

    out << R << " " << G << " " << B << "\n";
}
}