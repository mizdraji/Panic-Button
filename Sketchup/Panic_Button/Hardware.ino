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

// Envia SMS al destino fijo SMS_NUMERO_DESTINO (configuracion.h).
void Enviar_msj(const char* msj) {
  if (msj == NULL) return;

  Serial.print("SMS TX -> ");
  Serial.println(msj);

  SIM800L.print(SMS_CMGS_CMD);
  delay(50);
  yield();
  SIM800L.print(msj);
  delay(50);
  yield();
  SIM800L.write((char)26);
  delay(50);
  yield();
}

// --- RX SMS linea a linea ---

static void trim_sms_line(char* line) {
  if (line == NULL) return;
  char* start = line;
  while (*start == '\r' || *start == '\n' || *start == ' ' || *start == '\t') {
    start++;
  }
  if (start != line) {
    memmove(line, start, strlen(start) + 1);
  }
  size_t n = strlen(line);
  while (n > 0 && (line[n - 1] == '\r' || line[n - 1] == '\n' || line[n - 1] == ' ')) {
    line[--n] = '\0';
  }
}

static bool sms_has_token(const char* line) {
  if (line == NULL || line[0] == '\0') return false;
  return (strstr(line, rcv_policia_sms)   != NULL ||
          strstr(line, rcv_bomberos_sms)  != NULL ||
          strstr(line, rcv_medica_sms)    != NULL ||
          strstr(line, rcv_atendido_sms)  != NULL ||
          strstr(line, rcv_informado_sms) != NULL);
}

static bool sms_is_echo_line(const char* line) {
  if (line == NULL || line[0] == '\0') return true;
  if (line[0] == '>') return true;
  if (line[0] == '+') return true;
  if (strncmp(line, "OK", 2) == 0) return true;
  if (strncmp(line, "RING", 4) == 0) return true;
  if (strstr(line, "AT+CMGS") != NULL) return true;
  return false;
}

static void procesar_sms_linea(const char* mensaje) {
  if (!sms_has_token(mensaje)) return;

  uint32_t numrcv = extraer_numero(mensaje);

  if (strstr(mensaje, rcv_atendido_sms) != NULL && numrcv == numsnt) {
    t_atendido.enable();
  }
  if ((strstr(mensaje, rcv_policia_sms)  != NULL ||
       strstr(mensaje, rcv_bomberos_sms) != NULL ||
       strstr(mensaje, rcv_medica_sms)   != NULL) &&
      numrcv == numsnt && !checknum) {
    checknum = true;
    t_recibido.enable();
  }
  if (strstr(mensaje, rcv_informado_sms) != NULL && numrcv == numsnt) {
    Tinformadorcv_Led.enable();
  }
}

void poll_sim800_messages() {
  static char acc[SMS_LINE_BUF_SIZE];
  static uint16_t len = 0;

  if (!SIM800L.available()) return;

  uint8_t n = 0;
  while (SIM800L.available() && n < SIM800_READ_CHUNK) {
    char c = SIM800L.read();
    n++;

    if (c == '\n') {
      acc[len] = '\0';
      len = 0;
      trim_sms_line(acc);
      if (acc[0] == '\0') continue;
#if DEBUG_GSM
      Serial.println(acc);
#endif
      if (!sms_is_echo_line(acc)) {
#if !DEBUG_GSM
        Serial.print("SMS RX -> ");
        Serial.println(acc);
#endif
        procesar_sms_linea(acc);
      }
      continue;
    }

    if (len < sizeof(acc) - 1) {
      acc[len++] = c;
    } else {
      while (SIM800L.available() && c != '\n') {
        c = SIM800L.read();
        n++;
        if (n >= SIM800_READ_CHUNK) break;
      }
      len = 0;
    }
    yield();
  }
}

void processPendingButtons() {
  static uint32_t last_btn1_ms = 0;
  static uint32_t last_btn2_ms = 0;
  static uint32_t last_btn3_ms = 0;
  uint32_t now = millis();

  if (btn1_pending) {
    btn1_pending = false;
    if (last_btn1_ms == 0 || (uint32_t)(now - last_btn1_ms) >= BTN_DEBOUNCE_MS) {
      last_btn1_ms = now;
      statebutton1 = true;
      t5.enableIfNot();
    }
  }
  if (btn2_pending) {
    btn2_pending = false;
    if (last_btn2_ms == 0 || (uint32_t)(now - last_btn2_ms) >= BTN_DEBOUNCE_MS) {
      last_btn2_ms = now;
      statebutton2 = true;
      t6.enableIfNot();
    }
  }
  if (btn3_pending) {
    btn3_pending = false;
    if (last_btn3_ms == 0 || (uint32_t)(now - last_btn3_ms) >= BTN_DEBOUNCE_MS) {
      last_btn3_ms = now;
      statebutton3 = true;
      t7.enableIfNot();
    }
  }
}

bool processPendingLora() {
  static uint32_t last_lora_irq_ms = 0;

  if (!lora_irq_pending) return false;
  lora_irq_pending = false;

  uint32_t now = millis();
  if (last_lora_irq_ms != 0 && (uint32_t)(now - last_lora_irq_ms) < LORA_IRQ_DEBOUNCE_MS) {
    return false;
  }
  last_lora_irq_ms = now;
  return true;
}

