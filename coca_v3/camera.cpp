#include "camera.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/freeglut.h>
#include <cmath>

Camera cam;

static std::vector<AABB> g_collisionBoxes;

Camera::Camera()
{
    // spawn di luar gerbang, menghadap ke bangunan (+Z). x=0 SENGAJA
    // sejajar dengan pintu masuk & semua bukaan lorong interior (lihat
    // building.cpp) supaya jalan lurus dari sini benar-benar tembus
    // sampai ke belakang bangunan tanpa perlu geser kiri/kanan dulu.
    x = 0.0f;
    y = 1.6f;
    z = -12.0f;

    yaw = 180.0f;
    pitch = -5.0f;

    speed = 4.0f;
    eyeHeight = 1.6f;
}

void InitCamera()
{
    cam = Camera();
}

static void GetForwardVector(float &fx, float &fz)
{
    float ry = cam.yaw * (float)M_PI / 180.0f;
    fx = sinf(ry);
    fz = -cosf(ry);
}

void ApplyCamera()
{
    float ry = cam.yaw   * (float)M_PI / 180.0f;
    float rp = cam.pitch * (float)M_PI / 180.0f;

    float lx = sinf(ry) * cosf(rp);
    float ly = sinf(rp);
    float lz = -cosf(ry) * cosf(rp);

    gluLookAt(
        cam.x, cam.y, cam.z,
        cam.x+lx, cam.y+ly, cam.z+lz,
        0, 1, 0);
}

// ------------------------------------------------------------------
// Collision
// ------------------------------------------------------------------
void AddCollisionBox(float x1, float z1, float x2, float z2, float yBase, float height)
{
    AABB b;
    b.minX = (x1<x2)?x1:x2; b.maxX = (x1<x2)?x2:x1;
    b.minZ = (z1<z2)?z1:z2; b.maxZ = (z1<z2)?z2:z1;
    b.minY = yBase; b.maxY = yBase+height;
    g_collisionBoxes.push_back(b);
}

void ClearCollisionBoxes()
{
    g_collisionBoxes.clear();
}

static bool CollidesAt(float x, float z, float radius)
{
    float y0 = cam.y - cam.eyeHeight;
    float y1 = cam.y - cam.eyeHeight + 1.7f;
    for (size_t i = 0; i < g_collisionBoxes.size(); ++i) {
        const AABB &b = g_collisionBoxes[i];
        if (y1 < b.minY || y0 > b.maxY) continue;
        if (x + radius > b.minX && x - radius < b.maxX &&
            z + radius > b.minZ && z - radius < b.maxZ) {
            return true;
        }
    }
    return false;
}

static void TryMove(float dx, float dz)
{
    const float radius = 0.22f; // pas utk lorong/pintu ~0.9-1.0m; 0.3 kelewat mepet
    float nx = cam.x + dx;
    float nz = cam.z + dz;
    // slide per-axis supaya nggak "nempel" kalau nabrak diagonal
    if (!CollidesAt(nx, cam.z, radius)) cam.x = nx;
    if (!CollidesAt(cam.x, nz, radius)) cam.z = nz;
}

void UpdateCamera(int dtMs, bool wDown, bool sDown, bool aDown, bool dDown)
{
    float dt = dtMs / 1000.0f;
    float sp = cam.speed * dt;
    float fx, fz; GetForwardVector(fx, fz);
    float rx = fz, rz = -fx;

    float dx = 0, dz = 0;
    if (wDown) { dx += fx*sp; dz += fz*sp; }
    if (sDown) { dx -= fx*sp; dz -= fz*sp; }
    if (aDown) { dx -= rx*sp; dz -= rz*sp; }
    if (dDown) { dx += rx*sp; dz += rz*sp; }
    if (dx != 0 || dz != 0) TryMove(dx, dz);
}
