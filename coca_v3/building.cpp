#include "building.h"
#include "geometry.h"
#include "material.h"
#include "camera.h"   // AddCollisionBox

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/freeglut.h>
#include <cmath>

/* =====================================================================
   [MODEL] Denah -> koordinat.
   Hasil analisis denah_coca_right.png (strip ruko, urut dari depan/
   gerbang ke belakang/tempat pegawai):

     Z -10.0 .. -6.0   Area outdoor + gerbang (di luar dinding utama)
     Z  -6.0 .. -3.0   Pintu masuk + tempat duduk santai (lounge)
     Z  -3.0 .. -2.0   Lorong 1 (koridor sempit)
     Z  -2.0 ..  3.0   Tempat duduk utama (di bawah skylight)
     Z   3.0 ..  4.0   Lorong 2 (koridor sempit)
     Z   4.0 ..  7.0   Tempat duduk samping (semi-outdoor, kisi kayu)
     Z   7.0 .. 10.0   Tempat pegawai (separuh lebar barat, direcessed)

   Lebar bangunan penuh X: -4.0 .. 4.0 (8 m). Envelope luar dibiarkan
   utuh persegi, perbedaan lebar Tempat Pegawai dibuat sebagai partisi
   interior, bukan lubang di dinding luar.

   PENTING soal jalur tur: pintu masuk & SEMUA bukaan lorong interior
   sengaja dipusatkan di x=0, karena kamera spawn di x=0 dan berjalan
   lurus (W) tanpa geser. Kalau ada bukaan yang offset dari x=0, jalan
   lurus akan menabrak dinding solid di sebelah bukaan itu (bug yang
   sudah pernah terjadi & diperbaiki di versi sebelumnya).
   ===================================================================== */
static const float BX1 = -4.0f, BX2 = 4.0f;
static const float BZ1 = -6.0f, BZ2 = 10.0f;
static const float WALL_T = 0.20f;
static const float WALL_H = 3.2f;
static const float FLOOR_Y = 0.0f;
static const float CEIL_Y  = WALL_H;

// ---------------------------------------------------------------------
// Dinding axis-aligned dengan ketebalan eksplisit (extrude dari garis
// denah 2D), lewat DrawBox (vertex eksplisit, bukan glutSolidCube).
// ---------------------------------------------------------------------
static void BuildWallAlongX(float x1, float x2, float z, float yBase, float h,
                             float thick, MaterialID mat, bool collide = true) {
    ApplyMaterial(mat);
    DrawBox(x1, yBase, z - thick*0.5f, x2, yBase + h, z + thick*0.5f);
    if (collide) AddCollisionBox(x1, z - thick*0.5f, x2, z + thick*0.5f, yBase, h);
}
static void BuildWallAlongZ(float z1, float z2, float x, float yBase, float h,
                             float thick, MaterialID mat, bool collide = true) {
    ApplyMaterial(mat);
    DrawBox(x - thick*0.5f, yBase, z1, x + thick*0.5f, yBase + h, z2);
    if (collide) AddCollisionBox(x - thick*0.5f, z1, x + thick*0.5f, z2, yBase, h);
}

// Dinding sepanjang X dengan bukaan pintu (kusen 0.9 x 2.2 m realistis).
static void BuildWallXWithDoor(float x1, float x2, float z, float yBase, float h,
                                float thick, MaterialID wallMat, float doorCenterX) {
    const float dw = 0.9f, dh = 2.2f;
    float d0 = doorCenterX - dw*0.5f, d1 = doorCenterX + dw*0.5f;
    if (d0 - x1 > 0.02f) BuildWallAlongX(x1, d0, z, yBase, h, thick, wallMat);
    if (x2 - d1 > 0.02f) BuildWallAlongX(d1, x2, z, yBase, h, thick, wallMat);
    ApplyMaterial(wallMat);
    DrawBox(d0, yBase + dh, z - thick*0.5f, d1, yBase + h, z + thick*0.5f); // lintel
    AddCollisionBox(d0, z - thick*0.5f, d1, z + thick*0.5f, yBase + dh, h - dh);
    ApplyMaterial(MAT_WOOD);
    float ft = 0.06f;
    DrawBox(d0-ft, yBase, z-thick*0.5f-0.01f, d0, yBase+dh+ft, z+thick*0.5f+0.01f); // kusen kiri
    DrawBox(d1,    yBase, z-thick*0.5f-0.01f, d1+ft, yBase+dh+ft, z+thick*0.5f+0.01f); // kusen kanan
    DrawBox(d0-ft, yBase+dh, z-thick*0.5f-0.01f, d1+ft, yBase+dh+ft, z+thick*0.5f+0.01f); // kusen atas
    DrawBox(d0+0.02f, yBase+0.02f, z+thick*0.5f, d1-0.02f, yBase+dh-0.02f, z+thick*0.5f+0.04f); // daun pintu
}

