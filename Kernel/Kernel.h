//---------kernel.h---------

#ifndef Kernel_H_
#define Kernel_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <readline/readline.h>
#include "../Herramientas/tiempo.h"
#include "../Herramientas/serializaciones.h"
#include "../Herramientas/definicionesConexiones.h"
#include "../Herramientas/utilsServidor.h"

#include <pthread.h>

t_log* g_logger;
t_config* g_config;

typedef enum tipo_criterio{
	SC=0,
	SHC=1,
	EC=2,
}tipo_criterio;

typedef struct proceso{
	int current;
	int size;
	t_list* instrucciones;
	struct proceso* sig;
}proceso;



typedef struct config{
	int quantum;
	int gradoMultiprocesamiento;
	char* rutaLog;
	char* ip;
	char* puerto;
	t_list* PUERTO_SEEDS;
	t_list* IP_SEEDS;
	int RETARDO_GOSSIPING;
	int tiempoMetricas;
}config_t;


typedef struct metricas{
	int cantidadInsert;
	int cantidadSelect;
	clock_t tiempoInsert;
	clock_t tiempoSelect;
}metricas;

typedef struct hilo_enviado{
	pthread_cond_t* cond_t;
	pthread_mutex_t* mutex_t;
	instr_t* respuesta;
}hilo_enviado;

typedef struct criterio{
	int codigo_criterio;
	t_list * lista_memorias;
	sem_t mutex_criterio;
}criterio;

metricas m;
proceso* cola_ready=NULL;
config_t configuracion;
t_dictionary * diccionario_enviados;
t_dictionary * diccionario_criterios;
criterio* criterio_strong_hash_consistency;
criterio* criterio_strong_consistency;
criterio* criterio_eventual_consistency;
t_list* lista_tablas;
t_list* lista_instrucciones_ejecutadas;

int total_hilos=0;
int codigo_request=0;
int contador_ec=0;

//Semaforos mutex y lock
pthread_cond_t cond_ejecutar = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock_ejecutar = PTHREAD_MUTEX_INITIALIZER;

sem_t mutex_cantidad_hilos;
sem_t mutex_log;
sem_t semaforo_procesos_ready;
sem_t mutex_diccionario_enviados;
sem_t mutex_codigo_request;
sem_t mutex_diccionario_conexiones;
sem_t mutex_diccionario_criterios;
sem_t mutex_contador_ec;
sem_t mutex_lista_tablas;
sem_t mutex_configuracion;
sem_t mutex_lista_instrucciones_ejecutadas;
sem_t mutex_diccionario_conexiones;
//Proceso principal
int ejecutar();
void* ejecutar_proceso(void* un_proceso);
instr_t* ejecutar_instruccion(instr_t* instruccion);
instr_t* enviar_i(instr_t* i);
void finalizar_proceso(proceso* p);
void encolar_o_finalizar_proceso(proceso* p);
void* consola(void *param);
void iniciar_ejecutador();
void iniciar_consola();
instr_t* kernel_run(instr_t *nombre_archivo);
instr_t* kernel_add(instr_t *nombre_archivo);
instr_t *validar(instr_t * i);
void *metricar();
void metrics();
instr_t* kernel_metrics(instr_t * i);

//Inits
void inicializar_kernel();
void recibi_respuesta(instr_t* respuesta, char* remitente);
void inicializar_criterios();
void inicializarConfiguracion();
void inicializar_semaforos();
void iniciar_metricas();

//Conexiones Franquito
config_t configuracion;
t_dictionary *conexionesActuales;
void check_inicial(int argc, char* argv[]);
void (*callback)(instr_t *instruccion, char *remitente);
void ejecutar_requestRecibido(instr_t * instruccion,char* remitente);
void enviar_a(instr_t* i,char* destino);
int obtener_fd_out(char *proceso);
void responderHandshake(identificador *idsConexionEntrante);
char* miIPKernel;
void conectar_nueva_memoria(char* IPMemoria, char* PuertoMemoria, char* NombreMemoria);
//Getter y setters
instr_t *obtener_instruccion(proceso* p);
proceso* obtener_sig_proceso();
char* obtener_por_clave(char* ruta, char* key);
void encolar_proceso(proceso *p);
char* obtener_parametroN(instr_t* i,int index);
int obtener_codigo_request();
instr_t* obtener_respuesta_run(instr_t* i);
int obtener_fd_memoria(instr_t *i);
int obtener_codigo_criterio(char* criterio);
void agregar_tabla_a_criterio(instr_t* i);

//Herramientas
int hilos_disponibles();
void loggear(char* mensaje);
void informarMetricas();
void actualizar_configuracion();
void ejemplo_procesos();
void iniciar_log();
void procesar_instruccion_consola(char *instruccion);
void subir_cantidad_hilos();
void bajar_cantidad_hilos();
char* krn_concat(char* s1,char* s2);
void agregar_tabla(char* tabla);
bool existe_tabla(char* tablaBuscada);
void iniciar_consola();
void* consola(void* c);
void agregar_a_metricas(instr_t* i);

//Gossiping
void iniciar_ejecutador_gossiping();
void *ejecutar_gossiping();
void ejecutar_instruccion_gossip();
void gossipear_con_procesos_desconectados();
void gossipear_con_conexiones_actuales();

char* nombre_para_ip_y_puerto(char *ipBuscado, char* puertoBuscado);
bool contiene_IP_y_puerto(identificador *ids, char *ipBuscado, char *puertoBuscado);
void enviar_lista_gossiping(char* nombre);
t_list *conexiones_para_gossiping();
void actualizar_tabla_gossiping(instr_t* instruccion);
void devolver_gossip(instr_t *instruccion, char *remitente);
void imprimir_config_actual();
instr_t* mis_datos(cod_op codigoOperacion);

#endif /* kernel.h */
