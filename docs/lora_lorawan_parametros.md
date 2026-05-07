# Parametros LoRa/LoRaWAN y guia rapida SF test

## Configuracion LoRa/LoRaWAN actual del firmware

### 1) Tipo de activacion y credenciales

- Activacion: **ABP**.
- Credenciales definidas en `Sketchup/Panic_Button/ABPcredentials.h`:
  - `devID`: `00001`
  - `devAddr`: `0149cf9b`
  - `nwkSKey`: `d8e5a8e01bddfaf48e059992119be745`
  - `appSKey`: `3f19e1ef0da5286e178c5b84fc9e23ae`

> Nota: estas claves estan en texto plano en el firmware.

### 2) Clase LoRaWAN

- Clase configurada: `CLASS_C` (en `initLoraTec()`).

### 3) Modulo y pines

- `tipo_modulo = 1` (ESP32 + SX1276/TTGO Lora32).
- Pines del transceptor:
  - `CS = 18`
  - `RST = 14`
  - `DIO0 = 26`
  - `DIO1 = 34`
  - `DIO2 = 35`

### 4) Canal

- `RANDOM_CHANNEL = 1`
- `DEFAULT_CHANNEL = CH0`
- Comportamiento:
  - En inicializacion, con `RANDOM_CHANNEL=1`, se usa `MULTI`.
  - Si una llamada usa `canal_por_defecto=true`, fuerza `DEFAULT_CHANNEL` (`CH0`).

### 5) Data rate / Spreading Factor

- `DEFAULT_SF = SF7BW125`
- Vector de SF disponible:
  - `SFvector = { SF7BW125, SF8BW125, SF9BW125, SF10BW125 }`
- Límite de prueba adaptativa actual:
  - `MAX_SF = 2` => prueba hasta indice 2 (SF9).
- Reintentos por SF antes de avanzar:
  - `MAX_RETRY_SAME_SF = 2`

### 6) ACK y timeout

- Timeout ACK:
  - `tick_time = 100` ms
  - `timeout = 80`
  - total = `8000 ms` (8 s)
- Espera de ACK en PDR: **no bloqueante** (async con `millis()`), usando:
  - `sendPackageAckAsyncStart(...)`
  - `sendPackageAckAsyncPoll()`

### 7) PDR (prueba de red) y reintentos

- La PDR corre con `TaskScheduler` por la tarea `t_pdr` cada 1 segundo.
- Reintentos cortos:
  - Min/max aleatorio: `MIN_RANDOM=10`, `MAX_RANDOM=40` (segundos)
  - Maximo: `MAX_REINTENTOS=6`
- Pausas largas:
  - `LONG_TIME_TO_WAIT=180` s
  - `MAX_PAUSAS_LARGAS=3`
  - luego espera `UN_DIA=84600` s (como esta definido actualmente).

## Guia rapida: SF test (SF fijo)

Se agrego un modo de ensayo para fijar SF y comparar cobertura/ACK en campo.

### Parametros de prueba

En `Sketchup/Panic_Button/configuracion.h`:

- `FORCE_FIXED_SF_TEST`
  - `0`: modo normal (logica adaptativa)
  - `1`: fuerza SF fijo de prueba
- `FIXED_SF_INDEX`
  - `0` => SF7
  - `1` => SF8
  - `2` => SF9
  - `3` => SF10

### Como usarlo

1. Editar `configuracion.h`.
2. Poner `FORCE_FIXED_SF_TEST 1`.
3. Elegir `FIXED_SF_INDEX` segun el SF a ensayar.
4. Compilar y cargar firmware.
5. Verificar en monitor serie el log:
   - `-->SF FIJO TEST: X`
6. Repetir para cada SF (0,1,2,3) y comparar resultados.
7. Al terminar ensayos, volver a:
   - `FORCE_FIXED_SF_TEST 0`

### Sugerencia de campaña de pruebas

Por cada SF medir al menos:

- % de ACK exitoso (PDR)
- Tiempo hasta ACK
- Cantidad de reintentos
- Estabilidad por ubicacion/horario

Regla recomendada: elegir el **SF mas bajo** que mantenga PDR alto y estable en todos los puntos criticos.

