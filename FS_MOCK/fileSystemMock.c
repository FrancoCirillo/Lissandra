//-------fileSystem.c-------

#include "fileSystemMock.h"

int main() {
	printf(COLOR_ANSI_AMARILLO"	PROCESO FILESYSTEM"COLOR_ANSI_RESET"\n");

	t_dictionary * conexConocidas = dictionary_create();
	callback = ejecutar_instruccion;
	callbackHandshake = responderHandshake;

//	inicializar_configuracion();

	//	int conexion_con_memoria = conectar_con_proceso(MEMORIA, FILESYSTEM);

	int listenner = iniciar_servidor(IP_FILESYSTEM, PORT);

	vigilar_conexiones_entrantes(listenner, callback, conexConocidas, callbackHandshake, CONSOLA_FS);

	return 0;
}

void ejecutar_instruccion(instr_t* instruccion, int fdEntrante, t_dictionary* conexionesActuales){
	switch(instruccion->codigo_operacion){
		case CONSOLA_FS_SELECT:
		case CONSOLA_MEM_SELECT:
		case CONSOLA_KRN_SELECT: ejecutar_instruccion_select(instruccion, conexionesActuales); break;
		case CONSOLA_FS_INSERT:
		case CONSOLA_MEM_INSERT:
		case CONSOLA_KRN_INSERT: ejecutar_instruccion_insert(instruccion, conexionesActuales); break;
		case CONSOLA_FS_CREATE:
		case CONSOLA_MEM_CREATE:
		case CONSOLA_KRN_CREATE: ejecutar_instruccion_create(instruccion, conexionesActuales); break;
		case CONSOLA_FS_DESCRIBE:
		case CONSOLA_MEM_DESCRIBE:
		case CONSOLA_KRN_DESCRIBE: ejecutar_instruccion_describe(instruccion, conexionesActuales); break;
		case CONSOLA_FS_DROP:
		case CONSOLA_MEM_DROP:
		case CONSOLA_KRN_DROP: ejecutar_instruccion_drop(instruccion, conexionesActuales); break;
		default: break;
		}
}



void ejecutar_instruccion_select(instr_t* instruccion, t_dictionary* conexionesActuales){
	puts("Ejecutando instruccion Select");
	sleep(1);
	int tablaPreexistente = 1;
	if(tablaPreexistente){
		t_list * listaParam = list_create();
		list_add(listaParam, (char*) list_get(instruccion->parametros, 0)); //Tabla
		list_add(listaParam, (char*) list_get(instruccion->parametros, 1)); //Key
		list_add(listaParam, "Buenas soy un value"); //Value
		imprimir_donde_corresponda(DEVOLUCION_SELECT, instruccion,listaParam, conexionesActuales);
	}
	else{
		t_list * listaParam = list_create();
		char cadena [400];
		sprintf(cadena, "%s%s", (char*) list_get(instruccion->parametros, 0), "No existe");
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(ERROR_SELECT, instruccion,listaParam, conexionesActuales);
	}
}


void ejecutar_instruccion_insert(instr_t* instruccion, t_dictionary* conexionesActuales){
	puts("Ejecutando instruccion Insert");
	sleep(1);
	t_list * listaParam = list_create();

	list_add(listaParam, "Se inserto TABLA1 | 3 | Joya | 150515789");

	imprimir_donde_corresponda(CODIGO_EXITO, instruccion,listaParam, conexionesActuales);

}


void ejecutar_instruccion_create(instr_t* instruccion, t_dictionary* conexionesActuales){
	puts("Ejecutando instruccion Create");

	t_list * listaParam = list_create();

	list_add(listaParam, "Se creo TABLA2");

	imprimir_donde_corresponda(CODIGO_EXITO, instruccion,listaParam, conexionesActuales);

}


