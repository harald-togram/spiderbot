#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Ramp.h>

rampInt ramp0;
rampInt ramp1;
rampInt ramp2;
rampInt ramp3;
rampInt ramp4;
rampInt ramp5;
rampInt ramp6;
rampInt ramp7;
rampInt ramp8;
rampInt ramp9;
rampInt ramp10;
rampInt ramp11;

//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);
// you can also call it with a different address and I2C interface
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire);

// Depending on your servo make, the pulse width min and max may vary, you
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  400 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  500 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  1900 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates
#define metode SINUSOIDAL_INOUT
#define gentag ONCEFORWARD
#define HASTIGHED_DREJ 2000
#define HASTIGHED_GAA  600
#define LOFT 40

const char* ssid = "MY_WIFI_NAME";
const char* password = "MY_WIFI_PASSWORD";


//til http
WiFiServer server(80);
// Variable to store the HTTP request
String header;
// Auxiliar variables to store the current output state
String output1State = "off";
String output2State = "off";
String output3State = "off";
String output4State = "off";
// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

//serial stuff
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;

//øjne
int rightEyeXInt;
int rightEyeYInt;
int leftEyeXInt;
int leftEyeYInt;
float fuzzyX;
float fuzzyY;
float gammelFuzzyX;
float gammelFuzzyY;

int posX;
int posXGammel;
int posY;
int posYGammel;
int tid = 1000;

int pulsen;
int rigtigPulsGammel;

bool tilstand1 = false;
bool tilstand2 = false;
bool tilstand3 = false;
bool tilstand4 = false;

int gammelVert;
int gammelHoz;

int nummerY;
int nummerX;

void setup() {
  //Serial.begin(115200);
  Serial2.begin(115200);
  inputString.reserve(200);

  startOTA();

  Wire.begin(22, 23);
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  startRamp();

  hojreBagBen(130, 0, 0);
  while (hojreBagBenProcent() < 100) {
  }
  delay(500);
  venstreBagBen(130, 0, 0);
  while (venstreBagBenProcent() < 100) {
  }
  delay(500);
  venstreForBen(70, 0, 0);
  while (venstreForBenProcent() < 100) {
  }
  hojreForBen(70, 0, 0);
  while (hojreForBenProcent() < 100) {
  }
  delay(500);

  pinMode(0, INPUT);
}

void loop() {
  wifiLoop();
  ArduinoOTA.handle();
  serialLoop();
  if (tilstand3 == true) {
    kigPaaAnsigt();
    drej(posX, posY);
  }
}

void otaDelay(int tid) {
  for (int i = 0; i < tid; i++) {
    ArduinoOTA.handle();
    delay(1);
  }
}

int graderTilPuls(int grader) {
  int puls = map(grader, 0, 150, USMIN, USMAX);
  return puls;
}

void startRamp() {
  ramp0.setGrain(1);
  ramp0.go(graderTilPuls(100));

  ramp1.setGrain(1);
  ramp1.go(graderTilPuls(0));

  ramp2.setGrain(1);
  ramp2.go(graderTilPuls(0));

  ramp3.setGrain(1);
  ramp3.go(graderTilPuls(100));

  ramp4.setGrain(1);
  ramp4.go(graderTilPuls(200));

  ramp5.setGrain(1);
  ramp5.go(graderTilPuls(200));

  ramp6.setGrain(1);
  ramp6.go(graderTilPuls(100));

  ramp7.setGrain(1);
  ramp7.go(graderTilPuls(0));

  ramp8.setGrain(1);
  ramp8.go(graderTilPuls(0));

  ramp9.setGrain(1);
  ramp9.go(graderTilPuls(100));

  ramp10.setGrain(1);
  ramp10.go(graderTilPuls(200));

  ramp11.setGrain(1);
  ramp11.go(graderTilPuls(200));
}

void hojreBagBen(int servo1, int servo2, int servo3) {
  ramp0.go(graderTilPuls(servo1), tid, metode, gentag);
  ramp1.go(graderTilPuls(servo2), tid, metode, gentag);
  ramp2.go(graderTilPuls(servo3), tid, metode, gentag);
}

