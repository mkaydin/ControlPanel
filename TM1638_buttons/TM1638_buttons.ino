/*
#include "wiring_shift_mod.h"

#define CLOCK_TYPE CLOCK_INVERT
#define CLOCK_DELAY_US 1

const int strobe_pin =  4;
const int clock_pin  = 16;
const int data_pin   = 17;


void setup()
{
  pinMode(strobe_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);

  sendCommand(0x8f);  // activate
  reset();
}

void loop()
{
  uint8_t buttons = readButtons();

  for(uint8_t position = 0; position < 8; position++)
  {
    uint8_t mask = 0x1 << position;

    setLed(buttons & mask ? 1 : 0, position);
  }
}

void sendCommand(uint8_t value)
{
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, value);
  digitalWrite(strobe_pin, HIGH);
}

void reset()
{
  sendCommand(0x40); // set auto increment mode
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0xc0);   // set starting address to 0
  for(uint8_t i = 0; i < 16; i++)
  {
    shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0x00);
  }
  digitalWrite(strobe_pin, HIGH);
}

uint8_t readButtons(void)
{
  uint8_t buttons = 0;
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0x42);

  pinMode(data_pin, INPUT);

  for (uint8_t i = 0; i < 4; i++)
  {
    uint8_t v = shiftInMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US) << i;
    buttons |= v;
  }

  pinMode(data_pin, OUTPUT);
  digitalWrite(strobe_pin, HIGH);
  return buttons;
}

void setLed(uint8_t value, uint8_t position)
{
  pinMode(data_pin, OUTPUT);

  sendCommand(0x44);
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0xC1 + (position << 1));
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, value);
  digitalWrite(strobe_pin, HIGH);
}
*/

