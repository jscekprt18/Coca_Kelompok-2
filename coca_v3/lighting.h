#ifndef LIGHTING_H
#define LIGHTING_H

// Setup pencahayaan: GL_LIGHT0 = matahari/bulan (directional, siang/malam),
// GL_LIGHT1 = lampu interior utama (dekat skylight, ruang duduk utama),
// GL_LIGHT2 = lampu interior kedua (ruang staff). LIGHT1 & LIGHT2 nyala/
// mati bareng lewat ToggleInteriorLights().
void InitLighting();

// Lampu interior (LIGHT1+LIGHT2) - ditoggle tombol 'L'.
void ToggleInteriorLights();
bool IsInteriorLightsOn();

// Siang/malam - ditoggle tombol 'N'. Mengubah warna & intensitas LIGHT0
// (matahari saat siang, cahaya bulan redup kebiruan saat malam) serta
// ambient global, supaya lampu interior/jalan terasa berarti saat malam.
void ToggleDayNight();
bool IsDayTime();

// Dipanggil tiap frame dari Display() setelah ApplyCamera(), supaya posisi
// LIGHT1/LIGHT2 (dalam eye space) selalu ikut transform kamera terkini -
// sama seperti pola lama utk LIGHT1 sebelum lampu kedua ditambahkan.
void UpdateLightPositions();

#endif
