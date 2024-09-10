//Version
#define VERSION "V1.4"

//Serial
#define SERIAL_SPEED  115200
#define SERIAL_SIM    9600

//LoraTec
#define tipo_modulo 1                     //Es para definir el pinout de SPI con el modulo Lora. 0: ESP12, 1: ESP32
#define RANDOM_CHANNEL 1
#define DEFAULT_CHANNEL CH0
#define DEFAULT_SF SF7BW125               // Set Data Rate
#define DEBUG 0 
#define dbspk 1                           //lora send pakage

//configuracion GSM numeros de remitentes
struct remitente {
String Remitente1 = "3794003039";    //NUMERO REMITENTE 1
String Remitente2 = "";              //NUMERO REMITENTE 2
String Remitente3 = "";              //NUMERO REMITENTE 3
};
remitente numero;

//mensajes para enviar por sms
struct mensajes {
  String policia  = "policia";              //mensaje1
  String bomberos = "bomberos";             //mensaje2
  String medica   = "medica";               //mensaje3
};
mensajes msj;