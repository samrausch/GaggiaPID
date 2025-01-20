void runPID() {
  currentTemp = tc.readThermocouple();

  if (millis() - calcStartTime > calcSize) {
    myPID.Compute();
    Serial.println(currentTemp);
    calcStartTime += calcSize;
  }

  if (millis() - windowStartTime > WindowSize) {
    windowStartTime += WindowSize;
  }

  if (heatState < millis() - windowStartTime) {
    digitalWrite(relayPin, LOW);
  } else {
    digitalWrite(relayPin, HIGH);
  }
}

void updatePID() {
  myPID.SetTunings(Kp, Ki, Kd);
}