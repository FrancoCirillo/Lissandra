//--------misConexiones.c--------

#include "misConexiones.h"

int obtener_fd_out(char *proceso)
{
	identificador *idsProceso = (identificador *)dictionary_get(conexionesActuales, proceso);
	if(idsProceso == NULL){
		loggear_error(debug_logger,&mutex_log, string_from_format("Se desconoce completamente el proceso %s", proceso));
		return 0;
	}
	if (idsProceso->fd_out == 0)
	{ //Es la primera vez que se le quiere enviar algo a proceso

		loggear_info(debug_logger,&mutex_log, string_from_format("Es la primera vez que se quiere enviar algo al proceso %s", proceso));
		responderHandshake(idsProceso);
		return idsProceso->fd_out;
	}
	//	La conexion en el fd_out %d ya existia
	loggear_info(debug_logger,&mutex_log, string_from_format("Ya existia la conexion con el proceso %s", proceso));
	loggear_info(debug_logger,&mutex_log, string_from_format("Tiene el fd_out %d", idsProceso->fd_out));
	return idsProceso->fd_out;
}

void imprimir_donde_corresponda(cod_op codigoOperacion, instr_t *instruccion, t_list *listaParam)
{
	void *ultimoParametro = obtener_ultimo_parametro(instruccion);
	if (ultimoParametro != NULL){
		list_add(listaParam, (char*)ultimoParametro);
	}
	instr_t *miInstruccion;
	switch (quien_pidio(instruccion))
	{
	case CONSOLA_KERNEL:
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion + BASE_CONSOLA_KERNEL, listaParam);
		int conexionKernel = obtener_fd_out("Kernel");
		enviar_request(miInstruccion, conexionKernel);
		break;
	default:
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion, listaParam);
		if (codigoOperacion == CODIGO_EXITO)
			loggear_exito_proceso(miInstruccion, g_logger, &mutex_log);
		if (codigoOperacion > BASE_COD_ERROR){
			loggear_error_proceso(miInstruccion, g_logger, &mutex_log);
		}
		break;
	}
}

//hace el connect!!
void responderHandshake(identificador *idsConexionEntrante)
{
	t_list *listaParam = list_create();
	list_add(listaParam, nombreDeMemoria);
	list_add(listaParam, miIPMemoria);
	list_add(listaParam, configuracion.PUERTO);
	instr_t * miInstruccion = crear_instruccion(obtener_ts(), CODIGO_HANDSHAKE, listaParam);

	int fd_saliente = crear_conexion(idsConexionEntrante->ip_proceso, idsConexionEntrante->puerto, miIPMemoria, 0, g_logger, &mutex_log);

	enviar_request(miInstruccion, fd_saliente);

	idsConexionEntrante->fd_out = fd_saliente;
}

void enviar_datos(char* remitente){
	loggear_debug(debug_logger,&mutex_log, string_from_format("Enviando datos a %s", remitente));
	obtener_fd_out(remitente);
	loggear_debug(debug_logger,&mutex_log, string_from_format("Datos enviados"));
}

void enviar_datos_a_FS()
{
		t_list *listaParam = list_create();

		printf(COLOR_ANSI_VERDE "	PROCESO %s\n" COLOR_ANSI_RESET, nombreDeMemoria);
		printf("IP Memoria: %s.\n", miIPMemoria);

		list_add(listaParam, nombreDeMemoria);
		list_add(listaParam, miIPMemoria);
		list_add(listaParam, configuracion.PUERTO);
		instr_t * miInstruccion = crear_instruccion(obtener_ts(), CODIGO_HANDSHAKE, listaParam);

		int conexion_con_fs = crear_conexion(configuracion.IP_FS, configuracion.PUERTO_FS, miIPMemoria, 1, g_logger, &mutex_log);
		enviar_request(miInstruccion, conexion_con_fs);

		idsNuevasConexiones->fd_in = 0; //Ojo
		strcpy(idsNuevasConexiones->puerto, configuracion.PUERTO_FS);
		strcpy(idsNuevasConexiones->ip_proceso, configuracion.IP_FS);
		idsNuevasConexiones->fd_out = conexion_con_fs;
		dictionary_put(conexionesActuales, "FileSystem", idsNuevasConexiones);
}

