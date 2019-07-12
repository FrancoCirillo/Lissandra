//--------instructions.c--------

#include "instructions.h"

void evaluar_instruccion(instr_t* instr, char* remitente){
	loggear_info(string_from_format("Se inicia ejecutador"));
	pthread_t hilo_ejecutador;
	pthread_attr_t attr;
	instr_remitente* in=malloc(sizeof(instr_remitente));
	in->instruccion=instr;
	in->remitente=remitente;
	pthread_attr_init(&attr);
	pthread_create(&hilo_ejecutador,&attr,evaluar_instruccion2,in);
	pthread_detach(hilo_ejecutador);
	loggear_trace(string_from_format("Ejecutador iniciado"));
}

void evaluar_instruccion2(instr_remitente* in) {
	char* remitente=in->remitente;
	instr_t* instr=in->instruccion;

	loggear_trace(string_from_format("Evaluando instruccion recibida"));

	int codigoNeto = instr->codigo_operacion %100; //Los primeros dos digitos son los posibles codigos de operacion

	switch (codigoNeto) {

	case CODIGO_CREATE:
		loggear_debug(string_from_format("Me llego una instruccion CREATE."));
		execute_create(instr, remitente);
		break;

	case CODIGO_INSERT:
		loggear_debug(string_from_format("Me llego una instruccion INSERT."));
		ejecutar_instruccion_insert(instr, remitente);
		break;

	case CODIGO_SELECT:
		loggear_debug(string_from_format("Me llego una instruccion SELECT."));
		execute_select(instr, remitente);
		break;

	case CODIGO_DESCRIBE:
		loggear_debug(string_from_format("Me llego una instruccion DESCRIBE."));
		execute_describe(instr, remitente);
		break;

	case CODIGO_DROP:
		loggear_debug(string_from_format("Me llego una instruccion DROP."));
		execute_drop(instr, remitente);
		break;

	case CODIGO_CERRAR:
		loggear_info(string_from_format("Se cerrara el File System."));
		finalizar_FS(instr);
		break;

	case CODIGO_VALUE:
		loggear_trace(string_from_format("Me llego un CODIGO_VALUE"));
		enviar_tamanio_value(instr, remitente);
		break;

	default:
		loggear_warning(string_from_format("Me llego una instruccion invalida dentro del File System."));
	}

	liberar_instruccion(instr);
	free(in);
}
void liberar_instruccion(instr_t* instruccion){

	if(instruccion->codigo_operacion == CODIGO_DESCRIBE){
		list_destroy(instruccion->parametros);
	}
	else{
		list_destroy_and_destroy_elements(instruccion->parametros,free);
	}
	free(instruccion);
}
void execute_create(instr_t* instruccion, char* remitente) {
	char* tabla = obtener_nombre_tabla(instruccion);
	t_list* listaParam = list_create();

	if (!existe_tabla_en_mem(tabla)) {
		if(!puede_crear_particiones(instruccion)) {
			char* cadena = string_from_format("No hay bloques disponibles para crear las particiones de la tabla'%s'.", tabla);
			list_add(listaParam, cadena);
			imprimir_donde_corresponda(ERROR_CREATE, instruccion, listaParam, remitente);
			return;
		}
		agregar_tabla_a_mem(tabla); //la agrega a la mem
		inicializar_semaforo_tabla(tabla);
		agregar_a_contador_dumpeo(tabla);
		crear_directorio(g_ruta.tablas, tabla);
		crear_particiones(instruccion);
		crear_metadata(instruccion);
		crear_hilo_compactador(tabla);

		char* cadena = string_from_format("Se creo el directorio, el metadata y las particiones de la tabla: %s", tabla);
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam, remitente);
	}
	else {
		char* cadena = string_from_format("Error al crear la tabla '%s', ya existe en el FS.", tabla);
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(ERROR_CREATE, instruccion, listaParam, remitente);
	}
}

