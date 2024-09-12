


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

//TASK5: interrupcion por pulsador
void buttonTask1() {
  if (statebutton1) {
    // Realiza la tarea que deseas ejecutar después de la interrupción por pulsador
    Enviar_msj(numero.Remitente1, msj.policia);
    //Falta agregar enviar mensaje LORA
    Serial.println("Enviar mensaje1 y prender led1");
    digitalWrite(led3, HIGH);
    
    
    statebutton1 = false;             // Reinicia el estado del pulsador
  }
}

//TASK6: interrupcion por pulsador
void buttonTask2() {
  if (statebutton2) {
    // Realiza la tarea que deseas ejecutar después de la interrupción por pulsador
    Enviar_msj(numero.Remitente1, msj.bomberos);
    //Falta agregar enviar mensaje LORA
    Serial.println("Enviar mensaje2 y prender led2");
    
    statebutton2 = false;           // Reinicia el estado del pulsador
  }
}
//TASK7: interrupcion por pulsador
void buttonTask3() {
  if (statebutton3) {
    // Realiza la tarea que deseas ejecutar después de la interrupción por pulsador
    Enviar_msj(numero.Remitente1, msj.medica);
    //Falta agregar enviar mensaje LORA
    Serial.println("Enviar mensaje3 y prender led3");
    //led_blink();
    statebutton3 = false;           // Reinicia el estado del pulsador
  }
}

//LED ON
void encenderLED(uint8_t LED) {
  //prevMillis = millis();                CORRESPONDE AL PLAN B APAGAR LED EN CASO DE QUE NO FUNCIONEN LAS TAREAS
  digitalWrite(LED, HIGH);  // Encender el LED
  //Serial.println("encender led");

  
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

void buttonInterrupt1() {           //interrupcion pulsador1
  statebutton1 = true;
}

void buttonInterrupt2() {           //interrupcion pulsador2
  statebutton2 = true;
}

void buttonInterrupt3() {           //interrupcion pulsador3
  statebutton3 = true;
}