// Segmen kaca toko lebar (nyaris lantai-ke-plafon) dg mullion kayu tipis
// tiap ~0.9m - meniru fasad kaca depan COCA asli (bukan jendela kecil).
static void BuildGlassStorefrontSeg(float x0, float x1, float z, float yBase,
                                     float h, float thick, float sillY) {
    float top = yBase + h - 0.15f; // sisakan lintel/fasia krem di atas
    ApplyMaterial(MAT_CONCRETE);
    if (sillY - yBase > 0.02f) DrawBox(x0, yBase, z-thick*0.5f, x1, sillY, z+thick*0.5f); // kickplate rendah
    ApplyMaterial(MAT_WALL);
    DrawBox(x0, top, z-thick*0.5f, x1, yBase+h, z+thick*0.5f); // fasia atas krem
    AddCollisionBox(x0, z-thick*0.5f, x1, z+thick*0.5f, sillY, top-sillY);

    ApplyMaterial(MAT_GLASS);
    { Vec3 a={x0,sillY,z}, b={x1,sillY,z}, c={x1,top,z}, d={x0,top,z};
      DrawQuadV(a,b,c,d, 0,0,1, (x1-x0)*0.8f, 1.6f); }

    ApplyMaterial(MAT_WOOD);
    int nbay = (int)((x1-x0)/0.9f + 0.5f);
    if (nbay < 1) nbay = 1;
    for (int i = 1; i < nbay; ++i) {
        float mx = x0 + (x1-x0) * (float)i / nbay;
        DrawBox(mx-0.02f, sillY, z-thick*0.5f-0.005f, mx+0.02f, top, z+thick*0.5f+0.005f);
    }
    DrawBox(x0, sillY-0.03f, z-thick*0.5f-0.01f, x1, sillY+0.01f, z+thick*0.5f+0.01f);
    DrawBox(x0, top-0.01f,   z-thick*0.5f-0.01f, x1, top+0.03f,   z+thick*0.5f+0.01f);
}

// Dinding sepanjang X dengan fasad kaca toko lebar + 1 pintu kaca di
// tengah (x=0, supaya jalur kamera lurus tetap valid).
static void BuildWallXWithGlassStorefront(float x1, float x2, float z, float yBase,
                                           float h, float thick, MaterialID wallMat,
                                           float doorCenterX) {
    const float dw = 1.0f, dh = 2.3f;
    float d0 = doorCenterX - dw*0.5f, d1 = doorCenterX + dw*0.5f;
    float sillY = yBase + 0.05f;
    if (d0 - x1 > 0.05f) BuildGlassStorefrontSeg(x1, d0, z, yBase, h, thick, sillY);
    if (x2 - d1 > 0.05f) BuildGlassStorefrontSeg(d1, x2, z, yBase, h, thick, sillY);

    ApplyMaterial(wallMat);
    DrawBox(d0, yBase+dh, z - thick*0.5f, d1, yBase + h, z + thick*0.5f); // lintel pintu
    AddCollisionBox(d0, z - thick*0.5f, d1, z + thick*0.5f, yBase + dh, h - dh);
    ApplyMaterial(MAT_WOOD);
    float ft = 0.06f;
    DrawBox(d0-ft, yBase, z-thick*0.5f-0.01f, d0, yBase+dh+ft, z+thick*0.5f+0.01f);
    DrawBox(d1,    yBase, z-thick*0.5f-0.01f, d1+ft, yBase+dh+ft, z+thick*0.5f+0.01f);
    DrawBox(d0-ft, yBase+dh, z-thick*0.5f-0.01f, d1+ft, yBase+dh+ft, z+thick*0.5f+0.01f);
    ApplyMaterial(MAT_GLASS);
    DrawBox(d0+0.02f, yBase+0.02f, z+thick*0.5f, d1-0.02f, yBase+dh-0.02f, z+thick*0.5f+0.03f); // daun pintu kaca
}

// Dinding sepanjang X dengan bukaan jendela kayu grid + kaca.
static void BuildWallXWithWindow(float x1, float x2, float z, float yBase, float h,
                                  float thick, MaterialID wallMat,
                                  float winCenterX, float winW, float winH, float sillY) {
    float w0 = winCenterX - winW*0.5f, w1 = winCenterX + winW*0.5f;
    float wTop = sillY + winH;
    if (w0 - x1 > 0.02f) BuildWallAlongX(x1, w0, z, yBase, h, thick, wallMat);
    if (x2 - w1 > 0.02f) BuildWallAlongX(w1, x2, z, yBase, h, thick, wallMat);
    ApplyMaterial(wallMat);
    if (sillY - yBase > 0.02f) DrawBox(w0, yBase, z-thick*0.5f, w1, sillY, z+thick*0.5f);
    if (yBase + h - wTop > 0.02f) DrawBox(w0, wTop, z-thick*0.5f, w1, yBase+h, z+thick*0.5f);
    AddCollisionBox(w0, z-thick*0.5f, w1, z+thick*0.5f, sillY, winH);
    ApplyMaterial(MAT_GLASS);
    { Vec3 a={w0,sillY,z}, b={w1,sillY,z}, c={w1,wTop,z}, d={w0,wTop,z};
      DrawQuadV(a,b,c,d, 0,0,1, 2,2); }
    ApplyMaterial(MAT_WOOD);
    float ft = 0.05f;
    DrawBox(w0-ft, sillY-ft, z-thick*0.5f-0.01f, w1+ft, wTop+ft, z-thick*0.5f+0.03f);
    // kisi kayu rapat ala foto asli (jendela kayu grid depan COCA)
    int cols = 6, rows = 5;
    for (int i = 1; i < cols; ++i) {
        float mx = w0 + (w1-w0) * (float)i / cols;
        DrawBox(mx-0.02f, sillY, z-0.03f, mx+0.02f, wTop, z+0.03f);
    }
    for (int j = 1; j < rows; ++j) {
        float my = sillY + winH * (float)j / rows;
        DrawBox(w0, my-0.02f, z-0.03f, w1, my+0.02f, z+0.03f);
    }
}

