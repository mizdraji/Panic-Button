#define MAX_SF 2                  //Se refiere a la posicion dentro del vector de SF, es decir el maximo sera: SF9
#define MAX_RETRY_SAME_SF 2

const unsigned char SFvector[4] = { SF7BW125, SF8BW125, SF9BW125, SF10BW125 }; //toma solamente hasta los primeros 4 SF
uint8_t SF_actual = 0;            //para saber cual se esta usando
uint8_t SF_is_set = 0;            //para saber si esta seteado el SF
uint8_t SF_index = 0;             //para movernos dentro del vector
uint8_t cont_same_SF = 0;         //para probar mas d euna vez cada SF

//variables para timeout
#define tick_time 100 //base de tiempo para el delay
#define timeout 80 //timeout * tick_time = tiempo de time out = 8 segundos

#if tipo_modulo == 0
//RFM95W + ESP12 (Macro version)
const sRFM_pins RFM_pins = {
  .CS = 15,
  .RST = 0,
  .DIO0 = 5,
  .DIO1 = 5,
  .DIO2 = 5,
  .DIO5 = -1,
};
#endif

#if tipo_modulo == 1
//SX1276 + ESP32 (TTGO Lora32)
const sRFM_pins RFM_pins = {
  .CS = 18,
  .RST = 14,
  .DIO0 = 26,
  .DIO1 = 33,
  .DIO2 = 32,
  .DIO5 = -1,
};
#endif

//
uint8_t initLoraTec( void )
{
  if (lora.init())
  {
    lora.setDeviceClass(CLASS_C);
    //lora.setTxPower1(15); //max, fijo
    lora.setDataRate(DEFAULT_SF);
    if (RANDOM_CHANNEL)
    {
      lora.setChannel(MULTI);
    }
    else
    {
      lora.setChannel(DEFAULT_CHANNEL);
    }

    Serial.print("nwkSKey: "); Serial.println(nwkSKey);
    Serial.print("appSKey: "); Serial.println(appSKey);
    
    lora.setNwkSKey(nwkSKey);
    lora.setAppSKey(appSKey);
    lora.setDevAddr(devAddr); //Activacion Manual, devAddr predefinido
    Serial.print("devAddr: "); Serial.println(devAddr);

    //SF_is_set = 0;
    return 1; //todo OK
  }
  else
  {
    Serial.println("-->LoraTec: error transceptor");
    //if (nodo.modo_wifi) telnet.println("-->LoraTec: error transceptor\r");
    return 0; //error de transeptor
  }
}


/*La funcion send package contampla el seteo de SF
  siempre que haya un SF seteado (SF_is_set = 1), se intenta mover el SF para que no quede estancando siempre en un mismo SF
  inicialmente no hay ningun SF seteado (SF_is_set = 0), tambien cada vez que falla un ack o dato--> SF_is_set se pone en 0
*/
uint8_t sendPackage( char *data_to_send, uint8_t len, uint8_t rta_type, bool canal_por_defecto) {
  //añadir stop interrupt si es necesario aqui.

  uint8_t rta = 0; //es para almacenar el resultado de lora.readAck()
  uint8_t rcv = 0; //para hacer la consulta de datos entrantes
  uint8_t dato_ok = 0;
  uint8_t cont_timeout = 0;

  if (canal_por_defecto) {
    lora.setChannel(DEFAULT_CHANNEL);
  }
  else lora.setChannel(MULTI);
  
  if (canal_por_defecto) {
    lora.setDataRate(DEFAULT_SF);
  }
  else if (SF_is_set) {  
    if(SF_actual > 0){
      uint8_t sf_ant = SF_actual - 1;
      lora.setDataRate(SFvector[sf_ant]);
      SF_actual = sf_ant;
      if (dbspk) {
        Serial.print("Reducir SF: "); Serial.println(SF_actual);
        //if (nodo.modo_wifi) { telnet.print("Reducir SF: "); telnet.println(String(SF_actual)+"\r"); }
      }
    }
    else {
      lora.setDataRate(SFvector[SF_actual]);
      if (dbspk) {
        Serial.print("usar SF_actual: "); Serial.println(SF_actual);
        //if (nodo.modo_wifi) { telnet.print("usar SF_actual: "); telnet.println(String(SF_actual)+"\r"); }
      }
    }
  }
  else {              //no esta seteado el SF aun...
    lora.setDataRate(SFvector[SF_index]);
    SF_actual = SF_index;
    if (dbspk) {
      Serial.print("-->Test SF: "); Serial.println(SF_index);
      //if (nodo.modo_wifi) { telnet.print("-->Test SF: "); telnet.println(String(SF_index)+"\r"); }
    }
    cont_same_SF++;
    if (cont_same_SF >= MAX_RETRY_SAME_SF) {
      cont_same_SF = 0;
      SF_index++;
    }
    if (SF_index > MAX_SF) {
      SF_index = 0;
    }
  }
  
  if (canal_por_defecto) lora.sendUplink(data_to_send, len, 0, 1);
  else lora.sendUplink(data_to_send, len, 1, 1);

  switch (rta_type)
  {
    case espera_ACK:
      while (!rta && cont_timeout < timeout) {
        //Serial.println(">>esperando ack<<");
        lora.update();
        rta = lora.readAck();

        delay(tick_time);
        cont_timeout++; //contador de time out
        //Serial.print("cont_timeout: "); Serial.println(cont_timeout);
      }
      if (rta) {
        if (dbspk) {
          Serial.println("-->ack ok");
          //if (nodo.modo_wifi) telnet.println("-->ack ok\r");
        }
        SF_is_set = 1;
        //Elster_start();
        return 1; //llego el ack, con retur rompemos el while y salimos
      }
      else {
        if (dbspk) {
          Serial.println("-->NO ACK, time out");
          //if (nodo.modo_wifi) telnet.println("-->NO ACK, time out\r");
        }
        SF_is_set = 0;
        //Elster_start();
        return 0; //final por time out
      }
      break;

    case espera_Dato:
      //
      while (!dato_ok && cont_timeout < timeout) { //termina el while cuando termina el time out. dato_ok es 1 solo si el procesamiento fue correcto, sino seguira intentando hasta que termine el timout
        lora.update();
        //Serial.println(">>esperando dato<<");
        rcv  = lora.readData(datoEntrante);   //cargo los datos leidos en el array, luego los proceso, si el procesamiento esta okey, ahi recien es dato_ok = 1
        if (rcv > 1) {
          //dato_ok = procesarDatoEntrante();
        }
        delay(tick_time);
        cont_timeout++; //contador de time out
      }
      if (dato_ok) {
        if (dbspk) {
          Serial.println("-->dato OK");
          //if (nodo.modo_wifi) telnet.println("-->dato OK\r");
        }
        SF_is_set = 1;

        //Elster_start();
        return 1;
      }
      else {
        if (dbspk) {
          Serial.println("-->dato NO ok o ningun dato recibido, time out");
          //if (nodo.modo_wifi) telnet.println("-->dato NO ok o ningun dato recibido, time out\r");
        }
        SF_is_set = 0;
        
        //Elster_start();
        return 0;
      }
      break;
    case no_espera_ACK:
      //Elster_start();
      return 1;
    break;

  }
}