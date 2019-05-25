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

#include <pthread.h>

t_log* g_logger;
t_config* g_config;


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
	char* MEMORIA_3_IP;
	char* MEMORIA_8_IP;
	char* MEMORIA_9_IP;
	char* PUERTO_MEMORIA;
}config_t;


typedef struct metricas{
	int cantidadInsert;
	int cantidadSelect;
	clock_t tiempoInsert;
	clock_t tiempoSelect;
}metricas;

typedef struct memoria{
	int ip;
	int puerto;
}memoria;

typedef struct hilo_enviado{
	pthread_cond_t* cond_t;
	pthread_mutex_t* mutex_t;
	instr_t* respuesta;
}hilo_enviado;

typedef struct criterio{
	int codigo_criterio;
	t_list * lista_memorias;
	sem_t mutex_criterio;
};

metricas m;
proceso* cola_ready=NULL;
config_t configuracion;
int total_hilos=0;
t_dictionary * diccionario_enviados;
t_dictionary * criterios;
int codigo_request=0;

//Semaforos mutex y lock
pthread_cond_t cond_ejecutar = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock_ejecutar = PTHREAD_MUTEX_INITIALIZER;
sem_t mutex_cantidad_hilos;
sem_t mutex_log;
sem_t semaforo_procesos_ready;
sem_t mutex_diccionario_enviados;
sem_t mutex_codigo_request;
sem_t mutex_conexiones_actuales;

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
void kernel_run(char *nombre_archivo);
void recibi_respuesta(instr_t* respuesta);



//Conexiones Franquito
config_t configuracion;
t_dictionary *conexionesActuales;
void (*callback)(instr_t *instruccion, char *remitente);
void ejecutar_requestRecibido(instr_t * instruccion,char* remitente);
void enviar_a(instr_t* i,char* destino);
int obtener_fd_out(char *proceso);
void responderHandshake(identificador *idsConexionEntrante);
//Getter y setters
instr_t *obtener_instruccion(proceso* p);
proceso* obtener_sig_proceso();
memoria obtenerMemoria(instr_t* instr);
char* obtener_por_clave(char* ruta, char* key);
void encolar_proceso(proceso *p);
char* obtener_parametroN(instr_t* i,int index);
int obtener_codigo_request();

//Herramientas
int hilos_disponibles();
void inicializarConfiguracion();
void loggear(char* mensaje);
void informarMetricas();
void actualizar_configuracion();
void inicializar_semaforos();
void ejemplo_procesos();
void iniciar_log();
void procesar_instruccion_consola(char *instruccion);
void subir_cantidad_hilos();
void bajar_cantidad_hilos();


#endif /* kernel.h */
