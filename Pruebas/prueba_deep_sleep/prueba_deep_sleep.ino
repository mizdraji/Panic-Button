#define pin 39

uint timer = 0;        //contador de tiempo
uint tiempo = 10;      //tiempo en el que quiero que duerma en segundos
int intervalo = 1000;  //intervalo de tiempo en milisegundos, 1000 = 1 segundo
int32_t prevMillis = 0;
bool estado;  //analiza el estado de pin

RTC_DATA_ATTR int bootCount = 0;    //contador de las veces que vuelve del sleep mode

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(pin, INPUT_PULLUP);
  

  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  //Configure GPIO39 as ext0 wake up source for LOW logic level
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_39, 0);  //la interrupcion para el wakeup se va a generar en este pin, colocar en 1 para wakeup con HIGH Logic level

  //Print the wakeup reason for ESP32
  print_wakeup_reason();


  prevMillis = millis();
}

void loop() {
  estado = digitalRead(pin);

  if ((millis() - prevMillis) > intervalo) {
    Serial.print("estado: ");Serial.println(estado);
    prevMillis = millis();
    timer++;
    Serial.print("timer: ");
    Serial.println(timer);
  }

  if (estado == LOW) timer = 0;     //si se produce una interrupcion resetear contador timer para no entrar al modo sleep

  if (estado == HIGH && timer > tiempo) {
    Serial.println("Going to sleep now");
    esp_deep_sleep_start();
  }

  
}

//Function that prints the reason by which ESP32 has been awaken from sleep
void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch (wakeup_reason) {
    case 1: Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2: Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3: Serial.println("Wakeup caused by timer"); break;
    case 4: Serial.println("Wakeup caused by touchpad"); break;
    case 5: Serial.println("Wakeup caused by ULP program"); break;
    default: Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}