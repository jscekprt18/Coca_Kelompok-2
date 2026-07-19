#ifndef INPUT_H
#define INPUT_H

// Status tombol WASD (di-update oleh KeyDown/KeyUp, dibaca tiap frame di
// Idle() supaya gerakan halus & bisa diagonal - bukan 1 langkah tetap per
// event keypress seperti kerangka awal).
extern bool g_wDown, g_sDown, g_aDown, g_dDown;

void KeyDown(unsigned char key, int x, int y);
void KeyUp(unsigned char key, int x, int y);

// Mouse-look (FPS style): pointer di-warp balik ke tengah window tiap
// gerak supaya bisa muter terus tanpa mentok tepi layar.
void MouseMotion(int x, int y);
void InitInput(int winW, int winH);

#endif