// Dinding sepanjang Z (dinding barat/timur) dengan bukaan jendela.
static void BuildWallZWithWindow(float z1, float z2, float x, float yBase, float h,
                                  float thick, MaterialID wallMat,
                                  float winCenterZ, float winW, float winH, float sillY) {
    float w0 = winCenterZ - winW*0.5f, w1 = winCenterZ + winW*0.5f;
    float wTop = sillY + winH;
    if (w0 - z1 > 0.02f) BuildWallAlongZ(z1, w0, x, yBase, h, thick, wallMat);
    if (z2 - w1 > 0.02f) BuildWallAlongZ(w1, z2, x, yBase, h, thick, wallMat);
    ApplyMaterial(wallMat);
    if (sillY - yBase > 0.02f) DrawBox(x-thick*0.5f, yBase, w0, x+thick*0.5f, sillY, w1);
    if (yBase + h - wTop > 0.02f) DrawBox(x-thick*0.5f, wTop, w0, x+thick*0.5f, yBase+h, w1);
    AddCollisionBox(x-thick*0.5f, w0, x+thick*0.5f, w1, sillY, winH);
    ApplyMaterial(MAT_GLASS);
    { Vec3 a={x,sillY,w0}, b={x,sillY,w1}, c={x,wTop,w1}, d={x,wTop,w0};
      DrawQuadV(a,b,c,d, 1,0,0, 2,2); }
    ApplyMaterial(MAT_WOOD);
    float ft = 0.05f;
    DrawBox(x-thick*0.5f-0.03f, sillY-ft, w0-ft, x-thick*0.5f+0.01f, wTop+ft, w1+ft);
    // kisi kayu rapat ala foto asli (tempat duduk samping semi-outdoor)
    int cols = 6, rows = 5;
    for (int i = 1; i < cols; ++i) {
        float mz = w0 + (w1-w0) * (float)i / cols;
        DrawBox(x-0.03f, sillY, mz-0.02f, x+0.03f, wTop, mz+0.02f);
    }
    for (int j = 1; j < rows; ++j) {
        float my = sillY + winH * (float)j / rows;
        DrawBox(x-0.03f, my-0.02f, w0, x+0.03f, my+0.02f, w1);
    }
}

// ---------------------------------------------------------------------
// Lantai per-ruangan.
// ---------------------------------------------------------------------
static void BuildFloorSlab(float x1, float z1, float x2, float z2, float y,
                            MaterialID mat, float uRep = 1, float vRep = 1) {
    ApplyMaterial(mat);
    { Vec3 a={x1,y,z1}, b={x2,y,z1}, c={x2,y,z2}, d={x1,y,z2};
      DrawQuadV(a,d,c,b, 0,1,0, uRep, vRep); }
    ApplyMaterial(MAT_CONCRETE);
    { Vec3 a={x1,y-0.12f,z1}, b={x2,y-0.12f,z1}, c={x2,y-0.12f,z2}, d={x1,y-0.12f,z2};
      DrawQuadV(a,b,c,d, 0,-1,0, uRep, vRep); }
}

// Tanah/site di sekeliling bangunan - tanpa ini bangunan terlihat
// "melayang" saat dilihat dari luar footprint (jalan depan gerbang dsb).
static void BuildSiteGround() {
    ApplyMaterial(MAT_CONCRETE);
    const float G = 30.0f;
    Vec3 a={-G,-0.03f,-G}, b={G,-0.03f,-G}, c={G,-0.03f,G}, d={-G,-0.03f,G};
    DrawQuadV(a,d,c,b, 0,1,0, 24,24);
}

// ---------------------------------------------------------------------
// Plafon + bukaan skylight di ruang tempat duduk utama.
// ---------------------------------------------------------------------
static void BuildCeilingWithSkylightHole(float x1, float z1, float x2, float z2,
                                          float holeX1, float holeZ1, float holeX2, float holeZ2) {
    ApplyMaterial(MAT_WALL);
    Vec3 a,b,c,d;
    a={x1,CEIL_Y,z1}; b={x2,CEIL_Y,z1}; c={x2,CEIL_Y,holeZ1}; d={x1,CEIL_Y,holeZ1};
    DrawQuadV(a,d,c,b, 0,-1,0, 4,1);
    a={x1,CEIL_Y,holeZ2}; b={x2,CEIL_Y,holeZ2}; c={x2,CEIL_Y,z2}; d={x1,CEIL_Y,z2};
    DrawQuadV(a,d,c,b, 0,-1,0, 4,1);
    a={x1,CEIL_Y,holeZ1}; b={holeX1,CEIL_Y,holeZ1}; c={holeX1,CEIL_Y,holeZ2}; d={x1,CEIL_Y,holeZ2};
    DrawQuadV(a,d,c,b, 0,-1,0, 1,3);
    a={holeX2,CEIL_Y,holeZ1}; b={x2,CEIL_Y,holeZ1}; c={x2,CEIL_Y,holeZ2}; d={holeX2,CEIL_Y,holeZ2};
    DrawQuadV(a,d,c,b, 0,-1,0, 1,3);

    ApplyMaterial(MAT_METAL);
    float ft = 0.08f, riseH = 0.35f;
    DrawBox(holeX1-ft, CEIL_Y, holeZ1-ft, holeX1, CEIL_Y+riseH, holeZ2+ft);
    DrawBox(holeX2, CEIL_Y, holeZ1-ft, holeX2+ft, CEIL_Y+riseH, holeZ2+ft);
    DrawBox(holeX1, CEIL_Y, holeZ1-ft, holeX2, CEIL_Y+riseH, holeZ1);
    DrawBox(holeX1, CEIL_Y, holeZ2, holeX2, CEIL_Y+riseH, holeZ2+ft);
    ApplyMaterial(MAT_GLASS);
    { Vec3 ga={holeX1,CEIL_Y+riseH,holeZ1}, gb={holeX2,CEIL_Y+riseH,holeZ1},
           gc={holeX2,CEIL_Y+riseH,holeZ2}, gd={holeX1,CEIL_Y+riseH,holeZ2};
      DrawQuadV(ga,gd,gc,gb, 0,1,0, 2,2); }
}
static void BuildCeilingPlain(float x1, float z1, float x2, float z2) {
    ApplyMaterial(MAT_WALL);
    Vec3 a={x1,CEIL_Y,z1}, b={x2,CEIL_Y,z1}, c={x2,CEIL_Y,z2}, d={x1,CEIL_Y,z2};
    DrawQuadV(a,d,c,b, 0,-1,0, 4,2);
}

