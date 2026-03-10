#undef UNICODE
#undef _UNICODE
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>

#include <vector>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <iostream>

#include "vec3.h"
#include "ray.h"
#include "camera.h"
#include "triangle.h"

// ---------- Rotation helpers ----------

Vec3 rotateY(const Vec3& v, float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return Vec3(
        v.x * c + v.z * s,
        v.y,
        -v.x * s + v.z * c
    );
}

Vec3 rotateX(const Vec3& v, float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return Vec3(
        v.x,
        v.y * c - v.z * s,
        v.y * s + v.z * c
    );
}

// ---------- Global state for interaction ----------

float g_angleX = 0.7f;
float g_angleY = 0.3f;
bool  g_dragging = false;
int   g_lastMouseX = 0;
int   g_lastMouseY = 0;

// ---------- Raytracer: render one frame into pixels ----------

void renderScene(int width, int height, Camera& cam,
                 std::vector<uint32_t>& pixels,
                 float angleX, float angleY)
{
    // Rebuild cube with current rotation
    std::vector<Triangle> cube = {

        // Front face (z = 1)
        Triangle(
            rotateX( rotateY(Vec3(-1, -1,  1), angleY), angleX ),
            rotateX( rotateY(Vec3( 1, -1,  1), angleY), angleX ),
            rotateX( rotateY(Vec3( 1,  1,  1), angleY), angleX )
        ),
        Triangle(
            rotateX( rotateY(Vec3(-1, -1,  1), angleY), angleX ),
            rotateX( rotateY(Vec3( 1,  1,  1), angleY), angleX ),
            rotateX( rotateY(Vec3(-1,  1,  1), angleY), angleX )
        ),

        // Back face (z = -1)
        Triangle(
            rotateX( rotateY(Vec3(-1, -1, -1), angleY), angleX ),
            rotateX( rotateY(Vec3( 1,  1, -1), angleY), angleX ),
            rotateX( rotateY(Vec3( 1, -1, -1), angleY), angleX )
        ),
        Triangle(
            rotateX( rotateY(Vec3(-1, -1, -1), angleY), angleX ),
            rotateX( rotateY(Vec3(-1,  1, -1), angleY), angleX ),
            rotateX( rotateY(Vec3( 1,  1, -1), angleY), angleX )
        ),

        // Left face (x = -1)
        Triangle(
            rotateX( rotateY(Vec3(-1, -1, -1), angleY), angleX ),
            rotateX( rotateY(Vec3(-1, -1,  1), angleY), angleX ),
            rotateX( rotateY(Vec3(-1,  1,  1), angleY), angleX )
        ),
        Triangle(
            rotateX( rotateY(Vec3(-1, -1, -1), angleY), angleX ),
            rotateX( rotateY(Vec3(-1,  1,  1), angleY), angleX ),
            rotateX( rotateY(Vec3(-1,  1, -1), angleY), angleX )
        ),

        // Right face (x = 1)
        Triangle(
            rotateX( rotateY(Vec3(1, -1, -1), angleY), angleX ),
            rotateX( rotateY(Vec3(1,  1,  1), angleY), angleX ),
            rotateX( rotateY(Vec3(1, -1,  1), angleY), angleX )
        ),
        Triangle(
            rotateX( rotateY(Vec3(1, -1, -1), angleY), angleX ),
            rotateX( rotateY(Vec3(1,  1, -1), angleY), angleX ),
            rotateX( rotateY(Vec3(1,  1,  1), angleY), angleX )
        ),

        // Top face (y = 1)
        Triangle(
            rotateX( rotateY(Vec3(-1, 1, -1), angleY), angleX ),
            rotateX( rotateY(Vec3(-1, 1,  1), angleY), angleX ),
            rotateX( rotateY(Vec3( 1, 1,  1), angleY), angleX )
        ),
        Triangle(
            rotateX( rotateY(Vec3(-1, 1, -1), angleY), angleX ),
            rotateX( rotateY(Vec3( 1, 1,  1), angleY), angleX ),
            rotateX( rotateY(Vec3( 1, 1, -1), angleY), angleX )
        ),

        // Bottom face (y = -1)
        Triangle(
            rotateX( rotateY(Vec3(-1, -1, -1), angleY), angleX ),
            rotateX( rotateY(Vec3( 1, -1,  1), angleY), angleX ),
            rotateX( rotateY(Vec3(-1, -1,  1), angleY), angleX )
        ),
        Triangle(
            rotateX( rotateY(Vec3(-1, -1, -1), angleY), angleX ),
            rotateX( rotateY(Vec3( 1, -1, -1), angleY), angleX ),
            rotateX( rotateY(Vec3( 1, -1,  1), angleY), angleX )
        )
    };

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            float u = (x + 0.5f) / width - 0.5f;
            float v = (y + 0.5f) / height - 0.5f;

            Ray r = cam.getRay(u, v);

            float closest = 1e9f;
            const Triangle* hitTri = nullptr;

            for (const Triangle& tri : cube) {
                float t = tri.intersect(r);
                if (t > 0 && t < closest) {
                    closest = t;
                    hitTri = &tri;
                }
            }

            int idx = y * width + x;

            if (hitTri) {
                Vec3 N = (hitTri->v1 - hitTri->v0).cross(hitTri->v2 - hitTri->v0).normalized();

                Vec3 lightDir = Vec3(0.1f, 0.1f, -0.6f).normalized();
                float intensity = std::max(0.0f, N.dot(lightDir));

                Vec3 baseColor(0.6f, 0.4f, 0.2f);

                Vec3 shaded = baseColor * intensity;

                Vec3 hitPoint = r.origin + r.direction * closest;
                Vec3 viewDir = (cam.origin - hitPoint).normalized();
                Vec3 halfDir = (lightDir + viewDir).normalized();

                float spec = std::pow(std::max(0.0f, N.dot(halfDir)), 20.0f);
                Vec3 specular = Vec3(1.0f, 1.0f, 1.0f) * spec;

                Vec3 finalColor = shaded + specular;

                int R = std::clamp(int(finalColor.x * 255.0f), 0, 255);
                int G = std::clamp(int(finalColor.y * 255.0f), 0, 255);
                int B = std::clamp(int(finalColor.z * 255.0f), 0, 255);

                // 32-bit pixel in 0x00BBGGRR format for Win32
                pixels[idx] = (R << 16) | (G << 8) | (B);

            } else {
                Vec3 topColor    = Vec3(0.05f, 0.05f, 0.15f);
                Vec3 bottomColor = Vec3(0.02f, 0.02f, 0.05f);

                float t = (v + 0.5f);
                Vec3 sky = bottomColor * (1.0f - t) + topColor * t;

                int R = std::clamp(int(sky.x * 255.0f), 0, 255);
                int G = std::clamp(int(sky.y * 255.0f), 0, 255);
                int B = std::clamp(int(sky.z * 255.0f), 0, 255);

                pixels[idx] = (R << 16) | (G << 8) | (B);
            }
        }
    }
}

