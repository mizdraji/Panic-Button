/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
*/
#define led1  15
#define led2  2
#define led3  4
#define led_powerON 23
#define led_recibido 12  
#define led_atendido 22
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led_powerON, OUTPUT);
  pinMode(led_recibido, OUTPUT);
  pinMode(led_atendido, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(led1, HIGH);  // turn the LED on (HIGH is the voltage level)
  digitalWrite(led2, HIGH);  // turn the LED on (HIGH is the voltage level)
  digitalWrite(led3, HIGH);  // turn the LED on (HIGH is the voltage level)
  digitalWrite(led_powerON, HIGH);  // turn the LED on (HIGH is the voltage level)
  digitalWrite(led_recibido, HIGH);  // turn the LED on (HIGH is the voltage level)
  digitalWrite(led_atendido, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(led1, LOW);   // turn the LED off by making the voltage LOW
  digitalWrite(led2, LOW);   // turn the LED off by making the voltage LOW
  digitalWrite(led3, LOW);   // turn the LED off by making the voltage LOW
  digitalWrite(led_powerON, LOW);   // turn the LED off by making the voltage LOW
  digitalWrite(led_recibido, LOW);   // turn the LED off by making the voltage LOW
  digitalWrite(led_atendido, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);                      // wait for a second
}
