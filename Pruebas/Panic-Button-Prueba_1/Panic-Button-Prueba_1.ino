#include <lorawan.h>
#include "ABPcredentials.h"
#include "config_pines.h"

const unsigned long interval = 2000;  // 2 s interval to send message
uint32_t previousMillis = 0;          // will store last time message sent
unsigned int counter = 0;             // message counter

char myStr[50];
char outStr[255];
byte recvStatus = 0;

int count = 0;
bool button_state = false;

void IRAM_ATTR panic_show(void) {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 1000ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 1000) {
    digitalWrite(led2, HIGH);
    
    sprintf(myStr, "C%d", counter);

    Serial.print("Sending: ");
    Serial.println(myStr);

    lora.sendUplink(myStr, strlen(myStr), 0, 1);
    counter++;
  }
}

void setup() {
  Serial.begin(115200);
  delay(5000);
  if (!lora.init()) {
    Serial.println("RFM95 not detected");
    while (1)
      ;
  }
  Serial.println("RFM95 detected");

  //configure pines
  pinMode(button, INPUT_PULLUP);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(button), panic_show, RISING);

  // Set LoRaWAN Class change CLASS_A or CLASS_C
  lora.setDeviceClass(CLASS_C);

  // Set Data Rate
  lora.setDataRate(SF7BW125);

  // set channel to random
  lora.setChannel(CH0);

  // Put ABP Key and DevAddress here
  lora.setNwkSKey(nwkSKey);
  lora.setAppSKey(appSKey);
  lora.setDevAddr(devAddr);

  previousMillis = millis();
}

void loop() {
  // Check interval overflow
  if (millis() - previousMillis > interval) {
    previousMillis = millis();
    //digitalWrite(led1, LOW);
    count++;
    if (count > 3) {
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      count = 0;
    }
  }
  //check data recibe
  recvStatus = lora.readData(outStr);
  if (recvStatus) {
    digitalWrite(led1, HIGH);
    Serial.print("====>> ");
    Serial.println(outStr);
  }

  // Check Lora RX
  lora.update();
}
