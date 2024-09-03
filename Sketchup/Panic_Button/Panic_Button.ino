//Detalle de versiones:
/*/V1.0: 
* Envia paquetes lora
* Se crean tareas de prueba


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

String Numero_Remitente = "3794003039";


//Scheduler: Este objeto es el programador encargado de la ejecución de las tareas, el cual se tendrá que ejecutar en cada loop
//Creamos el Scheduler que se encargará de gestionar las tareas
Scheduler taskManager;



void setup() {
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
  //taskManager.addTask(TareaLED);           // Añadimos la tarea al programador de tareas
  //TareaLED.enable();                  // Activamos la tarea
  taskManager.addTask(SendLora);
  SendLora.enable();
  taskManager.addTask(blink2);
  blink2.enable();

  //configure pines
  //pinMode(button1, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  

  //attachInterrupt(digitalPinToInterrupt(button), panic_show, RISING);


  if (initLoraTec()) {
  Serial.println("-->LoraTec OK");
  Serial.print("-->devID: TLV2_DPEC_");
  Serial.println(devID);  //Activacion Manual, devID predefinido
  }
   
  

}

void loop() {

  recvStatus = lora.readData(datoEntrante);
  if(recvStatus) {
    Serial.print("====>> ");
    Serial.println(datoEntrante);
  }


  taskManager.execute();             // Es necesario ejecutar el runner en cada loop
  lora.update();                     //actualizacion lora

}


