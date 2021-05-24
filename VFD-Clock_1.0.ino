/*

  Vacuum Fluorescent Display Clock based on ESP8266
  Displays Date, day of week and time
  WiFi connection required since there is no way to set time and date except via NTP

  Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License

  Last edited: 2021-05-24

*/


#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTP.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 2, LCD_5x8DOTS);

const char *ssid     = "";
const char *password = "";

int thisDay;
int lastDay;
int thisHour;
int lastHour;
int thisMin;
int lastMin;
int curPos;

WiFiUDP wifiUdp;
NTP ntp(wifiUdp);


void setup() {
  lcd.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    lcd.print(".");
    delay(500);
  }
  lcd.print("Connected");
  ntp.ruleDST("CEST", Last, Sun, Mar, 2, 120);  // last sunday in march 2:00, timetone +120min (+1 GMT + 1h summertime offset)
  ntp.ruleSTD("CET", Last, Sun, Oct, 3, 60);    // last sunday in october 3:00, timezone +60min (+1 GMT)
  ntp.updateInterval(3600000);                  // Updates from NTP every hour
  ntp.begin();
}


void loop() {
  ntp.update();
  refDate();                                    // Refreshes date if applicable
  refTime();                                    // Refreshes time
  delay(1000);
}


void refDate() {
  thisDay = atoi(ntp.formattedTime("%j"));
  if (thisDay != lastDay) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(ntp.formattedTime("%d. %B %Y"));  // DD. Month YYYY
    lcd.setCursor(0, 1);
    lcd.print(ntp.formattedTime("%A"));         // Day of the week
    lastDay = thisDay;
  }
}


void refTime() {
  thisHour = atoi(ntp.formattedTime("%H"));
  thisMin = atoi(ntp.formattedTime("%M"));
  String today = ntp.formattedTime("%A");
  curPos = today.length();                      // Get the right cursor position depending on day of the week

  if (thisHour != lastHour) {                   // Update hours only after changes
    lcd.setCursor(curPos+1, 1);
    lcd.print(ntp.formattedTime("%H:"));
    lastHour = thisHour;
  }
  if (thisMin != lastMin) {                     // Update minutes only after changes
    lcd.setCursor(curPos+4, 1);
    lcd.print(ntp.formattedTime("%M:"));
    lastMin = thisMin;
  }
  lcd.setCursor(curPos+7, 1);
  lcd.print(ntp.formattedTime("%S"));           // Update seconds
}
