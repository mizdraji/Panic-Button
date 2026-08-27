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
  t_pdr.disable();
  t_recibido.disable();
  t_atendido.disable();
  t_ensayo.disable();
  lock.disable();
  t_sms_send.disable();
  Tinformadorcv_Led.disable();
  SleepSIM.disable();
#if MODO_DEMO
  Serial.println("MODO DEMO: deep sleep deshabilitado");
#endif
}


//Definimos nuestras tareas:

static char pending_sms_num[20];
static char pending_sms_msg[64];

void sendPendingSmsTask() {
  Enviar_msj(pending_sms_num, pending_sms_msg);
  t_sms_send.disable();
}

void queueSms(const char* numero, const char* msj) {
  if (numero == NULL || msj == NULL) return;
  strncpy(pending_sms_num, numero, sizeof(pending_sms_num) - 1);
  pending_sms_num[sizeof(pending_sms_num) - 1] = '\0';
  strncpy(pending_sms_msg, msj, sizeof(pending_sms_msg) - 1);
  pending_sms_msg[sizeof(pending_sms_msg) - 1] = '\0';
  Serial.print("SMS en cola (");
  Serial.print((unsigned long)DEMO_SMS_DELAY_MS);
  Serial.println(" ms)");
  t_sms_send.restartDelayed(DEMO_SMS_DELAY_MS);
}

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
    encenderLED(led1);

    numsnt = idempotencia_random();
    checknum = false;

    char mensaje_saliente_lora[50];
    char mensaje_saliente_sms[64];
    snprintf(mensaje_saliente_lora, sizeof(mensaje_saliente_lora), "%s, %lu", policia_lora, (unsigned long)numsnt);
    snprintf(mensaje_saliente_sms, sizeof(mensaje_saliente_sms), "%s, %lu", policia.c_str(), (unsigned long)numsnt);
    Serial.print("LORA TX -> ");
    Serial.println(mensaje_saliente_lora);
    sendPackage(mensaje_saliente_lora, strlen(mensaje_saliente_lora), no_espera_ACK,  1);
    yield();
    queueSms(numero.Remitente2.c_str(), mensaje_saliente_sms);
        
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
    encenderLED(led2);

    numsnt = idempotencia_random();
    checknum = false;

    char mensaje_saliente_lora[50];
    char mensaje_saliente_sms[64];
    snprintf(mensaje_saliente_lora, sizeof(mensaje_saliente_lora), "%s, %lu", bomberos_lora, (unsigned long)numsnt);
    snprintf(mensaje_saliente_sms, sizeof(mensaje_saliente_sms), "%s, %lu", bomberos.c_str(), (unsigned long)numsnt);
    Serial.print("LORA TX -> ");
    Serial.println(mensaje_saliente_lora);
    sendPackage(mensaje_saliente_lora, strlen(mensaje_saliente_lora), no_espera_ACK,  1);
    yield();
    queueSms(numero.Remitente2.c_str(), mensaje_saliente_sms);
    
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
    encenderLED(led3);

    numsnt = idempotencia_random();
    checknum = false;

    char mensaje_saliente_lora[50];
    char mensaje_saliente_sms[64];
    snprintf(mensaje_saliente_lora, sizeof(mensaje_saliente_lora), "%s, %lu", medica_lora, (unsigned long)numsnt);
    snprintf(mensaje_saliente_sms, sizeof(mensaje_saliente_sms), "%s, %lu", medica.c_str(), (unsigned long)numsnt);
    Serial.print("LORA TX -> ");
    Serial.println(mensaje_saliente_lora);
    sendPackage(mensaje_saliente_lora, strlen(mensaje_saliente_lora), no_espera_ACK,  1);
    yield();
    queueSms(numero.Remitente2.c_str(), mensaje_saliente_sms);
        
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
  timer = 0;
}

void tatendido() {
  encenderLED(led_atendido);
  t_atendido.disable();
  timer = 0;
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
    //timer = 0;                                              //Resetea el timer cuando el USB esta conectado para no entrar a modo sleep.
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
    timer = 0;
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

// Modo ensayo: cada ENSAYO_INTERVALO_MS segun ENSAYO_INCLUIR_SMS (solo LoRa o SMS+LoRa).
// SMS formato: "Ens,<contador>,<timestamp_ms>"
void ensayoTask() {
  if (ensayo_counter >= ENSAYO_TOTAL_MENSAJES) {
    Serial.println("ENSAYO finalizado: total de mensajes alcanzado");
    t_ensayo.disable();
    return;
  }

  ensayo_counter++;
  uint32_t ts_ms = millis();

  char payload_lora[64];
  snprintf(payload_lora, sizeof(payload_lora), "Le,%u,%lu", (unsigned int)ensayo_counter, (unsigned long)ts_ms);
  Serial.print("ENSAYO TX -> ");
  Serial.println(payload_lora);
  sendPackage(payload_lora, strlen(payload_lora), no_espera_ACK, 1);

  #if ENSAYO_INCLUIR_SMS
    char payload_sms[64];
    snprintf(payload_sms, sizeof(payload_sms), "Ens,%u,%lu", (unsigned int)ensayo_counter, (unsigned long)ts_ms);
    Enviar_msj(numero.Remitente2.c_str(), payload_sms);
  #endif

  timer = 0; //evita entrar en sleep durante el ensayo
}

void Sleeping_init(){
#if MODO_DEMO
  return;
#endif
  timer ++;
  Serial.print("timer: ");
  Serial.println(timer);
  if((digitalRead(button1) && digitalRead(button2) && digitalRead(button3)) == LOW && (timer > tiempo) && (slp == false)) {
    
    slp = true;
    Serial.println("Going to sleep now");
    SleepSIM.enable();
    SleepSIM.delay(2000);
  }
}


void dormirSIM800() {
  digitalWrite(DTR, HIGH);                //DTR HIGH -> DORMIR
  Serial.println("sleep now");
  delay(100);
  //SIM800L.print("AT+CFUN=0\r\n");       // = 0 Minimum functionality - Este comando se usa para reducir aun mas el consumo
  
  SIM800L.print("AT+CSCLK=1\r\n");        //= 1 Dormir
  
  SleepSIM.disable();
  esp_deep_sleep_start();
  
}

void despertarSIM800L() {
  digitalWrite(DTR, LOW);               //DTR LOW -> DESPIERTA
  Serial.println("wake up");
  delay(100);
  //SIM800L.print("AT+CFUN=1\r\n");     // = 1 Full functionality (Default) - Este comando se usa para reducir aun mas el consumo
  
  SIM800L.print("AT+CSCLK=0\r\n");      //= 0 Despierta
  
  delay(100);          // Esperar 100ms
}
