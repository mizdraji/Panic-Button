// Configuracion de pines del hardware.

void config_pines()
{
  pinMode(button1, INPUT);            //boton de policia      - GPIO 37
  pinMode(button2, INPUT);            //boton de bomberos     - GPIO 38
  pinMode(button3, INPUT);            //boton de ambulancia   - GPIO 39
  pinMode(ADC_powerON, INPUT);
  pinMode(led1, OUTPUT);              //LED1 confirm policia  - GPIO 15
  pinMode(led2, OUTPUT);              //LED2 confirm bomberos - GPIO 2
  pinMode(led3, OUTPUT);              //LED3 confirm medica   - GPIO 4
  pinMode(led_powerON, OUTPUT);       //LED blanco
  pinMode(led_recibido, OUTPUT);      //LED amarillo
  pinMode(led_atendido, OUTPUT);      //LED naranja
  pinMode(RFM_pins.DIO0, INPUT);                //PIN INTERRUPCION LORA
  pinMode(DTR, OUTPUT);      //PDR para modo sleep del sim800
}

// Estado inicial de los leds al arrancar.
void config_inicial()
{
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led_powerON, HIGH);
  digitalWrite(led_recibido, LOW);
  digitalWrite(led_atendido, LOW);
  digitalWrite(DTR, LOW);
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

// Pasarela simple entre Serial y SIM800 para diagnostico manual.
void Serialcom() {      
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
  // SIM800L.write("AT+CMGD=1,4\r"); // Limpia memoria de mensajes
  // delay(200);
  // Serialcom();
  SIM800L.print("AT+CSCS=\"GSM\"\r");
  delay(200);
  Serialcom();
  SIM800L.print("AT+CNMI=2,2,0,0,0\r"); //Configure the SIM800L on how to manage the Received SMS... Check the SIM800L AT commands manual
  delay(200);
  Serialcom();
}

// Prueba de red (PDR): inicia envio con ACK y hace polling no bloqueante.
// La funcion se ejecuta cada 1s desde t_pdr.
void pdr_function() {
  static char uncero[1] = {0};

  if (nodo.pdr_ok == 0) {
    if (nodo.t_wait == 0) {
      if (!isSendPackageAckAsyncWaiting()) {
        sendPackageAckAsyncStart(uncero, 1, 0);
      }

      int8_t ackStatus = sendPackageAckAsyncPoll();
      if (ackStatus == 2) return; //seguimos esperando ACK sin bloquear

      if (ackStatus == 1) {        //si llega el ACK se pone en 1 y entra al if
        nodo.pdr_ok = 1;
        nodo.t_wait = random_time(0,MAX_RANDOM_LARGO);
        nodo.pausa_larga = 0;
        nodo.cont_pausas_largas = 0;
        nodo.cont_reintento_corto = 0;
        Serial.println("-->PRUEBA DE RED: OK");
      }
      else if (ackStatus == -1) {
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
      } else {
        // Estado inesperado (sin envio activo): reintentar en el siguiente tick.
        return;
      }
    }
    else if (nodo.t_wait > 0) { //tiempo de espera 
      nodo.t_wait--; //t_pdr corre cada 1 segundo, asi que el decremento es en segundos reales
    }
  }
}

// Interrupciones con antirebote.
// Interrupcion pulsador1
void IRAM_ATTR buttonInterrupt1() {           
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200) {
    statebutton1 = true;
    t5.enableIfNot();                           //Habilita la tarea solo si estaba desactivada previamente
  }
  last_interrupt_time = interrupt_time;
}

// Interrupcion pulsador2
void IRAM_ATTR buttonInterrupt2() {           
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200) {
    statebutton2 = true;
    t6.enableIfNot();                           //Habilita la tarea solo si estaba desactivada previamente
  }
  last_interrupt_time = interrupt_time;
}

// Interrupcion pulsador3
void IRAM_ATTR buttonInterrupt3() {           
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200) {
    statebutton3 = true;
    t7.enableIfNot();                             //Habilita la tarea solo si estaba desactivada previamente
  }
  last_interrupt_time = interrupt_time;
}

// Interrupcion LoRa RX:
// solo levanta bandera, la lectura SPI se hace en loop() para evitar bloqueos en ISR.
void IRAM_ATTR onReceive() {
  // En ISR solo marcar bandera; evitar SPI/readData dentro de interrupcion.
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 30) {
    lora_irq_pending = true;
  }
  last_interrupt_time = interrupt_time;
}

// Genera un numero aleatorio de 8 digitos para idempotencia.
uint32_t idempotencia_random() {
    int32_t timestamp = esp_timer_get_time();                   // Obtener el timestamp actual en microsegundos
    srand((unsigned int)(timestamp));                           //Sembrar el generador de números aleatorios con el timestamp (opcional, para mayor variabilidad)
    uint32_t random_number = rand()  % 90000000 + 10000000;     // Generar un número aleatorio
    return random_number;
}

// Extrae numero de payload char[] del tipo "Lxx, 12345678".
// Tolera coma con o sin espacio.
uint32_t extraer_numero(char mensaje_completo[]) {
    char *coma_pos = strrchr(mensaje_completo, ',');                  // Buscar la ultima coma
    if (coma_pos != NULL) {
        char *parte_numerica = coma_pos + 1;                          // posicion despues de la coma
        while (*parte_numerica == ' ' || *parte_numerica == '\t') {   // tolerar formato ",123" o ", 123"
          parte_numerica++;
        }
        uint32_t numero = strtoul(parte_numerica, NULL, 10);
        return numero;
    }
    return 0;
}

// Extrae numero de payload String tomando la ultima coma.
uint32_t extraer_numero(String mensaje_completo) {
    int posicion_ultima_coma = mensaje_completo.lastIndexOf(',');
    if (posicion_ultima_coma != -1) {
        String numero = mensaje_completo.substring(posicion_ultima_coma + 1);  // despues de la coma
        numero.trim();                                                           // tolera espacios
        return numero.toInt();
    }
    return 0;
}

// Genera un numero aleatorio entre MIN y MAX.
uint16_t random_time(unsigned int MIN_,unsigned int MAX_) {
  return random(MIN_, MAX_);          //calcula un nuevo tiempo
}