// ---------------------------------------------------------------------
// Atap limasan (hip roof): 2 sisi trapesium panjang + 2 sisi segitiga
// ujung, overhang, underside, fasia, talang sederhana.
// ---------------------------------------------------------------------
static void BuildRoof() {
    const float OH = 0.6f;
    const float eaveX1 = BX1 - OH, eaveX2 = BX2 + OH;
    const float eaveZ1 = BZ1 - OH, eaveZ2 = BZ2 + OH;
    const float eaveY  = WALL_H + 0.25f;
    // Pitch landai (ruko asli genteng rendah di belakang kanopi, BUKAN
    // atap limasan curam berdiri sendiri seperti rumah tunggal)
    const float riseH  = 1.05f;
    const float ridgeY = eaveY + riseH;
    const float halfSpan = (eaveX2 - eaveX1) * 0.5f;
    float ridgeZ1 = eaveZ1 + halfSpan, ridgeZ2 = eaveZ2 - halfSpan;
    if (ridgeZ1 > ridgeZ2) { float m=(ridgeZ1+ridgeZ2)*0.5f; ridgeZ1=ridgeZ2=m; }

    ApplyMaterial(MAT_ROOF);
    { Vec3 a={eaveX1,eaveY,eaveZ1}, b={eaveX1,eaveY,eaveZ2}, c={0,ridgeY,ridgeZ2}, d={0,ridgeY,ridgeZ1};
      DrawQuadV(a,b,c,d, -0.4f,0.6f,0, 6,2); }
    { Vec3 a={eaveX2,eaveY,eaveZ1}, b={0,ridgeY,ridgeZ1}, c={0,ridgeY,ridgeZ2}, d={eaveX2,eaveY,eaveZ2};
      DrawQuadV(a,b,c,d, 0.4f,0.6f,0, 6,2); }
    { Vec3 a={eaveX1,eaveY,eaveZ1}, b={0,ridgeY,ridgeZ1}, c={eaveX2,eaveY,eaveZ1};
      DrawTriV(a,b,c, 0,0.6f,-0.5f); }
    { Vec3 a={eaveX1,eaveY,eaveZ2}, b={eaveX2,eaveY,eaveZ2}, c={0,ridgeY,ridgeZ2};
      DrawTriV(a,b,c, 0,0.6f,0.5f); }

    const float thick = 0.12f;
    ApplyMaterial(MAT_CONCRETE);
    { Vec3 a={eaveX1,eaveY-thick,eaveZ1}, b={eaveX1,eaveY-thick,eaveZ2}, c={0,ridgeY-thick,ridgeZ2}, d={0,ridgeY-thick,ridgeZ1};
      DrawQuadV(d,c,b,a, 0.4f,-0.6f,0, 6,2); }
    { Vec3 a={eaveX2,eaveY-thick,eaveZ1}, b={0,ridgeY-thick,ridgeZ1}, c={0,ridgeY-thick,ridgeZ2}, d={eaveX2,eaveY-thick,eaveZ2};
      DrawQuadV(d,c,b,a, -0.4f,-0.6f,0, 6,2); }

    // Lis fasia krem di keliling eave (garis putih tipis khas foto asli
    // di bawah genteng, kontras dengan genteng gelap)
    ApplyMaterial(MAT_WALL);
    const float fasciaH = 0.14f;
    DrawBox(eaveX1-0.05f, eaveY-thick-fasciaH, eaveZ1-0.05f, eaveX1+0.05f, eaveY-thick, eaveZ2+0.05f);
    DrawBox(eaveX2-0.05f, eaveY-thick-fasciaH, eaveZ1-0.05f, eaveX2+0.05f, eaveY-thick, eaveZ2+0.05f);
    DrawBox(eaveX1-0.05f, eaveY-thick-fasciaH, eaveZ1-0.05f, eaveX2+0.05f, eaveY-thick, eaveZ1+0.05f);
    DrawBox(eaveX1-0.05f, eaveY-thick-fasciaH, eaveZ2-0.05f, eaveX2+0.05f, eaveY-thick, eaveZ2+0.05f);

    ApplyMaterial(MAT_CONCRETE);
    DrawBox(eaveX1-0.03f, eaveY-thick, eaveZ1-0.03f, eaveX1+0.03f, eaveY, eaveZ2+0.03f);
    DrawBox(eaveX2-0.03f, eaveY-thick, eaveZ1-0.03f, eaveX2+0.03f, eaveY, eaveZ2+0.03f);
    DrawBox(eaveX1-0.03f, eaveY-thick, eaveZ1-0.03f, eaveX2+0.03f, eaveY, eaveZ1+0.03f);
    DrawBox(eaveX1-0.03f, eaveY-thick, eaveZ2-0.03f, eaveX2+0.03f, eaveY, eaveZ2+0.03f);

    ApplyMaterial(MAT_METAL);
    float gY = eaveY - thick - fasciaH - 0.06f;
    DrawBox(eaveX1-0.08f, gY, eaveZ1-0.05f, eaveX1+0.02f, gY+0.08f, eaveZ2+0.05f);
    DrawBox(eaveX2-0.02f, gY, eaveZ1-0.05f, eaveX2+0.08f, gY+0.08f, eaveZ2+0.05f);
}

