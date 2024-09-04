/*Detalle de versiones:
* V1.1: 
* Se agrega procesamiento lora.update y lora.readData en core0 mientras que el resto se ejecuta en core1.
*/

//librerias utilizadas
#include <TaskScheduler.h>          //libreria tareas programadas
#include "Task.h"                   //librerias tareas a realizar
#include <lorawan.h>                //libreria lora
#include "LoraTec.h"
#include "ABPcredentials.h"         //librerias lora credenciales
#include "config_pines.h"           //libreria para configuraciones de pines
#include "configuracion.h"
#include <SoftwareSerial.h>         //Libreria para definir tx y rx de sim800



SoftwareSerial SIM800L(RX, TX);   //RX y TX de heltec




//Scheduler: Este objeto es el programador encargado de la ejecución de las tareas, el cual se tendrá que ejecutar en cada loop
//Creamos el Scheduler que se encargará de gestionar las tareas
Scheduler taskManager;

TaskHandle_t Task0;     //Task0 para ejecutar en core0

void setup() {                              //setup run in core1
  SIM800L.begin(SERIAL_SIM);
  Serial.begin(SERIAL_SPEED);
  delay(5000);
  if (!lora.init()) {
    Serial.println("RFM95 not detected");
    while (1)
      ;
  }
  Serial.println("RFM95 detected");

  taskManager.init();
  Serial.println("Initialized scheduler");
  taskManager.addTask(TareaLED);           // Añadimos la tarea al programador de tareas
  TareaLED.enable();                  // Activamos la tarea
  taskManager.addTask(SendLora);
  SendLora.enable();
  taskManager.addTask(blink2);
  blink2.enable();

  //configure pines
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  //Se crea una tarea que se ejecutará en la función loop0(), con prioridad 1 y se ejecutará en el core0.
  xTaskCreatePinnedToCore(loop0, "Task0", 10000, NULL, 1, &Task0, 0);  

  //attachInterrupt(digitalPinToInterrupt(button), panic_show, RISING);


  if (initLoraTec()) {
  Serial.println("-->LoraTec OK");
  Serial.print("-->devID: TLV2_DPEC_");
  Serial.println(devID);  //Activacion Manual, devID predefinido
  }
   
}

void loop() {                                           //loop run in core1

  taskManager.execute();             // Es necesario ejecutar el runner en cada loop

}

uint8_t count = 0;

void loop0(void *parameter){                    //loop0 run in core0
  while(1){
  recvStatus = lora.readData(datoEntrante);
  if(recvStatus) {
    Serial.print("====>> ");
    Serial.println(datoEntrante);
  }
  if(count==0){
  Serial.print("loop0: ");Serial.println(xPortGetCoreID());
  count++;
  }
  lora.update();                     //actualizacion lora
  vTaskDelay(10);                     //delay para fallas de wtd
  
  }

  //vTaskDelay(10);                     //delay para fallas de wtd
}


