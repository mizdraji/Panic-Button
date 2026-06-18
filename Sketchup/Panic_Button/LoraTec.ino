#define MAX_RETRY_SAME_SF 2

// AU915 / US915 (Beelan `dataRates_t` en Struct.h): uplinks 125 kHz solo SF7..SF10
// (no hay SF11BW125/SF12BW125 en esta banda en la libreria; DR mas lento 125 kHz = SF10).
// Escalera adaptativa: de SF7 (menos alcance / mas rapido) hacia SF10 (maximo alcance 125 kHz).
// MAX_SF = ultimo indice valido; alinear FIXED_SF_INDEX en configuracion.h.
const unsigned char SFvector[] = {
  SF7BW125, SF8BW125, SF9BW125, SF10BW125
};
#define SF_VECTOR_COUNT ((uint8_t)(sizeof(SFvector) / sizeof(SFvector[0])))
#define MAX_SF          ((uint8_t)(SF_VECTOR_COUNT - 1u))
uint8_t SF_actual    = 0;            //para saber cual se esta usando
uint8_t SF_is_set    = 0;            //para saber si esta seteado el SF
uint8_t SF_index     = 0;             //para movernos dentro del vector
uint8_t cont_same_SF = 0;         //para probar mas d euna vez cada SF

//variables para timeout
#define tick_time 100 //base de tiempo para el delay
#define timeout   80 //timeout * tick_time = tiempo de time out = 8 segundos

bool ack_waiting_async = false;
uint32_t ack_waiting_start_ms = 0;

static void configureAndSendUplink(char *data_to_send, uint8_t len, bool canal_por_defecto) {
  if (canal_por_defecto) {
    lora.setChannel(DEFAULT_CHANNEL);
  }
  else lora.setChannel(MULTI);
  
  if (FORCE_FIXED_SF_TEST) {
    uint8_t sf_fixed = FIXED_SF_INDEX;
    if (sf_fixed > MAX_SF) sf_fixed = MAX_SF;
    lora.setDataRate(SFvector[sf_fixed]);
    SF_actual = sf_fixed;
    SF_is_set = 1;
    if (dbspk) {
      Serial.print("-->SF FIJO TEST: "); Serial.println(sf_fixed);
    }
  }
  else if (canal_por_defecto) {
    lora.setDataRate(DEFAULT_SF);
  }
  else if (SF_is_set) {
    if(SF_actual > 0){
      uint8_t sf_ant = SF_actual - 1;
      lora.setDataRate(SFvector[sf_ant]);
      SF_actual = sf_ant;
      if (dbspk) {
        Serial.print("Reducir SF: "); Serial.println(SF_actual);
      }
    }
    else {
      lora.setDataRate(SFvector[SF_actual]);
      if (dbspk) {
        Serial.print("usar SF_actual: "); Serial.println(SF_actual);
      }
    }
  }
  else {              //no esta seteado el SF aun...
    lora.setDataRate(SFvector[SF_index]);
    SF_actual = SF_index;
    if (dbspk) {
      Serial.print("-->Test SF: "); Serial.println(SF_index);
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
}

uint8_t sendPackageAckAsyncStart(char *data_to_send, uint8_t len, bool canal_por_defecto) {
  if (ack_waiting_async) return 0;
  configureAndSendUplink(data_to_send, len, canal_por_defecto);
  ack_waiting_async = true;
  ack_waiting_start_ms = millis();
  return 1;
}

int8_t sendPackageAckAsyncPoll() {
  if (!ack_waiting_async) return 0;

  lora.update();
  uint8_t rta = lora.readAck();
  if (rta) {
    if (dbspk) {
      Serial.println("-->ack ok");
    }
    SF_is_set = 1;
    ack_waiting_async = false;
    return 1;
  }

  if ((millis() - ack_waiting_start_ms) >= (uint32_t)(timeout * tick_time)) {
    if (dbspk) {
      Serial.println("-->NO ACK, time out");
    }
    SF_is_set = 0;
    ack_waiting_async = false;
    return -1;
  }

  return 2;
}

bool isSendPackageAckAsyncWaiting() {
  return ack_waiting_async;
}

#if tipo_modulo == 0
//RFM95W + ESP12 (Macro version)
const sRFM_pins RFM_pins = {
  .CS   = 15,
  .RST  = 0,
  .DIO0 = 5,
  .DIO1 = 5,
  .DIO2 = 5,
  .DIO5 = -1,
};
#endif

#if tipo_modulo == 1
//SX1276 + ESP32 (TTGO Lora32)
const sRFM_pins RFM_pins = {
  .CS = 18,         //18
  .RST = 14,
  .DIO0 = 26,       //26
  .DIO1 = 34,       //34
  .DIO2 = 35,       //35
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

  configureAndSendUplink(data_to_send, len, canal_por_defecto);

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
        }
        SF_is_set = 1;
        //Elster_start();
        return 1; //llego el ack, con retur rompemos el while y salimos
      }
      else {
        if (dbspk) {
          Serial.println("-->NO ACK, time out");
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

// --- Recepcion LoRa: validacion de payload y log de depuracion (DEBUG_LORA_RX) ---

bool isValidLoraPayload(const char *buf) {
  if (buf == NULL) return false;
  if (buf[0] != 'L') return false;

  const char *comma = strrchr(buf, ',');
  if (comma == NULL) return false;

  const char *p = comma + 1;
  while (*p == ' ' || *p == '\t') p++;
  if (*p == '\0') return false;

  bool hasDigit = false;
  while (*p != '\0') {
    if (isdigit((unsigned char)*p)) hasDigit = true;
    else if (*p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') return false;
    p++;
  }
  return hasDigit;
}

#if DEBUG_LORA_RX
static void loraRxPrintRaw(const char *buf, uint8_t status) {
  Serial.print("LORA RAW status=");
  Serial.print(status);
  Serial.print(" data=<");
  for (uint8_t i = 0; i < INPUTBUFF && buf[i] != '\0'; i++) {
    char c = buf[i];
    if (c == '\r') Serial.print("\\r");
    else if (c == '\n') Serial.print("\\n");
    else if (isPrintable(c)) Serial.print(c);
    else {
      Serial.print("\\x");
      if ((uint8_t)c < 16) Serial.print("0");
      Serial.print((uint8_t)c, HEX);
    }
  }
  Serial.println(">");
}
#endif

void loraRxDebugAfterRead(const char *buf, uint8_t status, bool by_irq) {
#if DEBUG_LORA_RX
  if (status > 0) {
    if (by_irq) Serial.println("IRQ LORA");
    else Serial.println("POLL LORA RX");
    loraRxPrintRaw(buf, status);
  }
#else
  (void)buf;
  (void)status;
  (void)by_irq;
#endif
}

void loraRxDebugCtrlFrame(uint8_t status) {
#if DEBUG_LORA_RX
  if (status == 1) {
    Serial.println("LORA CTRL frame (sin payload)");
  }
#else
  (void)status;
#endif
}