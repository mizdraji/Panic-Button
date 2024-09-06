#include <SoftwareSerial.h>

//pines para sim800
#define TX 17  //GPIO17 HELTEC
#define RX 16  //GPIO16 HELTEC
//#define SIMRST 2      //GPIO2 HELTEC

#define button1 37
#define button2 38
#define button3 39


SoftwareSerial SIM800L(RX, TX);  //RX y TX de heltec

String Numero_Remitente = "3794003039";

//char Received_SMS;                     //Here we store the full received SMS (with phone sending number and date/time) as char

struct str {
  String policia = "ayuda policia!!";
  String bomberos = "ayuda bomberos!!";
  String ambulancia = "ayuda ambulancia!!";
};
str msj;

bool statebutton1 = LOW;
bool statebutton2 = LOW;
bool statebutton3 = LOW;

void setup() {

  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);

  SIM800L.begin(9600);
  Serial.begin(115200);
  delay(5000);
  
  //configurar modulo como modo SMS
  Serial.println("iniciando ...");
  ReceiveMode();

  Enviar_msj(Numero_Remitente, "Inicializacion completa");
}

void loop() { 

  while(SIM800L.available()>0){
    char Received_SMS; 
    Received_SMS=SIM800L.read();                  //"char Received_SMS" is now containing the full SMS received
    Serial.print(Received_SMS);                   //Show it on the serial monitor (optional)  - 
  }
                 
  

  statebutton1 = digitalRead(button1);
  statebutton2 = digitalRead(button2);
  statebutton3 = digitalRead(button3);
  
  if (statebutton1) {
    Serial.println("button1");
    Enviar_msj(Numero_Remitente, msj.policia);
  }

  if (statebutton2) {
    Serial.println("button2");

    Enviar_msj(Numero_Remitente, msj.bomberos);
  }
  if (statebutton3) {
    Serial.println("button3");
    Enviar_msj(Numero_Remitente, msj.ambulancia);
  }

  delay(1000);
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