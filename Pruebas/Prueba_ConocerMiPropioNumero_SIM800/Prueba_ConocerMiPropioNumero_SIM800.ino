#include <SoftwareSerial.h>

String Remitente1 = "611";    //NUMERO REMITENTE 1
//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(16, 17); //SIM800L Tx & Rx is connected to Arduino #3 & #2
void Enviar_msj(String numero, String msj);
void ReceiveMode();
void Serialcom() ;

String mensaje_recibido = "";

void setup()
{
//Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
Serial.begin(115200);

//Begin serial communication with Arduino and SIM800L
mySerial.begin(9600);

Serial.println("Initializing...");
delay(1000);
  // Configurar el modo de texto para SMS
  mySerial.println("AT+CMGF=1");
  delay(1000);

   mySerial.println("AT+CSCS=\"GSM\"");
  delay(1000);

  // Iniciar el envío del SMS al número 611
  mySerial.println("AT+CMGS=\"611\"");
  delay(1000);

  // Escribir el mensaje "NUMERO"
  mySerial.print("NUMERO");

  // Enviar Ctrl+Z (ASCII 0x1A) para finalizar el mensaje
  mySerial.write(0x1A);

  updateSerial();

  delay(3000);

  mensaje_recibido = mySerial.readString();

  Serial.println(mensaje_recibido);
}

void loop()
{
updateSerial();

}


void updateSerial()
{
delay(500);
while (Serial.available())
{
mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
}
while(mySerial.available())
{
Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
}
}


//Función para enviar mensaje SMS
void Enviar_msj(String numero, String msj) {
  //Se establece el formato de SMS en ASCII
  //String config_numero = "AT+CMGS=\"+549" + numero + "\"\r\n";
  Serial.println(numero);

  //configurar modulo como modo SMS
  mySerial.write("AT+CMGF=1\r\n");
  delay(10);
  
  //Enviar comando para un nuevos SMS al numero establecido
  mySerial.print(numero);
  delay(10);

  //Enviar contenido del SMS
  mySerial.print(msj);
  delay(10);

  //Enviar Ctrl+Z
  mySerial.write((char)26);
  delay(10);
  Serial.println("Mensaje enviado");
}

//Set the SIM800L Receive mode  
void ReceiveMode() {       
  mySerial.println("AT"); //If everything is Okay it will show "OK" on the serial monitor
  delay(500);
  Serialcom();
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  delay(500);
  Serialcom();
  mySerial.println("AT+CNMI=2,2,0,0,0"); //Configure the SIM800L on how to manage the Received SMS... Check the SIM800L AT commands manual
  delay(500);
  Serialcom();
}

//This is used with ReceiveMode function, it's okay to use for tests with Serial monitor
void Serialcom() {      
  //delay(500);
  while(Serial.available()) {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}