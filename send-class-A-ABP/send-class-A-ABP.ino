


#include <lorawan.h>

#define led 16
#define button 4

//ABP Credentials
const char *devAddr = "0149cf9b";
const char *nwkSKey = "d8e5a8e01bddfaf48e059992119be745";
const char *appSKey = "3f19e1ef0da5286e178c5b84fc9e23ae";

const unsigned long interval = 2000;  // 2 s interval to send message
uint32_t previousMillis = 0;          // will store last time message sent
unsigned int counter = 0;             // message counter

char myStr[50];
char outStr[255];
byte recvStatus = 0;

bool button_state = false;

//RFM95W + ESP12 (Macro version)
const sRFM_pins RFM_pins = {
  .CS = 15,
  .RST = 0,
  .DIO0 = 5,
  .DIO1 = 5,
  .DIO2 = 5,
  .DIO5 = -1,
};

void IRAM_ATTR panic_show(void) {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 1000) {
    digitalWrite(led, HIGH);
    sprintf(myStr, "Counter-%d", counter);

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
  pinMode(led, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(button), panic_show, RISING);

  // Set LoRaWAN Class change CLASS_A or CLASS_C
  lora.setDeviceClass(CLASS_C);

  // Set Data Rate
  lora.setDataRate(SF7BW125);

  // set channel to random
  lora.setChannel(MULTI);

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
    digitalWrite(led, LOW);
  }


  recvStatus = lora.readData(outStr);
  if (recvStatus) {
    Serial.println(outStr);
  }

  // Check Lora RX
  lora.update();
}
