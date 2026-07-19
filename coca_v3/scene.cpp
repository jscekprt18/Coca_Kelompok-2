#include "scene.h"
#include "building.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/freeglut.h>

static GLuint g_sceneList = 0;

void DrawSceneList()
{
    if (g_sceneList == 0) {
        g_sceneList = glGenLists(1);
        glNewList(g_sceneList, GL_COMPILE);
            BuildCOCA();
        glEndList();
    }
    glCallList(g_sceneList);
}
