#ifdef _WIN32
#include <windows.h>
#endif

#include "building.h"
#include "camera.h"
#include "input.h"
#include "lighting.h"
#include "scene.h"

#include <GL/freeglut.h>
#include <iostream>

using namespace std;

// ======================================================
// Ukuran Window
// ======================================================
const int WINDOW_WIDTH  = 1280;
const int WINDOW_HEIGHT = 720;

static int g_lastTimeMs = 0;

// ======================================================
// Display
// ======================================================
void Display()
{
    if (IsDayTime())
        glClearColor(0.55f, 0.70f, 0.85f, 1.0f); // langit siang
    else
        glClearColor(0.03f, 0.04f, 0.10f, 1.0f); // langit malam
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    ApplyCamera();

    // lampu interior (LIGHT1/LIGHT2) diposisikan relatif world tiap frame,
    // supaya posisi eye-space-nya selalu ikut transform kamera terkini
    UpdateLightPositions();

    DrawSceneList();
    DrawDynamicGlows(); // bohlam lampu interior + lampu jalan (di luar display list)

    glutSwapBuffers();
}

// ======================================================
// Resize
// ======================================================
void Reshape(int w, int h)
{
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / h, 0.1, 500.0);

    glMatrixMode(GL_MODELVIEW);

    InitInput(w, h);
}

// ======================================================
// Idle: gerakan halus per-frame (bukan 1 langkah tetap per keypress)
// ======================================================
void Idle()
{
    int t = glutGet(GLUT_ELAPSED_TIME);
    int dt = t - g_lastTimeMs;
    if (dt > 0) {
        UpdateCamera(dt, g_wDown, g_sDown, g_aDown, g_dDown);
        g_lastTimeMs = t;
    }
    glutPostRedisplay();
}

// ======================================================
// Main
// ======================================================
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    InitCamera();

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("COCA Cafe 3D");

    InitLighting();
    InitInput(WINDOW_WIDTH, WINDOW_HEIGHT);

    cout << "Kontrol: WASD gerak, mouse arah pandang | L = lampu interior, "
            "N = siang/malam | ESC keluar" << endl;

    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutIdleFunc(Idle);
    glutKeyboardFunc(KeyDown);
    glutKeyboardUpFunc(KeyUp);
    glutPassiveMotionFunc(MouseMotion);
    glutSetCursor(GLUT_CURSOR_NONE);

    g_lastTimeMs = glutGet(GLUT_ELAPSED_TIME);
    glutMainLoop();

    return 0;
}
