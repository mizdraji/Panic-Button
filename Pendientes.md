# MEJORAS PENDIENTES
* * Pendiente configurar tiempos de encendido y apagado de leds.
* * Pendiente mejorar config_task en setup.
* * Agregar botón de reset.
* * Los leds encienden cuando intenta enviar el mensaje, eso no asegura que el mensaje se envio. El sim800 puede responder OK o ERROR. Extraer 
ese OK para prender leds de mensajes enviados, y parpadear leds cuando es ERROR.
* * Sensar tension de baterias para enviar mensaje.
* * Terminar la parte de almacenamiento en memoria EEPROM que se empezo en la versión 1.9.
* * Armar guia de usuario. Folleto explicativo del modo de uso.
* * Cambiar leds de 5mm por leds de 3mm.
* * Agregar reintentos de envio de mensaje si no se recibe la confirmación de recibido.
* * Ver la posibilidad de usar Vext para resetear SIM800 desde el rst pin.
* * Aviso de bateria baja por algun led.
* * Aviso de cobertura con alguna secuencia de led ya sea sms o lora.