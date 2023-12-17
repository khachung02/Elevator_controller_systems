#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL6cuQKuoh2"
#define BLYNK_TEMPLATE_NAME "Elevator"
#define BLYNK_AUTH_TOKEN "pn_P-1Ya0sqn-U6xPYfMhi4PuiY9XESM"

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Vip";
char pass[] ="khachung02";

char auth[] = "pn_P-1Ya0sqn-U6xPYfMhi4PuiY9XESM";

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
#include <ESP32Servo.h>

// Initialize the LiquidCrystal_I2C object to use the LCD screen
LiquidCrystal_I2C lcd(0x3F, 16, 2);
const int stepsPerRevolution = 2048;

#define button1 27
#define button2 14
#define button3 12
#define button4 13

#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 17

#define servoPin 33

#define btnFloor2 15
#define btnFloor3 23
#define btnFloor4 32

#define led1 25
#define led2 26
#define led3 2
#define led4 4

int buttonStates[4] = {0};
int floorButtonStates[3] = {0};
int currentFloor = 1;
int pos;
int blynkStates[4] = {0};
BlynkTimer timer;
//Blynk_WRITE functions to process data from controls in a Blynk application
BLYNK_WRITE(V1) {
  blynkStates[0] = param.asInt();
}
BLYNK_WRITE(V2) {
  blynkStates[1] = param.asInt();
}
BLYNK_WRITE(V3) {
  blynkStates[2] = param.asInt();
}
BLYNK_WRITE(V4) {
  blynkStates[3] = param.asInt();
}
// Initialize Stepper and Servo objects
Stepper myStepper = Stepper(stepsPerRevolution, IN1, IN3, IN2, IN4);
Servo myServo;

  int buttons[] = {button1, button2, button3, button4};
  int floorButtons[] = {btnFloor2, btnFloor3, btnFloor4};
  int leds[] = {led1, led2, led3, led4};
void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, pass);
  Blynk.begin(auth, ssid, pass);
//// Initialize the initial state for the controls in the Blynk application
  Blynk.virtualWrite(V1, 0);
  Blynk.virtualWrite(V2, 0);
  Blynk.virtualWrite(V3, 0);
  Blynk.virtualWrite(V4, 0);  

  myStepper.setSpeed(15);
  myServo.attach(servoPin);

  for (int i = 0; i < 4; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
    pinMode(leds[i], OUTPUT);
  }

  for (int i = 0; i < 3; i++) {
    pinMode(floorButtons[i], INPUT_PULLUP);
  }
  //Initialize the LCD 
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Elevator Project");
}

void loop() {
  
  for (int i = 0; i < 4; i++) {
    buttonStates[i] = digitalRead(buttons[i]);
  }
for (int i = 0; i < 3; i++) {
      floorButtonStates[i] = !digitalRead(floorButtons[i]);
  }
  ////////////////////////////////////////////////
    if (!buttonStates[3] || blynkStates[3]) {
    goToFloor(4, led4);
  } else if (!buttonStates[2] || blynkStates[2]) {
    goToFloor(3, led3);
  } else if (!buttonStates[1] || blynkStates[1]) {
    goToFloor(2, led2);
  } else if (!buttonStates[0] || blynkStates[0] ) {
    returnFloor1();
  }

  for (int i = 0; i < 3; i++) {
    if (floorButtonStates[i]) {
      // Nếu đang ở tầng đó thì về tầng 1
      if (currentFloor == i + 2) {
        returnFloor1();
      } else {
        goToFloor(i + 2, leds[i+1]);
      }
    }
  }
  Blynk.run();
  timer.run();
}
// Fuction go to Floor
void goToFloor(int targetFloor, int ledPin) {
  closeDoor();
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
  if(currentFloor < targetFloor){
    while(currentFloor < targetFloor){
      upStair();
      currentFloor++;
      display(currentFloor);
    }
    currentFloor = targetFloor;
  }
  else if(currentFloor > targetFloor){
    while(currentFloor > targetFloor){
      downStair();
      currentFloor--;
      display(currentFloor);
    }
    currentFloor = targetFloor;
  }
  openDoor();
  digitalWrite(ledPin, HIGH);
}
////////
void returnFloor1() {
  goToFloor(1, led1);
}

void upStair() {
  myStepper.step(4300);
  // delay(100);
}

void downStair() {
  myStepper.step(-4300);
  // delay(100);
}

void closeDoor() {
  for (pos = 90; pos < 180; pos++) {
    myServo.write(pos);
    delay(5);
  }
}
void openDoor() {
  for (pos = 180; pos > 90; pos--) {
    myServo.write(pos);
    delay(5);
  }
}
void display(int currentFloor){
    lcd.setCursor(0, 1);
    lcd.print("At the ");
    lcd.print(currentFloor);  
    lcd.print(" Floor");
}