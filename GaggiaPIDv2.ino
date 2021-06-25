#include <Adafruit_MCP9600.h>
#define tc_address (0x67)
Adafruit_MCP9600 mcp;

#include <Bounce2.h>
Bounce modeButton = Bounce();
Bounce upButton = Bounce();
Bounce downButton = Bounce();
const unsigned long intervalMs = 5;

#define NOOP __asm__ __volatile__ ("nop\n\t")

// if you're using a 128x64 i2c OLED, this is all you need
// if you're using something else, feel free to substitute your stuff here
#include <Adafruit_SSD1306.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include <Wire.h> 
// This stuff is just for text formatting so you can control the indents and line spacing
// if you're using a 128x64 OLED screen, you can keep all these values
// otherwise you'll want to change them depending on your display
const int indent1 = 88;
const int indent2 = 72;
const int bottom = 56;
const int line1 = 0;
const int line2 = 16;
const int line3 = 32;
const int upPin = 3;
const int modePin = 12;
const int downPin = 5;
const int relayPin = 9;
const int runPin = 7;
const int progPin = 8;
int brewTemp = 93;      // set the default brew temp here
int steamTemp = 155;    // set the default steam temp here
int runMode = 1;
int brewMode = 1;
int firstRun = 1;

#include <PID_v1.h>
double targetTemp;
double currentTemp;
double heatState;
double Kp = 10;
double Ki = 50;
double Kd = 30;
PID myPID(&currentTemp, &heatState, &targetTemp, Kp, Ki, Kd, DIRECT);
int WindowSize = 100;
unsigned long windowStartTime;

void setup() {
  pinMode(relayPin, OUTPUT);
  pinMode(runPin, INPUT_PULLUP);
  pinMode(progPin, INPUT_PULLUP);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println("Gaggia PID");
  display.println("v2.0");
  display.println("Now with STEAM!");
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
  display.print("  DN  |  ST  |  UP  ");
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
  targetTemp = brewTemp;
  myPID.SetOutputLimits(0, WindowSize);
  myPID.SetMode(AUTOMATIC);
}

void brewDisplay() {
  // this function updates the display when in brew/steam mode
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.println("Temp");
  if (brewMode == 1) {
    display.println("Brew  >");
  }
  if (brewMode == 2) {
    display.println("Steam >");
  }
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
  display.print("  DN  |  ST  |  UP  ");
  display.display();
}

void PIDdisplay() {
  // this function updates the display when it's in programming mode
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.println("P ");
  display.println("I ");
  display.println("D ");
  display.setCursor(indent2,line1);
  display.print(int(Kp));
  display.setCursor(indent2,line2);
  display.print(int(Ki));
  display.setCursor(indent2,line3);
  display.print(int(Kd));
  display.setCursor(0,bottom);
  display.setTextSize(1);
  display.print("  DN  |  CH  |  UP  ");
  display.setTextSize(2);
  switch (runMode) {
    case 1:
      display.setCursor(0,line1);
      display.print("  -->");
      break;
    case 2:
      display.setCursor(0,line2);
      display.print("  -->");
      break;
    case 3:
      display.setCursor(0,line3);
      display.print("  -->");
  }
  display.display();
}

void checkButtons() {
  // there's a lot going on in here
  // first we check the buttons to see if they've been pressed
  int modeSelect = 1;
  byte modeButtonState = modeButton.read();
  byte upButtonState = upButton.read();
  byte downButtonState = downButton.read();
  int amIrunning = digitalRead(runPin);
  // this is the swtich that goes between run and programming, connected to the runPin
  if (amIrunning == 0) {
    modeSelect = 1; //brewing
    brewDisplay();
  } else {
    modeSelect = 2; //programming
    PIDdisplay();
  }
  switch (modeSelect) {
    case 1: // we're brewing
      modeButton.update();
      modeButtonState = modeButton.read();
      // this section changes between steam and brew based on the brewMode variable, which is toggled by the mode button
      if (modeButtonState == LOW && modeButton.fallingEdge()) {
        switch (brewMode) {
          case 1: // Switch to steam
            brewMode = 2;
            brewTemp = targetTemp;
            targetTemp = steamTemp;
            //brewDisplay();
            break;
          case 2: // Switch to brew
            brewMode = 1;
            steamTemp = targetTemp;
            targetTemp = brewTemp;
            //brewDisplay();
        }
      }
      upButton.update();
      upButtonState = upButton.read();
      // here we're raising the temp of brew or steam
      if (upButtonState == LOW && upButton.fallingEdge()) {
        switch(brewMode) {
          case 1: // Adjust brew temp settings
            targetTemp++;
            //brewDisplay();
            break;
          case 2: // Adjust steam temp settings
            targetTemp++;
            //brewDisplay();    
        }
      }
      downButton.update();
      downButtonState = downButton.read();
      // here we're lowering the temp of brew or steam
      if (downButtonState == LOW && downButton.fallingEdge()) {
        switch(brewMode) {
          case 1: // Adjust brew temp settings
            targetTemp--;
            //brewDisplay();
            break;
          case 2: // Adjust steam temp settings
            targetTemp--;
            //brewDisplay();
        }
      }
      break;
    case 2: // we're programming
      modeButton.update();
      modeButtonState = modeButton.read();
      // since we're in programming mode, the buttons now control the PID variables
      if (modeButtonState == LOW && modeButton.fallingEdge()) {
        switch (runMode) {
          case 1: // Switch to Ki setting
            runMode = 2;
            //PIDdisplay();
            break;
          case 2: // Switch to Kd setting
            runMode = 3;
            //PIDdisplay();
            break;
          case 3: // Switch to Kp setting
            runMode = 1;
            //PIDdisplay();
        }
      }
      upButton.update();
      upButtonState = upButton.read();
      if (upButtonState == LOW && upButton.fallingEdge()) {
        switch(runMode) {
          case 1: // Adjust Kp settings
            Kp++;
            myPID.SetTunings(Kp, Ki, Kd);
            //PIDdisplay();
            break;
          case 2: // Adjust Ki setting
            Ki++;
            myPID.SetTunings(Kp, Ki, Kd);
            //PIDdisplay();
            break;
          case 3: // Adjust Kd setting
            Kd++;
            myPID.SetTunings(Kp, Ki, Kd);
            //PIDdisplay();       
        }
      }
      downButton.update();
      downButtonState = downButton.read();
      if (downButtonState == LOW && downButton.fallingEdge()) {
        switch(runMode) {
          case 1: // Adjust Kp settings
            Kp--;
            myPID.SetTunings(Kp, Ki, Kd);
            //PIDdisplay();
            break;
          case 2: // Adjust Ki setting
            Ki--;
            myPID.SetTunings(Kp, Ki, Kd);
            //PIDdisplay();
            break;
          case 3: // Adjust Kd setting
            Kd--;
            myPID.SetTunings(Kp, Ki, Kd);
            //PIDdisplay();
        }      
      }
  }
}

void loop() {
  checkButtons();   // see in-line documentation in the function above
  if (millis() - windowStartTime > WindowSize)
    // this is where we periodically read the TC, update the PID and then reset the timer for the next cycle
  { 
    currentTemp = mcp.readThermocouple();
    analogWrite(relayPin, map(heatState, 0, 100, 0, 1023));
    myPID.Compute();
    windowStartTime += WindowSize;
    if (firstRun) {
      delay(1000);
      brewDisplay();
      firstRun = 0;
    }
  }
}
