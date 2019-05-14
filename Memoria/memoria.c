//---------memoria.c---------

#include "memoria.h"

int main() {

	printf(COLOR_ANSI_CYAN"	PROCESO MEMORIA"COLOR_ANSI_RESET"\n");

	callback = ejecutar_instruccion;

	inicializar_configuracion();


	int listenner = iniciar_servidor(IP_MEMORIA, PORT);

	int conexion_con_fs = conectar_con_proceso(FILESYSTEM, MEMORIA);
	vigilar_conexiones_entrantes(listenner, callback, conexion_con_fs, CONSOLA_MEMORIA);


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


void ejecutar_instruccion(instr_t* instruccion, int conexionReceptor){

	switch(instruccion->codigo_operacion){
	case CONSOLA_MEM_SELECT:
	case CONSOLA_KRN_SELECT: ejecutar_instruccion_select(instruccion, conexionReceptor); break;
	case CONSOLA_KRN_RTA_SELECT:
	case CONSOLA_MEM_RTA_SELECT: ejecutar_instruccion_devolucion_select(instruccion, conexionReceptor); break;
	case CONSOLA_MEM_INSERT:
	case CONSOLA_KRN_INSERT: ejecutar_instruccion_insert(instruccion, conexionReceptor); break;
	case CONSOLA_MEM_CREATE:
	case CONSOLA_KRN_CREATE: ejecutar_instruccion_create(instruccion, conexionReceptor); break;
	case CONSOLA_MEM_DESCRIBE:
	case CONSOLA_KRN_DESCRIBE: ejecutar_instruccion_describe(instruccion, conexionReceptor); break;
	case CONSOLA_MEM_DROP:
	case CONSOLA_KRN_DROP: ejecutar_instruccion_drop(instruccion, conexionReceptor); break;
	case CONSOLA_MEM_JOURNAL:
	case CONSOLA_KRN_JOURNAL: ejecutar_instruccion_journal(instruccion, conexionReceptor); break;
	default: break;
	}
}

void ejecutar_instruccion_select(instr_t* instruccion, int conexionReceptor){
		puts("Ejecutando instruccion Select");
		int seEncontro = 0; //No cambiar hasta que se implemente conexionKERNEL
		sleep(1);//Buscar
		if(seEncontro){
			t_list * listaParam = list_create();
			list_add(listaParam, "Se encontro Tabla1 | 3 | MmMmMMMm");
			enviar_a_quien_corresponda(CODIGO_EXITO, instruccion, listaParam, conexionReceptor); //Seria conexionKERNEL, Falta implementar

		}
		else{
			puts("La tabla no se encontro en Memoria. Consultando al FS");
			enviar_request(instruccion, conexionReceptor);
		}

}


void ejecutar_instruccion_devolucion_select(instr_t* instruccion, int conexionReceptor){
	puts("Select realizado en FS, se guardo la siguiente tabla en la memoria:");
	print_instruccion(instruccion);
	t_list * listaParam = list_create();
	list_add(listaParam, "Se encontro Tabla1 | 3 | CHANCHAN");
	enviar_a_quien_corresponda(CODIGO_EXITO, instruccion,  listaParam, conexionReceptor);
}

void ejecutar_instruccion_insert(instr_t* instruccion, int conexionReceptor){
	puts("Ejecutando instruccion Insert");
}


void ejecutar_instruccion_create(instr_t* instruccion, int conexionReceptor){
	puts("Ejecutando instruccion Create");
}


void ejecutar_instruccion_describe(instr_t* instruccion, int conexionReceptor){
	puts("Ejecutando instruccion Describe");
}


void ejecutar_instruccion_drop(instr_t* instruccion, int conexionReceptor){
	puts("Ejecutando instruccion Drop");
}


void ejecutar_instruccion_journal(instr_t* instruccion, int conexionReceptor){
	puts("Ejecutando instruccion Journal");
}

void ejecutar_instruccion_exito(instr_t* instruccion, int conexionReceptor){
	puts("Instruccion exitosa:");
	print_instruccion(instruccion);
}



void enviar_a_quien_corresponda(cod_op codigoOperacion, instr_t* instruccion, t_list * listaParam, int conexionReceptor){
	instr_t * miInstruccion;
	switch(quienEnvio(instruccion)){
	case CONSOLA_KERNEL:
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion + BASE_CONSOLA_KERNEL, listaParam);
		puts("Esto se debio mandar al Kernel:");
		print_instruccion(miInstruccion);
//		enviar_request(miInstruccion, conexionReceptor);
		break;
	default:
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion , listaParam);
		print_instruccion(miInstruccion);
		break;
	}

}





