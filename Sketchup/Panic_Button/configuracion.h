//Version
#define VERSION "V1.6"

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

//mensajes para enviar y recibir por sms
struct mensajes {
  String policia          = "policia";              //mensaje1 para enviar
  String bomberos         = "bomberos";             //mensaje2 para enviar
  String medica           = "medica";               //mensaje3 para enviar
  String rcv_policia      = "policiarcv";           //policia recibido confirmacion
  String rcv_bomberos     = "bomberosrcv";          //bomberos recibido confirmacion
  String rcv_medica       = "medicarcv";            //medica recibido confirmacion
  String rcv_atendido     = "atendidorcv";          //Respuesta de operario, alerta atendida
};
mensajes msj;

//mensajes para enviar y recibir por lora
  char policia_lora[]     = "Lpolicia";
  char bomberos_lora[]    = "Lbomberos";
  char medica_lora[]      = "Lmedica";
  char policiarcv_lora[]  = "Lpoliciarcv";
  char bomberosrcv_lora[] = "Lbomberosrcv";
  char medicarcv_lora[]   = "Lmedicarcv";
  char atendidorcv_lora[] = "Latendidorcv";
  
