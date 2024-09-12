#include "pinout.h"

unsigned long previousMillis = 0;         // Variable para almacenar el tiempo anterior
const unsigned long ledOnTime = 500;      // Tiempo de encendido en milisegundos (0.5 segundos)
const unsigned long ledOffTime = 10000;   // Tiempo de apagado en milisegundos (5 segundos)
bool LED_state = LOW;                     // Estado actual del LED

//button status:
bool Statebutton1 = LOW;        //boton de policia
bool Statebutton2 = LOW;        //boton de bomberos
bool Statebutton3 = LOW;        //boton de ambulancia

bool statusLED = false;

//Scheduler: Este objeto es el programador encargado de la ejecución de las tareas, el cual se tendrá que ejecutar en cada loop
//Creamos el Scheduler que se encargará de gestionar las tareas
Scheduler taskManager;
Scheduler interrupt;

//Callback methods prototypes
void led_blink();
void blinkstb();
void loraSend();
void buttonTask1();       //button1 pin 37 policia
void buttonTask2();       //button2 pin 38 bomberos
void buttonTask3();       //button3 pin 39 ambulancia
void encenderLED(uint8_t LED); 
void apagarLED();


//Tareas:
//Task: Este objeto te permitirá configurar la ejecución de la función, así como su número de ejecuciones y cada cuanto tiempo.


//TASK0
//Task t_encenderLED(0, TASK_ONCE, []() { encenderLED(LED); }, &taskManager);  // Ejecuta inmediatamente

//TASK1
Task t_apagarLED(5000, TASK_ONCE, &apagarLED, &taskManager);   // Se ejecuta una vez después de 5 segundos   


//TASK2:  blink de stand by, es un blink de baja frecuencia para indicar que el dispositivo esta funcionando.
Task t2(500, TASK_FOREVER, &blinkstb, &taskManager);

//TASK3: Envia paquete lora.
//Task t3(5000, TASK_FOREVER, &loraSend, &taskManager);

//TASK4: Envia mensaje sms.

//TASK5: buttontask1
Task t5(100, TASK_FOREVER, &buttonTask1, &interrupt);                  // Ejecutar la tarea cada 100 ms
//TASK6: buttontask2
Task t6(100, TASK_FOREVER, &buttonTask2, &interrupt);                  // Ejecutar la tarea cada 100 ms
//TASK7: buttontask3
Task t7(100, TASK_FOREVER, &buttonTask3, &interrupt);                  // Ejecutar la tarea cada 100 ms




