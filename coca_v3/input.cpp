#include "input.h"
#include "camera.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/freeglut.h>
#include <cstdlib>

bool g_wDown=false, g_sDown=false, g_aDown=false, g_dDown=false;

static int  g_winW = 1280, g_winH = 720;
static bool g_mouseWarping = false;

void InitInput(int winW, int winH)
{
    g_winW = winW; g_winH = winH;
}

void KeyDown(unsigned char key, int, int)
{
    switch (key) {
        case 'w': case 'W': g_wDown = true; break;
        case 's': case 'S': g_sDown = true; break;
        case 'd': case 'D': g_aDown = true; break;
        case 'a': case 'A': g_dDown = true; break;
        case 27: exit(0); // ESC keluar
    }
}

void KeyUp(unsigned char key, int, int)
{
    switch (key) {
        case 'w': case 'W': g_wDown = false; break;
        case 's': case 'S': g_sDown = false; break;
        case 'd': case 'D': g_aDown = false; break;
        case 'a': case 'A': g_dDown = false; break;
    }
}

// hanya ganti a dan d

void MouseMotion(int x, int y)
{
    if (g_mouseWarping) { g_mouseWarping = false; return; }
    int cx = g_winW/2, cy = g_winH/2;
    int dx = x - cx, dy = y - cy;
    cam.yaw   += dx * 0.15f;
    cam.pitch -= dy * 0.15f;
    if (cam.pitch > 89.0f) cam.pitch = 89.0f;
    if (cam.pitch < -89.0f) cam.pitch = -89.0f;
    g_mouseWarping = true;
    glutWarpPointer(cx, cy);
}
