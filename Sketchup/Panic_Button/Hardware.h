//estructura para guardar variables del nodo
#define MAX_RANDOM_LARGO       50        //50 segundos
#define MAX_REINTENTOS         6         //es la cantidad  maxima de reintentos que hace con un intervalo de tiempo pequeño, luego espera un tiempo mas largo y vuelve a intentar. Min 6 para que use todos los SF  
#define MAX_PAUSAS_LARGAS      3         //Maximo de pausas largas que se realizan cuando los reintentos cortos fallan, superado este numero se espera un dia completo
#define LONG_TIME_TO_WAIT      180       //3 minutos
#define MIN_RANDOM             10        //10 segundos
#define MAX_RANDOM             40        //40 segundos
#define UN_DIA                 84600     //24hs = 86400 segundos

bool inhibir_tamper = false;             //tamper que se deberá puentear para limpiar memoria

struct str {
  uint8_t pdr_ok = 0;                       // =1 cuando la prueba de red dio ok, sino =0
  //variables de tiempo
  int32_t t_wait                = 0;        //tiempo de espera
  uint8_t pausa_larga           = 0;        //=1 esperar tiempo largo
  uint8_t cont_pausas_largas    = 0;        //cuenta las pausas largas que se realizan cuando los reintentos cortos fallan
  uint8_t cont_reintento_corto  = 0;        //usado para contar los intentos uplink de las funciones pdr, sync e ident
};
str nodo;

//interrupciones:
void IRAM_ATTR buttonInterrupt1();
void IRAM_ATTR buttonInterrupt2();
void IRAM_ATTR buttonInterrupt3();

void stop_interrupt();                      //funcion para detener interrupciones
void init_interrupt();                      //funcion para inicializar interrupciones
int16_t random_time(unsigned int MIN_,unsigned int MAX_);
void pdr_function();
void config_pines();