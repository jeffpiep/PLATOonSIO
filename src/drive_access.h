#ifndef DRIVE_ACCESS_h
#define DRIVE_ACCESS_h

#include <Arduino.h>
#include "atari.h"

class DriveAccess {
public:
  DriveAccess(DriveStatus*(*deviceStatusFunc)(int), SectorDataInfo*(*readSectorFunc)(int,uint32_t,byte*), boolean(*writeSectorFunc)(int,uint32_t,byte*,uint32_t), boolean(*formatFunc)(int,int));
  DriveStatus*      (*deviceStatusFunc)(int);
  SectorDataInfo*   (*readSectorFunc)(int,uint32_t,byte*);
  boolean           (*writeSectorFunc)(int,uint32_t, byte*,uint32_t);
  boolean           (*formatFunc)(int,int);
};

#endif
