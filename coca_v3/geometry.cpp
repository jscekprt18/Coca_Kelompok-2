#include "geometry.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/freeglut.h>

void DrawBox(float x1, float y1, float z1, float x2, float y2, float z2)
{
    float lx = (x1<x2)?x1:x2, hx = (x1<x2)?x2:x1;
    float ly = (y1<y2)?y1:y2, hy = (y1<y2)?y2:y1;
    float lz = (z1<z2)?z1:z2, hz = (z1<z2)?z2:z1;
    float w = hx-lx, h = hy-ly, d = hz-lz;

    glBegin(GL_QUADS);
        // depan (+Z)
        glNormal3f(0,0,1);
        glTexCoord2f(0,0); glVertex3f(lx,ly,hz);
        glTexCoord2f(w,0); glVertex3f(hx,ly,hz);
        glTexCoord2f(w,h); glVertex3f(hx,hy,hz);
        glTexCoord2f(0,h); glVertex3f(lx,hy,hz);
        // belakang (-Z)
        glNormal3f(0,0,-1);
        glTexCoord2f(0,0); glVertex3f(hx,ly,lz);
        glTexCoord2f(w,0); glVertex3f(lx,ly,lz);
        glTexCoord2f(w,h); glVertex3f(lx,hy,lz);
        glTexCoord2f(0,h); glVertex3f(hx,hy,lz);
        // kiri (-X)
        glNormal3f(-1,0,0);
        glTexCoord2f(0,0); glVertex3f(lx,ly,lz);
        glTexCoord2f(d,0); glVertex3f(lx,ly,hz);
        glTexCoord2f(d,h); glVertex3f(lx,hy,hz);
        glTexCoord2f(0,h); glVertex3f(lx,hy,lz);
        // kanan (+X)
        glNormal3f(1,0,0);
        glTexCoord2f(0,0); glVertex3f(hx,ly,hz);
        glTexCoord2f(d,0); glVertex3f(hx,ly,lz);
        glTexCoord2f(d,h); glVertex3f(hx,hy,lz);
        glTexCoord2f(0,h); glVertex3f(hx,hy,hz);
        // atas (+Y)
        glNormal3f(0,1,0);
        glTexCoord2f(0,0); glVertex3f(lx,hy,hz);
        glTexCoord2f(w,0); glVertex3f(hx,hy,hz);
        glTexCoord2f(w,d); glVertex3f(hx,hy,lz);
        glTexCoord2f(0,d); glVertex3f(lx,hy,lz);
        // bawah (-Y)
        glNormal3f(0,-1,0);
        glTexCoord2f(0,0); glVertex3f(lx,ly,lz);
        glTexCoord2f(w,0); glVertex3f(hx,ly,lz);
        glTexCoord2f(w,d); glVertex3f(hx,ly,hz);
        glTexCoord2f(0,d); glVertex3f(lx,ly,hz);
    glEnd();
}

void DrawQuadV(const Vec3 &a, const Vec3 &b, const Vec3 &c, const Vec3 &d,
               float nx, float ny, float nz, float uRep, float vRep)
{
    glBegin(GL_QUADS);
        glNormal3f(nx,ny,nz);
        glTexCoord2f(0,0);       glVertex3f(a.x,a.y,a.z);
        glTexCoord2f(uRep,0);    glVertex3f(b.x,b.y,b.z);
        glTexCoord2f(uRep,vRep); glVertex3f(c.x,c.y,c.z);
        glTexCoord2f(0,vRep);    glVertex3f(d.x,d.y,d.z);
    glEnd();
}

void DrawTriV(const Vec3 &a, const Vec3 &b, const Vec3 &c,
              float nx, float ny, float nz)
{
    glBegin(GL_TRIANGLES);
        glNormal3f(nx,ny,nz);
        glTexCoord2f(0,0); glVertex3f(a.x,a.y,a.z);
        glTexCoord2f(1,0); glVertex3f(b.x,b.y,b.z);
        glTexCoord2f(0.5f,1); glVertex3f(c.x,c.y,c.z);
    glEnd();
}

// CATATAN ARAH HADAP: font stroke GLUT digambar di bidang XY dan "terbaca
// benar" untuk penonton yang berada di sisi +Z memandang ke arah -Z.
// Kamera tur kita start di sisi -Z (luar gerbang) memandang ke arah +Z (ke
// arah bangunan) - artinya kita melihat sisi "belakang" tulisan itu, jadi
// tanpa koreksi hasilnya terbalik/mirror. Makanya sumbu X lokal di-mirror
// (scale -1) supaya terbaca benar dari sudut pandang tur kita.
void DrawStrokeText(const char *text, float x, float y, float z,
                     float scale, float r, float g, float b)
{
    glDisable(GL_LIGHTING);
    glColor3f(r, g, b);
    glLineWidth(4.0f);
    glPushMatrix();
        glTranslatef(x, y, z);
        glScalef(-scale, scale, scale);
        for (const char *p = text; *p; ++p) glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
    glPopMatrix();
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}
