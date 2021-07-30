#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

#define CLAW_PIN PB1
#define WEIGHT_ADJUSTOR_PIN PA_3

#define SERVO_PIN PA_4

#define BUTTON PA5

#define PULLEY_MOTOR_PIN PA_8
#define PULLEY_MOTOR_FREQ 100

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Servo claw;

#define ECHO PA5
#define TRIGG PA4

void printToDisplay(String text)
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(text);
  display.display();
}

void setupClaw()
{
  claw.attach(CLAW_PIN);
}

void setupDisplay()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.display();
}

void setupPulley()
{
  pinMode(PULLEY_MOTOR_PIN, OUTPUT);
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
  printToDisplay("AEFWF");
  delay(3000);
}


void sonarSetup() {
  pinMode(TRIGG, OUTPUT);
  pinMode(ECHO, INPUT);
}

long getSonarDistanceInCm() {
  long duration, inches, cm;

  digitalWrite(TRIGG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGG, LOW);
  duration = pulseIn(ECHO, HIGH);
  cm = duration / 29 / 2;

  return cm;
}

// main
void setup()
{
  setupAdjustor();
  setupPulley();
  // setupClaw();
  // make sure setupDisplay is last because for some reason you gotta call it after all pinModes are done 
  setupDisplay();

  // attachInterrupt(digitalPinToInterrupt(BUTTON), changeState, HIGH);
}

void loop()
{
  int adjustorValue = analogRead(WEIGHT_ADJUSTOR_PIN);
  printToDisplay("Adjustor: " + String(adjustorValue));
  printToDisplay("Sonar: " + String(getSonarDistanceInCm()) + " cm");
  adjustPulleyMotor(adjustorValue);
}
