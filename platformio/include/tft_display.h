/* TFT Display helper for esp32-weather-epd.
 * Copyright (C) 2025
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef __TFT_DISPLAY_H__
#define __TFT_DISPLAY_H__

#include <Arduino.h>
#include "api_response.h"

// Nombre de pages disponibles pour la navigation
#define TFT_NUM_PAGES 4

// Énumération des différentes pages
enum TFT_Page {
  PAGE_CURRENT_CONDITIONS = 0,  // Conditions météo actuelles
  PAGE_HOURLY_FORECAST = 1,     // Prévisions horaires
  PAGE_DAILY_FORECAST = 2,      // Prévisions journalières
  PAGE_SYSTEM_INFO = 3          // Infos système (batterie, WiFi, etc.)
};

// Initialiser l'écran TFT
void initTFT();

// Éteindre l'écran TFT (économie d'énergie)
void powerOffTFT();

// Initialiser les boutons
void initButtons();

// Vérifier si un bouton a été pressé et gérer la navigation
// Retourne true si une page a changé
bool checkButtons();

// Obtenir la page actuelle
uint8_t getCurrentPage();

// Définir la page actuelle
void setCurrentPage(uint8_t page);

// Afficher la page actuelle avec les données météo
void displayCurrentPage(const owm_resp_onecall_t &owm_onecall,
                       const owm_resp_air_pollution_t &owm_air_pollution,
                       float inTemp, float inHumidity,
                       uint32_t batteryVoltage, int wifiRSSI,
                       const tm &timeInfo);

// Affichage des pages individuelles
void drawTFTCurrentConditions(const owm_resp_onecall_t &owm_onecall,
                              const owm_resp_air_pollution_t &owm_air_pollution,
                              float inTemp, float inHumidity);

void drawTFTHourlyForecast(const owm_resp_onecall_t &owm_onecall,
                          const tm &timeInfo);

void drawTFTDailyForecast(const owm_resp_onecall_t &owm_onecall,
                         const tm &timeInfo);

void drawTFTSystemInfo(uint32_t batteryVoltage, int wifiRSSI,
                      const tm &timeInfo);

// Afficher un indicateur de page (en haut ou en bas de l'écran)
void drawPageIndicator(uint8_t currentPage, uint8_t totalPages);

#endif // __TFT_DISPLAY_H__
