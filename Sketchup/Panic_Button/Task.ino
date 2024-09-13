


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
    // Apagar el LED después de 0.5 segundos
    LED_state = LOW;                  // Cambiar estado del LED
    previousMillis = currentMillis;   // Actualizar tiempo anterior
    digitalWrite(LED_BUILTIN, LED_state);  // Apagar el LED
    //Serial.println("blink blink");
    
  } else if (LED_state == LOW && (currentMillis - previousMillis >= ledOffTime)) {
    // Encender el LED después de 7 segundos
    LED_state = HIGH;                 // Cambiar estado del LED
    previousMillis = currentMillis;   // Actualizar tiempo anterior
    digitalWrite(LED_BUILTIN, LED_state);  // Encender el LED
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
  if (statebutton1) {
    Enviar_msj(numero.Remitente1, msj.policia);               //SMS
    sendPackage(policia_lora, strlen(policia_lora), no_espera_ACK,  1);         //LORA
    #if DEBUG
    Serial.println("Enviar mensaje1 y prender led1");
    #endif
    digitalWrite(led1, HIGH);
    
    statebutton1 = false;             // Reinicia el estado del pulsador
  }
}

//TASK6: interrupcion por pulsador button2
void buttonTask2() {
  if (statebutton2) {
    Enviar_msj(numero.Remitente1, msj.bomberos);          //SMS
    sendPackage(bomberos_lora, strlen(bomberos_lora), no_espera_ACK,  1);      //LORA
    #if DEBUG
    Serial.println("Enviar mensaje2 y prender led2");
    #endif
    digitalWrite(led2, HIGH);
    
    statebutton2 = false;           // Reinicia el estado del pulsador
  }
}
//TASK7: interrupcion por pulsador button3
void buttonTask3() {
  if (statebutton3) {
    Enviar_msj(numero.Remitente1, msj.medica);            //SMS
    sendPackage(medica_lora, strlen(medica_lora), no_espera_ACK,  1);      //LORA
    #if DEBUG
    Serial.println("Enviar mensaje3 y prender led3");
    #endif
    digitalWrite(led3, HIGH);
    
    statebutton3 = false;           // Reinicia el estado del pulsador
  }
}

//LED ON
void encenderLED(uint8_t LED) {
  //prevMillis = millis();                CORRESPONDE AL PLAN B APAGAR LED EN CASO DE QUE NO FUNCIONEN LAS TAREAS
  digitalWrite(LED, HIGH);  // Encender el LED
  #if DEBUG
  Serial.print("encender led: ");Serial.println(LED);
  #endif

  
  t_apagarLED.enable();           // Habilitar la tarea para apagar el LED después de 5 segundos
  t_apagarLED.delay(delay_apagarLED);       //se ejecuta la tarea apagarLED con un delay de 50 segundos
  
  
}

//LED OFF
void apagarLED() {
  digitalWrite(led_atendido, LOW);  // Apagar el LED
  digitalWrite(led_recibido, LOW);  // Apagar el LED
  //Serial.println("funcion apagarled");
  t_apagarLED.disable();
}

volatile unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // 50 ms


//IMPORTANTE!! NO USAR SERIAL EN FUNCION DE INTERRUPCIONES
void buttonInterrupt1() {           //interrupcion pulsador1
  statebutton1 = true;
}

void buttonInterrupt2() {           //interrupcion pulsador2
  statebutton2 = true;
}

void buttonInterrupt3() {           //interrupcion pulsador3
  statebutton3 = true;
}
