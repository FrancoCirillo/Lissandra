/*
 * krnlMock.c
 *
 *  Created on: 17 may. 2019
 *      Author: utnso
 */

#include "krnMock.h"

int main() {

	printf(COLOR_ANSI_CYAN"	PROCESO KERNEL"COLOR_ANSI_RESET"\n");

	conexionesActuales = dictionary_create();
	callback = ejecutar_instruccion;

	idsNuevasConexiones = malloc(sizeof(identificador));

	inicializar_configuracion();

	t_list * listaParam = list_create();
	list_add(listaParam, "Kernel");
	list_add(listaParam, IP_KERNEL);
	list_add(listaParam, "4444");
	instr_t * miInstruccion = miInstruccion = crear_instruccion(obtener_ts(), CODIGO_HANDSHAKE, listaParam);

	int conexion_con_memoria =  crear_conexion(configuracion.MEMORIA_IP, configuracion.PUERTO_MEMORIA, IP_KERNEL);
	enviar_request(miInstruccion, conexion_con_memoria);

	idsNuevasConexiones->fd_in = 0; //Por las moscas
	strcpy(idsNuevasConexiones->puerto, configuracion.PUERTO_MEMORIA);
	strcpy(idsNuevasConexiones->ip_proceso, configuracion.MEMORIA_IP);
	idsNuevasConexiones->fd_out = conexion_con_memoria;
	dictionary_put(conexionesActuales, "Memoria_1", idsNuevasConexiones);

	int listenner = iniciar_servidor(IP_KERNEL, "4444");

	vigilar_conexiones_entrantes(listenner, callback, conexionesActuales, CONSOLA_KERNEL);

	//config_destroy(g_config);

	return 0;
}


void ejecutar_instruccion(instr_t* instruccion, int fdEntrante){

	switch(instruccion->codigo_operacion){
	case CONSOLA_KRN_EXITO: ejecutar_instruccion_exito(instruccion); break;
	case CONSOLA_KRN_SELECT: ejecutar_instruccion_select(instruccion); break;
	case CONSOLA_KRN_INSERT: ejecutar_instruccion_insert(instruccion); break;
	case CONSOLA_KRN_CREATE: ejecutar_instruccion_create(instruccion); break;
	case CONSOLA_KRN_DESCRIBE: ejecutar_instruccion_describe(instruccion); break;
	case CONSOLA_KRN_DROP: ejecutar_instruccion_drop(instruccion); break;
	case CONSOLA_KRN_JOURNAL: ejecutar_instruccion_journal(instruccion); break;
	default: break;
	}
}


void ejecutar_instruccion_insert(instr_t* instruccion){
	puts("Ejecutando instruccion Insert");
}


void ejecutar_instruccion_create(instr_t* instruccion){
	puts("Ejecutando instruccion Create");
}


void ejecutar_instruccion_describe(instr_t* instruccion){
	puts("Ejecutando instruccion Describe");
}


void ejecutar_instruccion_drop(instr_t* instruccion){
	puts("Ejecutando instruccion Drop");
}


void ejecutar_instruccion_journal(instr_t* instruccion){
	puts("Ejecutando instruccion Journal");
}

void ejecutar_instruccion_exito(instr_t* instruccion){
	puts("Instruccion exitosa:");
	print_instruccion(instruccion);
}

void ejecutar_instruccion_select(instr_t* instruccion){
		puts("Ejecutando instruccion Select");
		sleep(1);//Buscar
		if(dictionary_is_empty(conexionesActuales)) puts("Dicc vacio");
		else puts("Diccio lleno");
		int conexionMemoria= obtener_fd_out("Memoria_1");
		puts("Se tiene el fd_out");
		enviar_request(instruccion, conexionMemoria);

}
void inicializar_configuracion() {
	puts("Configuracion:");
	char* rutaConfig = "krnlMock.config";
	configuracion.MEMORIA_IP = obtener_por_clave(rutaConfig, "MEMORIA_IP");
	configuracion.PUERTO_MEMORIA = obtener_por_clave(rutaConfig, "PUERTO_MEMORIA");
	configuracion.RUTA_LOG = obtener_por_clave(rutaConfig, "RUTA_LOG");

}



//hace el connect!!
void responderHandshake(identificador* idsConexionEntrante){
	t_list * listaParam = list_create();
	list_add(listaParam, "Kernel"); //Tabla
	list_add(listaParam, IP_KERNEL); //Key
	list_add(listaParam, "4444");
	instr_t * miInstruccion = miInstruccion = crear_instruccion(obtener_ts(), CODIGO_HANDSHAKE, listaParam);

	int fd_saliente =  crear_conexion(idsConexionEntrante->ip_proceso, idsConexionEntrante->puerto, IP_KERNEL);
	enviar_request(miInstruccion, fd_saliente);
	idsConexionEntrante->fd_out = fd_saliente;

}
int obtener_fd_out(char* proceso){
	identificador* idsProceso = (identificador *) dictionary_get(conexionesActuales, proceso);
	if(idsProceso->fd_out==0){
		printf("Es la primera vez que se le quiere enviar algo a %s\n", proceso);
		responderHandshake(idsProceso);
	}
	return idsProceso->fd_out;
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
