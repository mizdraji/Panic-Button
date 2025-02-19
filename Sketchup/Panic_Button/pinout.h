//configuracion de pines
#define led1          15     //led indicador que se presiono correctamente boton1
#define led2          2      //led indicador que se presiono correctamente boton2
#define led3          4      //led indicador que se presiono correctamente boton3

#define led_powerON   23     //led indicador de alimentacion/encendido del modulo
#define led_recibido  12      //led de confirmacion de mensaje recibido
#define led_atendido  22     //led de alerta atendida - enviado por operario en base central.

#define ADC_powerON   13

#define button1       36     //boton de policia
#define button2       38     //boton de bomberos
#define button3       39     //boton de ambulancia

//pines para sim800
#define TX            17     //GPIO17 HELTEC - pines para sim800 - Se conecta a RX de sim800
#define RX            16     //GPIO16 HELTEC - pines para sim800 - Se conecta a TX de sim800
//#define SIMRST 2          //GPIO2 HELTEC