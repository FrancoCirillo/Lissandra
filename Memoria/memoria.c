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
//	iniciar_ejecutador_journal();

	iniciar_ejecutador_gossiping();

	vigilar_conexiones_entrantes(callback, CONSOLA_MEMORIA);
	//config_destroy(g_config);

	return 0;
}

void inicializar_configuracion()
{
	puts("Configuracion:");
	char *rutaConfig = "memoria.config";
	g_config = config_create(rutaConfig);
	configuracion.PUERTO = obtener_por_clave("PUERTO");
	configuracion.IP_FS = obtener_por_clave("IP_FS");
	configuracion.PUERTO_FS = obtener_por_clave("PUERTO_FS");
	configuracion.IP_SEEDS = string_array_to_list(config_get_array_value(g_config, "IP_SEEDS"));
	configuracion.PUERTO_SEEDS = string_array_to_list(config_get_array_value(g_config, "PUERTO_SEEDS"));
	imprimir_config_actual();
	configuracion.RETARDO_MEMORIA = atoi(obtener_por_clave("RETARDO_MEMORIA"));
	configuracion.RETARDO_FS = atoi(obtener_por_clave("RETARDO_FS"));
	configuracion.TAMANIO_MEMORIA = atoi(obtener_por_clave("TAMANIO_MEMORIA"));
	configuracion.RETARDO_JOURNAL = atoi(obtener_por_clave("RETARDO_JOURNAL"));
	configuracion.RETARDO_GOSSIPING = atoi(obtener_por_clave("RETARDO_GOSSIPING"));
	configuracion.MEMORY_NUMBER = atoi(obtener_por_clave("MEMORY_NUMBER"));
	configuracion.RUTA_LOG = obtener_por_clave("RUTA_LOG");
	configuracion.LOG_LEVEL =  log_level_from_string(obtener_por_clave("LOG_LEVEL"));
}


void iniciar_log()
{
	g_logger = log_create(configuracion.RUTA_LOG, nombreDeMemoria, 1, configuracion.LOG_LEVEL);
	loggear_debug(string_from_format("Logger en modo %s \n",log_level_as_string(configuracion.LOG_LEVEL)));
}

char *obtener_por_clave(char *clave)
{
	char *valor;
	valor = config_get_string_value(g_config, clave);
	printf(" %s: %s \n", clave, valor);
	puts("Valor obtenido");
	return valor;
}

void inicializar_semaforos()
{
	sem_init(&mutex_log, 0,1);
	sem_init(&mutex_journal, 0, 1);
	sem_init(&mutex_diccionario_conexiones, 0, 1);
}

void inicializar_estructuras_conexiones()
{
	fd_out_inicial = 0;
	if(list_size(configuracion.IP_SEEDS) != list_size(configuracion.PUERTO_SEEDS)){
		loggear_error(string_from_format("La cantidad de IPs no coinciden con la cantidad de Puertos"));
	}
	conexionesActuales = dictionary_create();
	idsNuevasConexiones = malloc(sizeof(identificador));
	callback = ejecutar_instruccion;
}

void empezar_conexiones()
{
	enviar_datos_a_FS();
	iniciar_servidor(miIPMemoria, configuracion.PUERTO);
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
	loggear_debug(string_from_format("Me llego una instruccion con el codOp %d de %s", instruccion->codigo_operacion, remitente));
	imprimir_instruccion(instruccion, loggear_trace);
	int codigoNeto = instruccion->codigo_operacion %100; //Los primeros dos digitos son los posibles codigos de operacion
	sem_wait(&mutex_journal);
	if(instruccion->codigo_operacion > BASE_COD_ERROR){
		loggear_trace(string_from_format("Me llego un codigo de error"));
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
			ejecutar_instruccion_describe(instruccion, remitente);
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

		case PETICION_GOSSIP:
			devolver_gossip(instruccion, remitente);
			break;

		case RECEPCION_GOSSIP:
			actualizar_tabla_gossiping(instruccion);
			break;

		default:
			loggear_warning(string_from_format("El comando no pertenece a la memoria"));
			break;
		}
	}
	sem_post(&mutex_journal);
}

void iniciar_ejecutador_journal(){
	loggear_debug(string_from_format("Iniciando ejecutador journal"));
	pthread_t hilo_ejecutador_journal;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&hilo_ejecutador_journal,&attr, &ejecutar_journal, NULL);
	pthread_detach(hilo_ejecutador_journal);
	loggear_debug(string_from_format("Ejecutador journal iniciado"));
}

void iniciar_ejecutador_gossiping(){
	loggear_debug(string_from_format("Iniciando ejecutador gossiping"));
	pthread_t hilo_ejecutador_gossiping;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&hilo_ejecutador_gossiping,&attr, &ejecutar_gossiping, NULL);
	pthread_detach(hilo_ejecutador_gossiping);
	loggear_debug(string_from_format("Ejecutador gossiping iniciado"));
}

void check_inicial(int argc, char* argv[])
{
	if(argc>2){
		puts("Uso: Memoria <IP>, IP vacio => IP = 127.0.0.3");
		exit(0);
	}
	if(argc==1 || strlen(argv[1])<2){
		puts("IP 127.0.0.3");
		miIPMemoria = IP_MEMORIA;
	}
	else if(argc==2){
		printf("IP %s\n", argv[1]);
		miIPMemoria = argv[1];
	}
	nombreDeMemoria = string_from_format("Memoria_%d", configuracion.MEMORY_NUMBER);
}

