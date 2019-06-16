//--------misConexiones.c--------

#include "misConexiones.h"

int obtener_fd_out(char *proceso)
{
	sem_wait(&mutex_diccionario_conexiones);
	identificador *idsProceso = (identificador *)dictionary_get(conexionesActuales, proceso);
	sem_post(&mutex_diccionario_conexiones); //quizas en todo obtener_fd_out iria
	if(idsProceso == NULL){
		loggear_error(string_from_format("Se desconoce completamente el proceso %s", proceso));
		return 0;
	}
	if (idsProceso->fd_out == 0)
	{ //Es la primera vez que se le quiere enviar algo a proceso

		loggear_debug(string_from_format("Es la primera vez que se quiere enviar algo al proceso %s", proceso));
		responderHandshake(idsProceso);
		return idsProceso->fd_out;
	}
	//	La conexion en el fd_out %d ya existia
	loggear_trace(string_from_format("Ya existia la conexion con el proceso %s", proceso));
	loggear_trace(string_from_format("Tiene el fd_out %d", idsProceso->fd_out));
	return idsProceso->fd_out;
}

void imprimir_donde_corresponda(cod_op codigoOperacion, instr_t *instruccion, t_list *listaParam)
{
	void *ultimoParametro = obtener_ultimo_parametro(instruccion);
	instr_t *miInstruccion;
	switch (quien_pidio(instruccion))
	{
	case CONSOLA_KERNEL:
		if (ultimoParametro != NULL){
			list_add(listaParam, (char*)ultimoParametro);
		}
		loggear_info(string_from_format("Enviando instruccion al Kernel"));
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion + BASE_CONSOLA_KERNEL, listaParam);
		int conexionKernel = obtener_fd_out("Kernel");
		enviar_request(miInstruccion, conexionKernel);
		break;
	default:
		loggear_debug(string_from_format("Loggeando resultado instruccion"));
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion, listaParam);
		if (codigoOperacion == CODIGO_EXITO)
			loggear_exito_proceso(miInstruccion);
		if (codigoOperacion > BASE_COD_ERROR){
			loggear_error_proceso(miInstruccion);
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

	int fd_saliente = crear_conexion(idsConexionEntrante->ip_proceso, idsConexionEntrante->puerto, miIPMemoria, 0);

	enviar_request(miInstruccion, fd_saliente);

	sem_wait(&mutex_diccionario_conexiones);
	idsConexionEntrante->fd_out = fd_saliente;
	sem_post(&mutex_diccionario_conexiones);
}

void enviar_datos(char* remitente){
	loggear_trace(string_from_format("Enviando datos a %s", remitente));
	obtener_fd_out(remitente);
	loggear_trace(string_from_format("Datos enviados"));
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

		int conexion_con_fs = crear_conexion(configuracion.IP_FS, configuracion.PUERTO_FS, miIPMemoria, 1);
		enviar_request(miInstruccion, conexion_con_fs);

		idsNuevasConexiones->fd_in = 0; //Ojo
		strcpy(idsNuevasConexiones->puerto, configuracion.PUERTO_FS);
		strcpy(idsNuevasConexiones->ip_proceso, configuracion.IP_FS);
		idsNuevasConexiones->fd_out = conexion_con_fs;
		sem_wait(&mutex_diccionario_conexiones);
		dictionary_put(conexionesActuales, "FileSystem", idsNuevasConexiones);
		sem_post(&mutex_diccionario_conexiones);
}

void pedir_tamanio_value(){

	loggear_debug(string_from_format("Pidiendo tamanio value"));
	int conexionFS = obtener_fd_out("FileSystem");
	t_list* listaParam = list_create();
	instr_t* miInstruccion = crear_instruccion(obtener_ts(), CODIGO_VALUE, listaParam);
	enviar_request(miInstruccion, conexionFS);
	loggear_trace(string_from_format("Tamanio del value pedido"));
}

void actualizar_tamanio_value(instr_t* instruccion){
	tamanioValue = atoi((char*) list_get(instruccion->parametros, 0));
	loggear_debug(string_from_format("Tamanio del value recibido: %d\n", tamanioValue));
}

