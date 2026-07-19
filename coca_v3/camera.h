#ifndef CAMERA_H
#define CAMERA_H

#include <vector>

struct Camera
{
    float x, y, z;
    float yaw, pitch;
    float speed;      // meter/detik
    float eyeHeight;

    Camera();
};

extern Camera cam;

void InitCamera();
void ApplyCamera();

// dipanggil dari Idle() tiap frame dgn delta-time (ms) dan status tombol;
// menggantikan versi lama yg gerak 1 langkah tetap tiap event keypress.
void UpdateCamera(int dtMs, bool wDown, bool sDown, bool aDown, bool dDown);

// ------------------------------------------------------------------
// Collision AABB sederhana (spec: player tidak boleh menembus dinding/
// pagar/kolom). Semua BuildXxx() di building.cpp mendaftarkan kotak
// tabrakan lewat AddCollisionBox() saat menggambar dinding/kolom/pagar.
// ------------------------------------------------------------------
struct AABB { float minX, minZ, maxX, maxZ, minY, maxY; };

void AddCollisionBox(float x1, float z1, float x2, float z2, float yBase, float height);
void ClearCollisionBoxes();

#endif
