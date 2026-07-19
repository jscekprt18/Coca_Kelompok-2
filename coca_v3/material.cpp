#include "material.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/freeglut.h>

static void SetMat(float ar,float ag,float ab,
                    float dr,float dg,float db,
                    float sr,float sg,float sb,
                    float shininess)
{
    GLfloat amb[4] = {ar,ag,ab,1.0f};
    GLfloat dif[4] = {dr,dg,db,1.0f};
    GLfloat spe[4] = {sr,sg,sb,1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  dif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spe);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    glColor3f(dr,dg,db); // fallback kalau lighting sedang off
}

void ApplyMaterial(MaterialID id)
{
    switch (id) {
        // Dinding krem hangat (facade asli: dinding & kanopi cream/off-white)
        case MAT_WALL:          SetMat(0.80f,0.76f,0.65f, 0.94f,0.89f,0.76f, 0.06f,0.06f,0.05f, 8.0f);  break;
        // Atap/plafon aksen merah marun tua (ceiling panel & atap seng asli)
        case MAT_ROOF:           SetMat(0.10f,0.06f,0.05f, 0.20f,0.11f,0.09f, 0.10f,0.08f,0.06f, 16.0f); break;
        case MAT_GLASS:          SetMat(0.35f,0.45f,0.50f, 0.55f,0.70f,0.75f, 0.85f,0.90f,0.92f, 90.0f); break;
        // Beton/plester krem terang (kolom kanopi, dinding luar)
        case MAT_CONCRETE:       SetMat(0.80f,0.76f,0.67f, 0.92f,0.87f,0.76f, 0.10f,0.10f,0.09f, 10.0f);  break;
        // Kayu walnut hangat (meja/kursi mid-century, kusen)
        case MAT_WOOD:            SetMat(0.42f,0.26f,0.14f, 0.62f,0.38f,0.19f, 0.18f,0.14f,0.10f, 22.0f); break;
        // Lantai teraso/bata terracotta (lantai indoor asli, area kasir/duduk utama)
        case MAT_FLOOR:           SetMat(0.52f,0.26f,0.16f, 0.66f,0.32f,0.19f, 0.10f,0.08f,0.06f, 14.0f); break;
        // Paving abu hangat outdoor (teras depan asli)
        case MAT_FLOOR_OUTDOOR:   SetMat(0.45f,0.43f,0.41f, 0.55f,0.53f,0.50f, 0.08f,0.08f,0.08f, 8.0f);  break;
        case MAT_METAL:           SetMat(0.20f,0.20f,0.21f, 0.30f,0.30f,0.32f, 0.55f,0.55f,0.58f, 70.0f); break;
        case MAT_LEAF:            SetMat(0.10f,0.25f,0.10f, 0.20f,0.45f,0.18f, 0.05f,0.08f,0.05f, 6.0f);  break;
        case MAT_PASTRY_A:        SetMat(0.35f,0.24f,0.14f, 0.85f,0.65f,0.35f, 0.20f,0.18f,0.12f, 10.0f); break;
        case MAT_PASTRY_B:        SetMat(0.30f,0.16f,0.08f, 0.75f,0.45f,0.20f, 0.20f,0.15f,0.10f, 10.0f); break;
        case MAT_PASTRY_C:        SetMat(0.38f,0.32f,0.18f, 0.90f,0.80f,0.55f, 0.25f,0.22f,0.15f, 10.0f); break;
        // Sofa bantal oranye (banquette dekat kasir, foto asli)
        case MAT_SOFA_ORANGE:     SetMat(0.45f,0.18f,0.04f, 0.92f,0.40f,0.10f, 0.15f,0.10f,0.05f, 12.0f); break;
        // Panel aksen marun (dinding di belakang sofa, foto asli)
        case MAT_ACCENT_MAROON:   SetMat(0.22f,0.09f,0.07f, 0.40f,0.16f,0.12f, 0.10f,0.07f,0.06f, 10.0f); break;
        // Huruf logo metal keperakan
        case MAT_SIGN_METAL:      SetMat(0.55f,0.55f,0.58f, 0.80f,0.80f,0.83f, 0.90f,0.90f,0.92f, 90.0f); break;
        default:                  SetMat(0.3f,0.3f,0.3f, 0.6f,0.6f,0.6f, 0.1f,0.1f,0.1f, 8.0f); break;
    }
}
