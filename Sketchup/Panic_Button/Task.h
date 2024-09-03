//Task: Este objeto te permitirá configurar la ejecución de la función, así como su número de ejecuciones y cada cuanto tiempo.


bool statusLED = false;
void led_blink();



//Tareas:
// Creamos la tarea indicando que se ejecute cada 500 milisegundos, para siempre, y llame a la función led_blink
Task TareaLED(1000, TASK_FOREVER, &led_blink, taskManager, true);


//turn led on -> para cada led

//turn led off -> para cada led
