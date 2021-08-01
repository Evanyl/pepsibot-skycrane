#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>
#include <Ultrasonic.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define WEIGHT_ADJUSTOR_PIN PA_3
#define BUTTON PA0

#define PULLEY_MOTOR_PIN PA_8
#define PULLEY_MOTOR_FREQ 100

#define ECHO PA5
#define TRIGG PA4

Ultrasonic sonar(TRIGG, ECHO);

enum STATE {
  WAITING,
  DESCENT,
  SLOW
};

STATE state = STATE::WAITING;
int adjustorValue;
int distance;

void setupPulley()
{
  pinMode(PULLEY_MOTOR_PIN, OUTPUT);
}

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

void setupAdjustor()
{
  pinMode(WEIGHT_ADJUSTOR_PIN, INPUT_ANALOG);
}

/** Adjusts the torque applied by the motor
 * @param value is an integer from 0 - 1023
 */
void adjustPulleyMotor(int value)
{
  pwm_start(PULLEY_MOTOR_PIN, PULLEY_MOTOR_FREQ, value * 4095 / 1023, RESOLUTION_12B_COMPARE_FORMAT);
}

void changeState() {
  switch (state) {
    case STATE::WAITING:
      adjustorValue = adjustorValue / 4;
      adjustPulleyMotor(adjustorValue);
      state = STATE::DESCENT;
      break;
    case STATE::DESCENT:
      adjustorValue = adjustorValue * 4;
      if (adjustorValue > 1023) adjustorValue = 1023;
      adjustPulleyMotor(adjustorValue);
      state = STATE::SLOW;
      break;
  }
  
  delay(200);
}

void setup()
{
  setupAdjustor();
  setupPulley();
  pinMode(BUTTON, INPUT_PULLDOWN);
  // make sure setupDisplay is last because for some reason you gotta call it after all pinModes are done 
  setupDisplay();

  attachInterrupt(digitalPinToInterrupt(BUTTON), changeState, RISING);
}

void loop()
{
  switch(state) {
    case STATE::WAITING:
      adjustorValue = analogRead(WEIGHT_ADJUSTOR_PIN);
      distance = sonar.read();
      print(
            "Adjustor: " + String(adjustorValue) + 
            "\nSonar: " + String(distance) + " cm"
           );
      adjustPulleyMotor(adjustorValue);
      break;
    case STATE::DESCENT:
      print("New Adjustor: " + String(adjustorValue));
      break;
    case STATE::SLOW:
      print("WE out here: " + String(adjustorValue));
      break;
  }
}
