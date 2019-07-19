//---------memoria.c---------

#include "memoria.h"

int main(int argc, char *argv[])
{
	signal(SIGPIPE, SIG_IGN);

	inicializar_semaforos();
	inicializar_configuracion();
	check_inicial(argc, argv);
	iniciar_log();
	
	inicializar_estructuras_conexiones();
	empezar_conexiones(argv);
	iniciar_ejecutador_journal();

	iniciar_ejecutador_gossiping();

	// . es el directorio Actual - Tener en cuenta que lo corremos desde la carpeta padre a la que tiene el binario
	vigilar_conexiones_entrantes(callback, actualizar_config, proceso_desconectado, ".", CONSOLA_MEMORIA);

	return 0;
}

void inicializar_configuracion()
{
	puts("Configuracion:");
	rutaConfig = "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/Memoria/memoria.config";
	g_config = config_create(rutaConfig);
	configuracion.PUERTO = obtener_por_clave("PUERTO");
	configuracion.IP_FS = obtener_por_clave("IP_FS");
	configuracion.PUERTO_FS = obtener_por_clave("PUERTO_FS");
	char** ip_seeds = config_get_array_value(g_config, "IP_SEEDS");
	configuracion.IP_SEEDS = string_array_to_list(ip_seeds);
	free(ip_seeds);
	char** puerto_seeds = config_get_array_value(g_config, "PUERTO_SEEDS");
	configuracion.PUERTO_SEEDS = string_array_to_list(puerto_seeds);
	free(puerto_seeds);
	imprimir_config_actual();
	//Le ponemos más tiempo porque, si lo corremos en una compu, la memoria devuelve lo pedido antes que el Kernel pueda dormir el hilo
	configuracion.RETARDO_MEMORIA = atoi(obtener_por_clave("RETARDO_MEMORIA")) + RETARDO_MINIMO;
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
	sem_init(&mutex_config, 0,1);
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
	auxiliarConexiones = dictionary_create();
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
	sem_wait(&mutex_journal);
	loggear_trace(string_from_format("Me llego una instruccion con el codOp %d de %s", instruccion->codigo_operacion, remitente));
	int codigoNeto = instruccion->codigo_operacion %100; //Los primeros dos digitos son los posibles codigos de operacion
	if(codigoNeto != PETICION_GOSSIP && codigoNeto != RECEPCION_GOSSIP){
		imprimir_instruccion(instruccion, loggear_trace);
	}
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
			ejecutar_instruccion_journal(instruccion, 1);
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

		case CODIGO_CERRAR:
			terminar_memoria(instruccion);
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

//	list_destroy_and_destroy_elements(instruccion->parametros, (void*)free);
//	free(instruccion);
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
void actualizar_config(){
	t_config * auxConfig;

	while(	(auxConfig = config_create(rutaConfig)) == NULL 	||
			!config_has_property(auxConfig, "RETARDO_MEMORIA") 	||
			!config_has_property(auxConfig, "RETARDO_FS") 		||
			!config_has_property(auxConfig, "RETARDO_JOURNAL") 	||
			!config_has_property(auxConfig, "RETARDO_GOSSIPING")||
			!config_has_property(auxConfig, "LOG_LEVEL")){
		config_destroy(auxConfig);
	}

	sem_wait(&mutex_config);
	configuracion.RETARDO_MEMORIA = config_get_int_value(auxConfig, "RETARDO_MEMORIA") + RETARDO_MINIMO;
	configuracion.RETARDO_FS = config_get_int_value(auxConfig, "RETARDO_FS");
	configuracion.RETARDO_JOURNAL = config_get_int_value(auxConfig, "RETARDO_JOURNAL");
	configuracion.RETARDO_GOSSIPING = config_get_int_value(auxConfig, "RETARDO_GOSSIPING");
	configuracion.LOG_LEVEL = log_level_from_string(config_get_string_value(auxConfig, "LOG_LEVEL"));

	sem_wait(&mutex_log);
	actualizar_log_level();
	sem_post(&mutex_log);
	config_destroy(auxConfig);
	loggear_info(string_from_format("Config actualizado!\n"
			"Retardo de accseso a Memoria Principal: %d\n"
			"Retardo de accseso a File System: %d\n"
			"Tiempo de Journal: %d\n"
			"Tiempo de Gossiping: %d\n"
			"Log level: %s"
			"\n"COLOR_ANSI_MAGENTA ">" COLOR_ANSI_RESET,
			(configuracion.RETARDO_MEMORIA-RETARDO_MINIMO), configuracion.RETARDO_FS, configuracion.RETARDO_JOURNAL, configuracion.RETARDO_GOSSIPING, log_level_as_string(configuracion.LOG_LEVEL)));
	sem_post(&mutex_config);
	printf("\n"COLOR_ANSI_MAGENTA ">" COLOR_ANSI_RESET);
	fflush(stdout);
}

void actualizar_log_level(){
	log_destroy(g_logger);
	g_logger = log_create(configuracion.RUTA_LOG, nombreDeMemoria, 1, configuracion.LOG_LEVEL);
}

void terminar_memoria(instr_t* instruccion){

	list_destroy_and_destroy_elements(instruccion->parametros, free);
	free(instruccion);

	list_destroy_and_destroy_elements(configuracion.IP_SEEDS, free);
	list_destroy_and_destroy_elements(configuracion.PUERTO_SEEDS, free);
	free(nombreDeMemoria);
	free(puntoMontaje);

	config_destroy(g_config);
	log_destroy(g_logger);

//	void chau_ip(char* nombre, identificador* ids){
//		free(ids->ip_proceso);
//		free(ids->puerto);
//	}
//	dictionary_iterator(conexionesActuales, (void*)chau_ip);
	dictionary_destroy_and_destroy_elements(conexionesActuales, free);
	dictionary_destroy_and_destroy_elements(auxiliarConexiones, (void*)free);

	free(memoriaPrincipal);

	void borrar_todo(char* nombreTabla, t_list* tabla){
		list_destroy_and_destroy_elements(tabla, free);
	}
	dictionary_iterator(tablaDeSegmentos, (void*)borrar_todo);
	dictionary_destroy(tablaDeSegmentos);
	list_destroy_and_destroy_elements(paginasSegunUso, free);
	free(sectorOcupado);
	puts("Gracias por usar Lissandra");
	exit(0);
}

void proceso_desconectado(char* nombreProceso){}