// ---------------------------------------------------------------------
// Kanopi beton modern depan pintu masuk + tulisan "COCA".
// ---------------------------------------------------------------------
static void BuildCanopy() {
    const float cz1 = -8.4f, cz2 = -5.6f;
    const float cx1 = -3.2f, cx2 = 3.2f;
    const float slabYtop = 3.0f, slabThick = 0.18f;

    // Slab kanopi utama, krem/off-white seperti foto asli
    ApplyMaterial(MAT_CONCRETE);
    DrawBox(cx1, slabYtop-slabThick, cz1, cx2, slabYtop, cz2);

    // Fascia depan berlapis (profil cornice klasik ala foto: 3 undakan
    // krem yang makin maju ke bawah, meniru list beton di atas tulisan COCA)
    DrawBox(cx1-0.05f, slabYtop-slabThick-0.10f, cz1, cx2+0.05f, slabYtop-slabThick, cz1+0.15f);
    DrawBox(cx1-0.10f, slabYtop-slabThick-0.22f, cz1-0.02f, cx2+0.10f, slabYtop-slabThick-0.10f, cz1+0.17f);
    DrawBox(cx1-0.05f, slabYtop-slabThick-0.34f, cz1, cx2+0.05f, slabYtop-slabThick-0.22f, cz1+0.15f);

    // Kolom penyangga krem
    float colY0 = 0.0f, colY1 = slabYtop - slabThick;
    DrawBox(cx1, colY0, cz1+0.1f, cx1+0.25f, colY1, cz1+0.35f);
    DrawBox(cx2-0.25f, colY0, cz1+0.1f, cx2, colY1, cz1+0.35f);
    AddCollisionBox(cx1, cz1+0.1f, cx1+0.25f, cz1+0.35f, colY0, colY1);
    AddCollisionBox(cx2-0.25f, cz1+0.1f, cx2, cz1+0.35f, colY0, colY1);

    DrawBox(cx1, colY1, cz1+0.1f, cx2, colY1+0.15f, cz1+0.35f);

    // Plafon bawah kanopi: aksen merah marun (seperti plafon kayu gelap di
    // foto interior dekat kasir), bukan krem polos
    ApplyMaterial(MAT_ACCENT_MAROON);
    { Vec3 a={cx1,slabYtop-slabThick-0.02f,cz1}, b={cx2,slabYtop-slabThick-0.02f,cz1},
           c={cx2,slabYtop-slabThick-0.02f,cz2}, d={cx1,slabYtop-slabThick-0.02f,cz2};
      DrawQuadV(a,d,c,b, 0,-1,0, 4,1); }

    // Tulisan "COCA" huruf timbul metal keperakan (bukan gelap), dipasang
    // di undakan fascia tengah persis seperti foto signage asli
    ApplyMaterial(MAT_SIGN_METAL);
    DrawStrokeText("COCA", 0.55f, slabYtop-slabThick-0.19f, cz1-0.03f, 0.0032f, 0.85f,0.85f,0.88f);

    // Aksen bola merah gantung (pendant lamp bulat merah khas foto interior)
    ApplyMaterial(MAT_SOFA_ORANGE);
    glPushMatrix();
    glTranslatef(0.0f, colY1 - 0.35f, cz1 + 0.25f);
    glutSolidSphere(0.09, 16, 12);
    glPopMatrix();
}

