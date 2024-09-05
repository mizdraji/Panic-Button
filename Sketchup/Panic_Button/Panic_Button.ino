/*Detalle de versiones:
* V1.2: 
* se cambia config pines por pinout y se crea funcion config_pines.
* Se crea Hardware.h y Hardware.ino
* Se crea funcion pdr para prueba de red pero aun no esta funcional.
*/

//librerias utilizadas
#include <TaskScheduler.h>          //libreria tareas programadas
#include "Task.h"                   //librerias tareas a realizar
#include <lorawan.h>                //libreria lora
#include "LoraTec.h"
#include "ABPcredentials.h"         //librerias lora credenciales
#include "pinout.h"                 //libreria para asignacion de pines.
#include "Hardware.h"
#include "configuracion.h"
#include <SoftwareSerial.h>         //Libreria para definir tx y rx de sim800

//int32_t prevMillis = 0;
//#define interval 1000

SoftwareSerial SIM800L(RX, TX);   //RX y TX de heltec




//Scheduler: Este objeto es el programador encargado de la ejecución de las tareas, el cual se tendrá que ejecutar en cada loop
//Creamos el Scheduler que se encargará de gestionar las tareas
Scheduler taskManager;

TaskHandle_t Task0;     //Task0 para ejecutar en core0

void setup() {                              //setup run in core1
  SIM800L.begin(SERIAL_SIM);
  Serial.begin(SERIAL_SPEED);
  config_pines();

  delay(5000);                              //falta crear variable para initial random time
  if (!lora.init()) {
    Serial.println("RFM95 not detected");
    while (1)
      ;
  }
  Serial.println("RFM95 detected");

  taskManager.init();
  Serial.println("Initialized scheduler");
  //taskManager.addTask(TareaLED);           // Añadimos la tarea al programador de tareas
  //TareaLED.enable();                  // Activamos la tarea
  taskManager.addTask(SendLora);
  SendLora.enable();
  taskManager.addTask(blink2);          //blink cada 10 segundos
  blink2.enable();
  
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
  // if ((millis() - prevMillis > interval) && (nodo.pdr_ok == 0)) {               //entra cada 1 segundo solo si no se establecio la conexion LORA
  // prevMillis = millis();
  // //Serial.println("entramos al if de pdr_function");
  //   pdr_function();
  // }
  taskManager.execute();             // Es necesario ejecutar el runner en cada loop

}


void loop0(void *parameter){                    //loop0 run in core0
  while(1){
  recvStatus = lora.readData(datoEntrante);
  if(recvStatus) {
    Serial.print("====>> ");
    Serial.println(datoEntrante);
  }
  lora.update();                     //actualizacion lora
  vTaskDelay(10);                     //delay para fallas de wtd
  
  }

  //vTaskDelay(10);                     //delay para fallas de wtd
}


