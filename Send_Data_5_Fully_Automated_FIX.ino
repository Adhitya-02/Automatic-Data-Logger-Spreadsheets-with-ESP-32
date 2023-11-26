#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RtcDS1302.h>
#include <WiFi.h>
#include <DHT.h>
#include <ESP32Time.h>
#include <GSheet32.h>
GSheet32 Sheet("AKfycbwLcXgMkeC8f3WyzLoUDm1-3P6BczW5PwOqbyab1m8p-E5HseUSjGtu-x8_cv0Ude4g0w");

#define countof(a) (sizeof(a) / sizeof(a[0]))
// RTC and LCD related definitions
ThreeWire myWire(2, 15, 4); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define the I2C address and LCD dimensions
#define DHTTYPE DHT11
#define DHTPIN 13

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Dirman_Ganteng";
const char* password = "12345678";

String temperatureC;
String temperatureF;
String humidityH;
String heatIndexC;
String heatIndexF;

const int redLEDPin = 5;
const int yellowLEDPin = 18;
const int greenLEDPin = 19;

unsigned int dataCounter = 0; // Counter for sent data points
bool sendData = false; // Flag to control sending data

const int buzzerPin = 16; // GPIO 16 (D16) for the buzzer

void setup() {
  Serial.begin(115200);
  Serial.println(F("DHT11 Test!"));
  dht.begin();
  Sheet.connectWiFi(ssid, password);
  
  Serial.print("compiled: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);
    
  // RTC and LCD setup
  lcd.init();
  lcd.init(); // Initialize it twice
  lcd.backlight();    

  // Clear the LCD display and set the cursor position to the first row
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Time:");

  pinMode(redLEDPin, OUTPUT);
  pinMode(yellowLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT); // Set D16 (GPIO 16) as an output for the buzzer

  digitalWrite(redLEDPin, LOW);
  digitalWrite(yellowLEDPin, LOW);
  digitalWrite(greenLEDPin, LOW);
  digitalWrite(buzzerPin, LOW); // Ensure the buzzer is initially off

  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();

  if (!Rtc.IsDateTimeValid()) 
  {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

      Serial.println("RTC lost confidence in the DateTime!");
      Rtc.SetDateTime(compiled);
  }

  if (Rtc.GetIsWriteProtected())
  {
      Serial.println("RTC was write protected, enabling writing now");
      Rtc.SetIsWriteProtected(false);
  }

  if (!Rtc.GetIsRunning())
  {
      Serial.println("RTC was not actively running, starting now");
      Rtc.SetIsRunning(true);
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled) 
  {
      Serial.println("RTC is older than compile time!  (Updating DateTime)");
      Rtc.SetDateTime(compiled);
  }
  else if (now > compiled) 
  {
      Serial.println("RTC is newer than compile time. (this is expected)");
  }
  else if (now == compiled) 
  {
      Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }
}

void printDateTime(const RtcDateTime &dt)
{
    char datestring[26];

    snprintf_P(datestring,
               countof(datestring),
               PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
               dt.Month(),
               dt.Day(),
               dt.Year(),
               dt.Hour(),
               dt.Minute(),
               dt.Second());
}

void printDateTimeToLCD(const RtcDateTime &dt)
{
    char datestring[16];

    snprintf_P(datestring,
               countof(datestring),
               PSTR("%02u:%02u:%02u"),
               dt.Hour(),
               dt.Minute(),
               dt.Second());
    lcd.print(datestring);
}

void loop() {
  // RTC and LCD code
  RtcDateTime now = Rtc.GetDateTime();
  printDateTime(now);
  
  lcd.setCursor(6, 0); // Set the cursor to the first row, seventh position
  printDateTimeToLCD(now);

  // IoT code
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(yellowLEDPin, HIGH); // Turn on yellow LED if connected to WiFi
  } else {
    digitalWrite(yellowLEDPin, LOW);
  }

  if (now.Hour() >= 15 && now.Hour() < 18) {
    sendData = true; // Enable data sending between 15:00 and 17:59
  } else if (now.Hour() >= 18) {
    sendData = false; // Reset the data counter if it's past 18:00
    dataCounter = 0;
  }
  
  if (sendData && dataCounter < 130) {
    float h = dht.readHumidity();
    float tC = dht.readTemperature(); // Celsius
    float tF = dht.readTemperature(true); // Fahrenheit

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(tC) || isnan(tF)) {
      digitalWrite(redLEDPin, HIGH); // Turn on red LED for reading failure
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    temperatureC = String(tC);
    temperatureF = String(tF);
    humidityH = String(h);

    float hic = dht.computeHeatIndex(tC, h, false);
    float hif = dht.computeHeatIndex(tF, h);

    heatIndexC = String(hic);
    heatIndexF = String(hif);

    Serial.print("Temperature: ");
    Serial.print(temperatureC);
    Serial.print(" °C / ");
    Serial.print(temperatureF);
    Serial.print(" °F");
    Serial.print("  Humidity: ");
    Serial.print(humidityH);
    Serial.print("%");
    Serial.print("  Heat Index: ");
    Serial.print(heatIndexC);
    Serial.print(" °C / ");
    Serial.print(heatIndexF);
    Serial.println(" °F");

    Serial.print("Data Counter: ");
    Serial.println(dataCounter);

    digitalWrite(greenLEDPin, HIGH);
    delay(10000);

    // Activate the buzzer for 5 seconds at 15:00 (3:00 PM)
    if (now.Hour() == 15 && now.Minute() == 0) {
      digitalWrite(buzzerPin, HIGH);
      delay(5000); // 5 seconds
      digitalWrite(buzzerPin, LOW); // Turn off the buzzer
    }

    Sheet.sendData(temperatureC, temperatureF, humidityH, heatIndexC, heatIndexF);

    dataCounter++; // Increment data counter
    delay(60000); // Wait for a moment before proceeding to the next loop
    digitalWrite(redLEDPin, LOW);
    digitalWrite(greenLEDPin, LOW);
    
    if (dataCounter >= 130) {
      sendData = false; // Stop sending data once the limit is reached
      digitalWrite(buzzerPin, HIGH);
      delay(5000); // 5 seconds
      digitalWrite(buzzerPin, LOW); // Turn off the buzzer
    }
  }
    // Display the counter data value on the second line of the LCD
  lcd.setCursor(0, 1); // Set the cursor to the second row, first position
  lcd.print("Data: ");
  lcd.print(dataCounter);
}
