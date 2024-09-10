//configuracion de pines para el setup
void config_pines( void )
{
  //configure pines
  pinMode(button1, INPUT);            //boton de policia      - GPIO 37
  pinMode(button2, INPUT);            //boton de bomberos     - GPIO 38
  pinMode(button3, INPUT);            //boton de ambulancia   - GPIO 39
  pinMode(led1, OUTPUT);              //LED1 confirm policia  - GPIO 15
  pinMode(led2, OUTPUT);              //LED2 confirm bomberos - GPIO 2
  pinMode(led3, OUTPUT);              //LED3 confirm medica   - GPIO 4
  pinMode(LED_BUILTIN, OUTPUT);       //LED integrado         - GPIO 25
}

//Función para enviar mensaje SMS
void Enviar_msj(String numero, String msj) {
  //Se establece el formato de SMS en ASCII
  String config_numero = "AT+CMGS=\"+549" + numero + "\"\r\n";
  Serial.println(config_numero);

  //configurar modulo como modo SMS
  SIM800L.write("AT+CMGF=1\r\n");
  delay(10);
  
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
  SIM800L.println("AT"); //If everything is Okay it will show "OK" on the serial monitor
  delay(500);
  Serialcom();
  SIM800L.println("AT+CMGF=1"); // Configuring TEXT mode
  delay(500);
  Serialcom();
  SIM800L.println("AT+CNMI=2,2,0,0,0"); //Configure the SIM800L on how to manage the Received SMS... Check the SIM800L AT commands manual
  delay(500);
  Serialcom();
}















//Prueba de Red: Es el primer paquete LoRa. Envia un cero para establecer la conexion con la red LoRaWAN.
void pdr_function() {
  if (nodo.pdr_ok == 0) {
    if (nodo.t_wait == 0) {
      char uncero[1]={0};
      if (sendPackage(uncero, 1, espera_ACK, 0)) {        //si llega el ACK se pone en 1 y entra al if
        nodo.pdr_ok = 1;
        nodo.t_wait = random_time(0,nodo.MAX_RANDOM_LARGO);
        nodo.pausa_larga = 0;
        nodo.cont_pausas_largas=0;
        nodo.cont_reintento_corto = 0;
        Serial.println("-->PRUEBA DE RED: OK");
      }
      else {
        nodo.pdr_ok = 0;
        nodo.cont_reintento_corto++;
        if (nodo.cont_reintento_corto >= nodo.MAX_REINTENTOS) {
          nodo.cont_reintento_corto = 0;
          nodo.pausa_larga = 1;
          nodo.cont_pausas_largas++;
          if (nodo.cont_pausas_largas >= nodo.MAX_PAUSAS_LARGAS) {
            nodo.cont_pausas_largas = 0;
            nodo.t_wait = nodo.UN_DIA; //esperar un dia completo
          }
          else nodo.t_wait = nodo.LONG_TIME_TO_WAIT; //este es un tiempo largo y fijo
          Serial.println("-->Cant Max de reintentos para PDR excedido, pausa larga");
          
          Serial.print("-->Esperando t = "); Serial.print(nodo.t_wait); Serial.println(" s para reintentar PDR...");
          
        }
        else {
          nodo.t_wait = random_time(nodo.MIN_RANDOM,nodo.MAX_RANDOM); //devuelve en segundos
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




int16_t random_time(unsigned int MIN_,unsigned int MAX_) {
  //calcula un nuevo tiempo
  return random(MIN_, MAX_);
}