void pedir_tamanio_value(){

	loggear_debug(debug_logger,&mutex_log, string_from_format("Pidiendo tamanio value"));
	int conexionFS = obtener_fd_out("FileSystem");
	t_list* listaParam = list_create();
	instr_t* miInstruccion = crear_instruccion(obtener_ts(), CODIGO_VALUE, listaParam);
	enviar_request(miInstruccion, conexionFS);
	loggear_debug(debug_logger,&mutex_log, string_from_format("Tamanio del value pedido"));
}

void actualizar_tamanio_value(instr_t* instruccion){
	tamanioValue = atoi((char*) list_get(instruccion->parametros, 0));
	loggear_info(g_logger,&mutex_log, string_from_format("Tamanio del value recibido: %d\n", tamanioValue));
}

void devolver_gossip(instr_t *instruccion, char *remitente){
	loggear_debug(debug_logger,&mutex_log, string_from_format("Devolviendo el gossip"));
	loggear_debug(debug_logger,&mutex_log, string_from_format("Enviando datos a %s", remitente));
	int conexionRemitente = obtener_fd_out(remitente);
	loggear_debug(debug_logger,&mutex_log, string_from_format("Datos enviados"));
	t_list* tablaGossiping = conexiones_para_gossiping();

	instr_t* miInstruccion = crear_instruccion(obtener_ts(), RECEPCION_GOSSIP, tablaGossiping);


	loggear_debug(debug_logger,&mutex_log, string_from_format("Envio mi tabla de datos al que me arranco el gossiping"));
	enviar_request(miInstruccion, conexionRemitente);
	loggear_debug(debug_logger,&mutex_log, string_from_format("Envio mi tabla de datos enviada"));


	actualizar_tabla_gossiping(instruccion);

}

void actualizar_tabla_gossiping(instr_t* instruccion){


	loggear_debug(debug_logger,&mutex_log, string_from_format("Actualizando tabla de gossiping"));

	int saltearProximos = 0;
	int i = 0;
	char* nombre;
	char* ip;
	char* puerto;

	void acutalizar_tabla(char* parametro){
		saltearProximos--;
		printf("Saltear Proximos = %d\n", saltearProximos);
		if(saltearProximos <= 0){
			if(i % 3 == 0){
				if(!dictionary_has_key(conexionesActuales, parametro)){
					nombre = strdup(parametro);
					loggear_info(debug_logger,&mutex_log, string_from_format("No conocia al proceso %s", parametro));
					i++;
					saltearProximos = 0;
				}
				else{
					loggear_info(debug_logger,&mutex_log, string_from_format("Ya tenia el proceso %s en la tabla", parametro));
					i+=3;
					saltearProximos = 3;
				}
			}
				else if(i % 3 == 1){
					ip = strdup(parametro);
					loggear_debug(debug_logger,&mutex_log, string_from_format("Su ip es %s", ip));
					i++;
				}
					else if(i % 3 == 2){
						puerto = strdup(parametro);
						loggear_debug(debug_logger,&mutex_log, string_from_format("Su puerto es %s", puerto));
						identificador identificadores = {
								.fd_out = 0,
								.fd_in = 0,
								.ip_proceso = *ip,
								.puerto = *puerto
						};

						identificador* idsConexionesActuales = malloc(sizeof(identificadores));
						memcpy(idsConexionesActuales, &identificadores, sizeof(identificadores));

						loggear_debug(debug_logger,&mutex_log, string_from_format("Se agrego al proceso %s al diccionario de conexiones conocidas", parametro));
						dictionary_put(conexionesActuales,nombre, idsConexionesActuales);
						i++;
					}
		}
	}

	list_iterate(instruccion->parametros, (void*)acutalizar_tabla);
}