// ---------------------------------------------------------------------
// Kasir + etalase pastry (meniru foto: panel kayu, LED strip, kaca 2
// tingkat berisi pastry, blok mesin espresso).
// ---------------------------------------------------------------------
static void BuildCashierCounter(float x1, float x2, float z, float depth) {
    const float topY = 0.95f, topThick = 0.05f;
    ApplyMaterial(MAT_WOOD);
    int slats = (int)((x2-x1)/0.15f);
    for (int i = 0; i < slats; ++i) {
        float sx0 = x1 + i*0.15f, sx1 = sx0 + 0.12f;
        if (sx1 > x2) sx1 = x2;
        DrawBox(sx0, 0.0f, z, sx1, topY-topThick, z+depth);
    }
    ApplyMaterial(MAT_CONCRETE);
    DrawBox(x1-0.02f, topY-topThick, z-0.02f, x2+0.02f, topY, z+depth+0.02f);

    // strip LED hangat di bawah top counter
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.75f, 0.30f);
    glBegin(GL_QUADS);
        glNormal3f(0,0,1);
        glVertex3f(x1, topY-topThick-0.03f, z+depth+0.005f);
        glVertex3f(x2, topY-topThick-0.03f, z+depth+0.005f);
        glVertex3f(x2, topY-topThick,       z+depth+0.005f);
        glVertex3f(x1, topY-topThick,       z+depth+0.005f);
    glEnd();
    glEnable(GL_LIGHTING);

    // etalase pastry kaca, 2 tingkat, di atas counter
    float ex0 = x1+0.3f, ex1 = x1+1.6f, ez0 = z+0.1f, ez1 = z+depth-0.1f;
    float caseY0 = topY, caseY1 = topY+0.55f;
    ApplyMaterial(MAT_GLASS);
    DrawBox(ex0, caseY0, ez0, ex1, caseY1, ez1);
    ApplyMaterial(MAT_METAL);
    DrawBox(ex0, caseY0, ez0, ex1, caseY0+0.02f, ez1);
    DrawBox(ex0, caseY0+0.25f, ez0, ex1, caseY0+0.27f, ez1); // rak tengah
    // pastry warna-warni di tiap rak (kotak2 kecil, cukup utk representasi vertex)
    MaterialID pastryMats[3] = {MAT_PASTRY_A, MAT_PASTRY_B, MAT_PASTRY_C};
    for (int shelf = 0; shelf < 2; ++shelf) {
        float sy0 = caseY0 + 0.03f + shelf*0.25f;
        float sy1 = sy0 + 0.10f;
        int n = 4;
        for (int i = 0; i < n; ++i) {
            float px0 = ex0 + 0.05f + i*((ex1-ex0-0.1f)/n);
            float px1 = px0 + (ex1-ex0-0.1f)/n - 0.03f;
            ApplyMaterial(pastryMats[(shelf+i)%3]);
            DrawBox(px0, sy0, ez0+0.03f, px1, sy1, ez1-0.03f);
        }
    }

    // blok mesin espresso + grinder (dekat ujung counter)
    ApplyMaterial(MAT_METAL);
    DrawBox(x2-1.0f, topY, z+0.15f, x2-0.2f, topY+0.42f, z+depth-0.15f);
    DrawBox(x2-1.25f, topY, z+0.15f, x2-1.05f, topY+0.55f, z+0.35f);
}

// ---------------------------------------------------------------------
// Meja makan pelanggan kayu (4 kursi), meniru foto meja cafe.
// ---------------------------------------------------------------------
static void BuildIndoorTableSet(float cx, float cz) {
    ApplyMaterial(MAT_WOOD);
    DrawBox(cx-0.55f, 0.72f, cz-0.55f, cx+0.55f, 0.78f, cz+0.55f); // top meja
    DrawBox(cx-0.05f, 0.0f, cz-0.05f, cx+0.05f, 0.72f, cz+0.05f);  // kaki tunggal tengah
    for (int i = 0; i < 4; ++i) {
        float ang = i * ((float)M_PI/2.0f);
        float ox = cosf(ang)*0.85f, oz = sinf(ang)*0.85f;
        float chx = cx+ox, chz = cz+oz;
        DrawBox(chx-0.22f, 0.42f, chz-0.22f, chx+0.22f, 0.47f, chz+0.22f); // dudukan
        DrawBox(chx-0.22f, 0.0f, chz-0.22f, chx-0.17f, 0.42f, chz-0.17f);
        DrawBox(chx+0.17f, 0.0f, chz-0.22f, chx+0.22f, 0.42f, chz-0.17f);
        DrawBox(chx-0.22f, 0.0f, chz+0.17f, chx-0.17f, 0.42f, chz+0.22f);
        DrawBox(chx+0.17f, 0.0f, chz+0.17f, chx+0.22f, 0.42f, chz+0.22f);
        DrawBox(chx-0.20f, 0.47f, chz+0.17f, chx+0.20f, 0.90f, chz+0.22f); // sandaran
    }
}

// ---------------------------------------------------------------------
// Sofa banquette oranye + panel dinding aksen marun di belakangnya,
// meniru foto interior asli (sofa oranye menempel dinding, meja kecil
// kayu + kursi hitam di depannya).
// ---------------------------------------------------------------------
static void BuildBanquetteSofa(float x, float z1, float z2, float wallX) {
    // panel aksen marun di dinding belakang sofa
    ApplyMaterial(MAT_ACCENT_MAROON);
    DrawBox(wallX-0.02f, 0.0f, z1, wallX+0.001f, 1.6f, z2);

    // badan sofa (bantalan duduk + sandaran) oranye
    ApplyMaterial(MAT_SOFA_ORANGE);
    DrawBox(x-0.28f, 0.40f, z1+0.05f, x+0.02f, 0.48f, z2-0.05f);       // dudukan
    DrawBox(x-0.05f, 0.20f, z1+0.05f, x+0.02f, 0.95f, z2-0.05f);        // sandaran (menempel dinding)
    DrawBox(x-0.30f, 0.0f,  z1+0.05f, x-0.24f, 0.40f, z2-0.05f);        // dasar/kaki panjang

    // meja kecil kayu bulat + 2 kursi hitam metal di depan sofa
    float tx = x - 0.75f, tz = (z1+z2)*0.5f;
    ApplyMaterial(MAT_WOOD);
    DrawBox(tx-0.30f, 0.68f, tz-0.30f, tx+0.30f, 0.73f, tz+0.30f);
    DrawBox(tx-0.04f, 0.0f, tz-0.04f, tx+0.04f, 0.68f, tz+0.04f);

    for (int s = -1; s <= 1; s += 2) {
        float ccz = tz + s*0.55f;
        float ccx = tx - 0.55f;
        ApplyMaterial(MAT_METAL); // kursi hitam metal seperti foto
        DrawBox(ccx-0.20f, 0.42f, ccz-0.20f, ccx+0.20f, 0.46f, ccz+0.20f);
        DrawBox(ccx-0.18f, 0.0f, ccz-0.02f, ccx+0.18f, 0.42f, ccz+0.02f);
        DrawBox(ccx-0.20f, 0.46f, ccz-0.20f, ccx+0.20f, 0.85f, ccz-0.15f);
    }
}