void devolver_gossip(instr_t *instruccion, char *remitente){
	loggear_info(string_from_format("Devolviendo el gossip"));
	loggear_trace(string_from_format("Enviando datos a %s", remitente));
	int conexionRemitente = obtener_fd_out(remitente);
	loggear_trace(string_from_format("Datos enviados"));
	t_list* tablaGossiping = conexiones_para_gossiping();

	instr_t* miInstruccion = crear_instruccion(obtener_ts(), RECEPCION_GOSSIP, tablaGossiping);


	loggear_debug(string_from_format("Envio mi tabla de datos al que me arranco el gossiping"));
	enviar_request(miInstruccion, conexionRemitente);
	loggear_trace(string_from_format("Envio mi tabla de datos enviada"));


	actualizar_tabla_gossiping(instruccion);

}

void actualizar_tabla_gossiping(instr_t* instruccion){


	loggear_debug(string_from_format("Recibi una tabla de Gossiping"));

	int saltearProximos = 0;
	int i = 0;
	char* nombre;
	char* ip;
	char* puerto;

	void acutalizar_tabla(char* parametro){
		saltearProximos--;
		loggear_trace(string_from_format("Saltear Proximos = %d\n", saltearProximos));
		if(saltearProximos <= 0){
			if(i % 3 == 0){
				sem_wait(&mutex_diccionario_conexiones);
				if(strcmp(nombreDeMemoria, parametro)!=0 && !dictionary_has_key(conexionesActuales, parametro)){
					sem_post(&mutex_diccionario_conexiones);
					nombre = strdup(parametro);
					loggear_info(string_from_format("No conocia al proceso %s", parametro));
					i++;
					saltearProximos = 0;
				}
				else{
					sem_post(&mutex_diccionario_conexiones);
					loggear_trace(string_from_format("Ya tenia el proceso %s en la tabla", parametro));
					i+=3;
					saltearProximos = 3;
				}
			}
				else if(i % 3 == 1){
					ip = strdup(parametro);
					loggear_trace(string_from_format("Su ip es %s", ip));
					i++;
				}
					else if(i % 3 == 2){
						puerto = strdup(parametro);
						loggear_trace(string_from_format("Su puerto es %s", puerto));
						identificador identificadores = {
								.fd_out = 0,
								.fd_in = 0
						};
						
						strcpy(identificadores.ip_proceso, ip);
						strcpy(identificadores.puerto, puerto);

						identificador* idsConexionesActuales = malloc(sizeof(identificadores));
						memcpy(idsConexionesActuales, &identificadores, sizeof(identificadores));

						loggear_debug(string_from_format("Se agrego al proceso %s al diccionario de conexiones conocidas", nombre));
						sem_wait(&mutex_diccionario_conexiones);
						dictionary_put(conexionesActuales,nombre, idsConexionesActuales);
						sem_post(&mutex_diccionario_conexiones);
						list_add(configuracion.IP_SEEDS, ip);
						list_add(configuracion.PUERTO_SEEDS, puerto);
						i++;
					}
		}
	}

	list_iterate(instruccion->parametros, (void*)acutalizar_tabla);

	loggear_info(string_from_format("Tabla actualizada:"));
	sem_wait(&mutex_diccionario_conexiones);
	imprimir_conexiones(conexionesActuales, loggear_info);
	sem_post(&mutex_diccionario_conexiones);

//	imprimir_config_actual();
}
void imprimir_config_actual(){

	char *texto = string_new();
	string_append_with_format(&texto, "Config actual:\n");
	
	int i=0;
	void imprimir(char* valor){
		string_append_with_format(&texto,"IP: %s\n", valor);
		string_append_with_format(&texto,"Puerto: %s\n", (char*)list_get(configuracion.PUERTO_SEEDS, i));
		i++;
	}
	list_iterate(configuracion.IP_SEEDS, (void*)imprimir);
	printf("%s",texto); //No log xq lo usamos al levantar el config
	free(texto);
}


t_list *conexiones_para_gossiping(){


	t_list *tablaGossiping = list_create();

	void juntar_ip_y_puerto(char* nombre, identificador* ids){
		if(strcmp(nombre, "FileSystem")!=0 && strcmp(nombre, nombreDeMemoria)!=0){
			loggear_trace(string_from_format("Agregando nombre %s", nombre));
			list_add(tablaGossiping, nombre);
			loggear_trace(string_from_format("Agregando IP %s", ids->ip_proceso));
			list_add(tablaGossiping, ids->ip_proceso);
			loggear_trace(string_from_format("Agregando puerto %s", ids->puerto));
			list_add(tablaGossiping, ids->puerto);

		}
	}

	sem_wait(&mutex_diccionario_conexiones);
	dictionary_iterator(conexionesActuales, (void *)juntar_ip_y_puerto);
	sem_post(&mutex_diccionario_conexiones);
	return tablaGossiping;

}

