// Evita dobles inclusiones del header en la misma unidad de compilacion.
#pragma once

//Version
#define VERSION       "V2.0"

// 1 = demo: sin deep sleep, firmware maximo robustez
#define MODO_DEMO        1
#define DEMO_SMS_DELAY_MS  1500UL   // pausa LoRa->SMS para evitar pico SIM800 + INT_WDT

//Serial
#define SERIAL_SPEED  115200
#define SERIAL_SIM    9600

//LoraTec
#define tipo_modulo     1                     //Es para definir el pinout de SPI con el modulo Lora. 0: ESP12, 1: ESP32
#define RANDOM_CHANNEL  1
#define DEFAULT_CHANNEL CH0
#define DEFAULT_SF      SF7BW125               // Set Data Rate
#define DEBUG_GSM       0                      //1=passthrough Serial<->SIM800 (comandos AT)
#define HEAP_REPORT_MS     300000UL            // 0=solo arranque; 300000=cada 5 min
#define HEAP_WARN_BYTES    20480UL             // aviso si free o min bajan de 20 KB
#define FORCE_FIXED_SF_TEST 1                  //1=fijar SF para ensayos, 0=logica normal/adaptativa
#define FIXED_SF_INDEX  3                      //AU915: indice en SFvector: 0=SF7, 1=SF8, 2=SF9, 3=SF10 (max 125 kHz)
// PDR de arranque ignora FORCE_FIXED_SF_TEST y usa DEFAULT_SF (SF7) para el ACK.
// Botones/ensayo ya usan el SF fijo. RX Class C: LORATEC_RX_FIXED_SF7 en Config.h
// 1 = escuchar siempre SF7/500 (LoraTec). 0 = RX1 (SF uplink + 10) para ensayar bajada SF10.

//configuracion ensayo
#define MODO_ENSAYO      0                       //1 = habilita modo ensayo, 0 = modo normal
#define ENSAYO_INCLUIR_SMS  1                    //1 = incluye SMS en el ensayo, 0 = solo LoRa  
#define ENSAYO_INTERVALO_MS 300000UL             //5 minutos = 300000UL
#define ENSAYO_DURACION_HS 24UL                  //48 horas - 24 horas
#define ENSAYO_TOTAL_MENSAJES ((ENSAYO_DURACION_HS * 60UL * 60UL * 1000UL) / ENSAYO_INTERVALO_MS) //576 mensajes - 288 mensajes

// Destino SMS (sin prefijo +549). Elegir 1 o 2; Enviar_msj/queueSms no reciben numero.
#define SMS_REMITENTE_1     "3794003039"
#define SMS_REMITENTE_2     "3795572356"
#define SMS_USAR_REMITENTE  2

#if SMS_USAR_REMITENTE == 1
#define SMS_NUMERO_DESTINO  SMS_REMITENTE_1
#elif SMS_USAR_REMITENTE == 2
#define SMS_NUMERO_DESTINO  SMS_REMITENTE_2
#else
#error "SMS_USAR_REMITENTE debe ser 1 o 2"
#endif
#define SMS_CMGS_CMD        "AT+CMGS=\"+549" SMS_NUMERO_DESTINO "\"\r\n"

// textos SMS (flash, sin heap)
  const char policia[]           = "policia";
  const char bomberos[]          = "bomberos";
  const char medica[]            = "medica";
  const char rcv_policia_sms[]   = "policiarcv";
  const char rcv_bomberos_sms[]  = "bomberosrcv";
  const char rcv_medica_sms[]    = "medicarcv";
  const char rcv_atendido_sms[]  = "atendidorcv";
  const char rcv_informado_sms[] = "informadorcv";

//mensajes para enviar y recibir por lora (flash)
  const char policia_lora[]      = "Lp";
  const char bomberos_lora[]     = "Lb";
  const char medica_lora[]       = "Lm";
  const char policiarcv_lora[]   = "Lpr";
  const char bomberosrcv_lora[]  = "Lbr";
  const char medicarcv_lora[]    = "Lmr";
  const char atendidorcv_lora[]  = "Lar";
  const char informadorcv_lora[] = "Lir";