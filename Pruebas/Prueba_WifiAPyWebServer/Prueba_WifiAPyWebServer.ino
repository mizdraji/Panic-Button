/*Prueba WIFI AP + WEB SERVER
En esta prueba se establece como wifi ap al esp8266 cuya ip local es 192.168.4.1
Se inicia un servidor web cuya autenticacion para entrar es admin//1234.
Dentro del servidor web se pueden cambiar los parametros de las variables policia, medica y bomberos.
*/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Preferences.h>

// Configuración del punto de acceso
const char* ssid = "ESP8266_Config";
const char* password = "12345678";

// Usuario y contraseña
const char* adminUser = "admin";
const char* adminPass = "1234";

// Servidor web y almacenamiento
ESP8266WebServer server(80);
Preferences preferences;

String numeroPolicia = "";
String numeroBomberos = "";
String numeroMedica = "";

// Página HTML mejorada con estilos CSS
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Configuracion ESP8266</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; background-color: #f4f4f4; }
        .container { max-width: 400px; margin: auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.1); }
        h2 { color: #333; }
        input { width: 100%; padding: 8px; margin: 10px 0; border: 1px solid #ccc; border-radius: 5px; }
        button { background: #28a745; color: white; padding: 10px; border: none; border-radius: 5px; cursor: pointer; }
        button:hover { background: #218838; }

        /* Estilo para la firma */
        footer { 
            margin-top: 30px; 
            font-size: 16px; 
            color: #333; 
            text-align: center;
            background-color: #f8f8f8;
            padding: 10px;
            border-radius: 10px;
            box-shadow: 0px 0px 5px rgba(0, 0, 0, 0.1);
        }
        
        footer p {
            margin: 0;
            font-weight: bold;
        }

        footer .signature {
            font-size: 14px;
            color: #888;
        }
    </style>
    <script>
        function saveData() {
            let police = document.getElementById("police").value;
            let fire = document.getElementById("fire").value;
            let medical = document.getElementById("medical").value;
            fetch("/save", {
                method: "POST",
                headers: { "Content-Type": "application/x-www-form-urlencoded" },
                body: `police=${police}&fire=${fire}&medical=${medical}`
            }).then(response => response.text()).then(alert);
        }
    </script>
</head>
<body>
    <div class="container">
        <h2>Configuracion Numeros de Emergencia</h2>
        <label>Policia: <input id="police" type="text"></label><br>
        <label>Bomberos: <input id="fire" type="text"></label><br>
        <label>Medica: <input id="medical" type="text"></label><br>
        <button onclick="saveData()">Guardar</button>
    </div>
    <!-- Firma personalizada -->
        <footer>        
        <p>Hecho por German Mizdraji</p>
        <p class="signature">Proyecto Final de Carrera</p>
    </footer>
</body>
</html>
)rawliteral";

void handleRoot() {
    if (!server.authenticate(adminUser, adminPass)) {
        return server.requestAuthentication();
    }
    server.send(200, "text/html", htmlPage);
}

void handleSave() {
    if (!server.authenticate(adminUser, adminPass)) {
        return server.requestAuthentication();
    }

    // Verifica si los datos no están vacíos antes de actualizar en la memoria
    preferences.begin("emergency", false);

    // Si el valor no está vacío, guarda el nuevo valor
    if (server.hasArg("police") && server.arg("police") != "") {
        preferences.putString("policia", server.arg("police"));
    }

    if (server.hasArg("fire") && server.arg("fire") != "") {
        preferences.putString("bomberos", server.arg("fire"));
    }

    if (server.hasArg("medical") && server.arg("medical") != "") {
        preferences.putString("medica", server.arg("medical"));
    }

    preferences.end();

    // Responde que los datos se guardaron
    server.send(200, "text/plain", "Datos guardados correctamente.");
    delay(3000);
    ESP.reset();
}

void setup() {
    Serial.begin(115200);
    if(WiFi.softAP(ssid, password)){
      Serial.println("Punto de acceso iniciado");
      Serial.print("IP del ESP8266: ");
      Serial.println(WiFi.softAPIP());
    }
    
    server.on("/", handleRoot);
    server.on("/save", HTTP_POST, handleSave);
    server.begin();
    Serial.println("Servidor Iniciado");


     // Inicializa las preferencias (memoria flash)
  preferences.begin("emergency", true);

  // Lee las variables guardadas
  numeroPolicia = preferences.getString("policia", "");  // Si no existe, se devuelve ""
  numeroBomberos = preferences.getString("bomberos", "");  // Si no existe, se devuelve ""
  numeroMedica = preferences.getString("medica", "");  // Si no existe, se devuelve ""

  // Imprime los valores leídos
  Serial.println("Número Policía: " + numeroPolicia);
  Serial.println("Número Bomberos: " + numeroBomberos);
  Serial.println("Número Medica: " + numeroMedica);

  preferences.end();  // Finaliza el acceso a la memoria
}

void loop() {
    server.handleClient();
}