/*
#include "wiring_shift_mod.h"

#define CLOCK_TYPE CLOCK_INVERT
#define CLOCK_DELAY_US 1

const int strobe_pin =  4;
const int clock_pin  = 16;
const int data_pin   = 17;

void setup()
{
  pinMode(strobe_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);

  Serial.begin(115200); // Serial haberleşmeyi başlat
  
  sendCommand(0x8f);  // activate
  reset();
}

void loop()
{
  uint8_t buttons = readButtons();

  for(uint8_t position = 0; position < 8; position++)
  {
    uint8_t mask = 0x1 << position;

    if(buttons & mask)
    {
      Serial.print("Button ");
      Serial.print(position);
      Serial.println(" pressed");
      delay(100);
    }
    
    setLed(buttons & mask ? 1 : 0, position);
  }

  delay(100); // Biraz bekle ki seri terminalde daha rahat görülebilsin
}

void sendCommand(uint8_t value)
{
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, value);
  digitalWrite(strobe_pin, HIGH);
}

void reset()
{
  sendCommand(0x40); // set auto increment mode
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0xc0);   // set starting address to 0
  for(uint8_t i = 0; i < 16; i++)
  {
    shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0x00);
  }
  digitalWrite(strobe_pin, HIGH);
}

uint8_t readButtons(void)
{
  uint8_t buttons = 0;
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0x42);

  pinMode(data_pin, INPUT);

  for (uint8_t i = 0; i < 4; i++)
  {
    uint8_t v = shiftInMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US) << i;
    buttons |= v;
  }

  pinMode(data_pin, OUTPUT);
  digitalWrite(strobe_pin, HIGH);
  return buttons;
}

void setLed(uint8_t value, uint8_t position)
{
  pinMode(data_pin, OUTPUT);

  sendCommand(0x44);
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0xC1 + (position << 1));
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, value);
  digitalWrite(strobe_pin, HIGH);
}
*/
/*

#include <WiFi.h>
#include <HTTPClient.h>
#include "wiring_shift_mod.h"

#define CLOCK_TYPE CLOCK_INVERT
#define CLOCK_DELAY_US 1

const int strobe_pin =  4;
const int clock_pin  = 16;
const int data_pin   = 17;

// WiFi bilgileri
const char* ssid = "TP-Link_B91F";
const char* password = "Bymk56212514668";
const char* serverUrl = "http://192.168.1.114:8080/Relay";

uint8_t lastButtonState = 0;

void setup()
{
  pinMode(strobe_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);

  Serial.begin(9600);

  // WiFi bağlantısını başlat
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  sendCommand(0x8f);  // activate
  reset();
}

void loop()
{
  uint8_t buttons = readButtons();

  for(uint8_t position = 0; position < 8; position++)
  {
    uint8_t mask = 0x1 << position;

    // Buton durumu değiştiyse POST isteği gönder
    if((buttons & mask) != (lastButtonState & mask))
    {
      Serial.print("Button ");
      Serial.print(position);
      Serial.println(" state changed");

      sendPostRequest(position, buttons & mask ? 1 : 0);
    }

    setLed(buttons & mask ? 1 : 0, position);
  }

  lastButtonState = buttons;
  delay(100); // Biraz bekle ki seri terminalde daha rahat görülebilsin
}

void sendCommand(uint8_t value)
{
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, value);
  digitalWrite(strobe_pin, HIGH);
}

void reset()
{
  sendCommand(0x40); // set auto increment mode
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0xc0);   // set starting address to 0
  for(uint8_t i = 0; i < 16; i++)
  {
    shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0x00);
  }
  digitalWrite(strobe_pin, HIGH);
}

uint8_t readButtons(void)
{
  uint8_t buttons = 0;
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0x42);

  pinMode(data_pin, INPUT);

  for (uint8_t i = 0; i < 4; i++)
  {
    uint8_t v = shiftInMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US) << i;
    buttons |= v;
  }

  pinMode(data_pin, OUTPUT);
  digitalWrite(strobe_pin, HIGH);
  return buttons;
}

void setLed(uint8_t value, uint8_t position)
{
  pinMode(data_pin, OUTPUT);

  sendCommand(0x44);
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0xC1 + (position << 1));
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, value);
  digitalWrite(strobe_pin, HIGH);
}

void sendPostRequest(uint8_t button, uint8_t state)
{
  if(WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // POST verilerini doğru formatta oluşturun
    String postData = "Relay" + String(button + 1) + "=" + String(state);

    int httpResponseCode = http.POST(postData);
    if(httpResponseCode > 0)
    {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    }
    else
    {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  else
  {
    Serial.println("Error in WiFi connection");
  }
}
*/
/*
#include <WiFi.h>
#include <HTTPClient.h>
#include "wiring_shift_mod.h"

#define CLOCK_TYPE CLOCK_INVERT
#define CLOCK_DELAY_US 1

const int strobe_pin =  4;
const int clock_pin  = 16;
const int data_pin   = 17;

// WiFi bilgileri
const char* ssid = "TP-Link_B91F";
const char* password = "Bymk56212514668";
const char* serverUrl = "http://192.168.1.114:8080/Relay";

uint8_t lastButtonState = 0;
uint8_t relayStates[8] = {0};  // Röle durumlarını tutmak için dizi
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;  // Debounce süresi

void setup()
{
  pinMode(strobe_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);

  Serial.begin(9600);

  // WiFi bağlantısını başlat
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  sendCommand(0x8f);  // activate
  reset();
}

void loop()
{
  uint8_t buttons = readButtons();

  if ((millis() - lastDebounceTime) > debounceDelay) {
    for(uint8_t position = 0; position < 8; position++)
    {
      uint8_t mask = 0x1 << position;

      // Buton durumu değiştiyse ve basıldıysa
      if ((buttons & mask) && !(lastButtonState & mask)) 
      {
        relayStates[position] = !relayStates[position];  // Rölenin durumunu değiştir
        Serial.print("Button ");
        Serial.print(position);
        Serial.print(" state changed to ");
        Serial.println(relayStates[position]);

        sendPostRequest(position, relayStates[position]);
        lastDebounceTime = millis();
      }

      setLed(relayStates[position], position);  // LED durumunu güncelle
    }

    lastButtonState = buttons;
  }

  delay(50);  // Döngüyü yavaşlatmak için bekleyin
}

void sendCommand(uint8_t value)
{
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, value);
  digitalWrite(strobe_pin, HIGH);
}

void reset()
{
  sendCommand(0x40); // set auto increment mode
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0xc0);   // set starting address to 0
  for(uint8_t i = 0; i < 16; i++)
  {
    shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0x00);
  }
  digitalWrite(strobe_pin, HIGH);
}

uint8_t readButtons(void)
{
  uint8_t buttons = 0;
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0x42);

  pinMode(data_pin, INPUT);

  for (uint8_t i = 0; i < 4; i++)
  {
    uint8_t v = shiftInMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US) << i;
    buttons |= v;
  }

  pinMode(data_pin, OUTPUT);
  digitalWrite(strobe_pin, HIGH);
  return buttons;
}

void setLed(uint8_t value, uint8_t position)
{
  pinMode(data_pin, OUTPUT);

  sendCommand(0x44);
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0xC1 + (position << 1));
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, value);
  digitalWrite(strobe_pin, HIGH);
}

void sendPostRequest(uint8_t button, uint8_t state)
{
    if(WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin(serverUrl);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        String postData = "RelayIndex=" + String(button) + "&RelayState=" + String(state);

        int httpResponseCode = http.POST(postData);
        if(httpResponseCode > 0)
        {
            String response = http.getString();
            Serial.println(httpResponseCode);
            Serial.println(response);
        }
        else
        {
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    }
    else
    {
        Serial.println("Error in WiFi connection");
    }
}

*/

#include <WiFi.h>
#include <HTTPClient.h>
#include "wiring_shift_mod.h"

#define CLOCK_TYPE CLOCK_INVERT
#define CLOCK_DELAY_US 1

const int strobe_pin =  4;
const int clock_pin  = 16;
const int data_pin   = 17;

// WiFi bilgileri
const char* ssid = "TP-Link_B91F";
const char* password = "Bymk56212514668";
const char* serverUrl = "http://192.168.1.114:8080";

