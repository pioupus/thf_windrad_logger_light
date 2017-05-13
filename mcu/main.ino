


#include "Arduino.h"


#include "vc.h"

#include <SPI.h>
#include <SD.h>
#include <RTClib.h>
#include <stdint.h>

//Occupied by SD-Shield:
//Digital #13 - SPI clock
//Digital #12 - SPI MISO
//Digital #11 - SPI MOSI
//Digital #10 - SD Card chip select (can cut a trace to re-assign)
//SDA connected to A4
//SCL connected to A5

const uint8_t PIN_LED_SSR = 13;

const uint8_t PIN_LED_WORKING = 4;
const uint8_t PIN_LED_OK = 3;
const uint8_t PIN_ANALOG = A0;
const uint8_t PIN_CHIP_SELECT = 10;
const uint8_t PIN_RELAIS_1 = 9;
const uint8_t PIN_RELAIS_2 = 6;
const uint8_t PIN_RELAIS_3 = 5;

const uint8_t MONTH_ON =    4;
const uint8_t MONTH_OFF =   10;
const uint8_t HOUR_ON =     20;
const uint8_t HOUR_OFF =    6;
const uint8_t PAUSE_SECONDS_x_10 =   100;


static RTC_DS1307 rtc;

bool serial_initialized = false;

void setup () {
	pinMode(PIN_RELAIS_1, OUTPUT);
	pinMode(PIN_RELAIS_2, OUTPUT);
	pinMode(PIN_RELAIS_3, OUTPUT);
	pinMode(PIN_CHIP_SELECT, OUTPUT);
	analogReference(INTERNAL);
	pinMode(PIN_LED_OK, OUTPUT);
	pinMode(PIN_LED_WORKING, OUTPUT);
	pinMode(PIN_LED_SSR, OUTPUT);

    Serial.begin(115200);

    if (! rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (1);
    }

    if (! rtc.isrunning()) {
        Serial.println("RTC is NOT running!");
        // following line sets the RTC to the date & time this sketch was compiled
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }
    
    Serial.print("Initializing SD card...");

    // see if the card is present and can be initialized:
    if (!SD.begin(PIN_CHIP_SELECT)) {
        Serial.println("Card failed, or not present");
        digitalWrite(PIN_LED_OK, LOW);
        // don't do anything more:
        return;
    }
    digitalWrite(PIN_LED_OK, HIGH);
    Serial.println("card initialized.");

}

//Beispiel Zeile:
//13.06.2007 13:49:30;28983

void loop () {

	static uint8_t led_state = 0;
	digitalWrite(PIN_LED_SSR, HIGH);

    uint32_t current_avg = 0;
    for (uint16_t i = 0; i< PAUSE_SECONDS_x_10;i++){
        current_avg += analogRead(PIN_ANALOG);
        delay(100);
        if ((led_state & 1) == 0){
        	digitalWrite(PIN_LED_WORKING, HIGH);
        }else{
        	digitalWrite(PIN_LED_WORKING, LOW);
        }
        led_state++;
    }
    
    current_avg /= PAUSE_SECONDS_x_10;
    

    DateTime now = rtc.now();
    char date_string_[10] = {};
    char date_fn_[10] = {};
    sprintf(date_string_,"%04d.%02d.%02d %02d:%02d:%02d\0",now.year(),now.month(),now.day(),now.hour(),now.minute(),now.second());
    sprintf(date_fn_,"%04d%02d%02d\0",now.year(),now.month(),now.day());

    String date_string(date_string_);
    String filename(date_fn_);

    String dataString = date_string +";"+String(current_avg);
    
    
    Serial.println(dataString);

    
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File dataFile = SD.open(filename+".txt", FILE_WRITE);

    bool SDerror;
    // if the file is available, write to it:
    if (dataFile) {
        dataFile.println(dataString);
        dataFile.close();
        digitalWrite(PIN_LED_OK, HIGH);
        SDerror = false;
        // print to the serial port too:
        //Serial.println(dataString);
    } else { // if the file isn't open, pop up an error:
        Serial.println("error opening "+filename+".txt");
        digitalWrite(PIN_LED_OK, LOW);
        SDerror = true;
    }
    if ((MONTH_ON <= now.month()) && (now.month() <= MONTH_OFF) && ((HOUR_ON <= now.hour()) || (now.hour() <= HOUR_OFF) )){
    	digitalWrite(PIN_RELAIS_1, HIGH);
    	digitalWrite(PIN_RELAIS_2, HIGH);
    	digitalWrite(PIN_RELAIS_3, HIGH);

        for (uint16_t i = 0; i< 20;i++){
            delay(50);
            if ((led_state & 1) == 0){
            	digitalWrite(PIN_LED_OK, LOW);
            }else{
            	digitalWrite(PIN_LED_OK, HIGH);
            }
            led_state++;
        }
        if(SDerror){
        	digitalWrite(PIN_LED_OK, LOW);
        }else{
        	digitalWrite(PIN_LED_OK, HIGH);
        }

    }else{
    	digitalWrite(PIN_RELAIS_1, LOW);
    	digitalWrite(PIN_RELAIS_2, LOW);
    	digitalWrite(PIN_RELAIS_3, LOW);
    }

}



