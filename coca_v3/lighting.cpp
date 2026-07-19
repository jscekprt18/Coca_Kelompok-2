#include "lighting.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/freeglut.h>

void InitLighting()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    GLfloat globalAmb[4] = {0.55f,0.53f,0.48f,1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);

    // LIGHT0: matahari (directional). Komponen Z dibuat cukup negatif
    // supaya dinding depan/fasad (menghadap -Z, ke arah jalan/kamera)
    // ikut kena cahaya - kalau tidak, fasad selalu gelap walau materialnya
    // krem terang (foto asli: siang mendung, cahaya rata dari depan-atas).
    glEnable(GL_LIGHT0);
    GLfloat sunDir[4] = {0.30f, 0.75f, -0.55f, 0.0f}; // w=0 -> directional
    GLfloat sunAmb[4] = {0.25f,0.25f,0.28f,1.0f};
    GLfloat sunDif[4] = {0.90f,0.87f,0.80f,1.0f};
    GLfloat sunSpe[4] = {0.30f,0.30f,0.30f,1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, sunDir);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  sunAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  sunDif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, sunSpe);

    // LIGHT1: lampu interior (point light dekat skylight/meja utama)
    glEnable(GL_LIGHT1);
    GLfloat lampAmb[4] = {0.05f,0.05f,0.04f,1.0f};
    GLfloat lampDif[4] = {1.0f,0.85f,0.55f,1.0f};
    GLfloat lampSpe[4] = {0.6f,0.5f,0.3f,1.0f};
    glLightfv(GL_LIGHT1, GL_AMBIENT,  lampAmb);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  lampDif);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lampSpe);
    glLightf (GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf (GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.06f);
    glLightf (GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.02f);
}
