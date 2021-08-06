#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>
#include <Ultrasonic.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SERVO_PIN PA3
Servo servo;
#define BUTTON PA0

#define ECHO PA5
#define TRIGG PA4

Ultrasonic sonar(TRIGG, ECHO);

enum STATE {
  WAITING,
  ASCENT,
  STANDBY,
  DESCENT,
  SLOW,
  DONE
};

STATE state = STATE::WAITING;
int adjustorValue;
int distance;

void setupDisplay() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.display();
}

void print(String text) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(text);
  display.display();
}

void servoSetup() {
  servo.attach(SERVO_PIN);
}

int lastInterruptMillis = 0;

void changeState() {
  if (millis() - lastInterruptMillis > 200) {
    switch (state) {
      case STATE::WAITING:
        state = STATE::ASCENT;
        break;
      case STATE::ASCENT:
        state = STATE::STANDBY;
        break;
      case STATE::STANDBY:
        state = STATE::DESCENT;
        break;
      case STATE::DESCENT:
        state = STATE::SLOW;
        break;
      case STATE::SLOW:
        state = STATE::DONE;
        break;
      case STATE::DONE:
        state = STATE::WAITING;
        break;
    }
    lastInterruptMillis = millis();
  }
}

void setup()
{
  pinMode(BUTTON, INPUT_PULLDOWN);
  servoSetup();

  // make sure setupDisplay is last because for some reason you gotta call it after all pinModes are done 
  setupDisplay();

  attachInterrupt(digitalPinToInterrupt(BUTTON), changeState, RISING);
}

int i = 0;

bool enteredDescent = false;
int descentTime;
void loop()
{
  switch(state) {
    case STATE::WAITING:
      print(
            "Waiting"
           );
      break;
    case STATE::ASCENT:
      print(
            "ASCENDING"
           );
      servo.write(0);
      break;
    case STATE::STANDBY:
      print(
            "STANDBY"
           );
      servo.write(80);
      break;
    case STATE::DESCENT:
      if (!enteredDescent) {
        descentTime = millis();
        enteredDescent = true;
      }
      if (millis() - descentTime > 2000)  {
        enteredDescent = false;
        changeState();
      }
      servo.write(180);
      print(
            "\n DESCENDING"
           );
      break;
    case STATE::SLOW:
      if (i < 7) {
          if (millis() - descentTime < 100) {
          print("Slow");
          servo.write(90);
        } else if (millis() - descentTime < 200){
          print("GO");
          servo.write(180);
        } else {
          descentTime = millis();
          i++;
        }
      } else {
        if (millis() - descentTime < 2500) {
          servo.write(180);
        } else {
          servo.write(90);
        }
      }
      
      break;
    case STATE::DONE:
      print(
        "DONE"
      );
      break;
  }
}
