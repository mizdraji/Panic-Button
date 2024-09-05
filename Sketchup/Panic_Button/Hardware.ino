//configuracion de pines para el setup
void config_pines( void )
{
  //configure pines
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
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