uint8_t lastButtonState = 0;
uint8_t relayStates[8] = {0};  // Röle durumlarını tutmak için dizi
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;  // Debounce süresi

void setup()
{
  pinMode(strobe_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);

  Serial.begin(9600);

  // WiFi bağlantısını başlat
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  sendCommand(0x8f);  // activate
  reset();
  
  // Başlangıçta mevcut röle durumlarını al
  updateRelayStates();
}

void loop()
{
  scroll();
  uint8_t buttons = readButtons();

  if ((millis() - lastDebounceTime) > debounceDelay) {
    for(uint8_t position = 0; position < 8; position++)
    {
      uint8_t mask = 0x1 << position;

      // Buton durumu değiştiyse ve basıldıysa
      if ((buttons & mask) && !(lastButtonState & mask)) 
      {
        // Röle durumunu güncelle
        relayStates[position] = !relayStates[position];  
        Serial.print("Button ");
        Serial.print(position);
        Serial.print(" state changed to ");
        Serial.println(relayStates[position]);

        // Röle durumunu sunucuya gönder
        sendPostRequest(position, relayStates[position]);

        // Debounce süresini güncelle
        lastDebounceTime = millis();
      }

      // LED durumunu güncelle
      setLed(relayStates[position], position);
    }

    // Son buton durumunu güncelle
    lastButtonState = buttons;
  }

  delay(200);  // Döngüyü yavaşlatmak için bekleyin
}

void sendCommand(uint8_t value)
{
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, value);
  digitalWrite(strobe_pin, HIGH);
}

void reset()
{
  sendCommand(0x40); // set auto increment mode
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0xc0);   // set starting address to 0
  for(uint8_t i = 0; i < 16; i++)
  {
    shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0x00);
  }
  digitalWrite(strobe_pin, HIGH);
}

uint8_t readButtons(void)
{
  uint8_t buttons = 0;
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0x42);

  pinMode(data_pin, INPUT);

  for (uint8_t i = 0; i < 4; i++)
  {
    uint8_t v = shiftInMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US) << i;
    buttons |= v;
  }

  pinMode(data_pin, OUTPUT);
  digitalWrite(strobe_pin, HIGH);
  return buttons;
}

void setLed(uint8_t value, uint8_t position)
{
  pinMode(data_pin, OUTPUT);

  sendCommand(0x44);
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0xC1 + (position << 1));
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, value);
  digitalWrite(strobe_pin, HIGH);
}

void sendPostRequest(uint8_t button, uint8_t state)
{
    if(WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        String postData = "/control"; // REST endpoint
        String payload = "RelayIndex=" + String(button) + "&RelayState=" + String(state);

        http.begin(serverUrl + postData);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        int httpResponseCode = http.POST(payload);
        if(httpResponseCode > 0)
        {
            String response = http.getString();
            Serial.println(httpResponseCode);
            Serial.println(response);
        }
        else
        {
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    }
    else
    {
        Serial.println("Error in WiFi connection");
    }
}

void updateRelayStates()
{
    if(WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        String url = String(serverUrl) + "/relay_status"; // String kullanarak URL birleştirme
        http.begin(url);
        int httpResponseCode = http.GET();
        if(httpResponseCode > 0)
        {
            String response = http.getString();
            Serial.println(response);
            
            for (int i = 0; i < 8; i++)
            {
                if (i < response.length())
                {
                    relayStates[i] = response.charAt(i) - '0'; // Karakterden integera çevir
                    setLed(relayStates[i], i);
                }
            }
        }
        else
        {
            Serial.print("Error on GET: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    }
    else
    {
        Serial.println("Error in WiFi connection");
    }
}

bool scroll()
{
  
  uint8_t scrollText[] =
  {
    /* */ /* */ /* */ /* */ /* */ /* */ /* */ /* */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*C*/ /*O*/ /*N*/ /*S*/ /*O*/ /*L*/ /*E*/ /* */
    0x39, 0x3F, 0x37, 0x6D, 0x3F, 0x38, 0x79, 0x00,
    /* */ /* */ /* */ /* */ /* */ /* */ /* */ /* */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*C*/ /*O*/ /*N*/ /*S*/ /*O*/ /*L*/ /*E*/ /* */
    0x39, 0x3F, 0x37, 0x6D, 0x3F, 0x38, 0x79, 0x00,
  };
  

  static uint8_t index = 0;
  uint8_t scrollLength = sizeof(scrollText);

  sendCommand(0x40);
  digitalWrite(strobe_pin, LOW);
  shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, 0xc0);

  for(int i = 0; i < 8; i++)
  {
    uint8_t c = scrollText[(index + i) % scrollLength];

    shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, c);
    shiftOutMod(data_pin, clock_pin, LSBFIRST, CLOCK_TYPE, CLOCK_DELAY_US, c != 0 ? 1 : 0);
  }

  digitalWrite(strobe_pin, HIGH);

  index = ++index % (scrollLength << 1);

  return index == 0;
}
