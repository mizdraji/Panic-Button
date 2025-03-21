# Boton de Panico - VERSION

* 1.0
* * Envia paquetes lora
* * Se crean tareas de prueba.

* 1.1
* * Se agrega procesamiento lora.update y lora.readData en core0 mientras que el resto se ejecuta en core1.

* 1.2
* * Se cambia config_pines.h por pinout.h.
* * Se crea función config_pines para setear entradas y salidas en setup.
* * Se crea Hardware.h y Hardware.ino.
* * Se crea función pdr_function() para prueba inicial de red LORA pero aun no está funcional.

* V1.3: 
* * Se configuran los 3 pulsadores como tareas que se activan a travez de interrupciones.
* * Se crea el objeto Scheduler interrupt para establecer como alta prioridad.
* * Se cambian los nombres de las tareas por t1-t7.
* * Se crean las tareas t5, t6 y t7 para interrupciones.
* * Se crea estructura de mensajes para continuar con SMS en proxima version.

* V1.4: 
* * Se agregan funciones Serialcom y ReceiveMode en Hardware.ino para enviar/recibir SMS.
* * Se envia uncero al iniciar el setup por LORA para establecer la conexión.
* * Pulsadores ahora envian mensajes y prende led para confirmar que se envio el mensaje.
* * Se bajan delays en funcion Enviar_msj para mejorar rendimiento.
* * Se agregan fotos en documentación.

* V1.5
* * Se agregan los pinout faltantes de leds.
* * Se agrega mensajes recibidos de confirmación. Pendiente agregar todos los mensajes recibidos sms, aun falta todo LORA.
* * Se agregan funciones prender y apagar led para trabajar con tareas programadas.
* * Se prende el led_recibido cuando se recibe una respuesta correcta durante 50 segundos.
* * Se agrega capacitor de 1000uF 10V entre VCC y GND del modulo GSM para evitar errores de lectura y reseteos del ESP32
por baja tension que genera el modulo SIM800.

* V1.6
* * Se agregan recepcion de mensajes faltantes para sms bomberos, medica y atendido.
* * Se agregan mensajes de recepcion para lora.
* * Se completan mensajes enviados lora al presionar pulsador.
* * Se agregan algunos print por DEBUG.
* * Se cambia GPIO37 de button1 por GPIO36, el 37 generaba problemas.
* * Se agregan tareas para apagar leds1,2 y 3.
* * BUG detectado: al presionar un boton para enviar un mensaje, envia 1 mensaje SMS pero se reciben 2 o 3 de lora (PASA EN EL GW DE LABORATORIO).
* * Se actualiza esquematico y se crea PCB para prototipo.

* V1.7
* * Se cambian algunas variables por define para mejorar rendimiento, ya que estas no eran modificadas durante el programa.
* * Se agrega control de rebotes para los tres pulsadores.
* * Se corrige la falla que enviaba dos veces lora.
* * Se cambia secuencia de leds: al presionar boton led1/2/3 -> se recibe respuesta led_recibido -> se recibe respuesta atendido led_atendido -> luego de cierto tiempo se apagan todos los leds.

* V1.8
* * Se crea config_task(); para organizar mejor el setup de task.
* * Se saca t5, t6 y t7 del loop para ser habilitados en las interrupciones.
* * Se agrega Remitente2 mastermonitor
* * Se actualiza los recibidos por lora
* * Se agrega lectura ADC al taskManager. Boton powerON funcional.
* * Se corrige divisor resistivo en lectura ADC, tanto en esquemático, placa prototipo y PCB.
* * Se mejora funcion Enviar_mensaje y ReceiveMode.

* V1.8.1 
* * Bloqueo de pulsadores por 10 segundos para evitar que presionen muchas veces en poco tiempo. Se crea tarea lock para bloquear que llama luego a la funcion unlock para desbloquear.
* * Se agrega funcion config_inicial en el setup para definir el estado de los leds cuando arranca el dispositivo.
* * Se cambian tipos de variables para los statebutton, se saca volatile bool y se deja solamente bool.

* V1.8.2
* * Se agregan nuevos mensajes recibidos: "informadorcv" y "cerradorcv". cerradorcv sin función aun.
* * Ahora rcv_informado apaga todos los leds luego de 15 segundos de recibir el mensaje. A futuro se aumentara este tiempo.
* * Se reduce a 3 segundos el tiempo del blinkstb.

* V1.8.3
* * Se crea prueba para generar numeros aleatorios para usarlo de idempotencia.
* * Ahora el mensaje saliente es: "mensaje, idempotencia". Ejemplo: "policia, 61283763" ; "Lpolicia, 61283763" (para lora).
* * Mensajes de entrada son similares a los salientes: "mensaje, idempotencia". Ejemplo: "policiarcv, 61283763" ; "Lpoliciarcv, 61283763" (para lora).
* * Update led_powerON = HIGH (al iniciar), ledOnTime = 200, ledOffTime = 2000. 

* V1.8.4
* * Se crean dos funciones extraer_numero en Hardware.ino, para obtener el número de idempotencia recibido. Una función es para mensajes lora del tipo char[] y la otra para SMS.
* * Se agrega if (SIM800L.available()) antes de while(SIM800L.available()>0) para hacer el código más seguro y eficiente. Permite asegurar de que solo se leerá los datos cuando realmente haya datos disponibles.
* * Se agrega lock.disable() dentro de config_task para que los botones ya esten habilitados al presionarse por primera vez.
* * Se agrega bandera checknum para comparar si el número idempotencia es el mismo que el recibido.
* * Se agrega condición de mensaje recibido: numrcv == numsnt y checknum == false, tanto en lora como para SMS.
* * Se usa función strncmp en vez de strcmp en datoEntrada para buscar la palabra clave dentro del mensaje recibido.
* * Se agrega informadorcv_lora y procesamiento de este mensaje.

* V1.8.5
* * Se agrega secuencia de luces cuando se recibe informadorcv, con la tarea Tinformadorcv_Led.
* * Se modifican algunos delays en el setup.
* * Se agrega la condicion numrcv == numsnt en atendido e informado.
* * Se agrega ESP.restart() cuando se recibe el mensaje ERROR, debido a que a veces inicia mal el SIM800 y debe resetearse el micro para establecer la correcta comunicación.

* V1.8.6
* * Se agrega lora como interrupcion, para priorizar mensajes recibidos.
* * Se agrega deep_sleep mode.
* * Los botones despierta del deep_sleep y envia el mensaje correspondiente.
* * No entra en modo sleep cuando esta conectado por usb.
* * Despierta del modo sleep conectando alimentación.
