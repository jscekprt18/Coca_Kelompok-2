#include "lighting.h"
#include "building.h" // LAMP_MAIN, LAMP_STAFF

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/freeglut.h>

static bool g_isDay      = true;
static bool g_interiorOn = true;

// LIGHT0: matahari (siang) atau cahaya bulan (malam). Komponen Z matahari
// dibuat cukup negatif supaya dinding depan/fasad (menghadap -Z, ke arah
// jalan/kamera) ikut kena cahaya - foto asli: siang mendung, cahaya rata
// dari depan-atas. Malam pakai directional redup kebiruan + ambient global
// digelapkan banyak, supaya lampu interior & lampu jalan terasa berarti.
static void ApplySunOrMoon()
{
    if (g_isDay) {
        GLfloat globalAmb[4] = {0.55f,0.53f,0.48f,1.0f};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);

        GLfloat sunDir[4] = {0.30f, 0.75f, -0.55f, 0.0f}; // w=0 -> directional
        GLfloat sunAmb[4] = {0.25f,0.25f,0.28f,1.0f};
        GLfloat sunDif[4] = {0.90f,0.87f,0.80f,1.0f};
        GLfloat sunSpe[4] = {0.30f,0.30f,0.30f,1.0f};
        glLightfv(GL_LIGHT0, GL_POSITION, sunDir);
        glLightfv(GL_LIGHT0, GL_AMBIENT,  sunAmb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  sunDif);
        glLightfv(GL_LIGHT0, GL_SPECULAR, sunSpe);
    } else {
        GLfloat globalAmb[4] = {0.06f,0.06f,0.10f,1.0f};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);

        GLfloat moonDir[4] = {-0.35f, 0.65f, 0.40f, 0.0f};
        GLfloat moonAmb[4] = {0.04f,0.04f,0.06f,1.0f};
        GLfloat moonDif[4] = {0.16f,0.18f,0.28f,1.0f};
        GLfloat moonSpe[4] = {0.10f,0.10f,0.14f,1.0f};
        glLightfv(GL_LIGHT0, GL_POSITION, moonDir);
        glLightfv(GL_LIGHT0, GL_AMBIENT,  moonAmb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  moonDif);
        glLightfv(GL_LIGHT0, GL_SPECULAR, moonSpe);
    }
}

static void ApplyInteriorState()
{
    if (g_interiorOn) {
        glEnable(GL_LIGHT1);
        glEnable(GL_LIGHT2);
    } else {
        glDisable(GL_LIGHT1);
        glDisable(GL_LIGHT2);
    }
}

void InitLighting()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_LIGHT0);
    ApplySunOrMoon();

    // LIGHT1 & LIGHT2: lampu gantung interior (point light), warna sama
    // (putih hangat) - posisinya diatur tiap frame di UpdateLightPositions().
    GLfloat lampAmb[4] = {0.05f,0.05f,0.04f,1.0f};
    GLfloat lampDif[4] = {1.0f,0.85f,0.55f,1.0f};
    GLfloat lampSpe[4] = {0.6f,0.5f,0.3f,1.0f};
    GLenum lampLights[2] = {GL_LIGHT1, GL_LIGHT2};
    for (int i = 0; i < 2; ++i) {
        GLenum L = lampLights[i];
        glLightfv(L, GL_AMBIENT,  lampAmb);
        glLightfv(L, GL_DIFFUSE,  lampDif);
        glLightfv(L, GL_SPECULAR, lampSpe);
        glLightf (L, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf (L, GL_LINEAR_ATTENUATION, 0.06f);
        glLightf (L, GL_QUADRATIC_ATTENUATION, 0.02f);
    }
    ApplyInteriorState();
}

void ToggleInteriorLights()
{
    g_interiorOn = !g_interiorOn;
    ApplyInteriorState();
}

bool IsInteriorLightsOn() { return g_interiorOn; }

void ToggleDayNight()
{
    g_isDay = !g_isDay;
    ApplySunOrMoon();
}

bool IsDayTime() { return g_isDay; }

void UpdateLightPositions()
{
    GLfloat p1[4] = {LAMP_MAIN.x,  LAMP_MAIN.y,  LAMP_MAIN.z,  1.0f};
    GLfloat p2[4] = {LAMP_STAFF.x, LAMP_STAFF.y, LAMP_STAFF.z, 1.0f};
    glLightfv(GL_LIGHT1, GL_POSITION, p1);
    glLightfv(GL_LIGHT2, GL_POSITION, p2);
}
