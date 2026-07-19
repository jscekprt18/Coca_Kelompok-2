#ifndef GEOMETRY_H
#define GEOMETRY_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/freeglut.h>

// ======================================================
// Primitif geometri vertex-explicit dasar. Semua fungsi "Build..." di
// building.cpp menyusun bangunan dari primitif-primitif ini, bukan dari
// glutSolidCube dsb, supaya tiap bagian (dinding, kusen, atap, dst) punya
// vertex + normal + texcoord sendiri yang mudah diedit.
// ======================================================

struct Vec3 { float x, y, z; };

// Kotak axis-aligned eksplisit (6 sisi, tiap sisi 4 vertex + normal +
// texcoord). Dipakai untuk dinding, kusen, kolom, furnitur, dsb.
void DrawBox(float x1, float y1, float z1, float x2, float y2, float z2);

// Quad datar dari 4 vertex eksplisit (untuk lantai, plafon, kaca, atap
// miring, dsb yang tidak selalu axis-aligned).
void DrawQuadV(const Vec3 &a, const Vec3 &b, const Vec3 &c, const Vec3 &d,
               float nx, float ny, float nz, float uRep = 1.0f, float vRep = 1.0f);

// Segitiga datar dari 3 vertex eksplisit (dipakai untuk ujung atap limasan).
void DrawTriV(const Vec3 &a, const Vec3 &b, const Vec3 &c,
              float nx, float ny, float nz);

// Teks vektor 3D (stroke font GLUT - tetap berbasis garis/vertex, bukan
// bitmap raster) untuk signage seperti tulisan "COCA" di kanopi.
void DrawStrokeText(const char *text, float x, float y, float z,
                     float scale, float r, float g, float b);

#endif
