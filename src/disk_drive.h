#ifndef DRIVE_H
#define DRIVE_H

#include <Arduino.h>
#include "atari.h"
#include "disk_image.h"

const uint32_t MIN_PRO_SECTOR_READ = 25000 - DELAY_T5 * 1000;

class DiskDrive {
public:
  DiskDrive();
  DriveStatus* getStatus();
  boolean setImageFile(File* file);
  uint32_t getImageSectorSize();
  SectorDataInfo* getSectorData(uint32_t sector, byte *data);
  uint32_t writeSectorData(uint32_t sector, byte* data, uint32_t len);
  boolean formatImage(File* file, int density);
  boolean hasImage();
private:
  DriveStatus  m_driveStatus;
  DiskImage    m_diskImage;
};

#endif
