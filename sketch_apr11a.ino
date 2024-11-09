#include <Servo.h>
#include <Keypad.h>
#include <Adafruit_Fingerprint.h>
#include <NewPing.h>
#include <SoftwareSerial.h>


SoftwareSerial mySerial(10, 11);



Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

const int trigPin = 12;
const int echoPin = 13;
#define MAX_DISTANCE 200


Servo myServo;


const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


const String correctPassword = "1234";

void setup() {
  Serial.begin(9600);

  myServo.attach(14);
  myServo.write(0);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("\n\nSmart Door Lock Test");
  finger.begin(57600);
  // delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
  delay(850);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
  }


}

void loop() {
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  distance = (duration * 0.034) / 2;

  Serial.println(distance);
  delay(500);

 

  if (distance < 8) {

    activateSecurity();
  }
}

void activateSecurity() {
  Serial.println("Security activated!");
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '#') {
        break;
      }
      checkPassword(key);
    }

    getFingerprintID();

    
    //    if (finger.getImage()) {
    //      int fingerprintID = finger.image2Tz();
    //      if (fingerprintID == 1 || fingerprintID == 2) {
    //        unlock();
    //        break;
    //      }
    //    }
  }
}



void checkPassword(char key) {
  static String enteredPassword = "";
  if (key == '*') {
    enteredPassword = "";
  } else {
    enteredPassword += key;
    if (enteredPassword.length() == correctPassword.length()) {
      if (enteredPassword == correctPassword) {
        unlock();

      } else {
        Serial.println("Incorrect password");
        enteredPassword = "";
      }
    }
  }
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_NOFINGER:
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      return p;
    case FINGERPRINT_IMAGEFAIL:
      return p;
    default:
      return p;
  }

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken and converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  unlock();
  delay(1500);
  return finger.fingerID;
}


void unlock() {
  Serial.println("Access granted!");
  myServo.write(90);
  delay(2000);
  myServo.write(0);
  delay(1000);
}
