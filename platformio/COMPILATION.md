# Guide de compilation et installation

## 🚀 Première compilation

Les bibliothèques Adafruit seront automatiquement téléchargées par PlatformIO lors de la première compilation.

### Étapes de compilation

1. **Ouvrir le terminal PlatformIO** dans VS Code

2. **Nettoyer le projet** (optionnel, recommandé) :
   ```bash
   pio run -t clean
   ```

3. **Compiler le projet** :
   ```bash
   pio run
   ```
   
   Ou pour un environnement spécifique :
   ```bash
   pio run -e esp32s3_n16r8
   ```

4. **Compiler et uploader** :
   ```bash
   pio run -t upload
   ```

### Dépendances installées automatiquement

Les bibliothèques suivantes seront téléchargées :
- ✅ Adafruit GFX Library @ 1.11.11
- ✅ Adafruit ST7735 and ST7789 Library @ 1.10.4
- ✅ Adafruit BME280 Library @ 2.3.0
- ✅ Adafruit BME680 Library @ 2.0.5
- ✅ Adafruit BusIO @ 1.17.4
- ✅ Adafruit Unified Sensor @ 1.1.15
- ✅ ArduinoJson @ 7.4.2
- ✅ GxEPD2 @ 1.6.5

## ⚙️ Configuration avant compilation

### 1. Configuration WiFi et API
Dans `src/config.cpp`, configurez :
```cpp
const char *WIFI_SSID     = "votre_ssid";
const char *WIFI_PASSWORD = "votre_mot_de_passe";
const String OWM_APIKEY   = "votre_cle_api_openweathermap";
```

**Note** : Ces valeurs peuvent aussi être dans `include/secrets.h` (recommandé pour ne pas les commiter)

### 2. Activer/Désactiver les affichages
Dans `include/config.h` :
```cpp
#define USE_EPD_DISPLAY       // Activer l'e-paper
#define USE_TFT_DISPLAY       // Activer le TFT ST7789
```

Commentez ce dont vous n'avez pas besoin :
```cpp
// #define USE_EPD_DISPLAY    // E-paper désactivé
#define USE_TFT_DISPLAY       // TFT uniquement
```

### 3. Vérifier les pins
Dans `src/config.cpp`, vérifiez que les pins correspondent à votre câblage :
```cpp
// TFT ST7789
const uint8_t PIN_TFT_MOSI  = 11;
const uint8_t PIN_TFT_SCLK  = 12;
const uint8_t PIN_TFT_CS    = 10;
const uint8_t PIN_TFT_DC    = 9;
const uint8_t PIN_TFT_RST   = 13;
const uint8_t PIN_TFT_BL    = 7;

// Boutons
const uint8_t PIN_BUTTON_1  = 38;
const uint8_t PIN_BUTTON_2  = 39;
```

## 🔍 Résolution de problèmes

### Erreur : "include file not found"
- Normal avant la première compilation
- PlatformIO téléchargera les bibliothèques automatiquement
- Relancez `pio run` si l'erreur persiste

### Erreur : "Sketch too big"
- Utilisez la partition `huge_app.csv` (déjà configurée)
- Vérifiez dans `platformio.ini` :
  ```ini
  board_build.partitions = huge_app.csv
  ```

### Erreur de mémoire PSRAM
- Vérifiez que votre ESP32-S3 dispose de PSRAM
- Configuration déjà présente dans `platformio.ini` pour l'ESP32-S3

### L'écran TFT ne s'affiche pas
1. Vérifiez que `USE_TFT_DISPLAY` est défini dans `config.h`
2. Vérifiez le câblage physique
3. Activez le debug dans le Serial Monitor (115200 bauds)

## 📊 Utilisation de la mémoire

### ESP32-S3 avec PSRAM recommandé
- **Flash** : 16 MB (huge_app partition)
- **RAM** : 512 KB (+ PSRAM 8 MB)
- **Fréquence** : 80 MHz (économie d'énergie)

### Consommation mémoire estimée
- Programme : ~1.5 MB
- Variables globales : ~150 KB
- Pile (stack) : ~32 KB
- Bibliothèques graphiques : ~500 KB

## 🔌 Upload via USB

### ESP32-S3 avec USB natif
```bash
pio run -e esp32s3_n16r8 -t upload
```

Le port série sera automatiquement détecté.

### Forcer un port spécifique
Dans `platformio.ini`, ajoutez :
```ini
upload_port = COM3  ; Windows
; upload_port = /dev/ttyUSB0  ; Linux
; upload_port = /dev/cu.usbserial-*  ; macOS
```

## 📺 Monitoring série

```bash
pio device monitor
```

Ou en une seule commande (upload + monitor) :
```bash
pio run -t upload && pio device monitor
```

## 🧪 Test rapide

Après l'upload, vous devriez voir dans le Serial Monitor :
```
TFT ST7789 initialisé
Boutons initialisés (Pull-Up)
Connecting to WiFi...
Connected to WiFi
Time synchronized
Fetching weather data...
Display updated
```

Testez les boutons :
- Bouton 1 : Change de page (précédente)
- Bouton 2 : Change de page (suivante)

## 📚 Documentation complémentaire

- [TFT_README.md](TFT_README.md) - Documentation complète du système TFT
- [README.md](README.md) - Documentation originale du projet
