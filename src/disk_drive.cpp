/*
 * disk_drive.cpp - Virtual disk drive class.
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
*/
#include "disk_drive.h"

DiskDrive::DiskDrive() {
  // reset device status
  memset(&m_driveStatus.statusFrame, 0, sizeof(m_driveStatus.statusFrame));

  // set standard attributes
  m_driveStatus.statusFrame.timeout_lsb = 0xE0;
}

DriveStatus* DiskDrive::getStatus() {
  m_driveStatus.statusFrame.commandStatus.writeProtect = m_diskImage.isReadOnly() ? 0x01 : 0x00;
  return &m_driveStatus;
}

boolean DiskDrive::setImageFile(File *file) {
  boolean result = m_diskImage.setFile(file);
  if (result) {
    // set device status
    memset(&m_driveStatus.statusFrame, 0, sizeof(m_driveStatus.statusFrame));
    m_driveStatus.statusFrame.commandStatus.enhancedDensity = m_diskImage.isEnhancedDensity() ? 0x01 : 0x00;
    m_driveStatus.statusFrame.commandStatus.doubleDensity = m_diskImage.isDoubleDensity() ? 0x01 : 0x00;
    m_driveStatus.statusFrame.hardwareStatus.writeProtect = m_diskImage.isReadOnly() ? 0x00 : 0x01;
    m_driveStatus.sectorSize = m_diskImage.getSectorSize();
  }
  return result;
}

SectorDataInfo* DiskDrive::getSectorData(uint32_t sector, byte *data) {
  if (m_diskImage.hasImage()) {
    uint32_t startTime = micros();
    
    SectorDataInfo *info = m_diskImage.getSectorData(sector, data);
    // store the status frame if valid
    if (info->validStatusFrame) {
      memcpy(&m_driveStatus.statusFrame, &(info->statusFrame), sizeof(m_driveStatus.statusFrame));
    }
    
    // for images with copy-protection, make read time consistent across all sector reads to 
    // allow skew-based protection to work
    if (m_diskImage.hasCopyProtection()) {
      uint32_t time = micros() - startTime;
      uint32_t delta = (MIN_PRO_SECTOR_READ * ((time - 1) / MIN_PRO_SECTOR_READ + 1)) - time;
      delay(delta / 1000);             // Arduino's delayMicroseconds() can't reliably 
      delayMicroseconds(delta % 1000); // handle an argument larger than 16,383
    }
    
    return info;
  } else {
    return NULL;
  }
}

uint32_t DiskDrive::writeSectorData(uint32_t sector, byte *data, uint32_t len) {
  return m_diskImage.writeSectorData(sector, data, len);
}

boolean DiskDrive::formatImage(File *file, int density) {
  return m_diskImage.format(file, density);
}

boolean DiskDrive::hasImage() {
  return m_diskImage.hasImage();
}
