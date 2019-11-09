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

#include "drive_stuff.h"

// Sd2Card card;
// SdVolume volume;
// File currDir;
// File file; // TODO: make this unnecessary
File file;
DiskDrive drive1;

DriveStatus *getDeviceStatus(int deviceId)
{
  return drive1.getStatus();
}

SectorDataInfo *readSector(int deviceId, uint32_t sector, byte *data)
{
  if (drive1.hasImage())
  {
    return drive1.getSectorData(sector, data);
  }
  else
  {
    return NULL;
  }
}

boolean writeSector(int deviceId, uint32_t sector, byte *data, uint32_t length)
{
  return (drive1.writeSectorData(sector, data, length) == length);
}

boolean format(int deviceId, int density)
{
  // char name[13];
  String name; //SPIFFS

  // get current filename
  // file.getFilename(name);
  name = file.name(); //SPIFFS

  // close and delete the current file
  file.close();
  // file.remove();  // NO SPIFFS equivalent, but can open for write with truncate to 0

  LOG_MSG("Remove old file: ");
  LOG_MSG_CR(name);

  // open new file for writing
  // file.open(&currDir, name, O_RDWR | O_SYNC | O_CREAT);
  file = SPIFFS.open("/" + name, "w");

  LOG_MSG("Created new file: ");
  LOG_MSG_CR(name);

  // allow the virtual drive to format the image (and possibly alter its size)
  if (drive1.formatImage(&file, density))
  {
    // set the new image file for the drive
    drive1.setImageFile(&file);
    return true;
  }
  else
  {
    return false;
  }

  return false;
}

void changeDisk(int deviceId)
{
  // dir_t dir;
  Dir dir = SPIFFS.openDir(IMDIR);
  char name[13];
  boolean imageChanged = false;

  while (!imageChanged)
  {
    // get next dir entry
    // int8_t result = currDir.readDir((dir_t *)&dir);

    //SPIFFS from here down to next comment block
    boolean result = dir.next(); //SPIFFS

    // if we have a valid file response code, open it
    if (result && dir.fileSize() > 0)
    {
      String tempName = dir.fileName(); //SPIFFS
      tempName.toCharArray(name, 13);   //SPIFFS
#ifdef DEBUG1
      LOGGING_UART.println(tempName);
#endif
      imageChanged = mountFilename(deviceId, name); // SPIFFS: MAKE name A STRING
    }
    else
    {
      dir.rewind();
      // result = dir.next();
    }

    /*     // if we got back a 0, rewind the directory and get the first dir entry
    if (!result)
    {
      currDir.rewind();
      result = currDir.readDir((dir_t *)&dir);
    }

    // if we have a valid file response code, open it
    if (result > 0 && isValidFilename((char *)&dir.name))
    {
      createFilename(name, (char *)dir.name);
      imageChanged = mountFilename(deviceId, name);
    }
 */
  }
}

boolean isValidFilename(char *s)
{
String temp = s;

#ifdef DEBUG1
  LOGGING_UART.print("Checking filename ... ");
  LOGGING_UART.println(s);
  LOGGING_UART.flush();
#endif
/*   return (s[0] != '.' &&   // ignore hidden files
          s[0] != '_' && ( // ignore bogus files created by OS X
                             (s[8] == 'A' && s[9] == 'T' && s[10] == 'R') || (s[8] == 'X' && s[9] == 'F' && s[10] == 'D')
#ifdef PRO_IMAGES
                             || (s[8] == 'P' && s[9] == 'R' && s[10] == 'O')
#endif
#ifdef ATX_IMAGES
                             || (s[8] == 'A' && s[9] == 'T' && s[10] == 'X')
#endif
#ifdef XEX_IMAGES
                             || (s[8] == 'X' && s[9] == 'E' && s[10] == 'X')
#endif
                                 ));
 */  

return (  s[0] != '.' &&   // ignore hidden files
          s[0] != '_' && ( // ignore bogus files created by OS X
          temp.endsWith("ATR") || temp.endsWith("XFD") 
#ifdef PRO_IMAGES
                               || temp.endsWith("PRO")
#endif
#ifdef ATX_IMAGES
                               || temp.endsWith("ATX")
#endif
#ifdef XEX_IMAGES
                               || temp.endsWith("XEX")
#endif
                                 ));
}

void createFilename(char *filename, char *name)
{

#ifdef DEBUG1
  LOGGING_UART.print("Creating filename from ");
  LOGGING_UART.print(name);
#endif

  for (int i = 0; i < 8; i++)
  {
    if (name[i] != ' ')
    {
      *(filename++) = name[i];
    }
  }
  if (name[8] != ' ')
  {
    *(filename++) = '.';
    *(filename++) = name[8];
    *(filename++) = name[9];
    *(filename++) = name[10];
  }
  *(filename++) = '\0';

#ifdef DEBUG1
  LOGGING_UART.print(" makes ");
  LOGGING_UART.println(filename);
  LOGGING_UART.flush();
#endif

}

