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

//configuracion GSM
String Numero_Remitente = "3794003039";     //NUMERO REMITENTE 1
String Numero_Remitente2 = "";              //NUMERO REMITENTE 2
String Numero_Remitente3 = "";              //NUMERO REMITENTE 3

struct mensajes {
  String policia = "ayuda policia!!";
  String bomberos = "ayuda bomberos!!";
  String ambulancia = "ayuda ambulancia!!";
};
mensajes msj;