t_list *conexiones_para_gossiping(){


	t_list *tablaGossiping = list_create();

	void juntar_ip_y_puerto(char* nombre, identificador* ids){
		list_add(tablaGossiping, nombre);
		list_add(tablaGossiping, ids->ip_proceso);
		list_add(tablaGossiping, ids->puerto);
	}

	dictionary_iterator(conexionesActuales, (void *)juntar_ip_y_puerto);

	return tablaGossiping;

}

void enviar_lista_gossiping(int conexionVieja){
	puts("Enviando lista de gossiping");
	t_list* listaGossiping= conexiones_para_gossiping();
	instr_t* miInstruccion = crear_instruccion(obtener_ts(), PETICION_GOSSIP, listaGossiping);
	enviar_request(miInstruccion, conexionVieja);
	puts("Lista de gossiping enviada");
}

void ejecutar_instruccion_gossip(){

	int largo = strlen(configuracion.IP_SEEDS);
	char* textoIPs =  string_substring(configuracion.IP_SEEDS, 1, largo-2); //- [ y ]
	char** ipsSeeds = string_split(textoIPs, ",");

	char** puertosSeeds = string_get_string_as_array(configuracion.PUERTO_SEEDS);

	t_list *listaParam = list_create();
	list_add(listaParam, nombreDeMemoria);
	list_add(listaParam, miIPMemoria);
	list_add(listaParam, configuracion.PUERTO);

	instr_t * miInstruccion = crear_instruccion(obtener_ts(), PETICION_GOSSIP, listaParam);

	int i = 0;
	void enviar_tabla_gossiping(char* unaIP){
		printf("IP seed: %s\n", unaIP);
		printf("Puerto seed: %s\n", puertosSeeds[i]);
		char* nombreProceso = nombre_para_ip_y_puerto(unaIP, puertosSeeds[i]);
		if(nombreProceso != NULL){
			puts("Encontre la key (el nombre del proceso)");
			int conexionVieja = obtener_fd_out(nombreProceso);
			enviar_lista_gossiping(conexionVieja);
			puts("Lista de gossiping enviada a la conexion vieja");
		}

		else{
			puts("Ese IP (y puerto) no estaban en las conexiones conocidas");
			int conexion = crear_conexion(unaIP, puertosSeeds[i], miIPMemoria, 0, g_logger, &mutex_log);
			if(conexion != -1){
				puts("Conexion creada");

				instr_t * miInstruccion = crear_instruccion(obtener_ts(), CODIGO_HANDSHAKE, listaParam);
				enviar_request(miInstruccion, conexion);
				instr_t * peticionDeSuTabla = crear_instruccion(obtener_ts(), PETICION_GOSSIP, listaParam);
				enviar_request(peticionDeSuTabla, conexion);


				puts("Peticion de gossiping enviada");
			}
		}
		i++;
	}
	string_iterate_lines(ipsSeeds, (void*)enviar_tabla_gossiping);

	loggear_info(g_logger, &mutex_log, string_from_format("Ejecutando instruccion Gossip"));


}

bool contiene_IP_y_puerto(identificador *ids, char *ipBuscado, char *puertoBuscado){
	return (strcmp(ids->ip_proceso, ipBuscado)==0) && (strcmp(ids->puerto, puertoBuscado)==0);
}

char* nombre_para_ip_y_puerto(char *ipBuscado, char* puertoBuscado){

	char* nombreEncontrado = NULL;

	void su_nombre(char* nombre, identificador* ids){
		printf("Buscando nombre para ip %s y puerto %s\n", ipBuscado, puertoBuscado);
		if(contiene_IP_y_puerto(ids, ipBuscado, puertoBuscado)){
			nombreEncontrado = strdup(nombre);
			printf("Nombre encontrado! : %s\n", nombreEncontrado);
		}
	}

	dictionary_iterator(conexionesActuales, (void *)su_nombre);

	return nombreEncontrado;
}
