//#define debug

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <LEAmDNS.h>

#ifndef STASSID
#define STASSID "wifi_network"
#define STAPSK "wifi_password"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

WebServer server(80);

#include <Adafruit_MCP9600.h>
#define tc_address (0x67)
Adafruit_MCP9600 tc;

#include <Bounce2.h>
Bounce2::Button modeButton = Bounce2::Button();
Bounce2::Button upButton = Bounce2::Button();
Bounce2::Button downButton = Bounce2::Button();
//Bounce modeButton = Bounce();
//Bounce upButton = Bounce();
//Bounce downButton = Bounce();
int modePushCounter = 0;
int modeDebounced = 0;
int modeLastDebounced = 0;
bool modePressed = false;
const unsigned long intervalMs = 5;

#define NOOP __asm__ __volatile__("nop\n\t")

#include <Adafruit_SSD1306.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include <Wire.h>

const int indent1 = 88;
const int indent2 = 72;
const int bottom = 56;
const int line1 = 0;
const int line2 = 16;
const int line3 = 32;
const int upPin = 27;    // Pi Pico pin
const int modePin = 26;  // Pi Pico pin
const int downPin = 28;  // Pi Pico pin
const int relayPin = 9;  // Pi Pico pin
int operatingMode = 1;   // 1=brew 2=steam
int configureMode = 1;   // 1=operate 2=configure
int PIDparam = 1;        // 1=Kp 2=Ki 3=Kd
int steamTemp = 140;     // default steam temp
int brewTemp = 85;       // default brew temp


#include <PID_v1.h>
double targetTemp;
double currentTemp;
double heatState;
double Kp = 70;
double Ki = 0.1;
double Kd = 1;
PID myPID(&currentTemp, &heatState, &targetTemp, Kp, Ki, Kd, DIRECT);
int WindowSize = 500;
int calcSize = 250;
unsigned long windowStartTime;
unsigned long calcStartTime;


void setup() {
  Serial.begin(9600);
#ifdef debug
  Serial.println("Debugging enabled...");
#endif
  Wire.setSDA(0);
  Wire.setSCL(1);
  Wire.begin();
  pinMode(relayPin, OUTPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println("Gaggia PID");
  display.println("Pico");
  display.println("v1.0");
  delay(2000);
  if (!tc.begin(tc_address)) {
    display.clearDisplay();
    display.println("TC_mcp ERROR");
    display.display();
    while (1)
      ;
  }
  tc.setADCresolution(MCP9600_ADCRESOLUTION_18);
  tc.setThermocoupleType(MCP9600_TYPE_K);
  tc.setFilterCoefficient(3);
  tc.enable(true);
  pinMode(modePin, INPUT_PULLUP);
  modeButton.attach(modePin);
  modeButton.interval(intervalMs);
  modeButton.setPressedState(LOW);
  pinMode(upPin, INPUT_PULLUP);
  upButton.attach(upPin);
  upButton.interval(intervalMs);
  upButton.setPressedState(LOW);
  pinMode(downPin, INPUT_PULLUP);
  downButton.attach(downPin);
  downButton.interval(intervalMs);
  downButton.setPressedState(LOW);
  windowStartTime = millis();
  calcStartTime = millis();
  targetTemp = brewTemp;
  myPID.SetOutputLimits(0, WindowSize);
  myPID.SetMode(AUTOMATIC);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    Serial.print(".");
//  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

//  if (MDNS.begin("gaggiapid")) {
//    Serial.println("MDNS responder started");
//  }
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
#ifdef debug
  Serial.print(configureMode);
  Serial.print(",");
  Serial.print(operatingMode);
  Serial.print(",");
  Serial.println(PIDparam);
#endif
  server.handleClient();
  MDNS.update();
  checkButtons();
  updateDisplay();
  runPID();
}
