//configuracion task para setup:
void config_task(){
  Serial.println("Initialized scheduler");
  taskManager.setHighPriorityScheduler(&interrupt);          //Configura Scheduler interrupt como alta prioridad
  taskManager.enableAll(true);                               //this will recursively enable the higher priority tasks as well
  t_apagarLED.disable();
  t_apagarLED1.disable();
  t_apagarLED2.disable();
  t_apagarLED3.disable();
  t5.disable();
  t6.disable();
  t7.disable();
  t_recibido.disable();
  t_atendido.disable();
}


//Definimos nuestras tareas:

//TASK1
void led_blink() {
  statusLED = !statusLED;
  digitalWrite(led3, statusLED);
  //Serial.println((String)millis() + " - Led: " + (String)statusLED);

}

//TASK2:  blink de stand by, es un blink de baja frecuencia para indicar que el dispositivo esta funcionando.
void blinkstb() {                       
  unsigned long currentMillis = millis();
  if (LED_state == HIGH && (currentMillis - previousMillis >= ledOnTime)) {
    // Apagar el LED después de 0.3 segundos
    LED_state = LOW;                  // Cambiar estado del LED
    previousMillis = currentMillis;   // Actualizar tiempo anterior
    digitalWrite(led_powerON, LED_state);  // Apagar el LED
  } 
  else if (LED_state == LOW && (currentMillis - previousMillis >= ledOffTime)) {
    // Encender el LED después de 3 segundos
    LED_state = HIGH;                 // Cambiar estado del LED
    previousMillis = currentMillis;   // Actualizar tiempo anterior
    digitalWrite(led_powerON, LED_state);  // Encender el LED
  }
}

//TASK3: envia paquete lora.
void loraSend() {
  char uncmd[1]={0x14};
  sendPackage(uncmd, 1, no_espera_ACK,  1);
  Serial.println("se envia uncmd");

}

//TASK4: Envia mensaje

//TASK5: interrupcion por pulsador button1
void buttonTask1() {
  if(!bloqueo) {
    Enviar_msj(numero.Remitente2, msj.policia);               //SMS
    
    sendPackage(policia_lora, strlen(policia_lora), no_espera_ACK,  1);         //LORA
    #if DEBUG
    Serial.println("Enviar mensaje1 y prender led1");
    #endif

    encenderLED(led1);
    
    statebutton1 = false;             // Reinicia el estado del pulsador
    t5.disable();

    bloqueo = true;
    lock.enable();
    lock.delay(tiempo_bloqueo);       //se ejecuta la tarea lock despues de tiempo_bloqueo (3 segundos).
  }
}

//TASK6: interrupcion por pulsador button2
void buttonTask2() {
  if(!bloqueo) {
    Enviar_msj(numero.Remitente2, msj.bomberos);          //SMS
    
    sendPackage(bomberos_lora, strlen(bomberos_lora), no_espera_ACK,  1);      //LORA
    #if DEBUG
    Serial.println("Enviar mensaje2 y prender led2");
    #endif

    encenderLED(led2);
    
    statebutton2 = false;           // Reinicia el estado del pulsador
    t6.disable();

    bloqueo = true;
    lock.enable();
    lock.delay(tiempo_bloqueo);       //se ejecuta la tarea lock despues de tiempo_bloqueo (3 segundos).
  }
}
//TASK7: interrupcion por pulsador button3
void buttonTask3() {
  if(!bloqueo) {
    Enviar_msj(numero.Remitente2, msj.medica);            //SMS
    
    sendPackage(medica_lora, strlen(medica_lora), no_espera_ACK,  1);      //LORA
    #if DEBUG
    Serial.println("Enviar mensaje3 y prender led3");
    #endif

    encenderLED(led3);
    
    statebutton3 = false;           // Reinicia el estado del pulsador
    t7.disable();

    bloqueo = true;
    lock.enable();
    lock.delay(tiempo_bloqueo);       //se ejecuta la tarea lock despues de tiempo_bloqueo (3 segundos).
  }
}

void unlock(){
  bloqueo = false;
  lock.disable();
  t5.disable();
  t6.disable();
  t7.disable();
}

void trecibido() {
  encenderLED(led_recibido);
  t_recibido.disable();
}

void tatendido() {
  encenderLED(led_atendido);
  //t_apagarLED.enable();
  //t_apagarLED.delay(delay_apagarLED);       //se ejecuta la tarea apagarLED con un delay de X segundos
  t_atendido.disable();
}


//LED ON
void encenderLED(uint8_t LED) {
  digitalWrite(LED, HIGH);  // Encender el LED
}

//LED OFF
void apagarLED() {            //apaga todos los leds
  digitalWrite(led1, LOW);  // Apagar el LED
  digitalWrite(led2, LOW);  // Apagar el LED
  digitalWrite(led3, LOW);  // Apagar el LED
  digitalWrite(led_recibido, LOW);  // Apagar el LED
  digitalWrite(led_atendido, LOW);  // Apagar el LED
  t_apagarLED.disable();
}

void apagarLED1() {
  digitalWrite(led1, LOW);  // Apagar el LED
  t_apagarLED1.disable();
}
void apagarLED2() {
  digitalWrite(led2, LOW);  // Apagar el LED
  t_apagarLED2.disable();
}
void apagarLED3() {
  digitalWrite(led3, LOW);  // Apagar el LED
  t_apagarLED3.disable();
}

//powerON
void powerON () {
  if(analogRead(ADC_powerON) > ADC_powerON_value) {         //USB CONECTADO
  digitalWrite(led_powerON, HIGH);
  t2.disable();
  }  
  else {                //USB DESCONECTADO
  t2.enable();
  } 
  }

