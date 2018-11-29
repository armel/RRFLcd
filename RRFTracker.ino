/*
    RRFTracker
    Learn more about RRF on https://f5nlg.wordpress.com
    Check video about RRFTracker on https://www.youtube.com/watch?v=3QY_RpRebKc
*/

#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <time.h>

//Time interval for the RRFTracker to refresh itself

#define REFRESH 3000  // 3 secondes...

//I2C LCD settings

#define WIDTH   16
#define HEIGHT   2

LiquidCrystal_I2C lcd(0x27, WIDTH, HEIGHT); // Change I2C adress 0x27, width and size if necessary...

//If debug, set true...

bool debug = false;

//Wifi config, change it !

const char* ssid     = "Your SSID";
const char* password = "Your Password";

//Time config

int timezone = 1;
int dst = 0;

//Global variables, not really clean...

String url = "http://rrf.f5nlg.ovh/";

String call_activ;
String call_previous = "RRF";
String call_previous_next = "RRF";
String call_time = "Waiting TX";

String tmp;

bool blanc = true;
bool blanc_alternate = true;
int qso = 0;

void setup() {
  //Init Debug mode if true...
  if(debug == true) {
    Serial.begin(115200);
  }

  //Init LED pins
  pinMode(D4, OUTPUT);        // HTTP Request
  pinMode(D5, OUTPUT);        // Transmitter activ
  pinMode(D8, OUTPUT);        // Transmitter inactiv

  //Init LCD
  lcd.begin(WIDTH, HEIGHT);
  lcd.init();
  
  //Turn on the LCD backlight.
  lcd.backlight();

  //We start by connecting to a WiFi network
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

  //Time configuration
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
    HTTPClient http;            //Declare an object of class HTTPClient
    http.begin(url);            //Specify request destination
    int httpCode = http.GET();  //Send the request

    if (httpCode > 0) {                                 //Check the returning code
      page = http.getString();                          //Get the request response page
      search_start = page.indexOf("transmitter\":\"");  //Search this pattern
      search_start += 14;                               //Shift...
      search_stop = page.indexOf("\"", search_start);   //And close it...

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

      //If transmitter...
      
      if (search_stop != search_start) {
        blanc = false;
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

        if(blanc_alternate == true) {               //Total TX
          tmp = "Total TX ";
          tmp += qso;
          tab = (WIDTH - tmp.length()) / 2;
          lcd.setCursor(tab, 1);
          lcd.print(tmp);
          blanc_alternate = false;
        }
        else {                                      //Last TX
          tab = (WIDTH - call_time.length()) / 2;
          lcd.setCursor(tab, 1);
          lcd.print(call_time);
          blanc_alternate = true;
        }
      }
    }
    http.end();      //Close connection
  }
  delay(REFRESH);    //Send a request after a pause
}
