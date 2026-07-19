#ifndef MATERIAL_H
#define MATERIAL_H

// ======================================================
// Material terpisah per jenis permukaan (spec: Wall, Roof, Glass,
// Concrete, Wood, Floor, Metal, + beberapa tambahan utk furnitur/plant).
// ApplyMaterial() set glMaterialfv (ambient/diffuse/specular/shininess)
// sebelum tiap kelompok DrawBox/DrawQuadV dipanggil di building.cpp.
// ======================================================
enum MaterialID {
    MAT_WALL,
    MAT_ROOF,
    MAT_GLASS,
    MAT_CONCRETE,
    MAT_WOOD,
    MAT_FLOOR,
    MAT_FLOOR_OUTDOOR,
    MAT_METAL,
    MAT_LEAF,
    MAT_PASTRY_A,
    MAT_PASTRY_B,
    MAT_PASTRY_C,
    MAT_SOFA_ORANGE,   // bantal/sofa oranye seperti area duduk indoor asli
    MAT_ACCENT_MAROON, // panel dinding/plafon merah marun (aksen)
    MAT_SIGN_METAL,    // huruf logo "COCA" metal keperakan
    MAT_ROAD,          // aspal jalan depan
    MAT_HOUSE_WALL_A,  // dinding rumah tetangga - terracotta hangat
    MAT_HOUSE_WALL_B,  // dinding rumah tetangga - krem pucat
};

void ApplyMaterial(MaterialID id);

#endif