// ---------- Win32 window procedure ----------

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {

    case WM_LBUTTONDOWN:
        g_dragging = true;
        g_lastMouseX = GET_X_LPARAM(lParam);
        g_lastMouseY = GET_Y_LPARAM(lParam);
        return 0;

    case WM_LBUTTONUP:
        g_dragging = false;
        return 0;

    case WM_MOUSEMOVE:
        if (g_dragging) {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);

            int dx = x - g_lastMouseX;
            int dy = y - g_lastMouseY;

            g_angleY += dx * 0.01f;
            g_angleX += dy * 0.01f;

            g_lastMouseX = x;
            g_lastMouseY = y;
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// ---------- Entry point ----------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    int width  = 300;
    int height = 300;

    WNDCLASS wc = {};
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = "RaytraceWindowClass";
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        "Raytraced Cube",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width + 16, height + 39,
        nullptr, nullptr, hInstance, nullptr
    );

    ShowWindow(hwnd, SW_SHOW);

    std::vector<uint32_t> pixels(width * height);
    Camera cam(60.0f, float(width) / float(height));

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = width;
    bmi.bmiHeader.biHeight      = -height; // top-down
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    MSG msg = {};
    HDC hdc = GetDC(hwnd);

    while (true) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                ReleaseDC(hwnd, hdc);
                return 0;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        renderScene(width, height, cam, pixels, g_angleX, g_angleY);

        StretchDIBits(
            hdc,
            0, 0, width, height,
            0, 0, width, height,
            pixels.data(),
            &bmi,
            DIB_RGB_COLORS,
            SRCCOPY
        );
    }
}