// ---------------------------------------------------------------------
// Area outdoor depan + gerbang.
// ---------------------------------------------------------------------
static void DrawSimplePot(float cx, float cz, float baseY) {
    ApplyMaterial(MAT_CONCRETE);
    DrawBox(cx-0.25f, baseY, cz-0.25f, cx+0.25f, baseY+0.35f, cz+0.25f);
    ApplyMaterial(MAT_LEAF);
    for (int i = 0; i < 5; ++i) {
        float ang = i * (2.0f*(float)M_PI/5.0f);
        float ox = cosf(ang)*0.18f, oz = sinf(ang)*0.18f;
        DrawBox(cx+ox-0.15f, baseY+0.35f, cz+oz-0.08f, cx+ox+0.15f, baseY+0.75f, cz+oz+0.08f);
    }
}
static void DrawOutdoorTableSet(float cx, float cz) {
    ApplyMaterial(MAT_WOOD);
    DrawBox(cx-0.5f, 0.72f, cz-0.4f, cx+0.5f, 0.78f, cz+0.4f);
    DrawBox(cx-0.45f, 0.0f, cz-0.35f, cx-0.38f, 0.72f, cz-0.28f);
    DrawBox(cx+0.38f, 0.0f, cz-0.35f, cx+0.45f, 0.72f, cz-0.28f);
    DrawBox(cx-0.45f, 0.0f, cz+0.28f, cx-0.38f, 0.72f, cz+0.35f);
    DrawBox(cx+0.38f, 0.0f, cz+0.28f, cx+0.45f, 0.72f, cz+0.35f);
    for (int s = -1; s <= 1; s += 2) {
        float chz = cz + s*0.75f;
        DrawBox(cx-0.25f, 0.45f, chz-0.02f, cx+0.25f, 0.5f, chz+0.35f*s);
        DrawBox(cx-0.25f, 0.0f, chz-0.02f, cx-0.20f, 0.45f, chz+0.02f);
        DrawBox(cx+0.20f, 0.0f, chz-0.02f, cx+0.25f, 0.45f, chz+0.02f);
    }
}
static void BuildOutdoorAndGate() {
    const float oz1 = -10.0f, oz2 = BZ1;
    const float ox1 = BX1, ox2 = BX2;

    BuildFloorSlab(ox1, oz1, ox2, oz2, FLOOR_Y, MAT_FLOOR_OUTDOOR, 6, 2);

    const float fenceH = 1.0f, fenceT = 0.15f;
    BuildWallAlongZ(oz1, oz2, ox1, FLOOR_Y, fenceH, fenceT, MAT_CONCRETE);
    BuildWallAlongZ(oz1, oz2, ox2, FLOOR_Y, fenceH, fenceT, MAT_CONCRETE);

    float gateW = 2.4f;
    float g0 = -gateW*0.5f, g1 = gateW*0.5f;
    BuildWallAlongX(ox1, g0, oz1, FLOOR_Y, fenceH, fenceT, MAT_CONCRETE);
    BuildWallAlongX(g1, ox2, oz1, FLOOR_Y, fenceH, fenceT, MAT_CONCRETE);
    ApplyMaterial(MAT_CONCRETE);
    DrawBox(g0-0.12f, FLOOR_Y, oz1-0.12f, g0, fenceH+0.3f, oz1+0.12f);
    DrawBox(g1,       FLOOR_Y, oz1-0.12f, g1+0.12f, fenceH+0.3f, oz1+0.12f);
    AddCollisionBox(g0-0.12f, oz1-0.12f, g0, oz1+0.12f, FLOOR_Y, fenceH+0.3f);
    AddCollisionBox(g1, oz1-0.12f, g1+0.12f, oz1+0.12f, FLOOR_Y, fenceH+0.3f);
    ApplyMaterial(MAT_METAL);
    DrawBox(g0, FLOOR_Y, oz1-0.03f, g0+0.9f, fenceH, oz1+0.30f);
    DrawBox(g1-0.9f, FLOOR_Y, oz1-0.30f, g1, fenceH, oz1+0.03f);

    DrawSimplePot(ox1+0.6f, oz2-0.6f, FLOOR_Y);
    DrawSimplePot(ox2-0.6f, oz2-0.6f, FLOOR_Y);
    DrawOutdoorTableSet(-1.5f, -8.6f);
    DrawOutdoorTableSet( 1.5f, -8.6f);
}

