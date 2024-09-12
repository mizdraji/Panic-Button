/*Detalle de versiones:
* V1.5: 
* Se agregan los pinout faltantes de leds.
* Se agrega mensajes recibidos de confirmación.
* Se agregan funciones prender y apagar led para trabajar con tareas programadas.
* Se prende el led_recibido cuando se recibe una respuesta correcta durante 50 segundos.
* Se agrega capacitor de 1000uF 10V entre VCC y GND del modulo GSM para evitar errores de lectura y reseteos del ESP32
por baja tension que genera el modulo SIM800.

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

int32_t prevMillis = 0;
#define interval 15000

SoftwareSerial SIM800L(RX, TX);              //RX y TX de heltec


TaskHandle_t Task0;                         //Task0 para ejecutar en core0

void setup() {                              //setup run in core1
  SIM800L.begin(SERIAL_SIM);
  Serial.begin(SERIAL_SPEED);
  config_pines();
  delay(5000);                              //falta crear variable para initial random time

  //config lora
  if (!lora.init()) {
    Serial.println("RFM95 not detected");
    while (1);
  }
  Serial.println("RFM95 detected");
  if (initLoraTec()) {
    Serial.println("-->LoraTec OK");
    Serial.print("-->devID: TLV2_DPEC_");
    Serial.println(devID);                  //Activacion Manual, devID predefinido
    char uncero[1]={0};
    sendPackage(uncero, 1, no_espera_ACK,  1);
  }

  //config Scheduler
  Serial.println("Initialized scheduler");
  taskManager.setHighPriorityScheduler(&interrupt);          //Configura Scheduler interrupt como alta prioridad
  taskManager.enableAll(true);                               //this will recursively enable the higher priority tasks as well
  t_apagarLED.disable();
  
  //Se crea una tarea que se ejecutará en la función loop0(), con prioridad 1 y se ejecutará en el core0.
  xTaskCreatePinnedToCore(loop0, "Task0", 10000, NULL, 1, &Task0, 0);  

  //configurar modulo GSM como modo SMS
  Serial.println("iniciando .........");
  ReceiveMode();
  Enviar_msj(numero.Remitente1, "Inicializacion completa");                        //provisorio de prueba, comprueba que envia mensaje correctamente al iniciar

  //config interrupt
  attachInterrupt(digitalPinToInterrupt(button1), buttonInterrupt1, RISING);            //habilita interrupcion pulsador1 con flanco ascendente
  attachInterrupt(digitalPinToInterrupt(button2), buttonInterrupt2, RISING);            //habilita interrupcion pulsador2 con flanco ascendente
  attachInterrupt(digitalPinToInterrupt(button3), buttonInterrupt3, RISING);            //habilita interrupcion pulsador3 con flanco ascendente

  prevMillis = millis();
}

void loop() {                                           //loop run in core1
//PLAN B APAGAR LED POR SI NO SALE CON TAREAS
  //   if (millis() - prevMillis > interval) { // && (nodo.pdr_ok == 0)) {               //entra cada 1 segundo solo si no se establecio la conexion LORA
  //   prevMillis = millis();
  //   //if(powerON()) digitalWrite(25, HIGH);
  //   //else  digitalWrite(25, LOW);
  //   digitalWrite(led_recibido, LOW);
  //   Serial.println("apagar led");

  // //  //pdr_function();
  //  }

  while(SIM800L.available()>0) {
    String mensaje_recibido = "";
    mensaje_recibido = SIM800L.readString(); 

    Serial.print(mensaje_recibido);
    if(mensaje_recibido.indexOf(msj.rcv_policia) != -1) {encenderLED(led_recibido);}
  }

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


