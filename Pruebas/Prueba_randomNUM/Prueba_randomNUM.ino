//Prueba generar numero aleatorio de 8 digitos para usarlos de idempotencia

  String policia          = "policia";              //mensaje1 para enviar

String mensaje_enviar = "";

int64_t timestamp = 0;
int64_t timestamp2 = 0;

void setup() {
  Serial.begin(115200);
  delay(5000);
}

void loop() {
        uint32_t random_number = generate_fixed_length_random();

        mensaje_enviar = policia + ", " + random_number;
        Serial.print("mensaje a enviar: "); Serial.println(mensaje_enviar);

      delay(2000);
}

// Función para generar un número aleatorio basado en el timestamp y mantener su longitud
uint32_t generate_fixed_length_random() {
    // Obtener el timestamp actual en microsegundos
    int64_t timestamp = esp_timer_get_time();

    // Sembrar el generador de números aleatorios con el timestamp (opcional, para mayor variabilidad)
    srand((unsigned int)(timestamp));

    // Generar un número aleatorio
    uint64_t random_number = rand()  % 90000000 + 10000000;

    return random_number;
}



