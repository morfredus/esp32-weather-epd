# Schéma de câblage - ESP32-S3 + TFT ST7789 + E-Paper

## 🔌 Connexions TFT ST7789 (SPI)

| Pin TFT | GPIO ESP32-S3 | Description |
|---------|---------------|-------------|
| VCC     | 3.3V          | Alimentation +3.3V |
| GND     | GND           | Masse |
| MOSI    | GPIO 11       | Données SPI (Master Out Slave In) |
| SCLK    | GPIO 12       | Horloge SPI |
| CS      | GPIO 10       | Chip Select (actif LOW) |
| DC      | GPIO 9        | Data/Command (0=commande, 1=données) |
| RST     | GPIO 13       | Reset (actif LOW) |
| BL      | GPIO 7        | Backlight (rétroéclairage) |

## 🔘 Connexions Boutons

| Bouton    | GPIO ESP32-S3 | Connexion | Description |
|-----------|---------------|-----------|-------------|
| BUTTON_1  | GPIO 38       | → GND     | Page précédente (Pull-Up interne) |
| BUTTON_2  | GPIO 39       | → GND     | Page suivante (Pull-Up interne) |
| BOOT      | GPIO 0        | → GND     | Bouton BOOT (réservé au bootloader) |

**Note** : Les boutons utilisent les pull-ups internes. Pas besoin de résistances externes.

## 📄 Connexions E-Paper (existant)

| Pin E-Paper | GPIO ESP32-S3 | Description |
|-------------|---------------|-------------|
| BUSY        | GPIO 14       | Signal occupé |
| CS          | GPIO 13       | Chip Select |
| RST         | GPIO 21       | Reset |
| DC          | GPIO 22       | Data/Command |
| SCK         | GPIO 18       | Horloge SPI |
| MISO        | GPIO 19       | Non utilisé (lecture non nécessaire) |
| MOSI        | GPIO 23       | Données SPI |
| PWR         | GPIO 26       | Contrôle alimentation (optionnel) |

## 🌡️ Connexions BME280/680 (I2C)

| Pin BME | GPIO ESP32-S3 | Description |
|---------|---------------|-------------|
| VCC     | 3.3V          | Alimentation |
| GND     | GND           | Masse |
| SDA     | GPIO 17       | Données I2C |
| SCL     | GPIO 16       | Horloge I2C |
| PWR     | GPIO 4        | Contrôle alimentation (optionnel) |

**Adresse I2C** : 0x76 (si SDO → GND) ou 0x77 (si SDO → VCC)

## 🔋 Connexion Batterie

| Pin | GPIO ESP32-S3 | Description |
|-----|---------------|-------------|
| BAT | A2 (ADC)      | Mesure tension batterie (diviseur de tension) |

## 📊 Schéma ASCII

```
                          ESP32-S3 DevKit
                    ┌────────────────────────┐
                    │                        │
    TFT MOSI ───────┤ GPIO 11                │
    TFT SCLK ───────┤ GPIO 12                │
    TFT CS   ───────┤ GPIO 10                │
    TFT DC   ───────┤ GPIO 9                 │
    TFT RST  ───────┤ GPIO 13                │
    TFT BL   ───────┤ GPIO 7                 │
                    │                        │
    BTN 1 ─────┬────┤ GPIO 38                │
               │    │                        │
    BTN 2 ─────┼─┬──┤ GPIO 39                │
               │ │  │                        │
    GND ───────┴─┴──┤ GND                    │
                    │                        │
    EPD BUSY ───────┤ GPIO 14                │
    EPD CS   ───────┤ GPIO 13 (partagé RST)  │
    EPD RST  ───────┤ GPIO 21                │
    EPD DC   ───────┤ GPIO 22                │
    EPD SCK  ───────┤ GPIO 18                │
    EPD MOSI ───────┤ GPIO 23                │
                    │                        │
    BME SDA  ───────┤ GPIO 17                │
    BME SCL  ───────┤ GPIO 16                │
    BME PWR  ───────┤ GPIO 4                 │
                    │                        │
    BAT ADC  ───────┤ A2 (GPIO ?)            │
                    │                        │
    3.3V ───────────┤ 3V3                    │
    GND  ───────────┤ GND                    │
                    └────────────────────────┘
```

## ⚠️ Notes importantes

### Conflit de GPIO 13
**Attention** : GPIO 13 est utilisé à la fois pour :
- TFT_RST (écran TFT)
- EPD_CS (e-paper)

**Solutions** :
1. **Utiliser un seul affichage à la fois** (recommandé si conflit)
2. **Modifier les pins** dans `config.cpp` :
   ```cpp
   // Option 1 : Changer le TFT RST
   const uint8_t PIN_TFT_RST = 14;  // Au lieu de 13
   
   // Option 2 : Changer le EPD CS
   const uint8_t PIN_EPD_CS = 15;   // Au lieu de 13
   ```
3. **Utiliser un multiplexeur** pour partager la GPIO

### Alimentation

- **Tension** : 3.3V pour tous les composants
- **Courant** :
  - TFT avec rétroéclairage : ~50-100 mA
  - E-Paper lors du rafraîchissement : ~100 mA
  - ESP32-S3 en fonctionnement : ~80 mA
  - **Total max** : ~300 mA

⚡ **Recommandation** : Alimentation 3.3V capable de fournir au moins 500 mA

### Bus SPI partagé

Le TFT et l'E-Paper partagent le bus SPI :
- **MOSI** et **SCLK** peuvent être partagés
- **CS** (Chip Select) doit être unique pour chaque périphérique
- **DC** doit être unique pour chaque périphérique

### Pull-Up / Pull-Down

- **Boutons** : Pull-Up interne activé dans le code (GPIO_PULLUP)
- **I2C** : Pull-ups externes de 4.7kΩ recommandés (souvent déjà sur les modules BME)

## 🧪 Test des connexions

### Test TFT
1. Uploader un sketch de test Adafruit ST7789
2. L'écran doit afficher des couleurs
3. Le rétroéclairage doit s'allumer

### Test Boutons
1. Uploader le code complet
2. Ouvrir le Serial Monitor (115200 bauds)
3. Appuyer sur les boutons
4. Vérifier les messages "Bouton X pressé - Page : Y"

### Test E-Paper
1. Vérifier les connexions SPI
2. Le BUSY devrait passer à LOW pendant le rafraîchissement
3. L'écran se met à jour en ~15 secondes

### Test BME280/680
1. Scanner I2C avec un sketch de test
2. L'adresse 0x76 ou 0x77 doit apparaître
3. Vérifier que les lectures ne sont pas NaN

## 🔧 Outils de diagnostic

### Scanner I2C
```cpp
#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin(17, 16); // SDA, SCL
  
  Serial.println("I2C Scanner");
  for (byte i = 8; i < 120; i++) {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0) {
      Serial.print("Found I2C device at 0x");
      Serial.println(i, HEX);
    }
  }
}
```

### Test GPIO
```cpp
void testGPIO(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  delay(500);
  digitalWrite(pin, LOW);
  delay(500);
}
```

## 📚 Références

- [Adafruit ST7789 Tutorial](https://learn.adafruit.com/adafruit-1-14-240x135-color-tft-breakout)
- [ESP32-S3 Pinout](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/hw-reference/esp32s3/user-guide-devkitc-1.html)
- [BME280 Datasheet](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf)
