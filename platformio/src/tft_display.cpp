/* TFT Display helper implementation for esp32-weather-epd.
 * Copyright (C) 2025
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "tft_display.h"
#include "config.h"
#include "conversions.h"
#include "_locale.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <cmath>

// Instance du TFT (utilise le SPI partagé)
Adafruit_ST7789 tft = Adafruit_ST7789(&SPI, PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);

// Variables pour la gestion des pages et boutons
static uint8_t currentPage = PAGE_CURRENT_CONDITIONS;
static unsigned long lastButtonPress = 0;
const unsigned long DEBOUNCE_DELAY = 200; // ms

// Couleurs
#define COLOR_BG       ST77XX_BLACK
#define COLOR_TEXT     ST77XX_WHITE
#define COLOR_TITLE    ST77XX_CYAN
#define COLOR_WARN     ST77XX_YELLOW
#define COLOR_ERROR    ST77XX_RED
#define COLOR_SUCCESS  ST77XX_GREEN
#define COLOR_DISABLED ST77XX_BLUE

// Initialiser l'écran TFT
void initTFT() {
  // Initialiser le SPI pour le TFT (si pas déjà fait ailleurs)
  SPI.begin(PIN_TFT_SCLK, -1, PIN_TFT_MOSI, PIN_TFT_CS);
  
  // Initialiser l'écran ST7789 en 240x240
  tft.init(240, 240); // Largeur, Hauteur
  tft.setRotation(0); // Portrait mode (0 = connecteur en bas)
  tft.fillScreen(COLOR_BG);
  
  // Activer le rétroéclairage
  pinMode(PIN_TFT_BL, OUTPUT);
  digitalWrite(PIN_TFT_BL, HIGH);
  
  Serial.println("TFT ST7789 initialisé (240x240)");
}

// Éteindre l'écran TFT
void powerOffTFT() {
  // Désactiver le rétroéclairage
  digitalWrite(PIN_TFT_BL, LOW);
  
  Serial.println("TFT ST7789 éteint");
}

// Initialiser les boutons
void initButtons() {
  pinMode(PIN_BUTTON_1, INPUT_PULLUP);
  pinMode(PIN_BUTTON_2, INPUT_PULLUP);
  
  Serial.println("Boutons initialisés (Pull-Up)");
}

// Vérifier si un bouton a été pressé
bool checkButtons() {
  unsigned long currentTime = millis();
  
  // Anti-rebond
  if (currentTime - lastButtonPress < DEBOUNCE_DELAY) {
    return false;
  }
  
  // Bouton 1 : Page précédente
  if (digitalRead(PIN_BUTTON_1) == LOW) {
    lastButtonPress = currentTime;
    if (currentPage > 0) {
      currentPage--;
    } else {
      currentPage = TFT_NUM_PAGES - 1; // Boucler vers la dernière page
    }
    Serial.print("Bouton 1 pressé - Page : ");
    Serial.println(currentPage);
    return true;
  }
  
  // Bouton 2 : Page suivante
  if (digitalRead(PIN_BUTTON_2) == LOW) {
    lastButtonPress = currentTime;
    currentPage = (currentPage + 1) % TFT_NUM_PAGES;
    Serial.print("Bouton 2 pressé - Page : ");
    Serial.println(currentPage);
    return true;
  }
  
  return false;
}

// Obtenir la page actuelle
uint8_t getCurrentPage() {
  return currentPage;
}

// Définir la page actuelle
void setCurrentPage(uint8_t page) {
  if (page < TFT_NUM_PAGES) {
    currentPage = page;
  }
}

// Afficher un indicateur de page
void drawPageIndicator(uint8_t current, uint8_t total) {
  const int dotSize = 6;
  const int dotSpacing = 16;
  const int startX = (240 - (total * dotSpacing)) / 2;
  const int y = 240 - 15;
  for (uint8_t i = 0; i < total; i++) {
    int x = startX + (i * dotSpacing);
    if (i == current) {
      tft.fillCircle(x, y, dotSize / 2, COLOR_TITLE);
    } else {
      tft.drawCircle(x, y, dotSize / 2, COLOR_DISABLED);
    }
  }
}

// Afficher la page actuelle
void displayCurrentPage(const owm_resp_onecall_t &owm_onecall,
                       const owm_resp_air_pollution_t &owm_air_pollution,
                       float inTemp, float inHumidity,
                       uint32_t batteryVoltage, int wifiRSSI,
                       const tm &timeInfo) {
  tft.fillScreen(COLOR_BG);
  
  switch (currentPage) {
    case PAGE_CURRENT_CONDITIONS:
      drawTFTCurrentConditions(owm_onecall, owm_air_pollution, inTemp, inHumidity);
      break;
      
    case PAGE_HOURLY_FORECAST:
      drawTFTHourlyForecast(owm_onecall, timeInfo);
      break;
      
    case PAGE_DAILY_FORECAST:
      drawTFTDailyForecast(owm_onecall, timeInfo);
      break;
      
    case PAGE_SYSTEM_INFO:
      drawTFTSystemInfo(batteryVoltage, wifiRSSI, timeInfo);
      break;
  }
  
  // Afficher l'indicateur de page
  drawPageIndicator(currentPage, TFT_NUM_PAGES);
}

// Page 1 : Conditions actuelles
void drawTFTCurrentConditions(const owm_resp_onecall_t &owm_onecall,
                              const owm_resp_air_pollution_t &owm_air_pollution,
                              float inTemp, float inHumidity) {
  tft.setCursor(8, 8);
  tft.setTextColor(COLOR_TITLE);
  tft.setTextSize(2);
  tft.println("Meteo Actuelle");
  
  // Température
  tft.setCursor(8, 32);
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(3);
  tft.print(owm_onecall.current.temp, 1);
  tft.print(" ");
  tft.print(TXT_UNITS_TEMP_ABBREVIATED);
  
  // Description météo
  tft.setCursor(8, 60);
  tft.setTextSize(2);
  tft.println(owm_onecall.current.weather[0].description);
  
  // Humidité
  tft.setCursor(8, 90);
  tft.setTextSize(1);
  tft.print("Humidite: ");
  tft.print(owm_onecall.current.humidity);
  tft.println("%");
  
  // Pression
  tft.setCursor(8, 110);
  tft.print("Pression: ");
  tft.print(owm_onecall.current.pressure);
  tft.print(" ");
  tft.println(TXT_UNITS_PRES_ABBREVIATED);
  
  // Vent
  tft.setCursor(8, 130);
  tft.print("Vent: ");
  tft.print(owm_onecall.current.wind_speed, 1);
  tft.print(" ");
  tft.println(TXT_UNITS_SPEED_ABBREVIATED);
  
  // Température intérieure
  if (!std::isnan(inTemp)) {
    tft.setCursor(8, 155);
    tft.setTextColor(COLOR_SUCCESS);
    tft.print("T. Int: ");
    tft.print(inTemp, 1);
    tft.print(" ");
    tft.println(TXT_UNITS_TEMP_ABBREVIATED);
  }
  
  // Humidité intérieure
  if (!std::isnan(inHumidity)) {
    tft.setCursor(8, 170);
    tft.print("H. Int: ");
    tft.print(inHumidity, 0);
    tft.println("%");
  }
}

// Page 2 : Prévisions horaires
void drawTFTHourlyForecast(const owm_resp_onecall_t &owm_onecall,
                          const tm &timeInfo) {
  tft.setCursor(8, 8);
  tft.setTextColor(COLOR_TITLE);
  tft.setTextSize(2);
  tft.println("Previsions 24h");
  
  int y = 32;
  const int lineHeight = 16;
  
  // Afficher les 12 prochaines heures
  for (int i = 0; i < 12 && i < HOURLY_GRAPH_MAX; i++) {
    tft.setCursor(8, y);
    tft.setTextColor(COLOR_TEXT);
    tft.setTextSize(1);
    
    // Heure
    tm tempTime = timeInfo;
    tempTime.tm_hour += i + 1;
    mktime(&tempTime);
    char timeStr[6];
    strftime(timeStr, sizeof(timeStr), "%H:%M", &tempTime);
    tft.print(timeStr);
    tft.print("  ");
    
    // Température
    tft.print(owm_onecall.hourly[i].temp, 1);
    tft.print((char)247); // Symbole degré
    tft.print("  ");
    
    // Précipitations
    if (owm_onecall.hourly[i].pop > 0) {
      tft.print((int)(owm_onecall.hourly[i].pop * 100));
      tft.print("%");
    }
    
    y += lineHeight;
    if (y > 220) break;
  }
}

// Page 3 : Prévisions journalières
void drawTFTDailyForecast(const owm_resp_onecall_t &owm_onecall,
                         const tm &timeInfo) {
  tft.setCursor(8, 8);
  tft.setTextColor(COLOR_TITLE);
  tft.setTextSize(2);
  tft.println("Previsions 7j");
  
  int y = 32;
  const int lineHeight = 22;
  
  // Afficher 7 jours
  for (int i = 0; i < 7 && i < 8; i++) {
    tft.setCursor(8, y);
    tft.setTextColor(COLOR_TEXT);
    tft.setTextSize(1);
    
    // Jour de la semaine
    tm tempTime = timeInfo;
    tempTime.tm_mday += i + 1;
    mktime(&tempTime);
    char dayStr[12];
    strftime(dayStr, sizeof(dayStr), "%a %d", &tempTime);
    tft.print(dayStr);
    
    // Températures min/max
    tft.setCursor(120, y);
    tft.setTextColor(COLOR_ERROR);
    tft.print(owm_onecall.daily[i].temp.max, 0);
    tft.print((char)247);
    
    tft.setCursor(160, y);
    tft.setTextColor(COLOR_TITLE);
    tft.print(owm_onecall.daily[i].temp.min, 0);
    tft.print((char)247);
    
    // Probabilité de précipitations
    tft.setCursor(200, y);
    tft.setTextColor(COLOR_TEXT);
    if (owm_onecall.daily[i].pop > 0) {
      tft.print((int)(owm_onecall.daily[i].pop * 100));
      tft.print("%");
    }
    
    y += lineHeight;
    if (y > 220) break;
  }
}

// Page 4 : Informations système
void drawTFTSystemInfo(uint32_t batteryVoltage, int wifiRSSI,
                      const tm &timeInfo) {
  tft.setCursor(8, 8);
  tft.setTextColor(COLOR_TITLE);
  tft.setTextSize(2);
  tft.println("Systeme");
  
  // Date et heure
  tft.setCursor(8, 32);
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);
  char dateTimeStr[32];
  strftime(dateTimeStr, sizeof(dateTimeStr), "%d/%m/%Y %H:%M:%S", &timeInfo);
  tft.println(dateTimeStr);
  
  // Batterie
  if (batteryVoltage != UINT32_MAX) {
    tft.setCursor(8, 60);
    tft.print("Batterie: ");
    tft.print(batteryVoltage);
    tft.println(" mV");
    
    // Pourcentage de batterie
    uint32_t percentage = constrain(
      map(batteryVoltage, MIN_BATTERY_VOLTAGE, MAX_BATTERY_VOLTAGE, 0, 100),
      0, 100
    );
    
    tft.setCursor(8, 80);
    if (percentage < 20) {
      tft.setTextColor(COLOR_ERROR);
    } else if (percentage < 50) {
      tft.setTextColor(COLOR_WARN);
    } else {
      tft.setTextColor(COLOR_SUCCESS);
    }
    tft.print("Charge: ");
    tft.print(percentage);
    tft.println("%");
    
    // Barre de batterie
    tft.drawRect(8, 100, 180, 16, COLOR_TEXT);
    int fillWidth = map(percentage, 0, 100, 0, 176);
    if (percentage < 20) {
      tft.fillRect(10, 102, fillWidth, 12, COLOR_ERROR);
    } else if (percentage < 50) {
      tft.fillRect(10, 102, fillWidth, 12, COLOR_WARN);
    } else {
      tft.fillRect(10, 102, fillWidth, 12, COLOR_SUCCESS);
    }
  }
  
  // WiFi
  tft.setCursor(8, 120);
  tft.setTextColor(COLOR_TEXT);
  tft.print("WiFi RSSI: ");
  tft.print(wifiRSSI);
  tft.println(" dBm");
  
  // Qualité du signal
  tft.setCursor(8, 140);
  if (wifiRSSI > -50) {
    tft.setTextColor(COLOR_SUCCESS);
    tft.println("Signal: Excellent");
  } else if (wifiRSSI > -60) {
    tft.setTextColor(COLOR_SUCCESS);
    tft.println("Signal: Bon");
  } else if (wifiRSSI > -70) {
    tft.setTextColor(COLOR_WARN);
    tft.println("Signal: Moyen");
  } else {
    tft.setTextColor(COLOR_ERROR);
    tft.println("Signal: Faible");
  }
  
  // Mémoire
  tft.setCursor(8, 170);
  tft.setTextColor(COLOR_TEXT);
  tft.print("RAM libre: ");
  tft.print(ESP.getFreeHeap() / 1024);
  tft.println(" KB");
  
  tft.setCursor(8, 190);
  tft.print("PSRAM libre: ");
  tft.print(ESP.getFreePsram() / 1024);
  tft.println(" KB");
}
