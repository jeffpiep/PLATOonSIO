#include <Arduino.h>

/* Oproti softu na http://whizzosoftware.com/sio2arduino/ jsem musel ještě navíc naimportovat knihovnu SPI.h viz nad a použít starou verzi knihovny sdfat (verze 21.9.2013. Také jsem použil jinou SD čtečku
 *  
 * sio2arduino.ino - An Atari 8-bit device emulator for Arduino.
 *
 * Copyright (c) 2012 Whizzo Software LLC (Daniel Noguerol)
 *
 * This file is part of the SIO2Arduino project which emulates
 * Atari 8-bit SIO devices on Arduino hardware.
 *
 * SIO2Arduino is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * SIO2Arduino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SIO2Arduino; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * 

Atari Data In  (SIO pin 3) -> Arduino digital pin 1 (TX) (note: this is usually the orange SIO cable)
Atari Ground   (SIO pin 4) -> Arduino ground (GND)
Atari Data Out (SIO pin 5) -> Arduino digital pin 0 (RX) (note: this is usually the green SIO cable)
Atari Command  (SIO pin 7) -> Arduino digital pin 2

*/

#include "config.h"
#include "atari.h"
#include "sio_channel.h"
#include "disk_drive.h"
#include "FS.h"

// Sd2Card card;
// SdVolume volume;
// File currDir;
// File file; // TODO: make this unnecessary
DriveStatus *getDeviceStatus(int deviceId);

SectorDataInfo *readSector(int deviceId, uint32_t sector, byte *data);

boolean writeSector(int deviceId, uint32_t sector, byte *data, uint32_t length);

boolean format(int deviceId, int density);

void changeDisk(int deviceId);

boolean isValidFilename(char *name);

void createFilename(char *filename, char *name);

int getFileList(int startIndex, int count, FileEntry *entries);

void changeDirectory(int ix);

boolean mountFilename(int deviceId, char *name);

void mountFileIndex(int deviceId, int ix);

#ifdef SELECTOR_BUTTON
boolean isSwitchPressed = false;
uint32_t lastSelectionPress;
boolean isFileOpened;
#endif
#ifdef RESET_BUTTON
uint32_t lastResetPress;
#endif
// #ifdef LCD_DISPLAY
// LiquidCrystal lcd(PIN_LCD_RD,PIN_LCD_ENABLE,PIN_LCD_DB4,PIN_LCD_DB5,PIN_LCD_DB6,PIN_LCD_DB7);
// #endif
