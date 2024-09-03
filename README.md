# Boton de Panico.

## Hardware:
* Heltec Lora Wifi V2
* Módulo SIM800L

## GPIO Utilizados:
* GPIO34 Reservado Lora -> DIO2      //se pueden conectar juntos a traves de diodos shotky para ir a un solo GPIO, cuando no se usa heltec
* GPIO35 Reservado Lora -> DIO1      //se pueden conectar juntos a traves de diodos shotky para ir a un solo GPIO, cuando no se usa heltec 
* GPIO26 Reservado Lora -> DIO0      //se pueden conectar juntos a traves de diodos shotky para ir a un solo GPIO, cuando no se usa heltec
* GPIO27 Reservado Lora -> MOSI
* GPIO14 Reservado Lora -> RST
* GPIO19 Reservado Lora -> MISO
* GPIO18 Reservado Lora -> CS
* GPIO5  Reservado Lora -> SCK
* *****************************
* GPIO16 RX SIM800L
* GPIO17 TX SIM800L
*******************************
* GPIO37 boton1 - pulsador con resistencia pull-down
* GPIO38 boton2 - pulsador con resistencia pull-down
* GPIO39 boton3 - pulsador con resistencia pull-down
* *****************************
* completar GPIO LEDS
*******************************
* Full Document TaskScheduler:
* https://github.com/arkhipenko/TaskScheduler/wiki/Full-Document

