void updateDisplay() {
  switch (configureMode) {
    case 1: // operate mode
      switch (operatingMode) {
        case 1: // Update target temp
          brewDisplay();
          break;
        case 2: // Steam
          steamDisplay();
      }
    case 2: // PID settings mode
      PIDdisplay();
  }
}

void steamDisplay() {
  display.clearDisplay();  display.setCursor(0,0);  display.setTextSize(2);
  display.println("Temp"); ////////////////////////////////////Temp display
  display.setCursor(indent1,line1);
  if (currentTemp < 100){
    display.print(" ");
  }
  display.print(int(currentTemp));

  display.println("Set -->"); /////////////////////////////////Setpoint display
  display.setCursor(indent1,line2);
  if (targetTemp < 100){
    display.print(" ");
  }
  display.print(int(targetTemp));

  display.setCursor(0, line3);
  display.println("Steam!");
  
  display.setCursor(0,bottom);
  display.setTextSize(1);
  display.print("  DN  | BREW |  UP  ");
  
  display.display();
}

void brewDisplay() {
  display.clearDisplay();  display.setCursor(0,0);  display.setTextSize(2);
  display.println("Temp"); ////////////////////////////////////Temp display
  display.setCursor(indent1,line1);
  if (currentTemp < 100){
    display.print(" ");
  }
  display.print(int(currentTemp));

  display.println("Set -->"); /////////////////////////////////Setpoint display
  display.setCursor(indent1,line2);
  if (targetTemp < 100){
    display.print(" ");
  }
  display.print(int(targetTemp));

  display.println("Duty"); ////////////////////////////////////Duty cycle display
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

  display.setCursor(0,bottom);  display.setTextSize(1);
  display.print("  DN  | STEAM |  UP ");

  display.display();
}

void PIDdisplay() {
  display.clearDisplay();  display.setCursor(0,0);  display.setTextSize(2);
  display.println("P ");  display.setCursor(indent2,line1);  display.print(int(Kp));
  display.println("I ");  display.setCursor(indent2,line2);  display.print(Ki);
  display.println("D ");  display.setCursor(indent2,line3);  display.print(int(Kd));

  display.setTextSize(2); /////////////////////////////////////Indicate current edit
  switch (PIDparam) {
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

  display.setCursor(0,bottom);  display.setTextSize(1);
  display.print("  DN  |  ST  |  UP  ");

  display.display();
}