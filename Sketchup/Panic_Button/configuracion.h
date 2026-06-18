// Evita dobles inclusiones del header en la misma unidad de compilacion.
#pragma once

//Version
#define VERSION       "V1.8.9"

//Serial
#define SERIAL_SPEED  115200
#define SERIAL_SIM    9600

//LoraTec
#define tipo_modulo     1                     //Es para definir el pinout de SPI con el modulo Lora. 0: ESP12, 1: ESP32
#define RANDOM_CHANNEL  1
#define DEFAULT_CHANNEL CH0
#define DEFAULT_SF      SF7BW125               // Set Data Rate
#define DEBUG           0 
#define dbspk           1                      //lora send pakage
#define DEBUG_LORA_RX   0                      //1=diagnostico RX LoRa, 0=normal
#define FORCE_FIXED_SF_TEST 0                  //1=fijar SF para ensayos, 0=logica normal/adaptativa
#define FIXED_SF_INDEX  0                      //AU915: indice en SFvector: 0=SF7, 1=SF8, 2=SF9, 3=SF10 (max 125 kHz)

//configuracion ensayo
#define MODO_ENSAYO      0                       //1 = habilita modo ensayo, 0 = modo normal
#define ENSAYO_INCLUIR_SMS  0                    //1 = incluye SMS en el ensayo, 0 = solo LoRa  
#define ENSAYO_INTERVALO_MS 42000UL             //5 minutos = 300000UL
#define ENSAYO_DURACION_HS 1UL                  //48 horas - 24 horas
#define ENSAYO_TOTAL_MENSAJES ((ENSAYO_DURACION_HS * 60UL * 60UL * 1000UL) / ENSAYO_INTERVALO_MS) //576 mensajes - 288 mensajes

//configuracion GSM numeros de remitentes
struct remitente {
String Remitente1 = "3794003039";    //NUMERO REMITENTE 1
String Remitente2 = "3795572356";    //NUMERO REMITENTE 2
String Remitente3 = "";              //NUMERO REMITENTE 3
};
remitente numero;

//mensajes para enviar y recibir por sms
//struct mensajes {
  String policia          = "policia";              //mensaje1 para enviar
  String bomberos         = "bomberos";             //mensaje2 para enviar
  String medica           = "medica";               //mensaje3 para enviar
  String rcv_policia      = "policiarcv";           //policia recibido confirmacion
  String rcv_bomberos     = "bomberosrcv";          //bomberos recibido confirmacion
  String rcv_medica       = "medicarcv";            //medica recibido confirmacion
  String rcv_atendido     = "atendidorcv";          //Respuesta de operario, alerta atendida
  String rcv_informado    = "informadorcv";         //Informadorcv cuando se da aviso a la autoridad correspondiente.
  String rcv_cerrado      = "cerradorcv";           // es cuando se da por finalizada la alerta, pero puede pasar mucho tiempo
//};
//mensajes msj;

//mensajes para enviar y recibir por lora
  char policia_lora[]     = "Lp";             //envio: policia
  char bomberos_lora[]    = "Lb";             //envio: bomberos
  char medica_lora[]      = "Lm";             //envio: medica
  char policiarcv_lora[]  = "Lpr";            //rx: policia recibido
  char bomberosrcv_lora[] = "Lbr";            //rx: bomberos recibido
  char medicarcv_lora[]   = "Lmr";            //rx: medica recibido
  char atendidorcv_lora[] = "Lar";            //rx: atendido recibido
  char informadorcv_lora[]= "Lir";            //rx: informado recibido