void checkButtons() {
  modeButton.update();
  if (modeButton.pressed() && modeButton.currentDuration() > 1000) {
    switch (configureMode) {
      case 1:  // operating
        configureMode = 2;
      case 2:  // configuring
        configureMode = 1;
    }
  } else if (modeButton.pressed()) {
    switch (configureMode) {
      case 1:  // operating mode
        switch (operatingMode) {
          case 1:  // Switch to Steam
            operatingMode = 2;
            brewTemp = targetTemp;
            targetTemp = steamTemp;
            steamDisplay();
            break;
          case 2:  // Switch to temp setting
            operatingMode = 1;
            steamTemp = targetTemp;
            targetTemp = brewTemp;
            brewDisplay();
            break;
        }
      case 2:  // PID settings mode
        switch (PIDparam) {
          case 1:
            PIDparam = 2;
          case 2:
            PIDparam = 3;
          case 3:
            PIDparam = 1;
        }
    }
  }
  upButton.update();
  if (upButton.pressed()) {
    switch(configureMode) {
      case 1: // operating
            switch (operatingMode) {
              case 1:  // Adjust brew temp
                targetTemp++;
                break;
              case 2:  // Adjust steam temp
                targetTemp++;
                break;
                break;
            }
          case 2:  // configuring
            switch (PIDparam) {
              case 1:  // Adjust Kp
                Kp = Kp + 1;
                break;
              case 2:  // Adjust Ki
                Ki = Ki + 0.1;
                break;
              case 3:  // Adjust Kd
                Kd = Kd + 1;
            }
        }
    }
    downButton.update();
    if (downButton.pressed()) {
      switch (configureMode) {
        case 1:
          switch (operatingMode) {
            case 1:  // Adjust brew temp
              targetTemp--;
              break;
            case 2:  // Adjust steam temp
              targetTemp--;
          }
          break;
        case 2:
          switch (PIDparam) {
            case 1:  // Adjust Kp
              Kp = Kp - 1;
              break;
            case 2:  // Adjust Ki
              Ki = Ki - 0.1;
              break;
            case 3:  // Adjust Kd
              Kd = Kd - 1;
          }
      }
    }
  }