// =====================================================================
// BuildCOCA(): orkestrasi seluruh bangunan, dipanggil dari scene.cpp.
// =====================================================================
void BuildCOCA()
{
    ClearCollisionBoxes(); // penting: dipanggil ulang tiap kali display list dibangun ulang

    BuildSiteGround();

    const float zLounge1 = -6.0f, zLounge2 = -3.0f;
    const float zLor1a = -3.0f,  zLor1b = -2.0f;
    const float zMain1 = -2.0f,  zMain2 = 3.0f;
    const float zLor2a = 3.0f,   zLor2b = 4.0f;
    const float zSide1 = 4.0f,   zSide2 = 7.0f;
    const float zStaff1 = 7.0f,  zStaff2 = 10.0f;

    /* ---- LANTAI per ruangan ---- */
    BuildFloorSlab(BX1, zLounge1, BX2, zLounge2, FLOOR_Y, MAT_FLOOR, 4,2);
    BuildFloorSlab(BX1, zLor1a,  BX2, zLor1b,  FLOOR_Y, MAT_FLOOR, 4,1);
    BuildFloorSlab(BX1, zMain1,  BX2, zMain2,  FLOOR_Y, MAT_FLOOR, 4,3);
    BuildFloorSlab(BX1, zLor2a,  BX2, zLor2b,  FLOOR_Y, MAT_FLOOR, 4,1);
    BuildFloorSlab(BX1, zSide1,  BX2, zSide2,  FLOOR_Y, MAT_FLOOR_OUTDOOR, 4,2);
    BuildFloorSlab(BX1, zStaff1, BX2, zStaff2, FLOOR_Y, MAT_FLOOR, 4,2);

    /* ---- DINDING LUAR ---- */
    BuildWallAlongZ(BZ1, BZ2, BX1, FLOOR_Y, WALL_H, WALL_T, MAT_WALL);
    BuildWallAlongZ(BZ1, zMain1, BX2, FLOOR_Y, WALL_H, WALL_T, MAT_WALL);
    BuildWallAlongZ(zMain2, zSide1, BX2, FLOOR_Y, WALL_H, WALL_T, MAT_WALL);
    BuildWallAlongZ(zSide2, BZ2, BX2, FLOOR_Y, WALL_H, WALL_T, MAT_WALL);
    BuildWallZWithWindow(zMain1, zMain2, BX2, FLOOR_Y, WALL_H, WALL_T, MAT_WALL,
                         (zMain1+zMain2)*0.5f, 3.2f, 1.8f, 0.7f);
    BuildWallXWithGlassStorefront(BX1, BX2, BZ1, FLOOR_Y, WALL_H, WALL_T, MAT_WALL, 0.0f);
    BuildWallAlongX(BX1, BX2, BZ2, FLOOR_Y, WALL_H, WALL_T, MAT_WALL);

    /* ---- PARTISI INTERIOR (bukaan lorong dipusatkan x=0, lihat catatan atas) ---- */
    const float corA = -0.5f, corB = 0.5f;
    BuildWallAlongX(BX1, corA, zLor1a, FLOOR_Y, WALL_H, WALL_T, MAT_WALL);
    BuildWallAlongX(corB, BX2, zLor1a, FLOOR_Y, WALL_H, WALL_T, MAT_WALL);
    BuildWallAlongX(BX1, corA, zMain1, FLOOR_Y, WALL_H, WALL_T, MAT_WALL);
    BuildWallAlongX(corB, BX2, zMain1, FLOOR_Y, WALL_H, WALL_T, MAT_WALL);
    BuildWallAlongX(BX1, corA, zMain2, FLOOR_Y, WALL_H, WALL_T, MAT_WALL);
    BuildWallAlongX(corB, BX2, zMain2, FLOOR_Y, WALL_H, WALL_T, MAT_WALL);
    BuildWallAlongX(BX1, corA, zLor2b, FLOOR_Y, WALL_H, WALL_T, MAT_WALL);
    BuildWallAlongX(corB, BX2, zLor2b, FLOOR_Y, WALL_H, WALL_T, MAT_WALL);
    BuildWallXWithDoor(BX1, BX2, zStaff1, FLOOR_Y, WALL_H, WALL_T, MAT_WALL, 0.0f);
    BuildWallAlongZ(zStaff1, zStaff2, -1.5f, FLOOR_Y, WALL_H, WALL_T, MAT_WALL); // digeser dari x=0, lihat catatan

    BuildWallZWithWindow(zSide1, zSide2, BX2, FLOOR_Y, WALL_H, WALL_T, MAT_WALL,
                         (zSide1+zSide2)*0.5f, 2.6f, 2.2f, 0.3f);

    /* ---- FURNITUR / INTERIOR ---- */
    BuildCashierCounter(-3.6f, -0.6f, -5.0f, 0.6f); // dibatasi x<=-0.6, tidak memotong jalur lorong x=0
    BuildIndoorTableSet(-1.9f, 0.8f);
    BuildIndoorTableSet( 1.8f, 1.5f);
    // sofa oranye + panel marun di dinding timur ruang lounge, meniru foto interior
    BuildBanquetteSofa(BX2 - 0.35f, zLounge1 + 0.3f, zLounge2 - 0.3f, BX2 - 0.10f);

    /* ---- PLAFON + SKYLIGHT ---- */
    BuildCeilingPlain(BX1, zLounge1, BX2, zLounge2);
    BuildCeilingPlain(BX1, zLor1a,  BX2, zLor1b);
    BuildCeilingWithSkylightHole(BX1, zMain1, BX2, zMain2, -1.2f, -0.5f, 1.2f, 1.8f);
    BuildCeilingPlain(BX1, zLor2a,  BX2, zLor2b);
    BuildCeilingPlain(BX1, zSide1,  BX2, zSide2);
    BuildCeilingPlain(BX1, zStaff1, BX2, zStaff2);

    /* ---- ATAP, KANOPI, OUTDOOR ---- */
    BuildRoof();
    BuildCanopy();
    BuildOutdoorAndGate();
}
