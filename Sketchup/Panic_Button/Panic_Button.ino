/*Detalle de versiones:
* V1.8.7:
* Se agrega modo sleep para el sim800. 
* Se agregan las funciones dormirSIM800 y despertarSIM800.
* Se hacen modificaciones con respecto a como se administra la interrupción de lora.
* Se cambia de lugar el lora.update() porque habia un BUG que mostraba el mensaje anterior recibido debido a que el buffer no se actualizaba en el tiempo correcto.
* Se retira todo el código viejo que correspondia al uso de 2 cores.
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

void setup() {                              
  SIM800L.begin(SERIAL_SIM);
  Serial.begin(SERIAL_SPEED);

  delay(3000);                              //falta crear variable para initial random time
  config_pines();
  config_inicial();

  //config lora
  if (!lora.init()) {
    Serial.println("RFM95 not detected");
  }

  Serial.println("RFM95 detected");
  if (initLoraTec()) {
    Serial.println("-->LoraTec OK");
    Serial.print("-->devID: PB");
    Serial.println(devID);                          //Activacion Manual, devID predefinido
    config_task();
    t_pdr.enable();
    while (nodo.pdr_ok == 0) {
      taskManager.execute();
    }
    t_pdr.disable();
  }
  
  

  //configurar modulo GSM como modo SMS
  //Serial.println("iniciando .........");
  ReceiveMode();
  //Enviar_msj(numero.Remitente1, "Inicializacion completa");                        //provisorio de prueba, comprueba que envia mensaje correctamente al iniciar

  //config Scheduler (ya inicializado antes de la prueba de red)
  delay(1000);
  //lora.update();                     //actualización lora, mantener en la primer linea del loop.
  memset(datoEntrante, 0, sizeof(datoEntrante));

  //config interrupt
  attachInterrupt(digitalPinToInterrupt(button1), buttonInterrupt1, RISING);            //habilita interrupcion pulsador1 con flanco ascendente
  attachInterrupt(digitalPinToInterrupt(button2), buttonInterrupt2, RISING);            //habilita interrupcion pulsador2 con flanco ascendente
  attachInterrupt(digitalPinToInterrupt(button3), buttonInterrupt3, RISING);            //habilita interrupcion pulsador3 con flanco ascendente
  attachInterrupt(digitalPinToInterrupt(RFM_pins.DIO0), onReceive, CHANGE);                       //habilita interrupciones para mensajes recibidos lora, se utiliza CHANGE para cuando la señal cambia HIGH <-->LOW. Con RISING se generan multiples interrupciones.

  uint64_t mask = (1ULL << GPIO_NUM_39) | (1ULL << GPIO_NUM_38) | (1ULL << GPIO_NUM_36);// | (1ULL << GPIO_NUM_13); //Comentar la ultima condición para hacer pruebas mientras esta conectado.
  esp_sleep_enable_ext1_wakeup(mask, ESP_EXT1_WAKEUP_ANY_HIGH);
  print_wakeup_pins();               // Imprimir qué pin causó el wakeup
  delay(2000);
  slp = false;
}

void loop() {      
  if(SIM800L.available()) {
    while(SIM800L.available()>0) {
      String mensaje_recibido = SIM800L.readString();
      uint32_t numrcv = extraer_numero(mensaje_recibido); 
      Serial.print(mensaje_recibido);
    
      //if(mensaje_recibido.indexOf("OK") != -1)  {Serial.println("se recibio OK");}                //comparo si recibo OK en el string de mensaje_recibido
      if(mensaje_recibido.indexOf("ERROR") != -1)  {
      //Serial.println("se recibio ERROR");
      //ESP.restart();                                                                              //Reset en caso de que falle el SIM800
      }

      if (mensaje_recibido.indexOf(rcv_atendido)  != -1 && numrcv == numsnt) t_atendido.enable();    //se ejecuta task de atendido
      if ((mensaje_recibido.indexOf(rcv_policia)  != -1 || 
         mensaje_recibido.indexOf(rcv_bomberos) != -1 || 
         mensaje_recibido.indexOf(rcv_medica)   != -1) && numrcv == numsnt && checknum == false) {
        checknum = true;
        //Serial.println("Recibi primero SMS");
        t_recibido.enable();      //se ejecuta task de recibido
      }

      if (mensaje_recibido.indexOf(rcv_informado) != -1 && numrcv == numsnt) {
        //t_apagarLED.enable();                                                       //se ejecuta task de informado
        //t_apagarLED.delay(delay_apagarLED);                                         //se ejecuta la tarea apagarLED con un delay de 15 segundos);
        Tinformadorcv_Led.enable();
      }
      //if (mensaje_recibido.indexOf(msj.rcv_cerrado) != -1) task.enable();    //se ejecuta task de cerrado
    }
  }
    
  taskManager.execute();             // Es necesario ejecutar el runner en cada loop
  interrupt.execute();
  
  if (SIM800L.available() || (digitalRead(button1) || digitalRead(button2) || digitalRead(button3)) == HIGH) timer = 0;     //si se produce una interrupcion resetear contador timer para no entrar al modo sleep
  
  lora.update();                     //actualización lora
  if(recvStatus) {
    //lorarcv = false;
    //lora.readData(datoEntrante);
    Serial.print("====>> ");
    Serial.println(datoEntrante);
    
    timer = 0;
    //memset(datoEntrante, 0, sizeof(datoEntrante));
    //lora.readData(datoEntrante);
    //Serial.println(datoEntrante);
    
    
      //Serial.print("====>> ");
      //Serial.println(datoEntrante);
      uint32_t numrcv = extraer_numero(datoEntrante);
      if(strncmp(datoEntrante,  atendidorcv_lora, strlen(atendidorcv_lora)) == 0 && 
        numrcv == numsnt) t_atendido.enable();                                                              //se ejecuta task de atendido
      
      if((strncmp(datoEntrante, policiarcv_lora,  strlen(policiarcv_lora))  == 0  ||                          //busca policiarcv_lora en los primeros lugares de datoEntrante, 
        strncmp(datoEntrante, bomberosrcv_lora, strlen(bomberosrcv_lora)) == 0  ||                          //si encuentra la palabra buscada devuelve un 0.
        strncmp(datoEntrante, medicarcv_lora,   strlen(medicarcv_lora))   == 0) && 
        numrcv == numsnt && checknum == false ) {
        checknum = true;
       //Serial.println("Recibi primero LORA");
       t_recibido.enable();                                                 //se ejecuta task de recibido
      }
      if(strncmp(datoEntrante, informadorcv_lora, strlen(informadorcv_lora)) == 0 && 
        numrcv == numsnt) {
        //t_apagarLED.enable();                                                       //se ejecuta task de informado
        //t_apagarLED.delay(delay_apagarLED);
        Tinformadorcv_Led.enable(); 
      }
    //}
    memset(datoEntrante, 0, sizeof(datoEntrante));
  }
  recvStatus = 0;
}

// Función para imprimir qué pin causó el wakeup
void print_wakeup_pins() {
  uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
  if (wakeup_pin_mask == 0) {
    Serial.println("No se detectó ningún pin de wakeup.");
    return;
  }
  despertarSIM800L();
  delay(100);
    for (int i = 0; i < GPIO_NUM_MAX; i++) {
      if ((wakeup_pin_mask & (1ULL << i)) != 0) {
        switch (i) {
          case 36:                      //button1 policia
            Serial.println("Wakeup causado por el pin 36");
            t5.enable();
            break;
          case 38:                      //button2 bomberos
            Serial.println("Wakeup causado por el pin 38");
            t6.enable();
            break;
          case 39:                      //button3 medica
            Serial.println("Wakeup causado por el pin 39");
            t7.enable();
            break;
          case 13:
            Serial.println("Wakeup causado por el pin 13");
            break;
        }
      }
    }
}