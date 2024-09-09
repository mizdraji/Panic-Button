//Definimos nuestras tareas:

//TASK1
void led_blink() {
  statusLED = !statusLED;
  digitalWrite(LED_BUILTIN, statusLED);
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
    Serial.println("blink blink");
    
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
    Serial.println("buttonTask1 ejecutada después de interrupción por pulsador");
    
    // Reinicia el estado del botón
    statebutton1 = false;
  }
}

//TASK6: interrupcion por pulsador
void buttonTask2() {
  if (statebutton2) {
    // Realiza la tarea que deseas ejecutar después de la interrupción por pulsador
    Serial.println("buttonTask2 ejecutada después de interrupción por pulsador");
    
    // Reinicia el estado del botón
    statebutton2 = false;
  }
}
//TASK7: interrupcion por pulsador
void buttonTask3() {
  if (statebutton3) {
    // Realiza la tarea que deseas ejecutar después de la interrupción por pulsador
    Serial.println("buttonTask3 ejecutada después de interrupción por pulsador");
    
    // Reinicia el estado del botón
    statebutton3 = false;
  }
}

void buttonInterrupt1() {
  statebutton1 = true;
}

void buttonInterrupt2() {
  statebutton2 = true;
}

void buttonInterrupt3() {
  statebutton3 = true;
}
