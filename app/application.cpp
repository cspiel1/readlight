#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <version.h>


void ShowInfo() {
    Serial.printf("\r\nSDK: v%s\r\n", system_get_sdk_version());
    Serial.printf("Free Heap: %d\r\n", system_get_free_heap_size());
    Serial.printf("CPU Frequency: %d MHz\r\n", system_get_cpu_freq());
    Serial.printf("System Chip ID: %x\r\n", system_get_chip_id());
    Serial.printf("SPI Flash ID: %x\r\n", spi_flash_get_id());
    Serial.println("Version: " VERSION);
    //Serial.printf("SPI Flash Size: %d\r\n", (1 << ((spi_flash_get_id() >> 16) & 0xff)));
}

void cb_serialReceive(Stream& stream, char arrivedChar,
        unsigned short availableCharsCount) {

    if (arrivedChar == '\n') {
        char str[availableCharsCount];
        int j=0;
        for (int i = 0; i < availableCharsCount; i++) {
            char c = stream.read();
            if ( (c>='a' && c<='z') ||
                    (c>='0' && c<='9') ||
                    (c>='A' && c<='Z') || c=='\r' || c=='\n' ) {
                if (c == '\r' || c == '\n') {
                    str[j] = 0;
                } else {
                    str[j] = c;
                }
                j++;
            }
        }

        if (!strcmp(str, "restart")) {
            System.restart();
        } else if (!strcmp(str, "info")) {
            ShowInfo();
        } else if (!strcmp(str, "help")) {
            Serial.println();
            Serial.println("available commands:");
            Serial.println("  help - display this message");
            Serial.println("  restart - restart the esp8266");
            Serial.println("  info - show esp8266 info");
            Serial.println();
        } else {
            Serial.printf("unknown command |%s|\r\n", str);
        }
    }
}

void deep_sleep() {
    Serial.println("Deep sleep...");
    digitalWrite(2, HIGH);
    delay(100);
    System.deepSleep(0xffffffff, eDSO_DISABLE_RF);
    delay(100);
}

Timer sleepTimer;

void interruptCallback() {
	Serial.println("LED off --> deep_sleep");
    deep_sleep();
}

void init() {
    WifiStation.enable(false);
    WifiStation.enableDHCP(false);
    WifiStation.disconnect();
    WifiStation.config("","",false);

    Serial.begin(SERIAL_BAUD_RATE);
    Serial.systemDebugOutput(false); // Debug output to serial

    Serial.printf("\r\n############ " VERSION " ############\r\n");
    Serial.println("Type 'help' and press enter for instructions.");
    Serial.println();
    pinMode(2, OUTPUT);
    pinMode(0, INPUT);
    digitalWrite(2, LOW);
	attachInterrupt(0, interruptCallback, FALLING);

    Serial.setCallback(cb_serialReceive);

    // 30 min
    sleepTimer.initializeMs(30*60*1000, deep_sleep);
    sleepTimer.start();
}
