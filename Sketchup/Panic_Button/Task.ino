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
  lock.disable();
  Tinformadorcv_Led.disable();
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
    idempotencia = idempotencia_random();
    numsnt = idempotencia.toInt();
    checknum = false;
    String mensaje_saliente = msj.policia + ", " + idempotencia;
    //Enviar_msj(numero.Remitente2, msj.policia);          //SMS
    Enviar_msj(numero.Remitente2, mensaje_saliente);       //SMS
    
    char mensaje_saliente_lora[50];
    sprintf(mensaje_saliente_lora, "%s, %s", policia_lora, idempotencia.c_str());                 //Concatena "policia" e "idempotencia" con una coma
    sendPackage(mensaje_saliente_lora, strlen(mensaje_saliente_lora), no_espera_ACK,  1);         //LORA
    //sendPackage(policia_lora, strlen(policia_lora), no_espera_ACK,  1);                         //LORA

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
    idempotencia = idempotencia_random();
    numsnt = idempotencia.toInt();
    checknum = false;
    String mensaje_saliente = msj.bomberos + ", " + idempotencia;
    //Enviar_msj(numero.Remitente2, msj.bomberos);          //SMS
    Enviar_msj(numero.Remitente2, mensaje_saliente);       //SMS
    
    char mensaje_saliente_lora[50];
    sprintf(mensaje_saliente_lora, "%s, %s", bomberos_lora, idempotencia.c_str());                 //Concatena "bomberos" e "idempotencia" con una coma
    sendPackage(mensaje_saliente_lora, strlen(mensaje_saliente_lora), no_espera_ACK,  1);         //LORA
    //sendPackage(bomberos_lora, strlen(bomberos_lora), no_espera_ACK,  1);                       //LORA

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
    idempotencia = idempotencia_random();
    numsnt = idempotencia.toInt();
    checknum = false;
    String mensaje_saliente = msj.medica + ", " + idempotencia;
    //Enviar_msj(numero.Remitente2, msj.medica);            //SMS
    Enviar_msj(numero.Remitente2, mensaje_saliente);        //SMS

    char mensaje_saliente_lora[50];
    sprintf(mensaje_saliente_lora, "%s, %s", medica_lora, idempotencia.c_str());                 //Concatena "media" e "idempotencia" con una coma
    sendPackage(mensaje_saliente_lora, strlen(mensaje_saliente_lora), no_espera_ACK,  1);         //LORA
    //sendPackage(medica_lora, strlen(medica_lora), no_espera_ACK,  1);      //LORA

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
  else t2.enable();                                         //USB DESCONECTADO
}


void informado_led() {
    unsigned long currentMillis = millis();
  if (LED_state2 == HIGH && (currentMillis - previousMillis >= 500)) {
    // Apagar el LED después de 0.3 segundos
    LED_state2 = LOW;                  // Cambiar estado del LED
    previousMillis = currentMillis;   // Actualizar tiempo anterior
    digitalWrite(led_powerON, LED_state2);  // Apagar el LED
    digitalWrite(led1, LED_state2); 
    digitalWrite(led2, LED_state2); 
    digitalWrite(led3, LED_state2);
    digitalWrite(led_recibido, LED_state2);
    digitalWrite(led_atendido, LED_state2);   
  } 
  else if (LED_state2 == LOW && (currentMillis - previousMillis >= 500)) {
    // Encender el LED después de 3 segundos
    LED_state2 = HIGH;                 // Cambiar estado del LED
    previousMillis = currentMillis;   // Actualizar tiempo anterior
    digitalWrite(led_powerON, LED_state2);  // Encender el LED
    digitalWrite(led1, LED_state2); 
    digitalWrite(led2, LED_state2); 
    digitalWrite(led3, LED_state2);
    digitalWrite(led_recibido, LED_state2); 
    digitalWrite(led_atendido, LED_state2);
    counterInformado ++;  
  }
  if (counterInformado > timesCounterInformado) {
    digitalWrite(led_powerON, LOW);  // Encender el LED
    digitalWrite(led1, LOW); 
    digitalWrite(led2, LOW); 
    digitalWrite(led3, LOW);
    digitalWrite(led_recibido, LOW); 
    digitalWrite(led_atendido, LOW); 
    Tinformadorcv_Led.disable();
    counterInformado = 0;
  }
}
