# Raytraced-Cube

Raytraced Cube is a small real‑time CPU raytracer written in C++. It renders a rotating cube using ray–triangle intersections, Lambert diffuse shading, and a Blinn–Phong specular highlight. The image is displayed in a native Win32 window using a software pixel buffer, without relying on external graphics libraries.

The cube can be rotated interactively by dragging with the left mouse button. A simple sky gradient is used as the background. The project is designed to be lightweight, self‑contained, and easy to build on any Windows system with a C++ compiler.

Simply download the folder named "raytracer" and open it in VSCode.

Build in VSCode Terminal with:

```
g++ main.cpp -lgdi32 -std=c++17 -O2 -o cube.exe
```
This creates a cube.exe file.

Either run in Terminal using:

```
./cube.exe
```
Or open the .exe from within the folder.

The project demonstrates the fundamentals of OOP, raytracing, shading, and real‑time rendering using only standard C++ and the Win32 API.
