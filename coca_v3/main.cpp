#ifdef _WIN32
#include <windows.h>
#endif

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
    glClearColor(0.55f, 0.70f, 0.85f, 1.0f); // langit sore
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    ApplyCamera();

    // lampu interior (LIGHT1) diposisikan relatif world, di atas meja
    // utama dekat skylight
    GLfloat lampPos[4] = {0.0f, 2.6f, 0.8f, 1.0f};
    glLightfv(GL_LIGHT1, GL_POSITION, lampPos);

    DrawSceneList();

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
