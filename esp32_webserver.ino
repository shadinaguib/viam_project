#include "WiFi.h"
#include <Adafruit_NeoPixel.h>
#include <WebServer.h>

const char* ssid = "Shadi_WIFI";         // Replace with your Wi-Fi SSID
const char* password = "shadi1naguib2";  // Replace with your Wi-Fi password

#define LED_PIN 13    // GPIO pin connected to the LEDs.
#define NUM_LEDS 150  // Number of LEDs in the strip

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
WebServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize the NeoPixel strip
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // Setup web server handlers
  server.on("/trash/command", []() {
    if (server.hasArg("type")) {
      String commandType = server.arg("type");
      setLEDColors(commandType);
      server.send(200, "text/plain", "LEDs updated based on " + commandType);
    } else {
      server.send(400, "text/plain", "Command type not specified");
    }
  });

  server.on("/trash/off", []() {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(255, 0, 0));  // Turn off
    }
    strip.show();
    server.send(200, "text/plain", "LED is off");
  });

  server.begin();
}

void setLEDColors(String command) {
    int oneThird = NUM_LEDS / 3;
    uint32_t red = strip.Color(255, 0, 0);  // Red
    uint32_t green = strip.Color(0, 255, 0);  // Green

    uint32_t firstThirdColor, secondThirdColor, thirdThirdColor;

    // Assign colors based on command
    if (command == "biodegradable") {
        firstThirdColor = green;
        secondThirdColor = red;
        thirdThirdColor = red;
    } else if (command == "metal") {
        firstThirdColor = red;
        secondThirdColor = green;
        thirdThirdColor = red;
    } else if (command == "plastic") {
        firstThirdColor = red;
        secondThirdColor = red;
        thirdThirdColor = green;
    } else {
        firstThirdColor = green;
        secondThirdColor = red;
        thirdThirdColor = red;
    }

    // Apply colors to each third
    for (int i = 0; i < oneThird+4; i++) {
        strip.setPixelColor(i, firstThirdColor);
    }
    for (int i = oneThird+6; i < 2 * oneThird+8; i++) {
        strip.setPixelColor(i, secondThirdColor);
    }
    for (int i = 2 * oneThird+8; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, thirdThirdColor);
    }
    strip.show();
}

void loop() {
  server.handleClient();
}
