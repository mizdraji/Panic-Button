#include "pinout.h"

unsigned long previousMillis = 0;         // Variable para almacenar el tiempo anterior
#define ledOnTime   500      // Tiempo de encendido en milisegundos (0.5 segundos)
#define ledOffTime  10000   // Tiempo de apagado en milisegundos (5 segundos)
bool LED_state = LOW;                     // Estado actual del LED

#define delay_apagarLED   15000            //delay para apagar el led despues que se encendio por recibir un mensaje de confirmacion
#define delay_apagarLED1  5000            //delay para apagar el led1 despues de cierto tiempo de que se envio un mensaje
#define delay_apagarLED2  5000            //delay para apagar el led2 despues de cierto tiempo de que se envio un mensaje
#define delay_apagarLED3  5000            //delay para apagar el led3 despues de cierto tiempo de que se envio un mensaje

//estados de los pulsadores
bool statebutton1 = false;         //monitorea el estado del button1 en pin 37
bool statebutton2 = false;         //monitorea el estado del button1 en pin 38
bool statebutton3 = false;         //monitorea el estado del button1 en pin 39
bool bloqueo      = false;         //Variable para monitorear bloqueo de pulsadores.
#define tiempo_bloqueo 10000       //10 segundos tiempo bloqueo de pulsadores

bool statusLED = false;

//variables para adc en GPIO13
#define ADC_powerON_value      2800     //voltage = ADC_powerON_value * (3.3 / 4095.0);     ADC_powerON_value = analogRead(ADC_powerON);
//debe ser mayor a 2.25V para estar conectado a usb
//ADC_powerON_value = 2.25 * 4095 / 3.3 = 2800

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
void apagarLED1();
void apagarLED2();
void apagarLED3();
void trecibido();
void tatendido();
void powerON();
void unlock();


//Tareas:
//Task: Este objeto te permitirá configurar la ejecución de la función, así como su número de ejecuciones y cada cuanto tiempo.


//TASK0 - REVISAR LUEGO, NO SE PUEDE INTRODUCIR TAREAS DENTRO DE OTRAS FUNCIONES.
//Task t_encenderLED(0, TASK_FOREVER, []() { encenderLED(led_recibido); }, &taskManager);  // Ejecuta inmediatamente


//TASK2:  blink de stand by, es un blink de baja frecuencia para indicar que el dispositivo esta funcionando.
Task t2(500, TASK_FOREVER, &blinkstb, &taskManager);

//TASK3: Envia paquete lora.
//Task t3(5000, TASK_FOREVER, &loraSend, &taskManager);

//TASK4: Envia mensaje sms.


Task t5(100, TASK_FOREVER, &buttonTask1, &interrupt);                //TASK5: buttontask1                
Task t6(100, TASK_FOREVER, &buttonTask2, &interrupt);                //TASK6: buttontask2          
Task t7(100, TASK_FOREVER, &buttonTask3, &interrupt);                //TASK7: buttontask3     
Task lock(3000, TASK_FOREVER, &unlock, &interrupt);                  //   

//tareas para apagar leds:
Task t_apagarLED(5000, TASK_FOREVER, &apagarLED, &taskManager);     //TASK apagar todos los leds
Task t_apagarLED1(5000, TASK_FOREVER, &apagarLED1, &taskManager);   //TASK apaga led1
Task t_apagarLED2(5000, TASK_FOREVER, &apagarLED2, &taskManager);   //TASK apaga led2
Task t_apagarLED3(5000, TASK_FOREVER, &apagarLED3, &taskManager);   //TASK apaga led3

Task t_recibido(1000, TASK_FOREVER, &trecibido, &taskManager);
Task t_atendido(1000, TASK_FOREVER, &tatendido, &taskManager);

Task ADCpower(10000, TASK_FOREVER, &powerON, &taskManager);       //se ejecuta cada 10 segundos para verificar si esta cargando con usb


