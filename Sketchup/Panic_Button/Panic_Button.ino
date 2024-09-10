/*Detalle de versiones:
* V1.4: 
* Se agregan funciones Serialcom y ReceiveMode en Hardware.ino para enviar/recibir SMS
*/

//librerias utilizadas
#define _TASK_PRIORITY
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

  Serial.println("Initialized scheduler");

  //Configura Scheduler interrupt como alta prioridad
  taskManager.setHighPriorityScheduler(&interrupt);
  taskManager.enableAll(true);  // this will recursively enable the higher priority tasks as well
  
  //Se crea una tarea que se ejecutará en la función loop0(), con prioridad 1 y se ejecutará en el core0.
  xTaskCreatePinnedToCore(loop0, "Task0", 10000, NULL, 1, &Task0, 0);  

  if (initLoraTec()) {
  Serial.println("-->LoraTec OK");
  Serial.print("-->devID: TLV2_DPEC_");
  Serial.println(devID);  //Activacion Manual, devID predefinido
  }

  attachInterrupt(digitalPinToInterrupt(button1), buttonInterrupt1, RISING);            //habilita interrupcion pulsador1 con flanco ascendente
  attachInterrupt(digitalPinToInterrupt(button2), buttonInterrupt2, RISING);            //habilita interrupcion pulsador2 con flanco ascendente
  attachInterrupt(digitalPinToInterrupt(button3), buttonInterrupt3, RISING);            //habilita interrupcion pulsador3 con flanco ascendente
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


