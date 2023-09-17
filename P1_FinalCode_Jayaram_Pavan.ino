#include <ArduinoBLE.h>

// Define the LED service and characteristics
BLEService ledService("c832968e-84f8-4688-bcf9-f2c1bcc182c2");
BLEByteCharacteristic redCharacteristic("199b1aaf-311e-4e98-888f-e8dd7bfe8017", BLERead | BLEWrite);
BLEByteCharacteristic greenCharacteristic("642c36aa-3963-44d4-82ac-1965ceadcac0", BLERead | BLEWrite);
BLEByteCharacteristic blueCharacteristic("4d423166-c108-49af-a324-94721ea0b813", BLERead | BLEWrite);

// Define pins for RGB LED
const int RED_PIN = 22;
const int GREEN_PIN = 23;
const int BLUE_PIN = 24;

// Enum to represent the LED state
enum State {
  Dark,
  Red,
  Blue,
  Green
};

State currentState = Dark; // Initialize the LED state as Dark
unsigned long stateChangeTime = 0;

void setup() {
  Serial.begin(115200);
  while(!Serial);

  // Configure LED pins as outputs
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  if(!BLE.begin())
  {
    Serial.println("Starting BLE Failed!");
    while(1);
  }

  // Set BLE device name and advertised service
  BLE.setLocalName("Nano33BLE");
  BLE.setAdvertisedService(ledService);

  // Add characteristics to the service
  ledService.addCharacteristic(redCharacteristic);
  ledService.addCharacteristic(greenCharacteristic);
  ledService.addCharacteristic(blueCharacteristic);

  // Add the service to BLE
  BLE.addService(ledService);

  // Set initial values for the characteristics
  redCharacteristic.writeValue(0);
  greenCharacteristic.writeValue(0);
  blueCharacteristic.writeValue(0);

  // Start advertising the BLE service
  BLE.advertise();

  Serial.println("BLE LED Peripheral");
}

void loop()
{
  BLEDevice central = BLE.central();

  if(central)
  {
    Serial.print("Connected to central: "); Serial.println(central.address());

    while(central.connected())
    {
      // If the red charactistic value is non zero then switchPressed() will get executed 
      if(redCharacteristic.written())
      {
        int val = redCharacteristic.value();
        Serial.println(val);
        if(val > 0)
        {
          switchPressed();
        }
        redCharacteristic.writeValue(0);
      }

      // If the green characteristic value is non zero then switchPressed() will get executed again
      if(greenCharacteristic.written())
      {
        int val = greenCharacteristic.value();
        Serial.println(val);
        if(val > 0)
        {
          switchPressed();
        }
        greenCharacteristic.writeValue(0);
      }

      // If the blue characteristic value is non zero then switchPressed() will get executed again 
      if(blueCharacteristic.written())
      {
        int val = blueCharacteristic.value();
        Serial.println(val);
        if(val > 0)
        {
          switchPressed();
        }
        blueCharacteristic.writeValue(0);
      }

      // Handle LED state transitions
      switch (currentState) {
        case Dark:
          // Set LED to dark color
          digitalWrite(RED_PIN, HIGH);
          digitalWrite(GREEN_PIN, HIGH);
          digitalWrite(BLUE_PIN, HIGH);
          break;

        case Red:
          // Set LED to red color
          digitalWrite(RED_PIN, LOW);
          digitalWrite(GREEN_PIN, HIGH);
          digitalWrite(BLUE_PIN, HIGH);

          if (millis() - stateChangeTime >= 5000) {
            currentState = Dark;
            stateChangeTime = millis();
          }
          break;

        case Blue:
          // Set LED to blue color
          digitalWrite(RED_PIN, HIGH);
          digitalWrite(GREEN_PIN, HIGH);
          digitalWrite(BLUE_PIN, LOW);

          if (millis() - stateChangeTime >= 4000) {
            currentState = Red;
            stateChangeTime = millis();
          }
          break;

        case Green:
          // Set LED to green color
          digitalWrite(RED_PIN, HIGH);
          digitalWrite(GREEN_PIN, LOW);
          digitalWrite(BLUE_PIN, HIGH);

          if (millis() - stateChangeTime >= 3000) {
            currentState = Blue;
            stateChangeTime = millis();
          }
          break;

        default:
          // Handle unexpected state, perhaps return to Dark
          currentState = Dark;
          break;
      }
    }

    Serial.print("Disconnected from Central: ");
    Serial.println(central.address());
  }
}

// Function to simulate a switch press and change LED color
void switchPressed() {
  switch (currentState) {
    case Dark:
      currentState = Red;
      stateChangeTime = millis();
      break;

    case Red:
      currentState = Blue;
      stateChangeTime = millis();
      break;

    case Blue:
      currentState = Green;
      stateChangeTime = millis();
      break;

    case Green:
      currentState = Dark;
      stateChangeTime = millis();
      break;

    default:
      // Handle unexpected state, perhaps return to Dark
      currentState = Dark;
      break;
  }
}