#define SIM800_AT_RESP_SIZE   128
#define SIM800_AT_TIMEOUT_MS  800

// Vacia RX del SIM800 (sin reenviar desde el monitor USB).
static void sim800_drain_rx(void) {
  while (SIM800L.available()) {
#if DEBUG_GSM
    Serial.write(SIM800L.read());
#else
    (void)SIM800L.read();
#endif
  }
}

// Envia AT, espera OK/ERROR con timeout y deja el UART limpio.
static bool sim800_send_at(const char* cmd) {
  if (cmd == NULL) return false;

  sim800_drain_rx();
  SIM800L.print(cmd);

#if DEBUG_GSM
  Serial.print(">> ");
  Serial.print(cmd);
#endif

  char resp[SIM800_AT_RESP_SIZE];
  uint16_t n = 0;
  uint32_t deadline = millis() + SIM800_AT_TIMEOUT_MS;

  while ((int32_t)(millis() - deadline) < 0) {
    while (SIM800L.available()) {
      char c = SIM800L.read();
#if DEBUG_GSM
      Serial.write(c);
#endif
      if (n < sizeof(resp) - 1) {
        resp[n++] = c;
        resp[n] = '\0';
      }
      if (strstr(resp, "OK") != NULL) return true;
      if (strstr(resp, "ERROR") != NULL) {
        Serial.print("SIM800 ERROR: ");
        Serial.println(cmd);
        return false;
      }
    }
    yield();
    delay(1);
  }

  Serial.print("SIM800 timeout: ");
  Serial.println(cmd);
  return false;
}

#if DEBUG_GSM
// Solo USB -> SIM800. El RX del modem lo consume poll_sim800_messages()
// (si Serialcom tambien lee SIM800L, se parte el SMS y no se procesa).
void Serialcom() {
  while (Serial.available()) {
    SIM800L.write(Serial.read());
  }
}
#endif

void ReceiveMode() {
  bool ok = true;
  ok &= sim800_send_at("AT\r");
  ok &= sim800_send_at("AT+CMGF=1\r");
  ok &= sim800_send_at("AT+CSCS=\"GSM\"\r");
  ok &= sim800_send_at("AT+CNMI=2,2,0,0,0\r");
  if (!ok) Serial.println("SIM800 SMS setup con errores");
  sim800_drain_rx();
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
        Serial.println("PDR OK");
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
            nodo.t_wait = UN_DIA;
          }
          else nodo.t_wait = LONG_TIME_TO_WAIT;
        }
        else {
          nodo.t_wait = random_time(MIN_RANDOM, MAX_RANDOM);
        }
        Serial.print("PDR FALLO, reintento en ");
        Serial.print(nodo.t_wait);
        Serial.println(" s");
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

// ISR: solo banderas. Antirebote y TaskScheduler en loop().
void IRAM_ATTR buttonInterrupt1() { btn1_pending = true; }
void IRAM_ATTR buttonInterrupt2() { btn2_pending = true; }
void IRAM_ATTR buttonInterrupt3() { btn3_pending = true; }
void IRAM_ATTR onReceive() { lora_irq_pending = true; }

// Genera un numero aleatorio de 8 digitos para idempotencia.
uint32_t idempotencia_random() {
    int32_t timestamp = esp_timer_get_time();                   // Obtener el timestamp actual en microsegundos
    srand((unsigned int)(timestamp));                           //Sembrar el generador de números aleatorios con el timestamp (opcional, para mayor variabilidad)
    uint32_t random_number = rand()  % 90000000 + 10000000;     // Generar un número aleatorio
    return random_number;
}

// Extrae numero de payload char[] del tipo "Lxx, 12345678".
// Tolera coma con o sin espacio.
uint32_t extraer_numero(const char* mensaje_completo) {
    const char *coma_pos = strrchr(mensaje_completo, ',');
    if (coma_pos != NULL) {
        const char *parte_numerica = coma_pos + 1;
        while (*parte_numerica == ' ' || *parte_numerica == '\t') {
          parte_numerica++;
        }
        return (uint32_t)strtoul(parte_numerica, NULL, 10);
    }
    return 0;
}

// Genera un numero aleatorio entre MIN y MAX.
uint16_t random_time(unsigned int MIN_,unsigned int MAX_) {
  return random(MIN_, MAX_);          //calcula un nuevo tiempo
}

void report_heap(bool force) {
  static uint32_t last_ms = 0;
  uint32_t now = millis();
  uint32_t free_now = ESP.getFreeHeap();
  uint32_t min_now = ESP.getMinFreeHeap();
  uint32_t maxblk = ESP.getMaxAllocHeap();
  bool warn = (free_now < HEAP_WARN_BYTES) || (min_now < HEAP_WARN_BYTES);
  bool periodic = (HEAP_REPORT_MS > 0) &&
                  (last_ms == 0 || (uint32_t)(now - last_ms) >= HEAP_REPORT_MS);

  if (!force && !warn && !periodic) return;

  last_ms = now;
  Serial.print("Heap free=");
  Serial.print(free_now);
  Serial.print(" min=");
  Serial.print(min_now);
  Serial.print(" maxblk=");
  Serial.println(maxblk);
  if (warn) Serial.println("Heap BAJO");
}