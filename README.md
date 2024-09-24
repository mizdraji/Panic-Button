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
*******************************
## MENSAJES ENVIADOS LORA
* char "Lpolicia"	- Base64: THBvbGljaWE=	//mensaje1 para enviar por lora.
* char "Lbomberos"	- Base64: TGJvbWJlcm9z 	//mensaje1 para enviar por lora.
* char "Lmedica"	- Base64: TG1lZGljYQ==	//mensaje1 para enviar por lora.
*****************************
## MENSAJES RECIBIDOS LORA
* char "Lpoliciarcv"	- Base64: THBvbGljaWFyY3Y=	//policia recibido por lora confirmacion - enviado de servidor de forma automática.
* char "Lbomberosrcv" 	- Base64: TGJvbWJlcm9zcmN2	//bomberos recibido por lora confirmacion - enviado de servidor de forma automática.
* char "Lmedicarcv"		- Base64: TG1lZGljYXJjdg==	//medica recibido por lora confirmacion - enviado de servidor de forma automática.
* char "Latendidorcv"	- Base64: TGF0ZW5kaWRvcmN2	//Respuesta por lora de operario, alerta atendida - enviado por operario de forma manual.
***********************
* Full Document TaskScheduler:
* https://github.com/arkhipenko/TaskScheduler/wiki/Full-Document

