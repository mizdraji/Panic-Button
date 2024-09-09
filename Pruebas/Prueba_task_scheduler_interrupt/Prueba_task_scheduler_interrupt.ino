#define _TASK_PRIORITY
#include <TaskScheduler.h>


#define buttonPin  37 // Pin donde está conectado el pulsador
#define LUZ 25
volatile bool buttonPressed = false;
bool statusLED = false;

Scheduler interrupt;
Scheduler led;

//Callback methods prototypes
void buttonTask();
void led_blink();


//Tasks1
Task t1(100, TASK_FOREVER, &buttonTask, &interrupt); // Ejecutar la tarea cada 100 ms
//TASK2
Task t2(1000, TASK_FOREVER, &led_blink, &led); 

void buttonTask() {
  if (buttonPressed) {
    // Realiza la tarea que deseas ejecutar después de la interrupción por pulsador
    Serial.println("Tarea ejecutada después de interrupción por pulsador");
    
    // Reinicia el estado del botón
    buttonPressed = false;
  }
}

void led_blink() {
  
  statusLED = !statusLED;
  digitalWrite(LUZ, statusLED);
}

void buttonInterrupt() {
  buttonPressed = true;
}


void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(LUZ, OUTPUT);
  
  // Configura la tarea que se activará después de la interrupción por pulsador
  led.setHighPriorityScheduler(&interrupt);
  led.enableAll(true);  // this will recursively enable the higher priority tasks as well
  
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonInterrupt, RISING);
}

void loop() {
  led.execute();
}