t_list* execute_insert(instr_t* instruccion, cod_op* codOp) {
	t_list *listaParam = list_create();
	char* tabla = obtener_nombre_tabla(instruccion);
	registro_t* registro = pasar_a_registro(instruccion);

	if (!existe_tabla_en_mem(tabla)) {
		char* cadena = string_from_format("No se pudo insertar %s |", (char *)list_get(instruccion->parametros, 0)); //Tabla
		string_append_with_format(&cadena, " %s |", (char *)list_get(instruccion->parametros, 1)); //Key
		string_append_with_format(&cadena, " %s |", (char *)list_get(instruccion->parametros, 2)); //Value
		string_append_with_format(&cadena, " %"PRIu64, (mseg_t)instruccion->timestamp); //Timestamp
		string_append_with_format(&cadena, " porque no existe esa tabla");
		*codOp = ERROR_INSERT;

		list_add(listaParam, cadena);
		return listaParam;
	}

	sem_wait(&mutex_dic_semaforos);
	sem_t* mutex_tabla = obtener_mutex_tabla(tabla);
	sem_post(&mutex_dic_semaforos);

	sem_wait(mutex_tabla);
	agregar_registro(tabla, registro);
	sem_post(mutex_tabla);
	//loggear_trace(string_from_format("Semaforos funcionando correctamente"));

	char* cadena = string_from_format("Se inserto %s |", (char *)list_get(instruccion->parametros, 0)); //Tabla
	string_append_with_format(&cadena, " %s |", (char *)list_get(instruccion->parametros, 1)); //Key
	string_append_with_format(&cadena, " %s |", (char *)list_get(instruccion->parametros, 2)); //Value
	string_append_with_format(&cadena, " %"PRIu64, (mseg_t)instruccion->timestamp); //Timestamp
	*codOp = CODIGO_EXITO;
	list_add(listaParam, cadena);
	return listaParam;
}

void execute_select(instr_t* instruccion, char* remitente) {
	char* tabla = obtener_nombre_tabla(instruccion);
	t_list *listaParam = list_create();

	if (!existe_tabla_en_mem(tabla)) {
		loggear_trace(string_from_format("No existe la tabla"));
		char* cadena = string_from_format("No existe la tabla '%s'", tabla);
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(ERROR_SELECT, instruccion, listaParam, remitente);
		return;
	}

	int key = (uint16_t)atoi(obtener_parametro(instruccion, 1));
	t_list* registros_key = obtener_registros_key(tabla, key);
	//Semaforos dentro de obtener_registros_key

	if(registros_key == NULL){
		loggear_trace(string_from_format("No se pudo abrir el .bin"));
		char* cadena = string_from_format("No se pudo abrir el .bin correspondiente a la tabla %s con la key '%d'", tabla, key);
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(ERROR_SELECT, instruccion, listaParam, remitente);
		return;
	}
	if(list_is_empty(registros_key)) {
		loggear_trace(string_from_format("No hay registros de la key"));
		char* cadena = string_from_format("No se encontraron registros en la tabla %s con la key '%d'", tabla, key);
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam, remitente);
		return;
	}

	loggear_trace(string_from_format("Registro encontrado"));

	char* value_registro_reciente = obtener_registro_mas_reciente(registros_key);

	list_add(listaParam, tabla);
	list_add(listaParam, string_itoa(key));
	list_add(listaParam, value_registro_reciente);
	imprimir_donde_corresponda(DEVOLUCION_SELECT, instruccion, listaParam, remitente);

	borrar_lista_registros(registros_key);
}

void execute_drop(instr_t* instruccion, char* remitente) {
	char* tabla = obtener_nombre_tabla(instruccion);
	t_list *listaParam = list_create();

	if (!existe_tabla_en_mem(tabla)) {
		char* cadena = string_from_format("No existe la tabla '%s'", tabla);
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(ERROR_DROP, instruccion, listaParam, remitente);
		return;
	}

	sem_wait(&mutex_dic_semaforos);
	sem_t* mutex_tabla = obtener_mutex_tabla(tabla);
	sem_post(&mutex_dic_semaforos);

	sem_wait(mutex_tabla);
	eliminar_nro_dump_de_tabla(tabla);
	eliminar_tabla_de_mem(tabla);
	int resultadoDrop = eliminar_directorio(tabla);
	sem_post(mutex_tabla);
	eliminar_mutex_de_tabla(tabla);
	//loggear_trace(string_from_format("Semaforos funcionando correctamente"));

	if(resultadoDrop == 0){
		char* cadena = string_from_format("Se elimino correctamente la tabla '%s'", tabla);
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam, remitente);
	}
	else {
		char* cadena = string_from_format("La tabla '%s' no pudo ser eliminada", tabla);
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(ERROR_DROP, instruccion, listaParam, remitente);
	}
}

