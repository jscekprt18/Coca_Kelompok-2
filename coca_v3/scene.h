#ifndef SCENE_H
#define SCENE_H

// Bangun (sekali) & gambar display list statis berisi seluruh bangunan
// COCA (dari BuildCOCA() di building.cpp). Dipakai di Display() supaya
// geometri tidak di-generate ulang tiap frame.
void DrawSceneList();

#endif
