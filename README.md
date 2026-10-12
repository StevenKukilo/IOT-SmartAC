# AC Pintar Hemat Energi (Smart AC berbasis IoT)

Sistem IoT berbasis **ESP32** yang menyalakan AC **hanya saat ruangan dipakai dan suhunya panas**. Kondisi ruangan dapat dipantau dan AC dapat dikendalikan dari **dashboard web atau HP** lewat MQTT. Proyek ini berjalan sepenuhnya di **simulator Wokwi** di dalam VS Code, jadi tidak perlu membeli komponen apa pun.

![Dashboard] https://stevenkukilo.github.io/IOT-SmartAC/


## Daftar isi

1. [Latar belakang](#latar-belakang)
2. [Fitur](#fitur)
3. [Komponen](#komponen)
4. [Cara kerja](#cara-kerja)
5. [Panduan menjalankan (untuk pemula)](#panduan-menjalankan-untuk-pemula)
6. [Cara uji dan demo](#cara-uji-dan-demo)
7. [Pemecahan masalah](#pemecahan-masalah)
8. [Topik MQTT](#topik-mqtt)
9. [Struktur repository](#struktur-repository)
10. [Keterbatasan](#keterbatasan)
11. [Rencana pengembangan](#rencana-pengembangan)

## Latar belakang

AC termasuk perangkat dengan konsumsi listrik terbesar, terutama di iklim tropis. Masalahnya, AC biasa bekerja "buta" karena hanya mengikuti remote:

- AC tetap menyala di ruangan kosong karena lupa dimatikan.
- Remote hanya bisa digunakan dari dalam ruangan.
- Tidak ada data berapa lama AC menyala.
- Suhu sering diatur terlalu rendah dan tidak pernah disesuaikan.

Proyek ini menjawab pertanyaan: *bagaimana membuat AC yang menyala hanya saat dibutuhkan, serta dapat dipantau dan dikendalikan dari jarak jauh?*

## Fitur

- Deteksi keberadaan orang (sensor PIR) dan pengukuran suhu serta kelembapan (DHT22)
- Mode **otomatis** dan **manual**
- Suhu target yang dapat diatur dari dashboard (18-30°C)
- Histeresis 1°C supaya relay tidak hidup-mati berulang
- Dashboard web: suhu, kelembapan, status ruangan, grafik riwayat, durasi AC menyala
- Estimasi listrik yang dihemat (dengan asumsi AC 800 W)

## Komponen

| Komponen | Jenis | Fungsi | Pin ESP32 |
|---|---|---|---|
| ESP32 DevKit C | Mikrokontroler | Memproses data dan terhubung ke WiFi | - |
| DHT22 | Sensor 1 | Mengukur suhu dan kelembapan | GPIO 4 |
| PIR | Sensor 2 | Mendeteksi gerakan manusia | GPIO 5 |
| Relay | Aktuator | Saklar yang menyalakan atau mematikan AC | GPIO 18 |

## Cara kerja

```
DHT22 + PIR → ESP32 → WiFi → Broker MQTT (HiveMQ) ⇄ Dashboard (web / HP)
                 │
                 └→ Relay (AC)
```

ESP32 membaca sensor setiap 2 detik, memutuskan apakah AC perlu menyala, lalu mengirim datanya ke broker MQTT. Dashboard membaca data dari broker yang sama dan dapat mengirim perintah balik ke ESP32.

**Logika otomatis:**

| Kondisi | AC |
|---|---|
| Ada orang **dan** suhu ≥ target | Menyala |
| Ruangan kosong selama 20 detik | Mati |
| Suhu ≤ target − 1°C | Mati |

---

## Panduan menjalankan (untuk pemula)

Panduan ini ditulis untuk orang yang baru pertama kali membuka proyek ini. Ikuti urutannya dari atas ke bawah. Seluruh proses memakan waktu sekitar 15-20 menit, terutama karena instalasi PlatformIO yang pertama kali cukup lama.

### Yang dibutuhkan

- Komputer Windows, macOS, atau Linux dengan **koneksi internet**
- **Visual Studio Code**: https://code.visualstudio.com
- **Git**: https://git-scm.com/downloads (boleh dilewati kalau memilih unduh ZIP di Langkah 2)
- Browser seperti Chrome, Edge, atau Firefox
- Akun untuk lisensi gratis Wokwi (dibuat di Langkah 4)

### Langkah 1: Pasang ekstensi di VS Code

1. Buka VS Code.
2. Klik ikon **Extensions** di sisi kiri (gambar empat kotak), atau tekan `Ctrl+Shift+X`.
3. Cari dan pasang dua ekstensi ini:
   - **PlatformIO IDE**
   - **Wokwi Simulator**
4. Setelah PlatformIO terpasang, **tutup lalu buka kembali VS Code**. Pemasangan pertama bisa makan waktu beberapa menit. Tunggu sampai ikon rumah kecil muncul di status bar bawah.

### Langkah 2: Ambil proyek ini ke komputer kamu

Pilih salah satu cara.

**Cara A: Memakai Git (clone)**

1. Di VS Code, tekan `F1`, ketik **Git: Clone**, lalu Enter.
2. Tempel alamat repository ini:
   ```
   https://github.com/USERNAME/NAMA-REPO.git
   ```
3. Pilih folder tujuan di komputer kamu.
4. Klik **Open** ketika VS Code menawarkan untuk membuka folder hasil clone.

Atau lewat terminal:

```bash
git clone https://github.com/USERNAME/NAMA-REPO.git
cd NAMA-REPO
code .
```

**Cara B: Unduh ZIP (tanpa Git)**

1. Di halaman GitHub repository ini, klik tombol hijau **Code**, lalu **Download ZIP**.
2. Ekstrak file ZIP-nya.
3. Di VS Code pilih **File → Open Folder**, lalu pilih folder hasil ekstrak.

> Pastikan yang kamu buka adalah folder yang langsung berisi file `platformio.ini`, bukan folder di atasnya. Kalau salah, PlatformIO tidak akan mengenali proyeknya.

### Langkah 3: Tentukan ID unik kamu

Proyek ini memakai broker MQTT publik yang dipakai banyak orang. Supaya datamu tidak bercampur dengan orang lain, kamu perlu **ID unik**.

1. Buka file `src/main.cpp`, cari baris:
   ```cpp
   const char* ID = "acpintar-budi123";
   ```
2. Ganti isinya dengan kode milikmu, misalnya `acpintar-namakamu482`.
3. Buka file `dashboard.html`, cari baris:
   ```js
   const ID = "acpintar-budi123";
   ```
4. Ganti dengan **ID yang sama persis** (huruf besar-kecil berpengaruh).
5. Simpan kedua file dengan `Ctrl+S`.

### Langkah 4: Aktifkan lisensi Wokwi (sekali saja)

1. Tekan `F1`, ketik **Wokwi: Request a new License**, lalu Enter.
2. Browser akan terbuka. Login (bisa dengan akun Google atau GitHub), lalu klik tombol untuk mendapatkan lisensi gratis.
3. Kembali ke VS Code dan konfirmasi bila diminta.

### Langkah 5: Build (kompilasi) kode

1. Klik ikon **centang (✓)** di status bar bawah VS Code, atau tekan `Ctrl+Alt+B`.
2. Tunggu proses berjalan. **Build pertama bisa 3-10 menit** karena PlatformIO mengunduh paket ESP32 dan library secara otomatis.
3. Build berhasil kalau terminal menampilkan tulisan hijau `SUCCESS`.

### Langkah 6: Jalankan simulator

1. Tekan `F1`, ketik **Wokwi: Start Simulator**, lalu Enter.
2. Panel simulasi terbuka dan menampilkan rangkaian ESP32, DHT22, sensor PIR, dan relay.
3. Lihat **Serial Monitor** di bagian bawah panel. Hasil yang benar:
   ```
   Menghubungkan WiFi... OK
   MQTT terhubung
   Suhu 24.0 C | Lembap 40% | Orang 0 | AC OFF | AUTO
   ```

Kalau baris di atas muncul, ESP32 virtualmu sudah online.

### Langkah 7: Buka dashboard

1. Cari file `dashboard.html` di folder proyek, lalu **klik dua kali** untuk membukanya di browser. (Atau klik kanan file tersebut lalu pilih *Reveal in File Explorer / Finder*.)
2. Lihat titik di pojok kanan atas halaman. Titik **hijau** dengan tulisan "Terhubung ke broker" berarti dashboard sudah terhubung.
3. Dalam sekitar 2 detik, angka suhu dan kelembapan akan terisi.

Dashboard memerlukan internet karena mengambil pustaka MQTT dari CDN dan terhubung ke broker publik.

**Membuka dari HP:** kirim file `dashboard.html` ke HP lalu buka dengan browser, atau unggah ke layanan hosting statis seperti Netlify Drop atau GitHub Pages (ganti nama file menjadi `index.html` bila diminta).

### Langkah 8: Hentikan atau jalankan ulang

- **Berhenti:** tekan `F1`, lalu pilih **Wokwi: Stop Simulator**.
- **Setelah mengubah kode:** ulangi **Build** (Langkah 5), lalu `F1 → Wokwi: Restart Simulator`.
- **Setelah mengubah `diagram.json` saja:** cukup restart simulator tanpa build ulang.

---

## Cara uji dan demo

ESP32 di simulator tidak bisa "melihat" orang sungguhan, jadi keberadaan orang dan suhu disimulasikan lewat komponen.

**Mengatur suhu:** klik komponen **DHT22** di panel simulator, lalu geser slider *Temperature*. Suhu bawaan 24°C berada di bawah target 25°C, sehingga AC tidak akan menyala. Naikkan suhunya dulu.

**Menyimulasikan orang masuk:** klik komponen **PIR**, lalu klik tombol **Simulate motion**.

**Menyimulasikan orang keluar:** berhenti menekan PIR dan tunggu 20 detik.

**Skenario demo (sekitar 2 menit):**

| Langkah | Aksi | Yang terlihat |
|---|---|---|
| 1 | Atur DHT22 ke 28°C, PIR tidak disentuh | Ruangan kosong, AC tetap mati, angka *listrik dihemat* mulai naik |
| 2 | Klik **Simulate motion** pada PIR | Relay "klik", dashboard menampilkan "AC menyala" |
| 3 | Berhenti menekan PIR, tunggu 20 detik | Ruangan kosong, AC mati otomatis |
| 4 | Turunkan suhu DHT22 ke 23°C, lalu gerakkan PIR | AC tetap mati karena ruangan sudah dingin |
| 5 | Di dashboard pilih **Manual**, lalu tekan tombol AC | AC menyala dari dashboard walau ruangan kosong |
| 6 | Pilih **Otomatis** | Sensor mengambil alih kembali |
| 7 | Geser slider **Suhu target** | Penanda pada lingkaran bergeser, ESP32 memakai target baru |

## Pemecahan masalah

| Masalah | Penyebab dan solusi |
|---|---|
| Ikon centang atau PlatformIO tidak muncul | Ekstensi belum selesai terpasang. Tunggu, lalu restart VS Code. |
| Build gagal: `platformio.ini` tidak ditemukan | Folder yang dibuka salah. Buka folder yang langsung berisi `platformio.ini`. |
| Build gagal: `DHT.h` atau `PubSubClient.h` tidak ditemukan | Pastikan bagian `lib_deps` di `platformio.ini` utuh, lalu build ulang dengan internet aktif. |
| Build gagal: `'Serial' was not declared` | Pastikan baris `#include <Arduino.h>` ada di paling atas `src/main.cpp`. |
| `Wokwi: Start Simulator` tidak muncul atau lisensi ditolak | Ulangi Langkah 4. |
| Error `firmware.bin not found` | Build belum berhasil, atau nama environment di `wokwi.toml` tidak sama dengan di `platformio.ini` (`esp32dev`). |
| WiFi tidak mau terhubung | Hentikan simulator lalu jalankan lagi. SSID di kode harus tepat `Wokwi-GUEST`. |
| Dashboard kosong, titik merah | Cek internet. Jaringan kantor atau kampus kadang memblokir WebSocket ke broker. Coba dengan hotspot HP. |
| Dashboard terhubung tapi angka tidak muncul | `ID` di dashboard dan di kode ESP32 belum sama persis, atau simulator belum berjalan. |
| AC tidak menyala padahal ada orang | Suhu DHT22 masih di bawah target. Naikkan suhunya atau turunkan suhu target. |
| Relay kebalikan (menyala saat seharusnya mati) | Tukar nilai `RELAY_ON` dan `RELAY_OFF` di `src/main.cpp`. |

## Topik MQTT

Awalan semua topik adalah `ID` unik kamu, misalnya `acpintar-budi123/suhu`.

| Topik | Arah | Isi |
|---|---|---|
| `suhu`, `lembap` | ESP32 → Dashboard | Angka suhu dan kelembapan |
| `orang` | ESP32 → Dashboard | `1` ada orang, `0` kosong |
| `ac` | ESP32 → Dashboard | `ON` atau `OFF` |
| `aktif`, `hemat` | ESP32 → Dashboard | Detik AC menyala, detik energi dihemat |
| `mode` | Dashboard → ESP32 | `AUTO` atau `MANUAL` |
| `cmd` | Dashboard → ESP32 | `ON` atau `OFF` (hanya berlaku di mode manual) |
| `setpoint` | Dashboard → ESP32 | Suhu target |

## Struktur repository

```
├── src/main.cpp        # Kode ESP32 (PlatformIO)
├── platformio.ini      # Konfigurasi build dan library
├── diagram.json        # Rangkaian Wokwi
├── wokwi.toml          # Konfigurasi simulator Wokwi
├── dashboard.html      # Dashboard web (MQTT lewat WebSocket)
└── docs/               # Tangkapan layar dan slide presentasi
```

## Keterbatasan

- Masih berupa simulasi: relay mewakili AC, dan orang diwakili sensor PIR virtual.
- PIR hanya mendeteksi gerakan, sehingga orang yang diam lama bisa dianggap tidak ada.
- Angka listrik yang dihemat adalah estimasi dengan asumsi AC 800 W, bukan hasil pengukuran.
- Broker MQTT publik tanpa autentikasi hanya cocok untuk demo, karena siapa pun yang tahu topikmu dapat mengirim perintah.

## Rencana pengembangan

- Pemancar inframerah agar dapat mengontrol AC split tanpa mengubah kelistrikan
- Sensor daya untuk data energi yang akurat
- Sensor kehadiran mmWave yang bisa mendeteksi orang yang diam
- Jadwal otomatis dan notifikasi ke HP
- Broker MQTT pribadi dengan autentikasi

## Teknologi

ESP32, Arduino framework, PlatformIO, Wokwi, MQTT (HiveMQ), MQTT.js, HTML, CSS, JavaScript

## Penulis

Nama kamu, [Institusi / Mata kuliah]

## Lisensi

MIT