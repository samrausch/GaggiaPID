#include <Adafruit_MCP9600.h>
#define tc_address (0x67)
Adafruit_MCP9600 mcp;

#include <Bounce2.h>
Bounce modeButton = Bounce();
Bounce upButton = Bounce();
Bounce downButton = Bounce();
const unsigned long intervalMs = 5;

#define NOOP __asm__ __volatile__ ("nop\n\t")

#include <Adafruit_SSD1306.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include <Wire.h> 

const int indent1 = 88;
const int indent2 = 72;
const int bottom = 56;
const int line1 = 0;
const int line2 = 16;
const int line3 = 32;
const int upPin = 3;
const int modePin = 12;
const int downPin = 5;
const int relayPin = 7;
int mode = 1;


#include <PID_v1.h>
double targetTemp;
double currentTemp;
double heatState;
double Kp = 15;
double Ki = 1;
double Kd = 100;
PID myPID(&currentTemp, &heatState, &targetTemp, Kp, Ki, Kd, DIRECT);
int WindowSize = 100;
unsigned long windowStartTime;

void setup() {
  pinMode(relayPin, OUTPUT);
  pinMode(groundPin, OUTPUT);
  digitalWrite(groundPin, HIGH);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println("Gaggia PID");
  display.println();
  display.println("v1.0");
  if (! mcp.begin(tc_address)) {
    display.clearDisplay();
    display.println("TC ERROR");
    display.display();
    while (1);
  }
  mcp.setADCresolution(MCP9600_ADCRESOLUTION_18);
  mcp.setThermocoupleType(MCP9600_TYPE_K);
  mcp.setFilterCoefficient(3);
  mcp.enable(true);
  
  display.display();
  
  delay(1000);
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.println("Temp");
  display.println("Set -->");
  display.println("Heat");
  display.setCursor(0,bottom);
  display.setTextSize(1);
  display.print("  DN  |  CH  |  UP  ");
  display.display();
  pinMode(modePin, INPUT_PULLUP);
  pinMode(upPin, INPUT_PULLUP);
  pinMode(downPin, INPUT_PULLUP);
  modeButton.attach(modePin);
  modeButton.interval(intervalMs);
  upButton.attach(upPin);
  upButton.interval(intervalMs);
  downButton.attach(downPin);
  downButton.interval(intervalMs);
  windowStartTime = millis();
  targetTemp = 85;
  myPID.SetOutputLimits(0, WindowSize);
  myPID.SetMode(AUTOMATIC);
}

void resetDisplay() {
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.println("Temp");
  display.println("Set -->");
  display.println("Heat");
  display.setCursor(indent1,line1);
  if (currentTemp < 100){
    display.print(" ");
  }
  display.print(int(currentTemp));
  display.setCursor(indent1,line2);
  if (targetTemp < 100){
    display.print(" ");
  }
  display.print(int(targetTemp));
  display.setCursor((indent1-12),line3);
  int heatDuty = map(heatState, 0, WindowSize, 0, 100);
  if (heatDuty < 1000){
    display.print(" ");
  }
  if (heatDuty < 100){
    display.print(" ");
  }
  if (heatDuty < 10){
    display.print(" ");
  }
  display.print(int(heatDuty));
  display.setCursor(0,bottom);
  display.setTextSize(1);
  display.print("  DN  |  CH  |  UP  ");
  display.display();
}

void PIDdisplay() {
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.println("P ");
  display.println("I ");
  display.println("D ");
  display.setCursor(indent2,line1);
  display.print(int(Kp));
  display.setCursor(indent2,line2);
  display.print(Ki);
  display.setCursor(indent2,line3);
  display.print(int(Kd));
  display.setCursor(0,bottom);
  display.setTextSize(1);
  display.print("  DN  |  CH  |  UP  ");
  display.setTextSize(2);
  switch (mode) {
    case 1:
      NOOP;
      break;
    case 2:
      display.setCursor(0,line1);
      display.print("  -->");
      break;
    case 3:
      display.setCursor(0,line2);
      display.print("  -->");
      break;
    case 4:
      display.setCursor(0,line3);
      display.print("  -->");
  }
  display.display();
}

void checkButtons() {
  modeButton.update();
  const byte modeButtonState = modeButton.read();
  if (modeButtonState == LOW && modeButton.fallingEdge()) {
    switch (mode) {
      case 1: // Switch to Kp setting
        mode = 2;
        PIDdisplay();
        break;
      case 2: // Switch to Ki setting
        mode = 3;
        PIDdisplay();
        break;
      case 3: // Switch to Kd setting
        mode = 4;
        PIDdisplay();
        break;
      case 4: // Switch to temp setting
        mode = 1;
        resetDisplay();
    }
  }
  upButton.update();
  const byte upButtonState = upButton.read();
  if (upButtonState == LOW && upButton.fallingEdge()) {
    switch(mode) {
      case 1: // Adjust temp settings
        targetTemp++;
        break;
      case 2: // Adjust PID settings
        Kp++;
        break;
      case 3: // Adjust Ki setting
        Ki = Ki + 0.1;
        break;
      case 4: // Adjust Kd setting
        Kd = Kd + 10;       
    }
  }
  downButton.update();
  const byte downButtonState = downButton.read();
  if (downButtonState == LOW && downButton.fallingEdge()) {
    switch(mode) {
      case 1: // Adjust temp settings
        targetTemp--;
        break;
      case 2: // Adjust Kp settings
        Kp--;
        break;
      case 3: // Adjust Ki setting
        Ki = Ki - 0.1;
        break;
      case 4: // Adjust Kd setting
        Kd = Kd - 10;
    }       
  }
}

void updateDisplay() {
  switch (mode) {
    case 1: // Update target temp
      resetDisplay();
      break;
    case 2: // Update Kp
      PIDdisplay();
      break;
    case 3: // Update Ki
      PIDdisplay();
      break;
    case 4: // Update Kd
      PIDdisplay();
  }
}

void loop() {
  checkButtons();
  currentTemp = mcp.readThermocouple();
  myPID.Compute();
  if (millis() - windowStartTime > WindowSize)
  { //time to shift the Relay Window
    windowStartTime += WindowSize;
  }
  if (heatState < millis() - windowStartTime) digitalWrite(relayPin, LOW);
  else digitalWrite(relayPin, HIGH);
  updateDisplay();
}
