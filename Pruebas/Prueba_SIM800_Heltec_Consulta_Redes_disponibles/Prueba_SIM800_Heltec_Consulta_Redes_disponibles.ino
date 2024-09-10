#include <SoftwareSerial.h>

//pines para sim800
#define TX 17  //GPIO17 HELTEC
#define RX 16  //GPIO16 HELTEC
//#define SIMRST 2      //GPIO2 HELTEC

SoftwareSerial SIM800L(RX, TX);  //RX y TX de heltec

void setup() {

  SIM800L.begin(9600);
  Serial.begin(115200);
  delay(5000);
  
  delay(1000);
  Serial.println("Iniciando consulta de redes...");

  // Enviar comando para listar las redes disponibles
  enviarComandoAT("AT+COPS=?");             //Este comando busca y lista las redes disponibles en el área.

//Formato de respuesta:
//+COPS: (estado, "nombre largo", "nombre corto", "código mcc+mnc", tecnología)
/*Estado: 0 = desconocido, 1 = disponible, 2 = actualmente registrado, 3 = no disponible.
  Nombre largo: Nombre largo del operador.
  Nombre corto: Nombre corto del operador.
  Código mcc+mnc: Identificación del operador (Mobile Country Code + Mobile Network Code).
  Tecnología: 0 = GSM, 1 = GSM Compact, 2 = UMTS (3G).
*/

}

void loop() { 

  // Leer la respuesta del SIM800 y mostrarla en el monitor serie
  if (SIM800L.available()) {
    String respuesta = SIM800L.readString();
    Serial.println(respuesta);
  }
}
void enviarComandoAT(String comando) {
  SIM800L.println(comando);
  delay(1000); // Esperar para dar tiempo a la respuesta
  while (SIM800L.available()) {
    Serial.write(SIM800L.read()); // Mostrar respuesta en el puerto serie
  }
}