#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define VERSIONA 0
#define VERSIONB 1
#define VERSION_ADDRESS 0 // EEPROM address
#define VERSION_LEN 2     // Length in bytes
#define SSID_ADDRESS 2
#define SSID_LEN 32
#define PASS_ADDRESS 34
#define PASS_LEN 63
#define IP_TYPE_ADDRESS 97   // for future use
#define STATIC_IP_ADDRESS 98 // length 4, for future use
#define STATIC_GW 102        // length 4, for future use
#define STATIC_DNS 106       // length 4, for future use
#define STATIC_MASK 110      // length 4, for future use
#define BAUD_ADDRESS 111
#define ECHO_ADDRESS 112
#define SERVER_PORT_ADDRESS 113 // 2 bytes
#define AUTO_ANSWER_ADDRESS 115 // 1 byte
#define TELNET_ADDRESS 116      // 1 byte
#define VERBOSE_ADDRESS 117
#define PET_TRANSLATE_ADDRESS 118
#define FLOW_CONTROL_ADDRESS 119
#define PIN_POLARITY_ADDRESS 120
#define DIAL0_ADDRESS 200
#define DIAL1_ADDRESS 250
#define DIAL2_ADDRESS 300
#define DIAL3_ADDRESS 350
#define DIAL4_ADDRESS 400
#define DIAL5_ADDRESS 450
#define DIAL6_ADDRESS 500
#define DIAL7_ADDRESS 550
#define DIAL8_ADDRESS 600
#define DIAL9_ADDRESS 650
#define BUSY_MSG_ADDRESS 700
#define BUSY_MSG_LEN 80
#define LAST_ADDRESS 780

#define SWITCH_PIN 12 // GPIO0 (programmind mode pin)
#define LED_PIN 5    // Status LED
#define DCD_PIN 5    // DCD Carrier Status
// #define RTS_PIN 13   // RTS Request to Send, connect to host's CTS pin
// #define CTS_PIN 12   // CTS Clear to Send, connect to host's RTS pin

#define LISTEN_PORT 6400 // Listen to this if not connected. Set to zero to disable.
#define RING_INTERVAL 3000    // How often to print RING when having a new incoming connection (ms)
#define MAX_CMD_LENGTH 256  // Maximum length for AT command
#define LED_TIME 15         // How many ms to keep LED on at activity
#define TX_BUF_SIZE 256 // Buffer where to read from serial before writing to TCP
// (that direction is very blocking by the ESP TCP stack,
// so we can't do one byte a time.)


// Telnet codes
#define DO 0xfd
#define WONT 0xfc
#define WILL 0xfb
#define DONT 0xfe


enum resultCodes_t
{
  R_OK,
  R_CONNECT,
  R_RING,
  R_NOCARRIER,
  R_ERROR,
  R_NONE,
  R_NODIALTONE,
  R_BUSY,
  R_NOANSWER
};
enum flowControl_t
{
  F_NONE,
  F_HARDWARE,
  F_SOFTWARE
};
enum pinPolarity_t
{
  P_INVERTED,
  P_NORMAL
}; // Is LOW (0) or HIGH (1) active?




String connectTimeString();

void setEEPROM(String inString, int startAddress, int maxLen);

void writeSettings();

String getEEPROM(int startAddress, int len);

void readSettings();

void defaultEEPROM();

void sendResult(int resultCode);

void sendString(String msg);

// Hold for 5 seconds to switch to 300 baud
// Slow flash: keep holding
// Fast flash: let go
int checkButton();

void updateLed();

void connectWiFi();

void disconnectWiFi();

void setBaudRate(int inSpeed);

void setCarrier(byte carrier);

String ipToString(IPAddress ip);

void displayNetworkStatus();

void displayCurrentSettings();

void displayStoredSettings();

void waitForSpace();

void welcome();

void displayHelp();

void storeSpeedDial(byte num, String location);

void hangUp();

/**
   Turn on the LED and store the time, so the LED will be shortly after turned off
*/
void led_on();

void answerCall();

void handleIncomingConnection();

void dialOut(String upCmd);

void command();

void handleFlowControl();

void handleCommandMode();

void handleConnectedMode();

void handleWebServer();

void handlePlusCount();

void checkConnection();

void modemSetup();

void modemStart();

void modemCycle();
