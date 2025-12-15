# Gestion de l'écran TFT ST7789 avec navigation par boutons

## Ajouts au projet esp32-weather-epd

Ce projet a été étendu pour supporter un écran TFT couleur ST7789 (240x320) en plus de l'écran e-paper d'origine. La navigation entre les différentes pages d'informations se fait via deux boutons physiques.

## 📌 Configuration matérielle

### Pins TFT ST7789
- **PIN_TFT_MOSI** (GPIO 11) : Données SPI vers l'écran
- **PIN_TFT_SCLK** (GPIO 12) : Horloge SPI
- **PIN_TFT_CS** (GPIO 10) : Chip Select
- **PIN_TFT_DC** (GPIO 9) : Data/Command
- **PIN_TFT_RST** (GPIO 13) : Reset
- **PIN_TFT_BL** (GPIO 7) : Rétroéclairage (Backlight)

### Pins Boutons
- **PIN_BUTTON_1** (GPIO 38) : Bouton page précédente (avec pull-up interne)
- **PIN_BUTTON_2** (GPIO 39) : Bouton page suivante (avec pull-up interne)
- **PIN_BUTTON_BOOT** (GPIO 0) : Bouton BOOT (réservé)

**Note** : Les boutons doivent être connectés entre le GPIO et GND. Le pull-up interne est activé dans le code.

## 🎨 Pages disponibles

L'affichage TFT propose **4 pages** navigables :

### Page 1 : Conditions actuelles
- Température extérieure
- Description météo
- Humidité et pression
- Vitesse du vent
- Température et humidité intérieures (si capteur BME présent)

### Page 2 : Prévisions horaires (24h)
- Les 12 prochaines heures
- Heure, température et probabilité de précipitations

### Page 3 : Prévisions journalières (7 jours)
- Jour de la semaine et date
- Températures min/max
- Probabilité de précipitations

### Page 4 : Informations système
- Date et heure actuelles
- Tension de la batterie (mV) et pourcentage
- Barre visuelle de charge
- Signal WiFi (RSSI) et qualité
- Mémoire RAM et PSRAM disponibles

## 🔧 Configuration

### Dans config.h

Activer/désactiver les affichages :
```cpp
#define USE_EPD_DISPLAY       // Support e-paper (optionnel)
#define USE_TFT_DISPLAY       // Support TFT ST7789 (nouveau)
```

Vous pouvez activer les deux simultanément, ou seulement le TFT.

### Dans platformio.ini

Les bibliothèques suivantes ont été ajoutées :
```ini
adafruit/Adafruit GFX Library @ 1.11.11
adafruit/Adafruit ST7735 and ST7789 Library @ 1.10.4
```

## 🎮 Utilisation

1. **Démarrage** : L'écran s'initialise sur la page 1 (conditions actuelles)

2. **Navigation** :
   - **Bouton 1** (GPIO 38) : Page précédente
   - **Bouton 2** (GPIO 39) : Page suivante
   - La navigation boucle (de la page 4 → page 1, et vice-versa)

3. **Indicateur de page** : Des petits cercles en bas de l'écran indiquent la page active

4. **Timeout** : Après 10 minutes d'inactivité, l'ESP32 entre en deep sleep (si activé)

## ⚡ Économie d'énergie

- Le rétroéclairage du TFT est éteint avant le deep sleep
- Le TFT consomme plus que l'e-paper, mais offre un affichage couleur instantané
- Mode interactif avec timeout de 10 minutes avant mise en veille

## 🔄 Mode de fonctionnement

### Mode Deep Sleep (par défaut)
- Récupération des données météo
- Affichage sur e-paper (si activé)
- Affichage sur TFT avec navigation pendant 10 minutes
- Deep sleep jusqu'à la prochaine mise à jour

### Mode interactif continu (optionnel)
Pour désactiver le deep sleep et rester en mode interactif :
- Commenter l'appel `beginDeepSleep()` dans `main.cpp`
- La fonction `loop()` prendra le relais pour la navigation continue

## 📁 Fichiers modifiés/ajoutés

### Nouveaux fichiers
- `include/tft_display.h` : Déclarations des fonctions TFT
- `src/tft_display.cpp` : Implémentation de l'affichage TFT

### Fichiers modifiés
- `include/config.h` : Ajout des macros et déclarations extern
- `src/config.cpp` : Définition des pins TFT et boutons
- `src/main.cpp` : Intégration de l'affichage TFT
- `platformio.ini` : Ajout des bibliothèques Adafruit

## 🎨 Personnalisation

### Couleurs
Dans `tft_display.cpp`, vous pouvez modifier les couleurs :
```cpp
#define COLOR_BG       ST77XX_BLACK    // Fond
#define COLOR_TEXT     ST77XX_WHITE    // Texte
#define COLOR_TITLE    ST77XX_CYAN     // Titres
#define COLOR_WARN     ST77XX_YELLOW   // Avertissements
#define COLOR_ERROR    ST77XX_RED      // Erreurs
#define COLOR_SUCCESS  ST77XX_GREEN    // Succès
```

### Nombre de pages
Modifier `TFT_NUM_PAGES` dans `tft_display.h` et ajouter les cas dans `displayCurrentPage()`

### Timeout d'inactivité
Dans `main.cpp`, modifier :
```cpp
const unsigned long INTERACTION_TIMEOUT = 600000; // 10 minutes en ms
```

## 🐛 Dépannage

### L'écran reste noir
- Vérifier le câblage du rétroéclairage (PIN_TFT_BL)
- Vérifier les connexions SPI (MOSI, SCLK, CS, DC, RST)
- Vérifier l'alimentation 3.3V

### Les boutons ne fonctionnent pas
- Vérifier que les boutons sont connectés entre GPIO et GND
- Le pull-up interne doit être activé (fait dans le code)
- Vérifier le debounce delay (200ms par défaut)

### Affichage corrompu
- Vérifier la vitesse SPI dans `tft_display.cpp`
- Vérifier que `TFT_WIDTH` et `TFT_HEIGHT` correspondent à votre écran

## 📝 Notes

- Le système conserve la gestion complète de l'e-paper
- Les deux affichages (e-paper + TFT) peuvent fonctionner simultanément
- Le TFT offre un retour visuel immédiat avec couleurs et interaction
- L'e-paper reste idéal pour l'économie d'énergie sur batterie
