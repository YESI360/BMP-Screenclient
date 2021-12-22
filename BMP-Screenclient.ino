// This example demonstrates beating an LED on heartbeat, and data relevant to Heart
// for more info see README at https://github.com/eringee/BioData/

#include <Heart.h>
// Create instance for sensor on analog input pin.
Heart heart(2);

// Optional variables for lighting onboard LED on heartbeat
int counter;  // counter for timing routine without using delay
int LED = 2; // onboard LED
unsigned long litMillis = 0;        // will store how long LED was lit up
const long ledInterval = 50;        // interval at which to blink LED (milliseconds)

// if you do not receive a heartbeat value in over 5 seconds, flush the BPM array and start fresh
const long flushInterval = 2000;    //interval at which to refresh values stored in array

//variable for attenuating data flow to serial port prevents crashes
const long printInterval = 20;       // millis

boolean doOnce = true;    // for only performing actions once when heartbeat is detected

// WiFi UDP connection ////
#include <WiFi.h>
#include <WiFiUdp.h>
const char* ssid     = "ESPdatos";
const char* password = "respiracion";
IPAddress ip(192, 168, 4, 1);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
WiFiUDP Udp;
IPAddress myIP;    // the IP address of your shield

////////////// TFT SCREEN //////////
#include <Wire.h>      // libreria para bus I2C
#include <Adafruit_GFX.h>   // libreria para pantallas graficas
#include <Adafruit_SSD1306.h>   // libreria para controlador SSD1306
#define ANCHO 128     // reemplaza ocurrencia de ANCHO por 128
#define ALTO 64       // reemplaza ocurrencia de ALTO por 64
#define OLED_RESET 4      // necesario por la libreria pero no usado
Adafruit_SSD1306 display(ANCHO, ALTO, &Wire, OLED_RESET);  // crea objeto

void setup() {
  Serial.begin(9600);  // works best in testing with 9600 or lower

  //optional LED for displaying heartbeat
  pinMode(LED, OUTPUT);
  litMillis = ledInterval;  // make sure the LED doesn't light up automatically

  // Initialize sensor.
  heart.reset();

//  Serial.print("Connecting to ");
//  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
//
//  Serial.println("");
//  Serial.println("WiFi connected");
//  Serial.println("IP address: ");
//  Serial.println(WiFi.localIP());
  myIP = WiFi.localIP();

  Wire.begin();         // inicializa bus I2C
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // inicializa pantalla con direccion 0x3C
}

void loop()
{

  heart.update(); // Update sensor.

  unsigned long currentMillis = millis();    // update time

  if (currentMillis % printInterval == 0) //impose a delay to avoid taxing your serial port
  {
    Serial.println(heart.getBPM());

    // Display values on the screen
    display.clearDisplay();
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(WHITE);        // Draw white text
    display.setCursor(0, 0);
    display.println(myIP);// Start at top-left corner
    display.println(F("Client connected ?"));
    display.setCursor(5, 16);
    display.setTextSize(2);             // Draw 2X-scale text
    display.setTextColor(WHITE);
    // display.invertDisplay(true);
    display.println(F("BPM"));
    display.setCursor(15, 35);
    display.println(heart.getBPM());////////////////////////////
    display.display();
    delay(50);


        Udp.beginPacket(ip, 8888);
        Udp.print(heart.getBPM());
        Udp.endPacket();
  }
  // An example of how to do something when a heartbeat is detected.
  // Remember that you should avoid using delays in order to preserve samplerate.

  if (heart.beatDetected())/////no llega!
  {
    Serial.println("beat");//no llega!!
    digitalWrite(LED, HIGH);

    if (doOnce == true) {         // only perform these actions once when a heartbeat is detected
      litMillis = currentMillis;
      Serial.println("beatTRUE");//no llega!!!
      digitalWrite(LED, HIGH);    // turn on an LED for visual feedback that heartbeat occurred

      Udp.beginPacket(ip, 8888);//no llega
      Udp.print("BEAT");
      Udp.endPacket();

      doOnce = false;
    }
  }
  else
  {
    doOnce = true;                // reset
  }

  // check to see if it's time to turn off the LED

  if (currentMillis - litMillis >= ledInterval) {   // if led interval has been surpassed turn it off
    digitalWrite(LED, LOW);
  }

}
