//---------memoria.c---------

#include "memoria.h"

int main(int argc, char *argv[])
{

	inicializar_semaforos();
	inicializar_configuracion();
	check_inicial(argc, argv);
	iniciar_log();


	inicializar_estructuras_conexiones();
	empezar_conexiones(argv);
//	iniciar_ejecutador_journal(); Piden desactivarlo en el checkpoint presencial

	inicializar_estructuras_memoria();

	vigilar_conexiones_entrantes(listenner, callback, conexionesActuales, CONSOLA_MEMORIA, g_logger, &mutex_log);
	//config_destroy(g_config);

	return 0;
}

void inicializar_configuracion()
{
	puts("Configuracion:");
	char *rutaConfig = "memoria.config";
	g_config = config_create(rutaConfig);
	configuracion.PUERTO = obtener_por_clave(rutaConfig, "PUERTO");
	configuracion.IP_FS = obtener_por_clave(rutaConfig, "IP_FS");
	configuracion.PUERTO_FS = obtener_por_clave(rutaConfig, "PUERTO_FS");
	configuracion.IP_SEEDS = obtener_por_clave(rutaConfig, "IP_SEEDS");
	configuracion.PUERTO_SEEDS = obtener_por_clave(rutaConfig, "PUERTO_SEEDS");
	configuracion.RETARDO_MEMORIA = atoi(obtener_por_clave(rutaConfig, "RETARDO_MEMORIA"));
	configuracion.RETARDO_FS = atoi(obtener_por_clave(rutaConfig, "RETARDO_FS"));
	configuracion.TAMANIO_MEMORIA = atoi(obtener_por_clave(rutaConfig, "TAMANIO_MEMORIA"));
	configuracion.RETARDO_JOURNAL = atoi(obtener_por_clave(rutaConfig, "RETARDO_JOURNAL"));
	configuracion.RETARDO_GOSSIPING = atoi(obtener_por_clave(rutaConfig, "RETARDO_GOSSIPING"));
	configuracion.MEMORY_NUMBER = atoi(obtener_por_clave(rutaConfig, "MEMORY_NUMBER"));
	configuracion.RUTA_LOG = obtener_por_clave(rutaConfig, "RUTA_LOG");
}


void iniciar_log()
{
	g_logger = log_create(configuracion.RUTA_LOG, nombreDeMemoria, 1, LOG_LEVEL_TRACE);
	debug_logger = log_create(configuracion.RUTA_LOG, nombreDeMemoria, 0, LOG_LEVEL_TRACE);
}

char *obtener_por_clave(char *ruta, char *clave)
{
	char *valor;
	valor = config_get_string_value(g_config, clave);
	printf(" %s: %s \n", clave, valor);

	return valor;
}

void inicializar_semaforos()
{
	sem_init(&mutex_log, 0,1);
	sem_init(&mutex_journal, 0, 1);
}

void inicializar_estructuras_conexiones()
{
	conexionesActuales = dictionary_create();
	idsNuevasConexiones = malloc(sizeof(identificador));
	callback = ejecutar_instruccion;
}

void empezar_conexiones()
{
	enviar_datos_a_FS();
	listenner = iniciar_servidor(miIPMemoria, configuracion.PUERTO, g_logger, &mutex_log);
	pedir_tamanio_value();
}


void inicializar_estructuras_memoria()
{
	gran_malloc_inicial();
	inicializar_sectores_memoria();
	inicializar_tabla_segmentos();

}

void ejecutar_instruccion(instr_t *instruccion, char *remitente)
{
	int codigoNeto = instruccion->codigo_operacion %100; //Los primeros dos digitos son los posibles codigos de operacion
	sem_wait(&mutex_journal);
	if(instruccion->codigo_operacion > BASE_COD_ERROR){
		loggear_debug(debug_logger,&mutex_log, string_from_format("Me llego un codigo de error"));
		ejecutar_instruccion_error(instruccion);
	}
	else{
		switch (codigoNeto)
		{
		case CODIGO_SELECT:
			ejecutar_instruccion_select(instruccion);
			break;
		case DEVOLUCION_SELECT:
			ejecutar_instruccion_devolucion_select(instruccion);
			break;
		case CODIGO_INSERT:
			ejecutar_instruccion_insert(instruccion, true);
			break;
		case CODIGO_CREATE:
			ejecutar_instruccion_create(instruccion);
			break;
		case CODIGO_DESCRIBE:
			ejecutar_instruccion_describe(instruccion);
			break;
		case CODIGO_DROP:
			ejecutar_instruccion_drop(instruccion);
			break;
		case CODIGO_JOURNAL:
			ejecutar_instruccion_journal(instruccion);
			break;
		case CODIGO_EXITO:
			ejecutar_instruccion_exito(instruccion);
			break;

		case CODIGO_SHOW:
			mostrar_paginas(instruccion);
			break;

		case CODIGO_VALUE:
			actualizar_tamanio_value(instruccion);
			inicializar_estructuras_memoria();
			break;
		default:
			loggear_info(g_logger,&mutex_log, string_from_format("El comando no pertenece a la memoria"));
			break;
		}
	}
	sem_post(&mutex_journal);
}

void iniciar_ejecutador_journal(){
	loggear_debug(debug_logger, &mutex_log, string_from_format("Iniciando ejecutador journal"));
	pthread_t hilo_ejecutador_journal;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&hilo_ejecutador_journal,&attr, &ejecutar_journal, NULL);
	pthread_detach(hilo_ejecutador_journal);
	loggear_debug(debug_logger, &mutex_log, string_from_format("Ejecutador iniciado journal"));
}

void check_inicial(int argc, char* argv[])
{
	if(argc>2){
		log_error(g_logger, "Uso: Memoria <IP>, IP vacio => IP = 127.0.0.3");
		exit(0);
	}
	if(strlen(argv[1])<2){
		puts("IP 127.0.0.3");
		miIPMemoria = IP_MEMORIA;
	}
	else if(argc==2){
		printf("IP %s\n", argv[1]);
		miIPMemoria = argv[1];
	}
	nombreDeMemoria = string_from_format("Memoria_%d", configuracion.MEMORY_NUMBER);
}

