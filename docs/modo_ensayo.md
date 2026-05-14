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
  - `1`: en cada tick se envian **SMS y LoRa** a la vez (`Ens,...` y `Le,...`)
- `ENSAYO_INTERVALO_MS`
  - intervalo entre envios en ms (lo usa el programador de tareas `t_ensayo` y el calculo de `ENSAYO_TOTAL_MENSAJES`)
  - valor de ejemplo: `300000` (5 minutos)
- `ENSAYO_DURACION_HS`
  - duracion total del ensayo
  - valor actual: `48` horas
- `ENSAYO_TOTAL_MENSAJES`
  - calculado automaticamente
  - con 48h y 5 min: `576` mensajes

Calculo:

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
- Se envia cada 5 minutos (o el valor de `ENSAYO_INTERVALO_MS`).
- Con `ENSAYO_INCLUIR_SMS 1`, SMS y LoRa salen en el mismo tick; con `0`, solo LoRa.
- Al llegar a `576`, la tarea se deshabilita automaticamente.
- Durante el ensayo se resetea `timer` para evitar sleep.

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

