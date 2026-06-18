/* Detalle de versiones
 * V1.8.8:
 *   PDR con TaskScheduler y ACK async; recepcion LoRa sin readData en ISR; poll RX de respaldo.
 *   Parser numeros tolerante; tokens LoRa compactos (Lp/Lpr/Lar/Lir); modo ensayo (LoRa; SMS opcional ENSAYO_INCLUIR_SMS).
 *   SF test opcional (FORCE_FIXED_SF_TEST); documentacion en docs/.
 *   Helpers de RX/depuracion en LoraTec (isValidLoraPayload, loraRxDebug*); pragma once en configuracion.h.
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

HardwareSerial SIM800L(2);      //UART2 HARDWARE

void setup() {                              
  SIM800L.begin(SERIAL_SIM, SERIAL_8N1, RX, TX);
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
  ReceiveMode();

  //config Scheduler (ya inicializado antes de la prueba de red)
  delay(1000);
  memset(datoEntrante, 0, sizeof(datoEntrante));

#if MODO_ENSAYO
  ensayo_counter = 0;
  t_ensayo.enable();
  Serial.print("MODO ENSAYO activo. Total mensajes: ");
  Serial.println((unsigned long)ENSAYO_TOTAL_MENSAJES);
#if ENSAYO_INCLUIR_SMS
  Serial.println("Ensayo: SMS + LoRa por tick");
#else
  Serial.println("Ensayo: solo LoRa por tick");
#endif
#endif

  //config interrupt
  attachInterrupt(digitalPinToInterrupt(button1), buttonInterrupt1, RISING);            //habilita interrupcion pulsador1 con flanco ascendente
  attachInterrupt(digitalPinToInterrupt(button2), buttonInterrupt2, RISING);            //habilita interrupcion pulsador2 con flanco ascendente
  attachInterrupt(digitalPinToInterrupt(button3), buttonInterrupt3, RISING);            //habilita interrupcion pulsador3 con flanco ascendente
  attachInterrupt(digitalPinToInterrupt(RFM_pins.DIO0), onReceive, RISING);            //habilita interrupcion de recepcion LoRa

  uint64_t mask = (1ULL << GPIO_NUM_39) | (1ULL << GPIO_NUM_38) | (1ULL << GPIO_NUM_36);// | (1ULL << GPIO_NUM_13); //Comentar la ultima condición para hacer pruebas mientras esta conectado.
  esp_sleep_enable_ext1_wakeup(mask, ESP_EXT1_WAKEUP_ANY_HIGH);
  print_wakeup_pins();               // Imprimir qué pin causó el wakeup
  delay(2000);
  slp = false;
}

void loop() {      
  static uint32_t last_lora_poll_ms = 0;

  if(SIM800L.available()) {
    while(SIM800L.available()>0) {
      String mensaje_recibido = SIM800L.readString();
      uint32_t numrcv = extraer_numero(mensaje_recibido); 
      Serial.print(mensaje_recibido);
      /*    
      if(mensaje_recibido.indexOf("ERROR") != -1)  {
        Serial.println("Se recibio ERROR en SIM800, reiniciando...");
        ESP.restart();                                                                              //Reset en caso de que falle el SIM800
      }
      */

      if (mensaje_recibido.indexOf(rcv_atendido)  != -1 && numrcv == numsnt) t_atendido.enable();    //se ejecuta task de atendido
      if ((mensaje_recibido.indexOf(rcv_policia)  != -1 || 
         mensaje_recibido.indexOf(rcv_bomberos) != -1 || 
         mensaje_recibido.indexOf(rcv_medica)   != -1) && numrcv == numsnt && checknum == false) {
        checknum = true;
        t_recibido.enable();      //se ejecuta task de recibido
      }

      if (mensaje_recibido.indexOf(rcv_informado) != -1 && numrcv == numsnt) {
        Tinformadorcv_Led.enable();
      }
    }
  }
    
  taskManager.execute();             // Es necesario ejecutar el runner en cada loop
  interrupt.execute();
  
  if (SIM800L.available() || (digitalRead(button1) || digitalRead(button2) || digitalRead(button3)) == HIGH) timer = 0;     //si se produce una interrupcion resetear contador timer para no entrar al modo sleep
  
  lora.update();                     //actualización lora
  bool do_lora_read = false;
  bool by_irq = false;

  if (lora_irq_pending) {
    lora_irq_pending = false;
    do_lora_read = true;
    by_irq = true;
  } else if ((millis() - last_lora_poll_ms) >= 100) {
    // respaldo: consultar RX periodicamente por si se pierde la interrupcion DIO0
    do_lora_read = true;
  }

  if (do_lora_read) {
    last_lora_poll_ms = millis();
    recvStatus = lora.readData(datoEntrante);
    loraRxDebugAfterRead(datoEntrante, recvStatus, by_irq);
  }

  if(recvStatus > 1) {
    if (!isValidLoraPayload(datoEntrante)) {
#if DEBUG_LORA_RX
      Serial.println("LORA descartado (trama no valida para app)");
#endif
      memset(datoEntrante, 0, sizeof(datoEntrante));
      recvStatus = 0;
      return;
    }

    Serial.print("====>> ");
    Serial.println(datoEntrante);
    
    timer = 0;
      uint32_t numrcv = extraer_numero(datoEntrante);
      if(strstr(datoEntrante, atendidorcv_lora) != NULL && 
        numrcv == numsnt) t_atendido.enable();                                                              //se ejecuta task de atendido
      
      if((strstr(datoEntrante, policiarcv_lora)  != NULL  || 
        strstr(datoEntrante, bomberosrcv_lora) != NULL  || 
        strstr(datoEntrante, medicarcv_lora)   != NULL) && 
        numrcv == numsnt && checknum == false ) {
        checknum = true;
       t_recibido.enable();                                                 //se ejecuta task de recibido
      }
      if(strstr(datoEntrante, informadorcv_lora) != NULL && 
        numrcv == numsnt) {
#if DEBUG_LORA_RX
        Serial.println("MATCH informado LORA");
#endif
        Tinformadorcv_Led.enable(); 
      }
    memset(datoEntrante, 0, sizeof(datoEntrante));
  }
  loraRxDebugCtrlFrame(recvStatus);
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