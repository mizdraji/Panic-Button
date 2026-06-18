# Modo Ensayo (LoRa y opcionalmente SMS)

## Objetivo

El modo ensayo envia un mensaje periodico por **LoRa** y, si se configura, tambien por **SMS** en el mismo instante, para medir:

- tasa de entrega,
- latencia de cada canal,
- diferencias de recepcion por horario.

## Parametros configurables

En `Sketchup/Panic_Button/configuracion.h`:

- `MODO_ENSAYO`
  - `0`: modo normal
  - `1`: habilita modo ensayo
- `ENSAYO_INCLUIR_SMS`
  - `0`: en cada tick solo se envia **LoRa** (`Le,...`)
  - `1`: en cada tick se envia **LoRa** y luego **SMS** (`Le,...` y `Ens,...`)
- `ENSAYO_INTERVALO_MS`
  - intervalo entre envios en ms (lo usa el programador de tareas `t_ensayo` y el calculo de `ENSAYO_TOTAL_MENSAJES`)
  - valor de ejemplo: `300000` (5 minutos)
- `ENSAYO_DURACION_HS`
  - duracion total del ensayo en horas
  - valor de ejemplo: `48` horas
  - valor de desarrollo actual en firmware: `1` hora
- `ENSAYO_TOTAL_MENSAJES`
  - calculado automaticamente: `(ENSAYO_DURACION_HS * 3600 * 1000) / ENSAYO_INTERVALO_MS`
  - con 48 h y 5 min: `576` mensajes
  - con 1 h y 42 s (config actual): `~86` mensajes

Calculo de ejemplo (48 h, intervalo 5 min):

- `48 horas * 60 min / 5 min = 576`

## Payload de ensayo

Cada envio incluye:

- contador incremental (`1..576`)
- timestamp relativo en milisegundos (`millis()`)

Formatos actuales:

- SMS: `Ens,<contador>,<timestamp_ms>`
- LoRa: `Le,<contador>,<timestamp_ms>`

Ejemplo:

- `Ens,34,10234567`
- `Le,34,10234567`

## Comportamiento

- El contador arranca en `1`.
- Se envia cada `ENSAYO_INTERVALO_MS` (ejemplo: 5 min = `300000`; desarrollo: `42000` = 42 s).
- Con `ENSAYO_INCLUIR_SMS 1`, en cada tick se envia **primero LoRa** y luego SMS (mismo criterio que los tres botones de alerta desde V1.8.9).
- Con `ENSAYO_INCLUIR_SMS 0`, solo LoRa.
- Al alcanzar `ENSAYO_TOTAL_MENSAJES`, la tarea se deshabilita automaticamente.
- Durante el ensayo se resetea `timer` para evitar deep sleep.

## Activar / desactivar

1. Editar `MODO_ENSAYO` y opcionalmente `ENSAYO_INCLUIR_SMS` en `configuracion.h`.
2. Compilar y cargar firmware.
3. Para volver a operacion normal, dejar `MODO_ENSAYO 0`.

## Nota para Node-RED

En servidor, almacenar:

- payload recibido,
- timestamp de recepcion,
- metricas de red (RSSI/SNR u otras).

Luego calcular latencia aproximada:

- `timestamp_recepcion_servidor - timestamp_ms_del_payload`

El `timestamp_ms` es relativo al uptime del nodo (no hora absoluta RTC).

