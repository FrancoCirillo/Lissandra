/*
 * krnlMock.c
 *
 *  Created on: 17 may. 2019
 *      Author: utnso
 */

#include "krnMock.h"

int main(int argc, char *argv[])
{

	printf(COLOR_ANSI_CYAN "	PROCESO KERNEL" COLOR_ANSI_RESET "\n");

	if (argc < 2)
	{
		puts("Uso: KERNEL_MOCK <NUMERERO-DE-MEMORIA_3> <NUMERERO-DE-MEMORIA_8> <NUMERERO-DE-MEMORIA_9>..."); //Se soportan hasta 3 Memorias para testing.
		//Para más memorias habría que arrancar con gossiping
		exit(0);
	}
	conexionesActuales = dictionary_create();
	callback = ejecutar_instruccion;

	inicializar_configuracion();

	t_list *listaParam = list_create();
	list_add(listaParam, "Kernel");
	list_add(listaParam, IP_KERNEL);
	list_add(listaParam, "4444");
	instr_t *miInstruccion = miInstruccion = crear_instruccion(obtener_ts(), CODIGO_HANDSHAKE, listaParam);

	if (argc > 1)
	{
		identificador *idsNuevasConexiones = malloc(sizeof(identificador));
		conexion_con_memoria_3 = crear_conexion(configuracion.MEMORIA_3_IP, configuracion.PUERTO_MEMORIA, IP_KERNEL);
		enviar_request(miInstruccion, conexion_con_memoria_3);
		idsNuevasConexiones->fd_in = 0; //Por las moscas
		strcpy(idsNuevasConexiones->puerto, configuracion.PUERTO_MEMORIA);
		strcpy(idsNuevasConexiones->ip_proceso, configuracion.MEMORIA_3_IP);
		idsNuevasConexiones->fd_out = conexion_con_memoria_3;
		dictionary_put(conexionesActuales, "Memoria_3", idsNuevasConexiones);
	}
	if (argc > 2)
	{
		identificador *idsNuevasConexiones = malloc(sizeof(identificador));
		conexion_con_memoria_8 = crear_conexion(configuracion.MEMORIA_8_IP, configuracion.PUERTO_MEMORIA, IP_KERNEL);
		enviar_request(miInstruccion, conexion_con_memoria_8);
		idsNuevasConexiones->fd_in = 0; //Por las moscas
		strcpy(idsNuevasConexiones->puerto, configuracion.PUERTO_MEMORIA);
		strcpy(idsNuevasConexiones->ip_proceso, configuracion.MEMORIA_8_IP);
		idsNuevasConexiones->fd_out = conexion_con_memoria_8;
		dictionary_put(conexionesActuales, "Memoria_8", idsNuevasConexiones);
	}
	if (argc > 3)
	{
		identificador *idsNuevasConexiones = malloc(sizeof(identificador));
		conexion_con_memoria_9 = crear_conexion(configuracion.MEMORIA_9_IP, configuracion.PUERTO_MEMORIA, IP_KERNEL);
		enviar_request(miInstruccion, conexion_con_memoria_9);
		idsNuevasConexiones->fd_in = 0; //Por las moscas
		strcpy(idsNuevasConexiones->puerto, configuracion.PUERTO_MEMORIA);
		strcpy(idsNuevasConexiones->ip_proceso, configuracion.MEMORIA_9_IP);
		idsNuevasConexiones->fd_out = conexion_con_memoria_9;
		dictionary_put(conexionesActuales, "Memoria_9", idsNuevasConexiones);
	}

	int listenner = iniciar_servidor(IP_KERNEL, "4444");

	vigilar_conexiones_entrantes(listenner, callback, conexionesActuales, CONSOLA_KERNEL);

	//config_destroy(g_config);

	return 0;
}

void ejecutar_instruccion(instr_t *instruccion, char *remitente)
{

	switch (instruccion->codigo_operacion)
	{
	case CONSOLA_KRN_EXITO:
		ejecutar_instruccion_exito(instruccion);
		break;
	case CONSOLA_KRN_SELECT:
		ejecutar_instruccion_select(instruccion);
		break;
	case CONSOLA_KRN_INSERT:
		ejecutar_instruccion_insert(instruccion);
		break;
	case CONSOLA_KRN_CREATE:
		ejecutar_instruccion_create(instruccion);
		break;
	case CONSOLA_KRN_DESCRIBE:
		ejecutar_instruccion_describe(instruccion);
		break;
	case CONSOLA_KRN_DROP:
		ejecutar_instruccion_drop(instruccion);
		break;
	case CONSOLA_KRN_JOURNAL:
		ejecutar_instruccion_journal(instruccion);
		break;
	default:
		break;
	}
}

void ejecutar_instruccion_insert(instr_t *instruccion)
{
	puts("Ejecutando instruccion Insert");
	sleep(1);
	char aQuienEnviar[12];
	sprintf(aQuienEnviar, "Memoria_%d", RECEPTOR_INSERT); //Para poder elegir en el .h a qué memoria enviarle (Para testearlo mas facil)
	int conexionMemoria = obtener_fd_out(aQuienEnviar);
	enviar_request(instruccion, conexionMemoria);
}

