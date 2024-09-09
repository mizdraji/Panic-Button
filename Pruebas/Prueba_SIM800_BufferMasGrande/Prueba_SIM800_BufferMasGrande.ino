//Prueba aumento de buffer para recibir mensajes mas largos

#include <SoftwareSerial.h>

//pines para sim800
#define TX 17  //GPIO17 HELTEC
#define RX 16  //GPIO16 HELTEC

String Numero_Remitente = "3794003039";

SoftwareSerial SIM800L(RX, TX); // RX, TX

const int BUFFER_SIZE = 128;
char buffer[BUFFER_SIZE];
int bufferIndex = 0;

void setup() {
  Serial.begin(115200);
  SIM800L.begin(9600);
  delay(5000);

  //configurar modulo como modo SMS
  Serial.println("iniciando ...");

  Enviar_msj(Numero_Remitente, "Inicializacion completa");

}

void loop() {
  while (SIM800L.available() > 0) {
    char receivedChar = SIM800L.read();
    if (bufferIndex < BUFFER_SIZE - 1) {
      buffer[bufferIndex] = receivedChar;
      bufferIndex++;
    } else {
      // Buffer full, handle or process the received data here
      bufferIndex = 0; // Reset buffer index
    }
  }

  if (bufferIndex > 0) {
    buffer[bufferIndex] = '\0'; // Null-terminate the buffer
    Serial.print(buffer);
    bufferIndex = 0; // Reset buffer index after processing
  }
}

void Serialcom() {      //This is used with ReceiveMode function, it's okay to use for tests with Serial monitor
  //delay(500);
  while(Serial.available()) {
    SIM800L.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(SIM800L.available()) {
    Serial.write(SIM800L.read());//Forward what Software Serial received to Serial Port
  }
}

void ReceiveMode() {       //Set the SIM800L Receive mode  
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

void Enviar_msj(String numero, String msj) {
  //Se establece el formato de SMS en ASCII
  String config_numero = "AT+CMGS=\"+549" + numero + "\"\r\n";
  Serial.println(config_numero);

  //Enviar comando para un nuevos SMS al numero establecido
  SIM800L.print(config_numero);
  delay(1000);

  //Enviar contenido del SMS
  SIM800L.print(msj);
  delay(1000);

  //Enviar Ctrl+Z
  SIM800L.write((char)26);
  delay(1000);
  Serial.println("Mensaje enviado");
}