//PRUEBA FUNCIONAL PULSADORES

#define button1 36
#define button2 38
#define button3 39

bool statebutton1 = false;
bool statebutton2 = false;
bool statebutton3 = false;

void IRAM_ATTR buttonInterrupt1() {           //interrupcion pulsador1
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) 
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    statebutton1 = true;
  }
  last_interrupt_time = interrupt_time;
}

void IRAM_ATTR buttonInterrupt2() {           //interrupcion pulsador2
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) 
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    statebutton2 = true;
  }
  last_interrupt_time = interrupt_time;
 
}

void IRAM_ATTR buttonInterrupt3() {           //interrupcion pulsador3
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) 
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    statebutton3 = true;
  }
  last_interrupt_time = interrupt_time;
}


void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("Iniciando....");

  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);

  pinMode(LED_BUILTIN, OUTPUT);

    //config interrupt
  attachInterrupt(digitalPinToInterrupt(button1), buttonInterrupt1, RISING);            //habilita interrupcion pulsador1 con flanco ascendente
  attachInterrupt(digitalPinToInterrupt(button2), buttonInterrupt2, RISING);            //habilita interrupcion pulsador2 con flanco ascendente
  attachInterrupt(digitalPinToInterrupt(button3), buttonInterrupt3, RISING);            //habilita interrupcion pulsador3 con flanco ascendente
}

void loop() {
//print push button
  if(statebutton1){
    statebutton1 = false;
    Serial.println("se presiono el boton1");
  } 
  if(statebutton2) {
    statebutton2 = false;
    Serial.println("se presiono el boton2");
    }
  if(statebutton3) {
    statebutton3 = false;
    Serial.println("se presiono el boton3");
  }
}

//desventaja: no funciona con interrupciones
//Para nuestro ejemplo se uso GPIO en serie con res10k y "R=10k, C=100nF" a masa. el otro extremo conectado a VCC. Funcionamiento OK.
//SIMPLIFICACION:
//botón conectado entre un pin GPIO y tierra. No se requiere ningún otro componente, deberá configurar el pin GPIO con resistencia interna en INPUT_PULLUP para que el pin GPIO permanezca ALTO cuando no se presione el botón.
bool debounce(int button) {
  static uint16_t state = 0;
  state = (state<<1) | digitalRead(button) | 0xfe00;
  return (state == 0xff00);
}
