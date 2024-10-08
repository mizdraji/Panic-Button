//configuracion de pines para el setup
void config_pines()
{
  //configure pines
  pinMode(button1, INPUT);            //boton de policia      - GPIO 37
  pinMode(button2, INPUT);            //boton de bomberos     - GPIO 38
  pinMode(button3, INPUT);            //boton de ambulancia   - GPIO 39
  pinMode(ADC_powerON, INPUT);        //conversor ADC para lectura de 5V USB
  pinMode(led1, OUTPUT);              //LED1 confirm policia  - GPIO 15
  pinMode(led2, OUTPUT);              //LED2 confirm bomberos - GPIO 2
  pinMode(led3, OUTPUT);              //LED3 confirm medica   - GPIO 4
  pinMode(led_powerON, OUTPUT);       //LED blanco
  pinMode(led_recibido, OUTPUT);      //LED amarillo
  pinMode(led_atendido, OUTPUT);      //LED naranja
  //pinMode(LED_BUILTIN, OUTPUT);       //LED integrado         - GPIO 25
}

//estado incial de los leds
void config_inicial()
{
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led_powerON, LOW);
  digitalWrite(led_recibido, LOW);
  digitalWrite(led_atendido, LOW);
}

//Función para enviar mensaje SMS
void Enviar_msj(String numero, String msj) {
  //Se establece el formato de SMS en ASCII
  String config_numero = "AT+CMGS=\"+549" + numero + "\"\r\n";
  Serial.println(config_numero);
  
  //Enviar comando para un nuevos SMS al numero establecido
  SIM800L.print(config_numero);
  delay(10);

  //Enviar contenido del SMS
  SIM800L.print(msj);
  delay(10);

  //Enviar Ctrl+Z
  SIM800L.write((char)26);
  delay(10);
  Serial.println("Mensaje enviado");
}

//This is used with ReceiveMode function, it's okay to use for tests with Serial monitor
void Serialcom() {      
  //delay(500);
  while(Serial.available()) {
    SIM800L.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(SIM800L.available()) {
    Serial.write(SIM800L.read());//Forward what Software Serial received to Serial Port
  }
}

//Set the SIM800L Receive mode  
void ReceiveMode() {       
  SIM800L.print("AT\r"); //If everything is Okay it will show "OK" on the serial monitor
  delay(200);
  Serialcom();
  SIM800L.write("AT+CMGF=1\r"); // Configuring TEXT mode
  delay(200);
  Serialcom();
  SIM800L.print("AT+CSCS=\"GSM\"\r");
  delay(200);
  Serialcom();
  SIM800L.print("AT+CNMI=2,2,0,0,0\r"); //Configure the SIM800L on how to manage the Received SMS... Check the SIM800L AT commands manual
  delay(200);
  Serialcom();
}

//Prueba de Red: Es el primer paquete LoRa. Envia un cero para establecer la conexion con la red LoRaWAN.
void pdr_function() {
  if (nodo.pdr_ok == 0) {
    if (nodo.t_wait == 0) {
      char uncero[1]={0};
      if (sendPackage(uncero, 1, espera_ACK, 0)) {        //si llega el ACK se pone en 1 y entra al if
        nodo.pdr_ok = 1;
        nodo.t_wait = random_time(0,MAX_RANDOM_LARGO);
        nodo.pausa_larga = 0;
        nodo.cont_pausas_largas=0;
        nodo.cont_reintento_corto = 0;
        Serial.println("-->PRUEBA DE RED: OK");
      }
      else {
        nodo.pdr_ok = 0;
        nodo.cont_reintento_corto++;
        if (nodo.cont_reintento_corto >= MAX_REINTENTOS) {
          nodo.cont_reintento_corto = 0;
          nodo.pausa_larga = 1;
          nodo.cont_pausas_largas++;
          if (nodo.cont_pausas_largas >= MAX_PAUSAS_LARGAS) {
            nodo.cont_pausas_largas = 0;
            nodo.t_wait = UN_DIA; //esperar un dia completo
          }
          else nodo.t_wait = LONG_TIME_TO_WAIT; //este es un tiempo largo y fijo
          Serial.println("-->Cant Max de reintentos para PDR excedido, pausa larga");
          
          Serial.print("-->Esperando t = "); Serial.print(nodo.t_wait); Serial.println(" s para reintentar PDR...");
          
        }
        else {
          nodo.t_wait = random_time(MIN_RANDOM,MAX_RANDOM); //devuelve en segundos
          Serial.println("-->PRUEBA DE RED: FALLO");
          
          Serial.print("-->Esperando t = "); Serial.print(nodo.t_wait); Serial.println(" s para reintentar PDR...");
          
        }
      }
    }
    else if (nodo.t_wait > 0) { //tiempo de espera 
      nodo.t_wait--; //vamos decrementando el t_wait
      
    }
  }
}

//FUNCIONES DE INTERRUPCIONES CON ANTIREBOTE:
//interrupcion pulsador1
void IRAM_ATTR buttonInterrupt1() {           
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) {
    statebutton1 = true;
    t5.enable();
  }
  last_interrupt_time = interrupt_time;
}
//interrupcion pulsador2
void IRAM_ATTR buttonInterrupt2() {           
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) {
    statebutton2 = true;
    t6.enable();
  }
  last_interrupt_time = interrupt_time;
}
//interrupcion pulsador3
void IRAM_ATTR buttonInterrupt3() {           
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) {
    statebutton3 = true;
    t7.enable();
  }
  last_interrupt_time = interrupt_time;
}

int16_t random_time(unsigned int MIN_,unsigned int MAX_) {
  //calcula un nuevo tiempo
  return random(MIN_, MAX_);
}