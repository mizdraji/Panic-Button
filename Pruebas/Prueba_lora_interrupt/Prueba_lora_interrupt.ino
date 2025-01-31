#include <lorawan.h>

#define tipo_modulo 0 

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

// ABP Credentials
const char *devAddr = "0149cf9b";
const char *nwkSKey = "d8e5a8e01bddfaf48e059992119be745";
const char *appSKey = "3f19e1ef0da5286e178c5b84fc9e23ae";

char buffer_rx[255] = {0};
char uncero[1] = {0};
volatile bool packetReceived = false; // Bandera para verificar datos



// Función de interrupción
void IRAM_ATTR onReceive() {
  packetReceived = true; // Cambia bandera cuando hay un paquete recibido
}

void setup() {
  Serial.begin(115200);
  delay(5000);
  Serial.println("Start..");

  if (!lora.init()) {
    Serial.println("RFM95 not detected");
    delay(5000);
    return;
  }

  // Configura los parámetros de LoRaWAN
  lora.setDeviceClass(CLASS_C);
  lora.setDataRate(SF7BW125);
  lora.setChannel(CH0);
  lora.setNwkSKey(nwkSKey);
  lora.setAppSKey(appSKey);
  lora.setDevAddr(devAddr);
Serial.println("aca se va a enviar un cero");
  lora.sendUplink(uncero, strlen(uncero), 0, 1);

  // Configurar interrupción en el pin DIO0
  pinMode(digitalPinToInterrupt(RFM_pins.DIO0), INPUT);
  attachInterrupt(digitalPinToInterrupt(RFM_pins.DIO0), onReceive, RISING);
}

void loop() {
   int recvStatus;
  //  for (int i = 0; i < 10001 ; i++){
  //   if (i > 9999) attachInterrupt(digitalPinToInterrupt(RFM_pins.DIO0), onReceive, RISING);
  //  }
  if (packetReceived) {
    packetReceived = false; // Resetea la bandera

    // Leer el paquete recibido
    if (lora.readData(buffer_rx)) {
      Serial.print("Received: ");
      Serial.println(buffer_rx);
    }
  }



  // Llamar a la FSM de LoRa para manejar la red
  lora.update();


       // Leer el paquete recibido
  // recvStatus = lora.readData(buffer_rx);
  // if(recvStatus) {
  //   Serial.println(buffer_rx);
  // }
}