void enviar_lista_gossiping(char* nombreProceso){
	if(strcmp(nombreDeMemoria, nombreProceso)!=0 && strcmp(nombreProceso, "FileSystem")!=0){
		int conexionVieja = obtener_fd_out(nombreProceso);
		loggear_debug(string_from_format("Enviando lista de Gossiping a %s", nombreProceso));
		t_list* listaGossiping = conexiones_para_gossiping();
		instr_t* miInstruccion = crear_instruccion(obtener_ts(), PETICION_GOSSIP, listaGossiping);
		enviar_request(miInstruccion, conexionVieja);
	}
}

void ejecutar_instruccion_gossip(){

	loggear_info(string_from_format("Ejecutando instruccion Gossip"));

	loggear_debug(string_from_format("Tabla enviada:"));
	sem_wait(&mutex_diccionario_conexiones);
	imprimir_conexiones(conexionesActuales, loggear_debug);
	sem_post(&mutex_diccionario_conexiones);

	gossipear_con_conexiones_actuales();

	gossipear_con_procesos_desconectados();

}

bool contiene_IP_y_puerto(identificador *ids, char *ipBuscado, char *puertoBuscado){
	return (strcmp(ids->ip_proceso, ipBuscado)==0) && (strcmp(ids->puerto, puertoBuscado)==0);
}

char* nombre_para_ip_y_puerto(char *ipBuscado, char* puertoBuscado){

	char* nombreEncontrado = NULL;

	void su_nombre(char* nombre, identificador* ids){
		loggear_trace(string_from_format("Buscando nombre para ip %s y puerto %s\n", ipBuscado, puertoBuscado));
		if(contiene_IP_y_puerto(ids, ipBuscado, puertoBuscado)){
			nombreEncontrado = strdup(nombre);
			loggear_trace(string_from_format("Nombre encontrado! : %s\n", nombreEncontrado));
		}
	}
	sem_wait(&mutex_diccionario_conexiones);
	dictionary_iterator(conexionesActuales, (void *)su_nombre);
	sem_post(&mutex_diccionario_conexiones);
	return nombreEncontrado;
}

void gossipear_con_conexiones_actuales(){

	loggear_trace(string_from_format("Gossipeando conexiones actuales"));
	void su_nombre(char* nombre, identificador* ids){
		sem_post(&mutex_diccionario_conexiones);
		enviar_lista_gossiping(nombre);
		sem_wait(&mutex_diccionario_conexiones);
	}
	sem_wait(&mutex_diccionario_conexiones);
	dictionary_iterator(conexionesActuales, (void *)su_nombre);
	sem_post(&mutex_diccionario_conexiones);
}

void gossipear_con_procesos_desconectados(){

	int i = 0;
	void enviar_tabla_gossiping(char* unaIP){
		loggear_trace(string_from_format("IP seed: %s\n", unaIP));
		loggear_trace(string_from_format("Puerto seed: %s\n", (char*)list_get(configuracion.PUERTO_SEEDS,i)));
		char* nombreProceso = nombre_para_ip_y_puerto(unaIP, (char*)list_get(configuracion.PUERTO_SEEDS,i));
		if(nombreProceso == NULL || ((identificador*)dictionary_get(conexionesActuales, nombreProceso))->fd_out==0){
			loggear_trace(string_from_format("El IP %s y Puerto %s no estaban en las conexiones conocidas", unaIP, (char*)list_get(configuracion.PUERTO_SEEDS,i)));
			int conexion = crear_conexion(unaIP, (char*)list_get(configuracion.PUERTO_SEEDS,i), miIPMemoria, 0);
			if(conexion != -1){
				puts("Conexion creada");
				instr_t * miInstruccion = mis_datos(CODIGO_HANDSHAKE);
				enviar_request(miInstruccion, conexion);
				instr_t * peticionDeSuTabla = mis_datos(PETICION_GOSSIP);
				enviar_request(peticionDeSuTabla, conexion);
			}
		}
		i++;
	}
	list_iterate(configuracion.IP_SEEDS, (void*)enviar_tabla_gossiping);
}

instr_t* mis_datos(cod_op codigoOperacion){
	t_list *listaParam = list_create();
	list_add(listaParam, nombreDeMemoria);
	list_add(listaParam, miIPMemoria);
	list_add(listaParam, configuracion.PUERTO);
	return crear_instruccion(obtener_ts(), codigoOperacion, listaParam);
}
