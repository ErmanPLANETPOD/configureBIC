void setup() {

  initGPIO();

  Serial.begin(115200);
  	  while(!Serial);

      // Initializing SPI and I2C and CAN
      Wire.begin();

      SPI.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, SPI_CS_PIN);

      initCAN();

      forceNormalMode();


      // Initializing PCF8574
      initPCF();
      writePCF(pcf8574, 0);
      // Bypassing the hardware error checking circuit
      enBypass();
      // Reading all data such that we store initial values
      readBICData(BIC0_ADDRESS, pcf8574);
      handleFaultOutputsInit();
      handleBMSActivationPulse();
      readBICData(BIC0_ADDRESS, pcf8574);
      while(!readBMSData(batteryCurrent, batteryVoltage, batteryFailureLevel));
      handleFaultOutputsInit();
      configureChargeDischarge();
      readBICData(BIC0_ADDRESS, pcf8574);
      while(!readBMSData(batteryCurrent, batteryVoltage, batteryFailureLevel));
      handleFaultOutputsInit();
      serialEnableOutput();
      delay(400);
}

void loop() {
if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.equalsIgnoreCase("UPDATE")) {
      handleUpdateProcess();
      serialEnableOutput();
    }
  }
  readBICData(BIC0_ADDRESS, pcf8574);
  while(!readBMSData(batteryCurrent, batteryVoltage, batteryFailureLevel));
  handleFaultOutputsInit();
}