int hojreBagBenProcent() {
  pwm.writeMicroseconds(0, ramp0.update());
  pwm.writeMicroseconds(1, ramp1.update());
  pwm.writeMicroseconds(2, ramp2.update());
  float komplet = ramp0.getCompletion();
  int intProcent = int(komplet);
  return intProcent;
}

void venstreBagBen(int servo1, int servo2, int servo3) {
  ramp3.go(graderTilPuls(200 - servo1), tid, metode, gentag);
  ramp4.go(graderTilPuls(200 - servo2), tid, metode, gentag);
  ramp5.go(graderTilPuls(200 - servo3), tid, metode, gentag);
}

int venstreBagBenProcent() {
  pwm.writeMicroseconds(3, ramp3.update());
  pwm.writeMicroseconds(4, ramp4.update());
  pwm.writeMicroseconds(5, ramp5.update());
  float komplet = ramp3.getCompletion();
  int intProcent = int(komplet);
  return intProcent;
}

void venstreForBen(int servo1, int servo2, int servo3) {
  ramp6.go(graderTilPuls(200 - servo1), tid, metode, gentag);
  ramp7.go(graderTilPuls(servo2), tid, metode, gentag);
  ramp8.go(graderTilPuls(servo3), tid, metode, gentag);
}

int venstreForBenProcent() {
  pwm.writeMicroseconds(8, ramp6.update());
  pwm.writeMicroseconds(7, ramp7.update());
  pwm.writeMicroseconds(6, ramp8.update());
  float komplet = ramp6.getCompletion();
  int intProcent = int(komplet);
  return intProcent;
}

void hojreForBen(int servo1, int servo2, int servo3) {
  ramp9.go(graderTilPuls(servo1), tid, metode, gentag);
  ramp10.go(graderTilPuls(200 - servo2), tid, metode, gentag);
  ramp11.go(graderTilPuls(200 - servo3), tid, metode, gentag);
}

int hojreForBenProcent() {
  pwm.writeMicroseconds(9, ramp9.update());
  pwm.writeMicroseconds(10, ramp10.update());
  pwm.writeMicroseconds(11, ramp11.update());
  float komplet = ramp9.getCompletion();
  int intProcent = int(komplet);
  return intProcent;
}

