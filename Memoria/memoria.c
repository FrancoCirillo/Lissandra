//---------memoria.c---------

#include "memoria.h"

int main() {

	printf(COLOR_ANSI_CYAN"	PROCESO MEMORIA"COLOR_ANSI_RESET"\n");
	t_dictionary * conexConocidas = dictionary_create();
	callback = ejecutar_instruccion;
	callbackHandshake = responderHandshake;

	identificador* idsNuevasConexiones = malloc(sizeof(identificador));

	inicializar_configuracion();

	t_list * listaParam = list_create();
	list_add(listaParam, "Memoria_1"); //Tabla
	list_add(listaParam, IP_MEMORIA); //Key
	list_add(listaParam, configuracion.PUERTO);
	instr_t * miInstruccion = miInstruccion = crear_instruccion(obtener_ts(), CODIGO_HANDSHAKE, listaParam);

	int conexion_con_fs =  crear_conexion(configuracion.IP_FS, configuracion.PUERTO_FS, IP_MEMORIA);
	enviar_request(miInstruccion, conexion_con_fs);
	idsNuevasConexiones->fd_in = 0; //Por las moscas
	strcpy(idsNuevasConexiones->puerto, configuracion.PUERTO_FS);
	strcpy(idsNuevasConexiones->ip_proceso, configuracion.IP_FS);
	idsNuevasConexiones->fd_out = conexion_con_fs;
	dictionary_put(conexConocidas, "FileSystem", idsNuevasConexiones);

	int listenner = iniciar_servidor(IP_MEMORIA, configuracion.PUERTO);

	vigilar_conexiones_entrantes(listenner, callback, conexConocidas, callbackHandshake, CONSOLA_MEMORIA);

	//config_destroy(g_config);

	return 0;
}


void inicializar_configuracion() {
	puts("Configuracion:");
	char* rutaConfig = "memoria.config";
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


void ejecutar_instruccion(instr_t* instruccion, int fdEntrante, t_dictionary* conexionesActuales){

	switch(instruccion->codigo_operacion){
	case CONSOLA_MEM_SELECT:
	case CONSOLA_KRN_SELECT: ejecutar_instruccion_select(instruccion, conexionesActuales); break;
	case CONSOLA_KRN_RTA_SELECT:
	case CONSOLA_MEM_RTA_SELECT: ejecutar_instruccion_devolucion_select(instruccion, conexionesActuales); break;
	case CONSOLA_MEM_INSERT:
	case CONSOLA_KRN_INSERT: ejecutar_instruccion_insert(instruccion, conexionesActuales); break;
	case CONSOLA_MEM_CREATE:
	case CONSOLA_KRN_CREATE: ejecutar_instruccion_create(instruccion, conexionesActuales); break;
	case CONSOLA_MEM_DESCRIBE:
	case CONSOLA_KRN_DESCRIBE: ejecutar_instruccion_describe(instruccion, conexionesActuales); break;
	case CONSOLA_MEM_DROP:
	case CONSOLA_KRN_DROP: ejecutar_instruccion_drop(instruccion, conexionesActuales); break;
	case CONSOLA_MEM_JOURNAL:
	case CONSOLA_KRN_JOURNAL: ejecutar_instruccion_journal(instruccion, conexionesActuales); break;
	default: break;
	}
}

void ejecutar_instruccion_select(instr_t* instruccion, t_dictionary* conexionesActuales){
		puts("Ejecutando instruccion Select");
		int seEncontro = 0; //No cambiar hasta que se implemente conexionKERNEL
		sleep(1);//Buscar
		if(seEncontro){
			//Directo para el Kernel
			t_list * listaParam = list_create();
			char cadena [400];
			sprintf(cadena, "%s%s%s%s%s%s%s%u","Se encontro ", (char*) list_get(instruccion->parametros, 0), " | ",(char*) list_get(instruccion->parametros, 1), " | ", (char*) list_get(instruccion->parametros, 2)," | ",(unsigned int)instruccion->timestamp);
			list_add(listaParam, cadena);

			imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam, conexionesActuales);

		}
		else{
			puts("La tabla no se encontro en Memoria. Consultando al FS");
			if(dictionary_is_empty(conexionesActuales)) puts("Dicc vacio");
			else puts("Diccio lleno");
			int conexionFS= obtener_fd_out("FileSystem", conexionesActuales);
			puts("Se tiene el fd_out");
			enviar_request(instruccion, conexionFS);
		}

}


void ejecutar_instruccion_devolucion_select(instr_t* instruccion, t_dictionary* conexionesActuales){
	puts("Select realizado en FS, se guardo la siguiente tabla en la memoria:");
	print_instruccion(instruccion);
	t_list * listaParam = list_create();
	char cadena [400];
	sprintf(cadena, "%s%s%s%s%s%s%s%u","Se encontro ", (char*) list_get(instruccion->parametros, 0), " | ",(char*) list_get(instruccion->parametros, 1), " | ", (char*) list_get(instruccion->parametros, 2)," | ",(unsigned int)instruccion->timestamp);
	list_add(listaParam, cadena);
	imprimir_donde_corresponda(CODIGO_EXITO, instruccion,  listaParam, conexionesActuales);
}

void ejecutar_instruccion_insert(instr_t* instruccion, t_dictionary* conexionesActuales){
	puts("Ejecutando instruccion Insert");
}


void ejecutar_instruccion_create(instr_t* instruccion, t_dictionary* conexionesActuales){
	puts("Ejecutando instruccion Create");
}


void ejecutar_instruccion_describe(instr_t* instruccion, t_dictionary* conexionesActuales){
	puts("Ejecutando instruccion Describe");
}


void ejecutar_instruccion_drop(instr_t* instruccion, t_dictionary* conexionesActuales){
	puts("Ejecutando instruccion Drop");
}


void ejecutar_instruccion_journal(instr_t* instruccion, t_dictionary* conexionesActuales){
	puts("Ejecutando instruccion Journal");
}

void ejecutar_instruccion_exito(instr_t* instruccion, t_dictionary* conexionesActuales){
	puts("Instruccion exitosa:");
	print_instruccion(instruccion);
}

void imprimir_donde_corresponda(cod_op codigoOperacion, instr_t* instruccion, t_list * listaParam,  t_dictionary* conexionesActuales){
	instr_t * miInstruccion;
	switch(quienEnvio(instruccion)){
	case CONSOLA_KERNEL:
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion + BASE_CONSOLA_KERNEL, listaParam);
		int conexionKernel = obtener_fd_out("Kernel", conexionesActuales);
		enviar_request(miInstruccion, conexionKernel);
		break;
	default:
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion , listaParam);
			if(codigoOperacion == CODIGO_EXITO) loggear_exito(miInstruccion);
			if(codigoOperacion > BASE_COD_ERROR) loggear_error(miInstruccion);
		break;
	}

}

//hace el connect!!
void responderHandshake(identificador* idsConexionEntrante){
	t_list * listaParam = list_create();
	list_add(listaParam, "Memoria_1"); //Tabla
	list_add(listaParam, IP_MEMORIA); //Key
	list_add(listaParam, configuracion.PUERTO);
	instr_t * miInstruccion = miInstruccion = crear_instruccion(obtener_ts(), CODIGO_HANDSHAKE, listaParam);

	int fd_saliente =  crear_conexion(idsConexionEntrante->ip_proceso, idsConexionEntrante->puerto, IP_MEMORIA);
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
