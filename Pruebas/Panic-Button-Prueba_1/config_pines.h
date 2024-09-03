//configuracion de pines
#define led1 16           //led indicador que se presiono correctamente boton1
#define led2 17           //led indicador que se presiono correctamente boton2
#define led3 4            //led indicador que se presiono correctamente boton3
#define button1 37        //boton de policia
#define button2 38        //boton de bomberos
#define button3 39        //boton de ambulancia

#define tipo_modulo 1

#if tipo_modulo == 0
//RFM95W + ESP12 (Macro version)
const sRFM_pins RFM_pins = {
  .CS = 15,
  .RST = 0,
  .DIO0 = 5,
  .DIO1 = 5,
  .DIO2 = 5,
  .DIO5 = -1,
};
#endif

#if tipo_modulo == 1
//SX1276 + ESP32 (TTGO Lora32)
const sRFM_pins RFM_pins = {
  .CS = 18,
  .RST = 14,
  .DIO0 = 26,
  .DIO1 = 33,
  .DIO2 = 32,
  .DIO5 = -1,
};
#endif
