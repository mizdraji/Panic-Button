// Declaramos la función que vamos a usar



void led_blink() {

  statusLED = !statusLED;
  digitalWrite(LED_BUILTIN, statusLED);
  //Serial.println((String)millis() + " - Led: " + (String)statusLED);

}

void loraSend() {
  char uncmd[1]={0x14};
  sendPackage(uncmd, 1, no_espera_ACK,  1);
  Serial.println("se envia uncmd");

}