/**
 * Returns a list of files in the current directory.
 *
 * startIndex = the first valid file in the directory to start from
 * count = how many files to return
 * entries = a pointer to the a FileEntry array to hold the returned data
 */
int getFileList(int startIndex, int count, FileEntry *entries)
{
  // dir_t dir;
  Dir dir = SPIFFS.openDir(IMDIR); //SPIFFS
  char name[13];
  int currentEntry = 0;

  // currDir.rewind();

  int ix = 0;
  while (ix < count)
  {
    // if (currDir.readDir((dir_t *)&dir) < 1)
    if (!dir.next()) // SPIFFS
    {
      break;
    }
    String tempName = dir.fileName(); // SPIFFS: NEED TO STRIP OFF FIRST /
    int lastIdx = tempName.lastIndexOf("/");
    if (lastIdx != -1)
    {
      tempName.remove(0, lastIdx + 1);
    }
    // SPIFFS insert spaces to make exact 8.3
    lastIdx = tempName.lastIndexOf(".");
    String suffix = tempName.substring(lastIdx+1);
    tempName.remove(lastIdx);
    while (lastIdx<8) {
      tempName.concat(" ");
      lastIdx++;
    } 
    tempName.concat(suffix);
    tempName.toCharArray(name, 13); //SPIFFS
//#ifdef DEBUG1
//    LOGGING_UART.println(tempName);
//    LOGGING_UART.println(name);
//#endif
    // if (isValidFilename((char *)&dir.name) || (DIR_IS_SUBDIR(&dir) && dir.name[0] != '.'))
    if (isValidFilename(name))
    {
      if (currentEntry >= startIndex)
      {
        memcpy(entries[ix].name, name, 11);
        // if (DIR_IS_SUBDIR(&dir))
        // {
        //   entries[ix].isDirectory = true;
        // }
        // else
        // {
        entries[ix].isDirectory = false;
        // }
        ix++;
#ifdef DEBUG1
        LOGGING_UART.print("Added file to list: ");
        LOGGING_UART.println(name);
        LOGGING_UART.flush();
#endif
      }
      currentEntry++;
    }
  }

  //  return ix;
  return 0;
}

/**
 * Changes the SD card directory.
 *
 * ix = index number (or -1 to go to parent directory)
 */
void changeDirectory(int ix)
{
  /*   FileEntry entries[1];
  char name[13];
  File subDir;

  if (ix > -1)
  {
    getFileList(ix, 1, entries);
    createFilename(name, entries[0].name);
    if (subDir.open(&currDir, name, O_READ))
    {
      currDir = subDir;
    }
  }
  else
  {
    if (subDir.openRoot(&volume))
    {
      currDir = subDir;
    }
  } */
}

/**
 * Mount a file with the given name.
 *
 * deviceId = the drive ID
 * name = the name of the file to mount
 */
boolean mountFilename(int deviceId, char *name) // SPIFFS: MAKE name a STRING
{
  String tempName = name;
  // close previously open file
  //  if (file.isOpen())
  //{
  file.close();
  // }
#ifdef DEBUG1
  LOGGING_UART.print("mounting file ... ");
  LOGGING_UART.println(name);
#endif
  // if (file.open(&currDir, name, O_RDWR | O_SYNC) && drive1.setImageFile(&file))
  file = SPIFFS.open(IMDIR + tempName, "r+"); // path should be an absolute path starting with a slash
  if (!file)
  {
    #ifdef DEBUG1
    LOGGING_UART.print("mountFilename: Could not open ");
    LOGGING_UART.println(name);
    #endif
    return false;
  }
  else
  {
#ifdef DEBUG1
    LOGGING_UART.println("YES Mounted file!");
    LOGGING_UART.flush();
#endif
  }

  if (drive1.setImageFile(&file))
  {
    LOG_MSG_CR(name);
#ifdef DEBUG1
    LOGGING_UART.println(name);
    LOGGING_UART.flush();
#endif
    // #ifdef LCD_DISPLAY
    //     lcd.clear();
    //     lcd.print(name);
    //     lcd.setCursor(0, 1);
    // #endif

    return true;
  }

  return false;
}

/**
 * Mount a file with the given index number.
 *
 * deviceId = the drive ID
 * ix = the index of the file to mount
 */
void mountFileIndex(int deviceId, int ix)
{
  FileEntry entries[1]; // SPIFFS NOTE: CAN I MAKE THIS STRINGS OR USE A SPIFFS CALL?
  char name[13];        // SPIFFS NOTE: MAKE STRING INSTEAD

  // figure out what filename is associated with the index
  getFileList(ix, 1, entries); // SPIFFS NOTE: CAN I MAKE THIS STRINGS OR USE A SPIFFS CALL?

  // build a full 8.3 filename
  createFilename(name, entries[0].name); // SPIFFS NOTE: CAN GET RID OF THIS CALL?

  // mount the image
  mountFilename(deviceId, name); // SPIFFS: MAKE name A STRING
}

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
