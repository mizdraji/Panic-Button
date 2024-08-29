#include <SoftwareSerial.h>

#define TX 17         //GPIO17 HELTEC
#define RX 16         //GPIO16 HELTEC

#define SIMRST 2      //GPIO2 HELTEC

SoftwareSerial SIM800L(RX, TX);   //RX y TX de heltec

String Numero_Remitente = "3794003039";

void setup(){

SIM800L.begin(9600);
Serial.begin(115200);

SIM800L.println("AT");

Enviar_msj(Numero_Remitente, "Inicializacion completa");
}

void loop(){

}

void Enviar_msj(String numero, String msj)
{
  //Se establece el formato de SMS en ASCII
  String config_numero = "AT+CMGS=\"+549" + numero + "\"\r\n";
  Serial.println(config_numero);

  //configurar modulo como modo SMS
  SIM800L.write("AT+CMGF=1\r\n");
  delay(1000);
  
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