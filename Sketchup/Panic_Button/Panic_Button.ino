/*Detalle de versiones:
* V1.8.4: 
* Se crean dos funciones extraer_numero en Hardware.ino, para obtener el número de idempotencia recibido. Una función es para mensajes lora del tipo char[] y la otra para SMS.
* Se agrega if (SIM800L.available()) antes de while(SIM800L.available()>0) para hacer el código más seguro y eficiente. Permite asegurar de que solo se leerá los datos cuando realmente haya datos disponibles.
* Se agrega lock.disable() dentro de config_task para que los botones ya esten habilitados al presionarse por primera vez.
* Se agrega bandera checknum para comparar si el numero idempotencia es el mismo que el recibido.
* Se agrega condición de mensaje recibido: numrcv == numsnt y checknum == false, tanto en lora como para SMS.
* Se usa función strncmp en vez de strcmp en datoEntrada para buscar la palabra clave dentro del mensaje recibido.
* Se agrega informadorcv_lora y procesamiento de este mensaje.

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

SoftwareSerial SIM800L(RX, TX);              //RX y TX de heltec

//TaskHandle_t Task0;                         //Task0 para ejecutar en core0

void setup() {                              //setup run in core1
  SIM800L.begin(SERIAL_SIM);
  Serial.begin(SERIAL_SPEED);
  config_pines();
  config_inicial();
  delay(5000);                              //falta crear variable para initial random time

  //config lora
  if (!lora.init()) {
    Serial.println("RFM95 not detected");
    while (1);
  }
  Serial.println("RFM95 detected");
  if (initLoraTec()) {
    Serial.println("-->LoraTec OK");
    Serial.print("-->devID: PB");
    Serial.println(devID);                          //Activacion Manual, devID predefinido
    char uncero[1]={0};
    sendPackage(uncero, 1, no_espera_ACK,  1);      //envia un cero a travez de lora al iniciar para establecer la conexión
  }

  //config Scheduler
  config_task();
  
  //Se crea una tarea que se ejecutará en la función loop0(), con prioridad 1 y se ejecutará en el core0.
  //xTaskCreatePinnedToCore(loop0, "Task0", 10000, NULL, 1, &Task0, 0);  

  //configurar modulo GSM como modo SMS
  Serial.println("iniciando .........");
  ReceiveMode();
  Enviar_msj(numero.Remitente1, "Inicializacion completa");                        //provisorio de prueba, comprueba que envia mensaje correctamente al iniciar

  //config interrupt
  attachInterrupt(digitalPinToInterrupt(button1), buttonInterrupt1, RISING);            //habilita interrupcion pulsador1 con flanco ascendente
  attachInterrupt(digitalPinToInterrupt(button2), buttonInterrupt2, RISING);            //habilita interrupcion pulsador2 con flanco ascendente
  attachInterrupt(digitalPinToInterrupt(button3), buttonInterrupt3, RISING);            //habilita interrupcion pulsador3 con flanco ascendente
}

void loop() {                                           //loop run in core1
if(SIM800L.available()) {
  while(SIM800L.available()>0) {
    String mensaje_recibido = SIM800L.readString();
    uint32_t numrcv = extraer_numero(mensaje_recibido); 
    Serial.print(mensaje_recibido);
    
    //if(mensaje_recibido.indexOf("OK") != -1)  {Serial.println("se recibio OK");}                //comparo si recibo OK en el string de mensaje_recibido
    //if(mensaje_recibido.indexOf("ERROR") != -1)  {Serial.println("se recibio ERROR");}

    if (mensaje_recibido.indexOf(msj.rcv_atendido) != -1) t_atendido.enable();    //se ejecuta task de atendido
    if ((mensaje_recibido.indexOf(msj.rcv_policia) != -1 || mensaje_recibido.indexOf(msj.rcv_bomberos) != -1 || mensaje_recibido.indexOf(msj.rcv_medica) != -1) && numrcv == numsnt && checknum == false) {
        checknum = true;
        Serial.println("Recibi primero SMS");
        t_recibido.enable();      //se ejecuta task de recibido
    }

    if (mensaje_recibido.indexOf(msj.rcv_informado) != -1) {
      t_apagarLED.enable();                                                       //se ejecuta task de informado
      t_apagarLED.delay(delay_apagarLED);                                         //se ejecuta la tarea apagarLED con un delay de 15 segundos);
    }
    //if (mensaje_recibido.indexOf(msj.rcv_cerrado) != -1) task.enable();    //se ejecuta task de cerrado
}
}
    

  taskManager.execute();             // Es necesario ejecutar el runner en cada loop
  interrupt.execute();

  recvStatus = lora.readData(datoEntrante);
  if(recvStatus) {
    Serial.print("====>> ");
    Serial.println(datoEntrante);
    uint32_t numrcv = extraer_numero(datoEntrante);
    if(strncmp(datoEntrante,  atendidorcv_lora, strlen(atendidorcv_lora)) == 0) {Serial.println("atendidorcv");t_atendido.enable();}        //se ejecuta task de atendido
    if((strncmp(datoEntrante, policiarcv_lora,  strlen(policiarcv_lora))  == 0  ||                          //busca policiarcv_lora en los primeros lugares de datoEntrante, 
        strncmp(datoEntrante, bomberosrcv_lora, strlen(bomberosrcv_lora)) == 0  ||                          //si encuentra la palabra buscada devuelve un 0.
        strncmp(datoEntrante, medicarcv_lora,   strlen(medicarcv_lora))   == 0) && 
        numrcv == numsnt && checknum == false ) {

       checknum = true;
       Serial.println("Recibi primero LORA");
       t_recibido.enable();                                                 //se ejecuta task de recibido
    }
    if(strncmp(datoEntrante, informadorcv_lora, strlen(informadorcv_lora)) == 0) {
      t_apagarLED.enable();                                                       //se ejecuta task de informado
      t_apagarLED.delay(delay_apagarLED);   
    }

  }
  lora.update();                     //actualizacion lora

}


// void loop0(void *parameter){                    //loop0 run in core0
//   while(1){
//   recvStatus = lora.readData(datoEntrante);
//   if(recvStatus) {
//     Serial.print("====>> ");
//     Serial.println(datoEntrante);
//     if(strcmp(datoEntrante,atendidorcv_lora) == 0) t_atendido.enable();    //se ejecuta task de atendido
//     if(strcmp(datoEntrante, policiarcv_lora) == 0 ||
//        strcmp(datoEntrante,bomberosrcv_lora) == 0 ||
//        strcmp(datoEntrante,medicarcv_lora) == 0)  t_recibido.enable();      //se ejecuta task de recibido

//   }
//   lora.update();                     //actualizacion lora
//   vTaskDelay(10);                     //delay para fallas de wtd
  
//   }

//   //vTaskDelay(10);                     //delay para fallas de wtd
// }



