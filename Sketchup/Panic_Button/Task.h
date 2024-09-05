
unsigned long previousMillis = 0;         // Variable para almacenar el tiempo anterior
const unsigned long ledOnTime = 500;      // Tiempo de encendido en milisegundos (0.5 segundos)
const unsigned long ledOffTime = 10000;   // Tiempo de apagado en milisegundos (5 segundos)
bool LED_state = LOW;                     // Estado actual del LED

//button status:
bool Statebutton1 = LOW;        //boton de policia
bool Statebutton2 = LOW;        //boton de bomberos
bool Statebutton3 = LOW;        //boton de ambulancia

bool statusLED = false;
void led_blink();
void blinkstb();

void loraSend();



//Tareas:
//Task: Este objeto te permitirá configurar la ejecución de la función, así como su número de ejecuciones y cada cuanto tiempo.


//TASK1
Task TareaLED(1000, TASK_FOREVER, &led_blink);    


//TASK2:  blink de stand by, es un blink de baja frecuencia para indicar que el dispositivo esta funcionando.
Task blink2(500, TASK_FOREVER, &blinkstb);

//TASK3: Envia paquete lora.
Task SendLora(5000, TASK_FOREVER, &loraSend);