void ejecutar_instruccion_describe(instr_t* instruccion, t_dictionary* conexionesActuales){
	puts("Ejecutando instruccion Describe");
	t_list * listaParam = list_create();

	list_add(listaParam, "Metadata Tabla3:	Tipo de consistencia: SC	Numero de Particiones: 4	Tiempo entre compactaciones: 60000");

	imprimir_donde_corresponda(CODIGO_EXITO, instruccion,listaParam, conexionesActuales);

}


void ejecutar_instruccion_drop(instr_t* instruccion, t_dictionary* conexionesActuales){
	puts("Ejecutando instruccion Drop");
	t_list * listaParam = list_create();

	list_add(listaParam, "Se borro la Tabla 3");

	imprimir_donde_corresponda(CODIGO_EXITO, instruccion,listaParam, conexionesActuales);
}

void imprimir_donde_corresponda(cod_op codigoOperacion, instr_t* instruccion, t_list * listaParam,  t_dictionary* conexionesActuales){
	instr_t * miInstruccion;
	switch(quienEnvio(instruccion)){

	case CONSOLA_KERNEL:
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion + BASE_CONSOLA_KERNEL, listaParam);
		int conexionReceptor1 = obtener_fd_out("Memoria_1", conexionesActuales);
		enviar_request(miInstruccion, conexionReceptor1);
		break;
	case CONSOLA_MEMORIA:
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion + BASE_CONSOLA_MEMORIA, listaParam);
		int conexionReceptor2 = obtener_fd_out("Memoria_1", conexionesActuales);
		enviar_request(miInstruccion, conexionReceptor2);
		break;
	default:
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion, listaParam);
		//Se pidio desde la consola de FS
		if(codigoOperacion == DEVOLUCION_SELECT){
			imprimir_registro(miInstruccion);
		}
		if(codigoOperacion == CODIGO_EXITO){
					loggear_exito(miInstruccion);
				}
		if(codigoOperacion > BASE_CONSOLA_FS){ //Es error
					loggear_error(miInstruccion);
				}
		break;
	}

}


void inicializar_configuracion() {
	puts("Configuracion:");
	char* rutaConfig = "fsMock.config";
	configuracion.PUERTO_ESCUCHA = obtener_por_clave(rutaConfig, "PUERTO_ESCUCHA");
	configuracion.TAMANIO_VALUE = atoi(obtener_por_clave(rutaConfig, "TAMANIO_VALUE"));
	configuracion.TIEMPO_DUMP= atoi(obtener_por_clave(rutaConfig, "TIEMPO_DUMP"));
	configuracion.RUTA_LOG = obtener_por_clave(rutaConfig, "RUTA_LOG");
}


void loggear(char* valor) {
	g_logger = log_create(configuracion.RUTA_LOG, "memoria", 1, LOG_LEVEL_INFO);
	log_info(g_logger, valor);
	log_destroy(g_logger);
}


char* obtener_por_clave(char* ruta, char* clave) {
	char* valor;
	g_config = config_create(ruta);
	valor = config_get_string_value(g_config, clave);
	printf(" %s: %s \n", clave, valor);

	return valor;
}

void responderHandshake(identificador* idsConexionEntrante){
	t_list * listaParam = list_create();
	list_add(listaParam, "FileSystem");
	list_add(listaParam, IP_FILESYSTEM);
	list_add(listaParam, PORT); //TODO configuracion.PUERTO
	instr_t * miInstruccion = miInstruccion = crear_instruccion(obtener_ts(), CODIGO_HANDSHAKE, listaParam);

	int fd_saliente =  crear_conexion(idsConexionEntrante->ip_proceso, idsConexionEntrante->puerto, IP_FILESYSTEM);
	enviar_request(miInstruccion, fd_saliente);
	idsConexionEntrante->fd_out = fd_saliente;

}

int obtener_fd_out(char* proceso,t_dictionary* conexionesActuales){
	identificador* idsProceso = (identificador *) dictionary_get(conexionesActuales, proceso);
	if(idsProceso->fd_out==0){
		responderHandshake(idsProceso);
	}
	return idsProceso->fd_out;
}