void execute_describe(instr_t* instruccion, char* remitente) {
	t_list* listaParam = list_create();
	int parametros_instr = list_size(instruccion->parametros);
	parametros_instr-=(!quien_pidio(instruccion))?1:0;
	if(parametros_instr == 0) { //DESCRIBE
		char* ruta = string_from_format("%s", g_ruta.tablas);
		loggear_debug(string_from_format("La ruta es %s\n", ruta));
		DIR* directorio = opendir(ruta);
		if (directorio == NULL) {
			loggear_error(string_from_format("Error: No se puede abrir el directorio %s\n", ruta));
			closedir(directorio);
			free(ruta);
			return;
		}

		struct dirent* directorio_leido;
		while((directorio_leido = readdir(directorio)) != NULL) {
			char* tabla = string_from_format("%s", directorio_leido->d_name);
			if(!string_contains(tabla, ".")) { //readdir devuelve las entradas . y ..
				loggear_trace(string_from_format("La ruta de la tabla es %s\n", tabla));
				imprimirMetadata(tabla);
				char* consistencia = obtener_dato_metadata(tabla, "CONSISTENCY");
				char* particiones = obtener_dato_metadata(tabla, "PARTITIONS");
				char* tiempo_comp = obtener_dato_metadata(tabla, "COMPACTATION_TIME");

				list_add(listaParam, tabla);
				list_add(listaParam, consistencia);
				list_add(listaParam, particiones);
				list_add(listaParam, tiempo_comp);
			}
		}
		loggear_trace(string_from_format("Tablas leidas, enviando"));
		imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam, remitente);
		free(ruta);
		closedir(directorio);

	}
	else { //DESCRIBE <NOMBRE_TABLA>
		char* tabla = obtener_nombre_tabla(instruccion);
		if(!existe_tabla_en_mem(tabla)) {
			imprimir_donde_corresponda(ERROR_DESCRIBE, instruccion, listaParam, remitente);
			return;
		}
		char* consistencia = obtener_dato_metadata(tabla, "CONSISTENCY");
		char* particiones = obtener_dato_metadata(tabla, "PARTITIONS");
		char* tiempo_comp = obtener_dato_metadata(tabla, "COMPACTATION_TIME");
		imprimirMetadata(tabla);

		list_add(listaParam, tabla);
		list_add(listaParam, consistencia);
		list_add(listaParam, particiones);
		list_add(listaParam, tiempo_comp);
		imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam, remitente);
	}
}

//----------------------------------------------

void ejecutar_instruccion_insert(instr_t* instruccion, char* remitente){
	cod_op codOp;
	t_list* resultadoInsert;
	int tam_value = strlen(obtener_parametro(instruccion, 2));

	if(tam_value > config_FS.tamanio_value) {
		t_list *listaParam = list_create();
		char* cadena = string_from_format("El tamanio del value introducido (%d) es mayor al tamanio admitido (%d)", tam_value, config_FS.tamanio_value);
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(ERROR_SELECT, instruccion, listaParam, remitente);
	}

	if(quien_pidio(instruccion) == CONSOLA_FS){
		resultadoInsert = list_duplicate(execute_insert(instruccion, &codOp));
		imprimir_donde_corresponda(codOp, instruccion, resultadoInsert, remitente);
		//return (int) codOp;
	}
	else{
		resultadoInsert = execute_insert(instruccion, &codOp);
		if(codOp == ERROR_INSERT){
			instruccion->codigo_operacion = CONSOLA_MEM_INSERT; //Para que el error se muestre en la memoria
			imprimir_donde_corresponda(codOp, instruccion, resultadoInsert, remitente);
		}
		//return (int) codOp;
	}
}


