/*
    RRFTracker
    Learn more about RRF on https://f5nlg.wordpress.com
    Check video about RRFTracker on https://www.youtube.com/watch?v=3QY_RpRebKc
*/

#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <time.h>

// Time interval for the RRFTracker to refresh itself

#define REFRESH 3000  // Wait for 3 secondes...

// I2C LCD settings

#define WIDTH   16
#define HEIGHT   2

LiquidCrystal_I2C lcd(0x27, WIDTH, HEIGHT); // Change I2C adress 0x27, width and size if necessary...

// If debug, set true...

bool debug = false;

// Wifi config, change it !

const char* ssid     = "Your SSID";
const char* password = "Your Password";

// Time config

int timezone = 1;
int dst = 0;
time_t start;

// Global variables, not really clean...

String url = "http://rrf.f5nlg.ovh/api/svxlink/RRF";

String call_activ;
String call_previous = "RRF";
String call_previous_next = "RRF";
String call_time = "Waiting TX";
String start_time = "";

String tmp;

bool blanc = true;
int blanc_alternate = 0;
int qso = 0;
int qso_total = 0;
int qso_swap = 0;

void setup() {
  // Init Debug mode if true...
  if(debug == true) {
    Serial.begin(115200);
  }

  //Init LED pins
  pinMode(D4, OUTPUT);        // HTTP Request
  pinMode(D5, OUTPUT);        // Transmitter activ
  pinMode(D8, OUTPUT);        // Transmitter inactiv

  // Init LCD
  lcd.begin(WIDTH, HEIGHT);
  lcd.init();
  
  // Turn on the LCD backlight.
  lcd.backlight();

  // We start by connecting to a WiFi network
  lcd.setCursor(0, 0);
  lcd.print("Connecting to ");
  lcd.setCursor(0, 1);
  lcd.print(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.print(".");
  }

  lcd.setCursor(0, 0);
  lcd.print("WiFi connected");
  lcd.setCursor(0, 1);
  lcd.print("IP : ");
  lcd.print(WiFi.localIP());

  delay(1000);

  lcd.clear();

  // Time configuration
  lcd.setCursor(0, 0);
  lcd.print("Waiting for time");
  lcd.setCursor(0, 1);

  configTime(timezone * 3600, dst * 3600, "pool.ntp.org", "time.nist.gov");
  while (!time(nullptr)) {
    delay(250);
    lcd.print(".");
  }
  
  delay(1000);
}

void loop() {
  int tab;
  int search_start, search_stop;
  time_t now = time(nullptr);
  struct tm * timeinfo;
  String page;

  timeinfo = localtime(&now);  
  if (WiFi.status() == WL_CONNECTED) {

    digitalWrite(D4, HIGH);
    HTTPClient http;            // Declare an object of class HTTPClient
    http.begin(url);            // Specify request destination
    int httpCode = http.GET();  // Send the request

    if (httpCode > 0) {                                 // Check the returning code
      if(start_time == "") {                            // Format start_time...
          start = now;
      }
      
      page = http.getString();                          // Get the request response page
      yield();                                          // Watchdog...
      search_start = page.indexOf("transmitter\":\"");  // Search this pattern
      search_start += 14;                               // Shift...
      search_stop = page.indexOf("\"", search_start);   // And close it...

      digitalWrite(D4, LOW);
      
      lcd.clear();
      
      if (debug == true) {
        Serial.println(page);
        Serial.println(search_start);
        Serial.println(search_stop);
        Serial.println("----------");
        Serial.println(call_activ);
        Serial.println(call_previous);
        Serial.println(call_previous_next);
        Serial.println("----------");
        Serial.println(url);
        Serial.println("----------");
        Serial.println(qso);
      }

      tab = (WIDTH - call_previous.length()) / 2;
      lcd.setCursor(tab, 0);
      lcd.print(call_previous);

      // If transmitter...
      
      if (search_stop != search_start) {
        tmp = page.substring(search_start, search_stop);
        
        if (tmp == "RRF1") {
          url = "http://rrf.f5nlg.ovh/";
          call_activ = "RRF2 <-> RRF1";
        }
        else if (tmp == "RRF2") {
          url = "http://rrf2.f5nlg.ovh/";
          call_activ = "RRF1 <-> RRF2";
        }
        else {
          digitalWrite(D5, HIGH);
          digitalWrite(D8, LOW);
          call_activ = tmp;

          if (call_activ.startsWith("(60) F1ZCY") == true)        // Optimize F1ZCY ...
            call_activ = "(60) F1ZCY V/U";
          else if (call_activ.startsWith("(84) F5LLJ") == true)   // Optimize F5LLJ ...
            call_activ = "(84) F5LLJ V>U";
            
          if (call_previous_next != call_activ) {
            call_previous_next = call_activ;
            qso++;
          }
          else {
            blanc = false;
          }
        }

        call_time = "Last TX ";
        
        if(timeinfo->tm_hour < 10) {
          call_time += "0";
        }
        call_time += timeinfo->tm_hour;
        call_time += ":";
        if(timeinfo->tm_min < 10) {
          call_time += "0";
        }  
        call_time += timeinfo->tm_min;
        
        tab = (WIDTH - call_activ.length()) / 2;
        lcd.setCursor(tab, 1);        
        lcd.print(call_activ);
      }
      // If no Transmitter...
      else {
        digitalWrite(D5, LOW);
        digitalWrite(D8, HIGH);
        
        if (blanc == false) {
          blanc = true;
          qso++;
        }

        call_previous = call_previous_next;

        if(blanc_alternate == 0) {              // Last TX time
          tab = (WIDTH - call_time.length()) / 2;
          lcd.setCursor(tab, 1);
          lcd.print(call_time);
          blanc_alternate = 1;
        }
        else if(blanc_alternate == 1) {         // TX today
          tmp = "TX Today ";
          tmp += qso;
          tab = (WIDTH - tmp.length()) / 2;
          lcd.setCursor(tab, 1);
          lcd.print(tmp);
          blanc_alternate = 2;
        }
        else if(blanc_alternate == 2) {         // Boot time
          start_time = "Up " + uptime((long)now - (long)start);
          tab = (WIDTH - start_time.length()) / 2;
          lcd.setCursor(tab, 1);
          lcd.print(start_time);
          blanc_alternate = 3;          
        }        
        else if(blanc_alternate == 3) {         // TX total
          if(timeinfo->tm_hour == 0 && timeinfo->tm_min == 0) {
            qso_total += qso;
            qso = 0;
          }
          tmp = "TX Total ";
          tmp += qso_total;
          tab = (WIDTH - tmp.length()) / 2;
          lcd.setCursor(tab, 1);
          lcd.print(tmp);
          blanc_alternate = 0;
        }
      }
    }
    http.end();      // Close connection
  }
  yield();
  delay(REFRESH);    // Send a request after a pause
  yield();
}

// Convert timestamp in day, hour and minute

String uptime(long n) {   
  String tmp = "";
  
  int day = n / (24 * 3600); 
  n = n % (24 * 3600); 
  int hour = n / 3600; 
  n %= 3600;
  int minute = n / 60 ;
  
  if (day < 10) {
    tmp += "0" + String(day);
  }
  else {
    tmp += String(day);
  }

  tmp += " d, ";
  
  if (hour < 10) {
    tmp += "0" + String(hour);
  }
  else {
    tmp += String(hour);    
  }

  tmp += ":";

  if (minute < 10) {
    tmp += "0" + String(minute);
  }
  else {
    tmp += String(minute);    
  }

  yield();
  
  return tmp;
} 
