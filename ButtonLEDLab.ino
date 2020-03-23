/*
 * Teresa Barker & Alex Hirsh
 * IST 402 - 001
 * 
  Button LED

  This example creates a BLE peripheral with service that contains a
  characteristic to control an LED and another characteristic that
  represents the state of the button.

  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.
  - Button connected to pin 4

  You can use a generic BLE central app, like LightBlue (iOS and Android) or
  nRF Connect (Android), to interact with the services and characteristics
  created in this sketch.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>
#include <Arduino_LSM6DS3.h>

const int ledPin = LED_BUILTIN; // set ledPin to on-board LED
const int buttonPin = 4; // set buttonPin to digital pin 4

BLEService ledService("19B10010-E8F2-537E-4F6C-D104768A1214"); // create service

// create switch characteristic and allow remote device to read and write
BLEByteCharacteristic ledCharacteristic("19B10011-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
// create BLEFloat characteristic for accelCharacteristic
BLEFloatCharacteristic accelCharacteristic("19B10012-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  pinMode(ledPin, OUTPUT); // use the LED as an output

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  // begin accelerometer initilization
    if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }

  // set the local name peripheral advertises
  BLE.setLocalName("BLE ACCEL LED");
  // set the UUID for the service this peripheral advertises:
  BLE.setAdvertisedService(ledService);

  // add the characteristics to the service
  ledService.addCharacteristic(ledCharacteristic);
  ledService.addCharacteristic(accelCharacteristic);

  // add the service
  BLE.addService(ledService);

  ledCharacteristic.writeValue(0);
  accelCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  // z accelerometer reading
  float x, y, z;
  // reading acceleromoter
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
    // printing out accel reading
    Serial.print('\t');
    Serial.print("z-axis: ");
    Serial.println(z);
  }
        
  // poll for BLE events
  BLE.poll();
  
  // wait for a BLE central
  BLEDevice central = BLE.central();

  // if a central is connected to the peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's BT address:
    Serial.println(central.address());
    // turn on the LED to indicate the connection:
    // digitalWrite(LED_BUILTIN, HIGH);
    } while (central.connected()) {
        BLE.poll(); 
        if (ledCharacteristic.written()) {
        // update LED, either central has written to characteristic
        if (ledCharacteristic.value()) {
          Serial.println("LED on");
          digitalWrite(ledPin, HIGH);
        } else {
          Serial.println("LED off");
          digitalWrite(ledPin, LOW);
        }
    }
    } // when the central disconnects, turn off the LED:
      // digitalWrite(LED_BUILTIN, LOW);
      // Serial.print("Disconnected from central: ");
      // Serial.println(central.address());
} 
