# Boton de Panico.

## Hardware:
* Heltec Lora Wifi V2
* Módulo SIM800L
*******************************
## GPIO Utilizados:
* GPIO34 Reservado Lora -> DIO2      //se pueden conectar juntos a traves de diodos shotky para ir a un solo GPIO, cuando no se usa heltec.
* GPIO35 Reservado Lora -> DIO1      //se pueden conectar juntos a traves de diodos shotky para ir a un solo GPIO, cuando no se usa heltec.
* GPIO26 Reservado Lora -> DIO0      //se pueden conectar juntos a traves de diodos shotky para ir a un solo GPIO, cuando no se usa heltec.
* GPIO27 Reservado Lora -> MOSI
* GPIO14 Reservado Lora -> RST
* GPIO19 Reservado Lora -> MISO
* GPIO18 Reservado Lora -> CS
* GPIO5  Reservado Lora -> SCK
* *****************************
* GPIO16 RX SIM800L
* GPIO17 TX SIM800L
* GPIO21 DTR SIM800L (sleep/wake del modulo GSM)
*******************************
* GPIO36 button1 - pulsador con resistencia pull-down.
* GPIO38 button2 - pulsador con resistencia pull-down.
* GPIO39 button3 - pulsador con resistencia pull-down.
* *****************************
* GPIO15 led1 - led indicador que se presiono correctamente boton1.
* GPIO2  led2 - led indicador que se presiono correctamente boton2.
* GPIO4  led3 - led indicador que se presiono correctamente boton3.
*******************************
* GPIO23 led_powerON	- led indicador de alimentacion/encendido del modulo.
* GPIO12 led_recibido	- led de confirmacion de mensaje recibido.
* GPIO22 led_atendido	- led de alerta atendida - enviado por operario en base central.
*******************************
* GPIO13 ADC_powerON	- entrada ADC para medir alimentacion usb +5V.
*******************************
## ORDEN DE ENVIO (V1.8.9)
Al presionar cualquier boton (policia, bomberos, medica) o en modo ensayo con SMS habilitado: **primero LoRa, luego SMS**.

*******************************
## MENSAJES ENVIADOS SMS
* String "policia"              //mensaje1 para enviar.
* String "bomberos"             //mensaje2 para enviar.
* String "medica"               //mensaje3 para enviar.
*******************************
## MENSAJES RECIBIDOS SMS
* String "policiarcv"           //policia recibido confirmacion - enviado de servidor de forma automática.
* String "bomberosrcv"          //bomberos recibido confirmacion - enviado de servidor de forma automática.
* String "medicarcv"            //medica recibido confirmacion - enviado de servidor de forma automática.
* String "atendidorcv"          //Respuesta de operario, alerta atendida - enviado por operario de forma manual.
* String "informadorcv"         //informadorcv cuando se da aviso a la policia. Activa un temporizador para apagar todos los leds.
* String "cerradorcv"           //Es cuando se da por finalizada la alerta, pero puede pasar mucho tiempo. Todavia sin funcionalidad en el módulo.
*******************************
## MENSAJES ENVIADOS LORA
Formato: `<token>, <idempotencia>` (tokens compactos desde V1.8.8).

* char "Lp"   //policia
* char "Lb"   //bomberos
* char "Lm"   //medica
* char "Le"   //modo ensayo (contador + timestamp)
*****************************
## MENSAJES RECIBIDOS LORA
* char "Lpr"  //policia recibido confirmacion - enviado de servidor de forma automatica.
* char "Lbr"  //bomberos recibido confirmacion - enviado de servidor de forma automatica.
* char "Lmr"  //medica recibido confirmacion - enviado de servidor de forma automatica.
* char "Lar"  //Respuesta de operario, alerta atendida - enviado por operario de forma manual.
* char "Lir"  //informadorcv cuando se da aviso a la autoridad correspondiente.
***********************
## COMUNICACION GSM
* UART: **HardwareSerial UART2** (9600 baud, `SERIAL_8N1`) — reemplaza `SoftwareSerial` desde V1.8.9.
* Sleep del SIM800: pin **DTR** (GPIO21) + comando `AT+CSCLK=1` antes de `esp_deep_sleep_start()`.
* Wake: DTR en LOW + `AT+CSCLK=0` al despertar por boton o alimentacion.
* Deep sleep: tras `tiempo` segundos sin actividad (`#define tiempo` en `Task.h`; **20 s en pruebas**, **120 s o mas en produccion**). Los botones GPIO36/38/39 despiertan el ESP32.
***********************
* Full Document TaskScheduler:
* https://github.com/arkhipenko/TaskScheduler/wiki/Full-Document

