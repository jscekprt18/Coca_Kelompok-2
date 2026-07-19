#ifndef GEOMETRY_H
#define GEOMETRY_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/freeglut.h>


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


void DrawStrokeText(const char *text, float x, float y, float z,
                     float scale, float r, float g, float b);

#endif
