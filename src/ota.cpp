#include "ota.h"

void OTAsetup()
{

    // Port defaults to 8266
    // ArduinoOTA.setPort(8266);

    // Hostname defaults to esp8266-[ChipID]
    // ArduinoOTA.setHostname("myesp8266");

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA.onStart([]() {
        String type;
        SPIFFS.end();
        SIO_UART.end();
        if (ArduinoOTA.getCommand() == U_FLASH)
        {
            type = "sketch";
        }
        else
        { // U_FS
            type = "filesystem";
            // NOTE: if updating FS this would be the place to unmount FS using FS.end()
        }
        LOG_MSG_CR("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
        LOG_MSG_CR("");
        LOG_MSG_CR("OTA End");
    });
#ifdef DEBUG1
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        LOGGING_UART.printf("Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        LOGGING_UART.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
        {
            LOGGING_UART.println("Auth Failed");
        }
        else if (error == OTA_BEGIN_ERROR)
        {
            LOGGING_UART.println("Begin Failed");
        }
        else if (error == OTA_CONNECT_ERROR)
        {
            LOGGING_UART.println("Connect Failed");
        }
        else if (error == OTA_RECEIVE_ERROR)
        {
            LOGGING_UART.println("Receive Failed");
        }
        else if (error == OTA_END_ERROR)
        {
            LOGGING_UART.println("End Failed");
        }
    });
#endif
    ArduinoOTA.begin();
}