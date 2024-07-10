#include <Arduino.h>
#include <mcp_can.h>
#include <SPI.h>


const int SPI_CS_PIN = 38;    // CS Pin for MCP2515 on SCU
const int SPI_SCK_PIN = 1;    // SCK Pin for MCP2515 on SCU
const int SPI_MISO_PIN = 6;   // MISO Pin for MCP2515 on SCU
const int SPI_MOSI_PIN = 5;;  // MOSI Pin for MCP2515 on SCU

MCP_CAN CAN(SPI_CS_PIN);

// Functions to initialize the MCP2515
byte mcp2515_readRegister(byte address) {
    byte value;
    digitalWrite(SPI_CS_PIN, LOW); // Select MCP2515
    SPI.transfer(MCP2515_READ);    // Send read command
    SPI.transfer(address);         // Send address
    value = SPI.transfer(0x00);    // Read value
    digitalWrite(SPI_CS_PIN, HIGH); // Deselect MCP2515
    return value;
}

// Function to modify a register
void mcp2515_modifyRegister(byte address, byte mask, byte data) {
    digitalWrite(SPI_CS_PIN, LOW); // Select MCP2515
    SPI.transfer(MCP2515_BIT_MODIFY); // Send bit modify command
    SPI.transfer(address);             // Send address
    SPI.transfer(mask);                // Send mask
    SPI.transfer(data);                // Send data
    digitalWrite(SPI_CS_PIN, HIGH);    // Deselect MCP2515
}

void forceNormalMode() {
    byte mode = mcp2515_readRegister(CANSTAT) & 0xE0;
    if (mode != 0x00) {
        // Serial.println("Forcing MCP2515 to Normal Mode");
        mcp2515_modifyRegister(CANCTRL, 0xE0, 0x00);
        delay(10);
        mode = mcp2515_readRegister(CANSTAT) & 0xE0;
        if (mode == 0x00) {
            // Serial.println("MCP2515 in Normal Mode");
        } else {
            // Serial.print("Failed to enter Normal Mode, current mode: 0x");
            // Serial.println(mode, HEX);
        }
    }
}

void initCAN() {
    // Initialize CAN with Normal mode using library's function
    if (CAN.begin(MCP_ANY, CAN_250KBPS, MCP_20MHZ) == CAN_OK) {
        // Serial.println("CAN BUS Shield init ok!");
    } else {
        // Serial.println("CAN BUS Shield init fail");
        while (1);
    }
}

// Function to configure CANBus communication mode
void configureCANBus(uint8_t address) {
    uint32_t id = 0x0C0300 | address;
    unsigned char data[4] = {0xC2, 0x00, 0x03, 0x00};  // SYSTEM_CONFIG to 0x0003

    byte sndStat = CAN.sendMsgBuf(id, 1, 4, data);
    if (sndStat == CAN_OK) {
        Serial.println("SYSTEM_CONFIG Command Sent to Activate CANBus Communication Mode");
    } else {
        Serial.print("Error Sending SYSTEM_CONFIG Command: ");
        Serial.println(sndStat, HEX);
    }

    delay(500); // Allow time for the BIC to process the command
}

// Function to set bidirectional battery mode
void setBidirectionalBatteryMode(uint8_t address) {
    uint32_t id = 0x0C0300 | address;
    unsigned char data[4] = {0x40, 0x01, 0x01, 0x00};  // BIDIRECTIONAL_CONFIG to 0x0001

    byte sndStat = CAN.sendMsgBuf(id, 1, 4, data);
    if (sndStat == CAN_OK) {
        Serial.println("BIDIRECTIONAL_CONFIG Command Sent to Set Bidirectional Battery Mode");
    } else {
        Serial.print("Error Sending BIDIRECTIONAL_CONFIG Command: ");
        Serial.println(sndStat, HEX);
    }

    delay(500); // Allow time for the BIC to process the command
}

// Main function to activate bidirectional battery mode
void activateBidirectionalMode(uint8_t address) {
    configureCANBus(address);          // Step 1: Set SYSTEM_CONFIG to 0x0003
    setBidirectionalBatteryMode(address); // Step 2: Set BIDIRECTIONAL_CONFIG to 0x0001

    // Step 3: Repower the supply
    Serial.println("Please repower the supply to activate the bidirectional battery mode.");
}

void setup() {
    Serial.begin(115200);
    SPI.begin(SPI.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, SPI_CS_PIN));
    // Initialize CAN bus
    if (CAN.begin(MCP_ANY, CAN_250KBPS, MCP_20MHZ) == CAN_OK) {
        Serial.println("CAN Bus Initialized Successfully!");
    } else {
        Serial.println("CAN Bus Initialization Failed!");
        while (1);
    }
  forceNormalMode();

    // Set the BIC0_ADDRESS (change this to your actual address)
    uint8_t BIC0_ADDRESS = 0x00; // Example address

    // Activate bidirectional battery mode
    activateBidirectionalMode(BIC0_ADDRESS);
}

void loop() {
    // The main loop can be used for other tasks
}
