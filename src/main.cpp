#include <Arduino.h>

/*
PlatoSIO - Jeff Piepmeier
Modifed version of SIO2Arduino
*/

#define MTR_PIN 16 // motor control pin on SIO bus

#include "config.h"
#include "FS.h"
#include "drive_stuff.h"
#include "modem.h"
#include "ota.h"
#include <Esp.h>

// #ifdef LCD_DISPLAY
// #include <LiquidCrystal.h>
// #endif

// moved all the SIO2Arduino to drive_stuff.cpp
/**
 * Global variables
 */
DriveAccess driveAccess(getDeviceStatus, readSector, writeSector, format);
DriveControl driveControl(getFileList, mountFileIndex, changeDirectory);
SIOChannel sioChannel(PIN_ATARI_CMD, &SIO_UART, &driveAccess, &driveControl);

boolean diskState = true;

void SIO_CALLBACK()
{
  // LOG_MSG_CR("Callback");
  // inform the SIO channel that an incoming byte is available
  sioChannel.processIncomingByte();
}

void setup()
{

  pinMode(MTR_PIN, INPUT_PULLDOWN_16); // motor control pin turns on when R: driver starts. driver is PNP open collector output.

  SPIFFS.begin();

  // initialize serial port to Atari
  SIO_UART.begin(19200); //Serial uses UART0, which is mapped to pins GPIO1 (TX) and GPIO3 (RX).

#ifdef DEBUG
  // Serial.swap(); //Serial may be remapped to GPIO15 (TX) and GPIO13 (RX) by calling Serial.swap() after Serial.begin.
  // set up logging serial port
  LOGGING_UART.begin(56000);
  // Serial1.setDebugOutput(true);
  LOG_MSG_CR(" XXXXXX ");
  LOG_MSG_CR(" XXXXXX ");
#endif
#ifdef DEBUG1
  LOGGING_UART.begin(57600);
  LOGGING_UART.println("");
  LOGGING_UART.print("XXXXX   ");
  LOGGING_UART.println(ESP.getResetReason());
  LOGGING_UART.flush();
#endif

  /*   // set pin modes
  #ifdef SELECTOR_BUTTON
  pinMode(PIN_SELECTOR, INPUT);
  #endif
  #ifdef RESET_BUTTON
  pinMode(PIN_RESET, INPUT);
  #endif

  #ifdef LCD_DISPLAY
  // set up LCD if appropriate
  lcd.begin(16, 2);
  lcd.print("SIO2Arduino");
  lcd.setCursor(0,1);
  #endif

  initialize SD card
  LOG_MSG("Initializing SD card...");
  pinMode(PIN_SD_CS, OUTPUT);

  if (!card.init(SPI_HALF_SPEED, PIN_SD_CS)) {
    LOG_MSG_CR(" failed.");
    #ifdef LCD_DISPLAY
      lcd.print("SD Init Error");
    #endif     
    return;
  }
  
  if (!volume.init(&card)) {
    LOG_MSG_CR(" failed.");
    #ifdef LCD_DISPLAY
      lcd.print("SD Volume Error");
    #endif     
    return;
  }

  if (!currDir.openRoot(&volume)) {
    LOG_MSG_CR(" failed.");
    #ifdef LCD_DISPLAY
      lcd.print("SD Root Error");
    #endif     
    return;
  }

  LOG_MSG_CR(" done.");
  #ifdef LCD_DISPLAY
    lcd.print("READY");
  #endif
 */
  if (!mountFilename(0, (char *)BOOTIMAGE))
  {
    LOG_MSG_CR("Mount file failed.");
  }

  modemSetup();

  OTAsetup();
}

void loop()
{
  int busState = digitalRead(MTR_PIN);

  if (busState == LOW)
  {
    // add logic to go back to disk from modem
    if (diskState)
    {
      // let the SIO channel do its thing
      sioChannel.runCycle();

#ifdef SELECTOR_BUTTON
      // watch the selector button (accounting for debounce)
      if (digitalRead(PIN_SELECTOR) == HIGH && millis() - lastSelectionPress > 250)
      {
        lastSelectionPress = millis();
        changeDisk(0);
      }
#endif

#ifdef RESET_BUTTON
      // watch the reset button
      if (digitalRead(PIN_RESET) == HIGH && millis() - lastResetPress > 250)
      {
        lastResetPress = millis();
        mountFilename(0, "/AUTORUN.ATR");
      }
#endif

#if defined(NODEMCU) || defined(ARDUINO_TEENSY)
      if (SIO_UART.available())
        SIO_CALLBACK();
#endif
    }
    else
    {
      // switch serial back to 19200
      // initialize serial port to Atari
      SIO_UART.end();
      SIO_UART.begin(19200); //Serial uses UART0, which is mapped to pins GPIO1 (TX) and GPIO3 (RX).
#ifdef DEBUG
      // SIO_UART.swap(); //Serial may be remapped to GPIO15 (TX) and GPIO13 (RX) by calling Serial.swap() after Serial.begin.
#endif
      diskState = true;
    }
  }
  else
  {
    if (diskState)
    {
      LOG_MSG_CR("switching to modem state!");
      modemStart();
      diskState = false;
    }
    else
    {
      modemCycle();
    }
  }
  ArduinoOTA.handle();



}
