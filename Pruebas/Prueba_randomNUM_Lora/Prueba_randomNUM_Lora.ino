//Se genera el mensaje que contiene la palabra a enviar + numero idempotencia de 8 digitos, 
//el mensaje es del tipo char para ser mandado con la función: uint8_t sendPackage( char *data_to_send, uint8_t len, uint8_t rta_type, bool canal_por_defecto)
//Se prueba la funcion toInt(), para convertir un string en int.

char policia[] = "policia";

String idempotencia = "";
uint32_t idempotencia_numerico = 0;

char mensaje_saliente[50];

void setup() {
  Serial.begin(115200);
  delay(5000);
}

void loop() {
  // Combinar las cadenas en un solo char

  idempotencia = idempotencia_random();
  idempotencia_numerico = idempotencia.toInt();

  Serial.print("idempotencia_numerico: "); Serial.println(idempotencia_numerico);
sprintf(mensaje_saliente, "%s, %s", policia, idempotencia.c_str());  // Concatena "policia" y "idempotencia" con una coma

// Imprimir el mensaje final
printf("Mensaje: %s\n", mensaje_saliente);
  // put your main code here, to run repeatedly:
delay(2000);
}

//Genera un numero aleatorio de 8 digitos para usar de idempotencia
uint32_t idempotencia_random() {
    int32_t timestamp = esp_timer_get_time();                   // Obtener el timestamp actual en microsegundos
    srand((unsigned int)(timestamp));                           //Sembrar el generador de números aleatorios con el timestamp (opcional, para mayor variabilidad)
    uint32_t random_number = rand()  % 90000000 + 10000000;     // Generar un número aleatorio
    return random_number;
}
