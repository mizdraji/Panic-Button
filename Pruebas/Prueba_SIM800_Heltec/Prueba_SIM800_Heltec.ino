#include <SoftwareSerial.h>

//pines para sim800
#define TX 17         //GPIO17 HELTEC
#define RX 16         //GPIO16 HELTEC
//#define SIMRST 2      //GPIO2 HELTEC

#define button1 37


SoftwareSerial SIM800L(RX, TX);   //RX y TX de heltec

String Numero_Remitente = "3794003039";

bool statebutton1 = LOW;

void setup(){

pinMode(button1, INPUT);

SIM800L.begin(9600);
Serial.begin(115200);

SIM800L.println("AT");

Enviar_msj(Numero_Remitente, "Inicializacion completa");
}

void loop(){

statebutton1 = digitalRead(button1);
Serial.print("statebutton1: ");Serial.println(statebutton1);
  if(statebutton1) { 
    
    Enviar_msj(Numero_Remitente, "Se presiono el boton de policia!! Auxilio!!"); 
    
    }

delay(1000);


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