void ejecutar_instruccion_create(instr_t *instruccion)
{
	puts("Ejecutando instruccion Create");
	sleep(1);
	char aQuienEnviar[12];
	sprintf(aQuienEnviar, "Memoria_%d", RECEPTOR_CREATE); //Para poder elegir en el .h a qué memoria enviarle (Para testearlo mas facil)
	int conexionMemoria = obtener_fd_out(aQuienEnviar);
	enviar_request(instruccion, conexionMemoria);
}

void ejecutar_instruccion_describe(instr_t *instruccion)
{
	puts("Ejecutando instruccion Describe");
	sleep(1);
	char aQuienEnviar[12];
	sprintf(aQuienEnviar, "Memoria_%d", RECEPTOR_DESCRIBE); //Para poder elegir en el .h a qué memoria enviarle (Para testearlo mas facil)
	int conexionMemoria = obtener_fd_out(aQuienEnviar);
	enviar_request(instruccion, conexionMemoria);
}

void ejecutar_instruccion_drop(instr_t *instruccion)
{
	puts("Ejecutando instruccion Drop");
	sleep(1);
	char aQuienEnviar[12];
	sprintf(aQuienEnviar, "Memoria_%d", RECEPTOR_DROP); //Para poder elegir en el .h a qué memoria enviarle (Para testearlo mas facil)
	int conexionMemoria = obtener_fd_out(aQuienEnviar);
	enviar_request(instruccion, conexionMemoria);
}

void ejecutar_instruccion_journal(instr_t *instruccion)
{
	puts("Ejecutando instruccion Journal");
	sleep(1);
	char aQuienEnviar[12];
	sprintf(aQuienEnviar, "Memoria_%d", RECEPTOR_JOURNAL); //Para poder elegir en el .h a qué memoria enviarle (Para testearlo mas facil)
	int conexionMemoria = obtener_fd_out(aQuienEnviar);
	enviar_request(instruccion, conexionMemoria);
}

void ejecutar_instruccion_exito(instr_t *instruccion)
{
	puts("Instruccion exitosa:");
	print_instruccion(instruccion);
}

void ejecutar_instruccion_select(instr_t *instruccion)
{
	puts("Ejecutando instruccion Select");
	sleep(1);
	char aQuienEnviar[12];
	sprintf(aQuienEnviar, "Memoria_%d", RECEPTOR_SELECT); //Para poder elegir en el .h a qué memoria enviarle (Para testearlo mas facil)
	int conexionMemoria = obtener_fd_out(aQuienEnviar);
	enviar_request(instruccion, conexionMemoria);
}
void inicializar_configuracion()
{
	puts("Configuracion:");
	char *rutaConfig = "krnlMock.config";
	configuracion.MEMORIA_3_IP = obtener_por_clave(rutaConfig, "MEMORIA_3_IP");
	configuracion.MEMORIA_8_IP = obtener_por_clave(rutaConfig, "MEMORIA_8_IP");
	configuracion.MEMORIA_9_IP = obtener_por_clave(rutaConfig, "MEMORIA_9_IP");
	configuracion.PUERTO_MEMORIA = obtener_por_clave(rutaConfig, "PUERTO_MEMORIA");
	configuracion.RUTA_LOG = obtener_por_clave(rutaConfig, "RUTA_LOG");
}

//hace el connect!!
void responderHandshake(identificador *idsConexionEntrante)
{
	t_list *listaParam = list_create();
	list_add(listaParam, "Kernel");  //Tabla
	list_add(listaParam, IP_KERNEL); //Key
	list_add(listaParam, "4444");
	instr_t *miInstruccion = miInstruccion = crear_instruccion(obtener_ts(), CODIGO_HANDSHAKE, listaParam);

	int fd_saliente = crear_conexion(idsConexionEntrante->ip_proceso, idsConexionEntrante->puerto, IP_KERNEL);
	enviar_request(miInstruccion, fd_saliente);
	idsConexionEntrante->fd_out = fd_saliente;
}
int obtener_fd_out(char *proceso)
{
	identificador *idsProceso = (identificador *)dictionary_get(conexionesActuales, proceso);
	if (idsProceso->fd_out == 0)
	{ //Es la primera vez que se le quiere enviar algo a proceso
		responderHandshake(idsProceso);
	}
	return idsProceso->fd_out;
}
void loggear(char *valor)
{
	g_logger = log_create(configuracion.RUTA_LOG, "KernalMock", 1, LOG_LEVEL_INFO);
	log_info(g_logger, valor);
	log_destroy(g_logger);
}

char *obtener_por_clave(char *ruta, char *clave)
{
	char *valor;
	g_config = config_create(ruta);
	valor = config_get_string_value(g_config, clave);
	printf(" %s: %s \n", clave, valor);

	return valor;
}