void wifiLoop() {
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {// loop while the client's connected

      ArduinoOTA.handle();
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            /*
                        Serial.println("header start ---------------");
                        Serial.println(header);
                        Serial.println("header end -----------------");*/
            int nummerIndex = header.indexOf("quantity=");//+9
            int nummerIndex3 = header.indexOf("quantity2=");//+10
            Serial.print("nummerIndex er");
            Serial.println(nummerIndex);
            Serial.print("nummerIndex3 er");
            Serial.println(nummerIndex3);
            // turns the GPIOs on and off
            if (header.indexOf("GET /1/on") >= 0) {
              output1State = "on";
              tilstand1 = true;
              gaaFrem();
              //skridt();
            } else if (header.indexOf("GET /1/off") >= 0) {
              output1State = "off";
              tilstand1 = false;
            } else if (header.indexOf("GET /2/on") >= 0) {
              output2State = "on";
              tilstand2 = true;
              startDrej();
            } else if (header.indexOf("GET /2/off") >= 0) {
              output2State = "off";
              tilstand2 = false;
              slutDrej();
            }
            else if (header.indexOf("GET /3/on") >= 0) {
              output3State = "on";
              tilstand3 = true;
            }
            else if (header.indexOf("GET /3/off") >= 0) {
              output3State = "off";
              tilstand3 = false;
            }
            else if (header.indexOf("GET /4/on") >= 0) {
              output4State = "on";
              tilstand4 = true;
              dans();
            }
            else if (header.indexOf("GET /4/off") >= 0) {
              output4State = "off";
              tilstand4 = false;
            }

            else if (nummerIndex >= 0 && nummerIndex < 300) {
              int nummerIndex2 = header.indexOf(" HTTP");
              String valueX = header.substring(nummerIndex + 9, nummerIndex2);
              nummerX = valueX.toInt();
              drej(nummerX, nummerY);
              Serial.print("nummer x er");
              Serial.println(nummerX);
            }
            else if (nummerIndex3 >= 0 && nummerIndex3 < 300) {
              int nummerIndex4 = header.indexOf(" HTTP");
              String valueY = header.substring(nummerIndex3 + 10, nummerIndex4);
              nummerY = valueY.toInt();
              drej(nummerX, nummerY);
              Serial.print("nummer Y er ");
              Serial.println(nummerY);
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");

            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");

            // Display current state, and ON/OFF buttons for GPIO 26
            client.println("<p>gaa frem " + output1State + "</p>");
            // If the output26State is off, it displays the ON button
            if (output1State == "off") {
              client.println("<p><a href=\"/1/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/1/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            // Display current state, and ON/OFF buttons for GPIO 27
            client.println("<p>start drej " + output2State + "</p>");
            // If the output27State is off, it displays the ON button
            if (output2State == "off") {
              client.println("<p><a href=\"/2/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/2/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            // Display current state, and ON/OFF buttons for GPIO 27
            client.println("<p>kig paa ansigt " + output3State + "</p>");
            // If the output27State is off, it displays the ON button
            if (output3State == "off") {
              client.println("<p><a href=\"/3/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/3/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            // Display current state, and ON/OFF buttons for GPIO 27
            client.println("<p>fjerdeKnap " + output4State + "</p>");
            // If the output27State is off, it displays the ON button
            if (output4State == "off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            client.println("</body></html>");

            client.println("<form action=\"/get\">\"<label for=\"quantity\">hoz (-50:50)</label><input type=\"number\" id=\"quantity\" name=\"quantity\" min=\"-50\" max=\"50\"><input type=\"submit\"></a></p></form>");
            client.println("<form action=\"/get\">\"<label for=\"quantity2\">vert (-100:100)</label><input type=\"number\" id=\"quantity2\" name=\"quantity2\" min=\"-100\" max=\"100\"><input type=\"submit\"></a></p></form>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
      //serialLoop();
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void startOTA() {
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
   ArduinoOTA.setHostname("spiderbot");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  })
  .onEnd([]() {
    Serial.println("\nEnd");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void gaaFrem() {
  tid = HASTIGHED_GAA;
  hojreBagBen(100, LOFT, LOFT * 2); //sænk højre bagben
  while (hojreBagBenProcent() < 50) {
    ArduinoOTA.handle();
  }

  venstreForBen(150, LOFT * 2, LOFT * 2);//løft venstre forben
  while (venstreForBenProcent() < 50 && hojreBagBenProcent() < 100) {
    ArduinoOTA.handle();
  }
  venstreForBen(150, 0, 0); //sæt venstre forben ned
  hojreBagBen(100, 0, 0);//stræk hojre bagben ud
  while (venstreForBenProcent() < 100 && hojreBagBenProcent() < 100) {
    ArduinoOTA.handle();
  }

  venstreBagBen(100, LOFT, LOFT * 2);//sænk venstre bagben
  while (venstreBagBenProcent() < 50) {
    ArduinoOTA.handle();
  }

  hojreForBen(150, LOFT * 2, LOFT * 2);//løft hojre forben
  while (hojreForBenProcent() < 100 && venstreBagBenProcent() < 100) {
    ArduinoOTA.handle();
  }

  hojreForBen(150, 0, 0);//stræk hojre forben ud
  venstreBagBen(100, 0, 0);//stræk venstre bagben ud
  while (hojreForBenProcent() < 100 && venstreBagBenProcent() < 100) {
    ArduinoOTA.handle();
  }

  venstreBagBen(50, 0, 0);
  hojreBagBen(50, 0, 0);
  venstreForBen(100, 0, 0);
  hojreForBen(100, 0, 0);
  while (venstreBagBenProcent() < 100 && hojreBagBenProcent() < 100 && venstreForBenProcent() < 100 && hojreForBenProcent() < 100) {
    ArduinoOTA.handle();
  }

  hojreForBen(100, LOFT, LOFT * 2); // sænk hojre forben
  while (hojreForBenProcent() < 50) {
    ArduinoOTA.handle();
  }

  venstreBagBen(100, LOFT * 2, LOFT * 4); // bøj og drej venstre bagben
  while (venstreBagBenProcent() < 50 && hojreForBenProcent() < 100) {
    ArduinoOTA.handle();
  }

  hojreForBen(100, 0, 0);
  venstreBagBen(100, 0, 0);// stræk venstre bagben ud
  while (venstreBagBenProcent() < 100 && hojreForBenProcent() < 100) {
    ArduinoOTA.handle();
  }

  venstreForBen(100, LOFT, LOFT * 2); // sænk venstre forben
  while (venstreForBenProcent() < 50) {
    ArduinoOTA.handle();
  }

  hojreBagBen(100, LOFT * 2, LOFT * 4);
  while (hojreBagBenProcent() < 50 && venstreForBenProcent() < 100) {
    ArduinoOTA.handle();
  }
  venstreForBen(100, 0, 0);
  hojreBagBen(100, 0, 0);
  while (hojreBagBenProcent() < 100 && venstreForBenProcent() < 100) {
    ArduinoOTA.handle();
  }
}

void bounce(int antalBounces) {
  tid = HASTIGHED_DREJ;
  for (int i = 0; i < antalBounces; i++) {
    hojreForBen(100, 100 + LOFT, 100);
    venstreForBen(100, 100 - LOFT, 100);
    hojreBagBen(100, 100 + LOFT, 100);
    venstreBagBen(100, 100 - LOFT, 100);
    while (hojreForBenProcent() < 100 && venstreForBenProcent() < 100 && hojreBagBenProcent() < 100 && venstreBagBenProcent() < 100) {
      ArduinoOTA.handle();
    }
    hojreForBen(100, 100 - LOFT, 100);
    venstreForBen(100, 100 + LOFT, 100);
    hojreBagBen(100, 100 - LOFT, 100);
    venstreBagBen(100, 100 + LOFT, 100);
    while (hojreForBenProcent() < 100 && venstreForBenProcent() < 100 && hojreBagBenProcent() < 100 && venstreBagBenProcent() < 100) {
      ArduinoOTA.handle();
    }
  }
  hojreForBen(100, 100, 100);
  venstreForBen(100, 100, 100);
  hojreBagBen(100, 100, 100);
  venstreBagBen(100, 100, 100);
  while (hojreForBenProcent() < 100 && venstreForBenProcent() < 100 && hojreBagBenProcent() < 100 && venstreBagBenProcent() < 100) {
    ArduinoOTA.handle();
  }
}


void wiggle(int antalBounces) {
  tid = HASTIGHED_DREJ;
  for (int i = 0; i < antalBounces; i++) {
    hojreForBen(100, 100 + LOFT, 100 + LOFT);
    venstreForBen(100, 100 - LOFT, 100 - LOFT);
    hojreBagBen(100, 100 + LOFT, 100 + LOFT);
    venstreBagBen(100, 100 - LOFT, 100 - LOFT);
    while (hojreForBenProcent() < 100 && venstreForBenProcent() < 100 && hojreBagBenProcent() < 100 && venstreBagBenProcent() < 100) {
      ArduinoOTA.handle();
    }
    hojreForBen(100, 100 - LOFT, 100 - LOFT);
    venstreForBen(100, 100 + LOFT, 100 + LOFT);
    hojreBagBen(100, 100 - LOFT, 100 - LOFT);
    venstreBagBen(100, 100 + LOFT, 100 + LOFT);
    while (hojreForBenProcent() < 100 && venstreForBenProcent() < 100 && hojreBagBenProcent() < 100 && venstreBagBenProcent() < 100) {
      ArduinoOTA.handle();
    }
  }
  hojreForBen(100, 100, 100);
  venstreForBen(100, 100, 100);
  hojreBagBen(100, 100, 100);
  venstreBagBen(100, 100, 100);
  while (hojreForBenProcent() < 100 && venstreForBenProcent() < 100 && hojreBagBenProcent() < 100 && venstreBagBenProcent() < 100) {
    ArduinoOTA.handle();
  }
}

void drej(int drejHoz, int drejVert) {
  int forskelVert = drejVert - gammelVert;
  if (forskelVert < 0) {
    forskelVert = 0 - forskelVert;
  }
  int forskelHoz = drejHoz - gammelHoz;
  if (forskelHoz < 0) {
    forskelHoz = 0 - forskelHoz;
  }
  if (forskelVert < forskelHoz && forskelHoz != 0) {
    tid = (HASTIGHED_DREJ / 100) * forskelHoz;
  }
  if (forskelVert >= forskelHoz && forskelVert != 0) {
    tid = (HASTIGHED_DREJ / 100) * forskelVert;
  }
  if (drejVert <= 50 && drejVert >= -50) {
    hojreForBen(150 + drejHoz, 50 - drejVert, 50 - drejVert);
    venstreForBen(150 - drejHoz, 50 - drejVert, 50 - drejVert);
    hojreBagBen(50 + drejHoz, 50 + drejVert, 50 + drejVert);
    venstreBagBen(50 - drejHoz, 50 + drejVert, 50 + drejVert);
  }
  else if (drejVert > 50) {
    hojreForBen(150 + drejHoz, 0, 0);
    venstreForBen(150 - drejHoz, 0, 0);
    hojreBagBen(50 + drejHoz, 50 + drejVert, 50 + drejVert);
    venstreBagBen(50 - drejHoz, 50 + drejVert, 50 + drejVert);
  }
  else if (drejVert < -50) {
    hojreForBen(150 + drejHoz, 50 - drejVert, 50 - drejVert);
    venstreForBen(150 - drejHoz, 50 - drejVert, 50 - drejVert);
    hojreBagBen(50 + drejHoz, 0, 0);
    venstreBagBen(50 - drejHoz, 0, 0);
  }
  while (hojreForBenProcent() < 100) {
    ArduinoOTA.handle();
    venstreForBenProcent();
    hojreBagBenProcent();
    venstreBagBenProcent();
  }
  gammelVert = drejVert;
  gammelHoz = drejHoz;
}

void startDrej() {
  tid = HASTIGHED_DREJ;

  hojreBagBen(50, 50, 50);
  venstreBagBen(50, 50, 50);
  hojreForBen(150, 50, 50);
  venstreForBen(150, 50, 50);
  while (hojreBagBenProcent() < 100) {
    venstreBagBenProcent();
    hojreForBenProcent();
    venstreForBenProcent();
    ArduinoOTA.handle();
  }
}

void slutDrej() {
  hojreBagBen(100, 0, 0);
  venstreBagBen(100, 0, 0);
  hojreForBen(100, 0, 0);
  venstreForBen(100, 0, 0);
  while (hojreBagBenProcent() < 100) {
    venstreBagBenProcent();
    hojreForBenProcent();
    venstreForBenProcent();
    ArduinoOTA.handle();
  }
}


void serialEvent() {
  while (Serial2.available()) {
    // get the new byte:
    char inChar = (char)Serial2.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

void serialLoop() {
  serialEvent();
  if (stringComplete) {
    Serial.print(inputString);
    // clear the string:
    int indexes[5];
    indexes[0] = inputString.indexOf("left eye: ("); // +11
    indexes[1] = inputString.indexOf(", ", indexes[0]); // +2
    indexes[2] = inputString.indexOf("), right eye: ("); // +15
    indexes[3] = inputString.indexOf(", ", indexes[2] + 5); // 3
    indexes[4] = inputString.indexOf("), nose: ("); // 10
    Serial.print("index er ");
    for (int i = 0; i < 5; i++) {
      Serial.print(indexes[i]);
      Serial.print(", ");
    }
    Serial.println();
    if (indexes[0] > 0) {
      String firstValue = inputString.substring(indexes[0] + 11, indexes[1]);
      leftEyeXInt = firstValue.toInt();
      String secondValue = inputString.substring(indexes[1] + 1, indexes[2]);
      leftEyeYInt = secondValue.toInt();

      String rightEyeXValue = inputString.substring(indexes[2] + 15, indexes[3]);
      rightEyeXInt = rightEyeXValue.toInt();
      String rightEyeYValue = inputString.substring(indexes[3] + 1, indexes[4]);
      rightEyeYInt = rightEyeYValue.toInt();

      Serial.print("leftEye er (");
      Serial.print(leftEyeXInt);
      Serial.print(", ");
      Serial.print(leftEyeYInt);
      Serial.print(") ");
      Serial.print(" rightEye er (");
      Serial.print(rightEyeXInt);
      Serial.print(", ");
      Serial.print(rightEyeYInt);
      Serial.println(")");

      float leftEyeXFloat = float(leftEyeXInt);
      float leftEyeYFloat = float(leftEyeYInt);
      float rightEyeXFloat = float(rightEyeXInt);
      float rightEyeYFloat = float(rightEyeYInt);
      float blandetX = (rightEyeXFloat + leftEyeXFloat) / 2;
      float blandetY = (leftEyeYFloat + rightEyeYFloat) / 2;
      Serial.print("blandet er ");
      Serial.print(blandetX);
      Serial.print(" ");
      Serial.println(blandetY);
      fuzzyX = (1.0 / 320.0) * blandetX;
      fuzzyY = (1.0 / 240.0) * blandetY;
      Serial.print("fuzzy er ");
      Serial.print(fuzzyX);
      Serial.print(" : ");
      Serial.println(fuzzyY);
    }
    inputString = "";
    stringComplete = false;
  }
}

long aktiveretTid;
#define TUNE 40
int kigTilstand;

void kigPaaAnsigt() {
  if (gammelFuzzyX != fuzzyX || gammelFuzzyY != fuzzyY) {
    /*if (fuzzyX < 0.45 && posX > -50 && fuzzyX != 0) {
      aktiveretTid = millis();
      posX = posX - ((0.45 - fuzzyX) * TUNE);
      posX = constrain(posX, -50, 50);
      }
      else if (fuzzyX > 0.55 && posX < 50 && fuzzyX != 0) {
      aktiveretTid = millis();
      posX = posX + ((fuzzyX - 0.55) * TUNE);
      posX = constrain(posX, -50, 50);
      }
    */
    posX = posX - (((0.50 - fuzzyX) * TUNE) * (fuzzyX < 0.47 && posX > -50 && fuzzyX != 0));
    posX = posX + (((fuzzyX - 0.50) * TUNE) * (fuzzyX > 0.53 && posX < 50 && fuzzyX != 0));
    posX = constrain(posX, -50, 50);

    posY = posY + (((0.50 - fuzzyY) * TUNE) * (fuzzyY < 0.47 && posY < 100 && fuzzyY != 0));
    posY = posY - (((fuzzyY - 0.50) * TUNE) * (fuzzyY > 0.53 && posY > -100 && fuzzyY != 0));
    posY = constrain(posY, -100, 100);
    /*
      if (fuzzyY < 0.45 && posY < 100 && fuzzyY != 0) {
      aktiveretTid = millis();
      posY = posY + ((0.45 - fuzzyY) * TUNE);
      posY = constrain(posY, -100, 100);
      }
      else if (fuzzyY > 0.55 && posY > -100 && fuzzyY != 0) {
      aktiveretTid = millis();
      posY = posY - ((fuzzyY - 0.55) * TUNE);
      posY = constrain(posY, -100, 100);
      }*/
    Serial.print("pos er ");
    Serial.print(posX);
    Serial.print(" ");
    Serial.println(posY);
    aktiveretTid = millis();
  }
  if (millis() > aktiveretTid + 30000) {
    posX = ((kigTilstand == 0) * -40) + ((kigTilstand == 1) * 0) + ((kigTilstand == 2) * -40) + ((kigTilstand == 3) * 40) + ((kigTilstand == 4) * 0) + ((kigTilstand == 5) * 40);
    posY = ((kigTilstand == 0) * 0) + ((kigTilstand == 1) * 0) + ((kigTilstand == 2) * 0) + ((kigTilstand == 3) * 100) + ((kigTilstand == 4) * 100) + ((kigTilstand == 5) * 100);
    aktiveretTid = millis();
    kigTilstand = (kigTilstand * (kigTilstand < 4) + (kigTilstand < 4));
  }
  gammelFuzzyX = fuzzyX;
  gammelFuzzyY = fuzzyY;
}

void skridt() {
  hojreForBen(100, 50, 100);
  venstreBagBen(100, 50, 100);
  while (hojreForBenProcent() < 50) {
    ArduinoOTA.handle();
    venstreBagBenProcent();
  }
  hojreForBen(150, 0, 0);
  venstreBagBen(150, 0, 0);
  while (hojreForBenProcent() < 100) {
    ArduinoOTA.handle();
    venstreBagBenProcent();
  }
  venstreForBen(100, 50, 100);
  hojreBagBen(100, 50, 100);
  while (hojreForBenProcent() < 50) {
    venstreForBenProcent();
  }
  venstreForBen(150, 0, 0);
  venstreBagBen(100, 0, 0);
  hojreForBen(150, 0, 0);
  hojreBagBen(100, 0, 0);
  while (hojreForBenProcent() < 100) {
    hojreBagBenProcent();
    venstreBagBenProcent();
    venstreForBenProcent();
  }
}

#define danseTal 50
void dans() {
  tid = HASTIGHED_DREJ;
  hojreForBen(100, danseTal, danseTal * 2);
  hojreBagBen(100, danseTal, danseTal * 2);
  while (hojreBagBenProcent() < 50) {
    hojreForBenProcent();
    venstreBagBenProcent();
    venstreForBenProcent();
  }

  for (int i = 0; i < 2; i++) {
    venstreBagBen(100, 0, 0);
    venstreForBen(100, 0, 0);
    while (venstreForBenProcent() < 100) {
      hojreBagBenProcent();
      venstreBagBenProcent();
      if (hojreForBenProcent() == 100) {
        hojreBagBen(100, 0, 0);
        hojreForBen(100, 0, 0);
      }
    }
    venstreBagBen(100, danseTal, danseTal * 2);
    venstreForBen(100, danseTal, danseTal * 2);
    while (venstreForBenProcent() < 100) {
      venstreBagBenProcent();
      hojreBagBenProcent();
      if (hojreForBenProcent() == 100) {
        hojreForBen(100, danseTal, danseTal * 2);
        hojreBagBen(100, danseTal, danseTal * 2);
      }
    }
  }

  for (int i = 0; i < 2; i++) {
    venstreBagBen(100, danseTal, danseTal * 2);
    venstreForBen(100, danseTal, danseTal * 2);
    hojreBagBen(100, danseTal, danseTal * 2);
    hojreForBen(100, danseTal, danseTal * 2);
    while (hojreForBenProcent() < 100) {
      venstreForBenProcent();
      venstreBagBenProcent();
      hojreBagBenProcent();
    }
    venstreBagBen(100, 0, 0);
    venstreForBen(100, 0, 0);
    hojreBagBen(100, 0, 0);
    hojreForBen(100, 0, 0);
    while (hojreForBenProcent() < 100) {
      venstreForBenProcent();
      venstreBagBenProcent();
      hojreBagBenProcent();
    }
  }

  venstreBagBen(100, 0, 0);
  venstreForBen(100, 0, 0);
  hojreBagBen(100, 0, 0);
  hojreForBen(100, 0, 0);
  while (hojreForBenProcent() < 100) {
    hojreBagBenProcent();
    venstreForBenProcent();
    venstreBagBenProcent();
  }
}
