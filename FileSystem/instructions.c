//--------instructions.c--------

#include "instructions.h"

void evaluar_instruccion(instr_t* instr, char* remitente) {

	loggear_trace(string_from_format("Evaluando instruccion recibida"));

	int codigoNeto = instr->codigo_operacion %100; //Los primeros dos digitos son los posibles codigos de operacion

	switch (codigoNeto) {

	case CODIGO_CREATE:
		loggear_FS("Me llego una instruccion CREATE.");
		execute_create(instr, remitente);
		break;

	case CODIGO_INSERT:
		loggear_FS("Me llego una instruccion INSERT.");
		ejecutar_instruccion_insert(instr, remitente);
		break;

	case CODIGO_SELECT:
		loggear_FS("Me llego una instruccion SELECT.");
		execute_select(instr, remitente);
		break;

	case CODIGO_DESCRIBE:
		loggear_FS("Me llego una instruccion DESCRIBE.");
		execute_describe(instr, remitente);
		break;

	case CODIGO_DROP:
		loggear_FS("Me llego una instruccion DROP.");
		execute_drop(instr, remitente);
		break;

	case CODIGO_CERRAR:
		loggear_FS("Se cerrara el File System.");
		finalizar_FS();
		break;

	case CODIGO_VALUE:
		loggear_trace(string_from_format("Me llego un CODIGO_VALUE"));
		enviar_tamanio_value(remitente);
		break;

	default:
		loggear_FS("Me llego una instruccion invalida dentro del File System.");
	}
}

void execute_create(instr_t* instruccion, char* remitente) {
	char* tabla = obtener_nombre_tabla(instruccion);
	t_list* listaParam = list_create();

	if (!existe_tabla(tabla)) {
		if(!puede_crear_particiones(instruccion)) {
			char* cadena = string_from_format("No hay bloques disponibles para crear las particiones de la tabla'%s'.", tabla);
			list_add(listaParam, cadena);
			imprimir_donde_corresponda(ERROR_CREATE, instruccion, listaParam, remitente);
			free(cadena);
			return;
		}
		agregar_tabla(tabla); //la agrega a la mem
		inicializar_semaforo_tabla(tabla);
		agregar_a_contador_dumpeo(tabla);
		crear_directorio(g_ruta.tablas, tabla);
		crear_particiones(instruccion);
		crear_metadata(instruccion);
		char* cadena = string_from_format("Se creo el directorio, el metadata y las particiones de la tabla: %s", tabla);
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam, remitente);
		free(cadena);
	}
	else {
		char* cadena = string_from_format("Error al crear la tabla '%s', ya existe en el FS.", tabla);
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(ERROR_CREATE, instruccion, listaParam, remitente);
		free(cadena);
	}
}

t_list* execute_insert(instr_t* instruccion, cod_op* codOp) { //no esta chequeado
	t_list *listaParam = list_create();
	char* tabla = obtener_nombre_tabla(instruccion);
	registro_t* registro = pasar_a_registro(instruccion); //VALIDAR SI TAM_VALUE ES MAYOR AL MAX_TAM_VALUE

	if (!existe_tabla(tabla)) {
		char* cadena = string_from_format("No se pudo insertar %s |", (char *)list_get(instruccion->parametros, 0)); //Tabla
		string_append_with_format(&cadena, " %s |", (char *)list_get(instruccion->parametros, 1)); //Key
		string_append_with_format(&cadena, " %s |", (char *)list_get(instruccion->parametros, 2)); //Value
		string_append_with_format(&cadena, " %"PRIu64, (mseg_t)instruccion->timestamp); //Timestamp
		string_append_with_format(&cadena, " porque no existe esa tabla");
		*codOp = ERROR_INSERT;

		list_add(listaParam, cadena);
		return listaParam;
	}

	if (!existe_mutex(tabla)) {
		//Provisorio hasta hacer mini-create de tablas pre-existentes
		char* cadena = string_from_format("No se pudo realizar el INSERT porque esa tabla fue creada en otra sesion.\n");
		string_append_with_format(&cadena, "Si desea realizar un INSERT en una tabla con el nombre '%s',", tabla);
		string_append_with_format(&cadena, " por favor eliminela (DROP %s) y creela nuevamente.", tabla);

		list_add(listaParam, cadena);
		return listaParam;
	}

	sem_t mutex_tabla;
	sem_wait(&mutex_dic_semaforos);
	int sem_value = obtener_mutex_tabla(tabla, &mutex_tabla);
	sem_post(&mutex_dic_semaforos);
	printf("Obtuve sem_value y es: %d\n", sem_value);	//Testing: Imprime el valor del semaforo

	//sem_wait(&mutex_tabla);
	if(!sem_trywait(&mutex_tabla)){	//Testing: si puede hacer wait lo hace y devuelve 0
		agregar_registro(tabla, registro);
		sem_post(&mutex_tabla);
		puts("--Tremendos esos semaforos--");
	}
	else{	//Testing hasta que funcione correctamente
		agregar_registro(tabla, registro);
		puts("--Malisimos esos semaforos--");

		//Testing2: si otro proceso lo bloquea y la cosa no funca
		//char* cadena = string_from_format("No se pudo insertar %s |", (char *)list_get(instruccion->parametros, 0)); //Tabla
		//string_append_with_format(&cadena, " %s |", (char *)list_get(instruccion->parametros, 1)); //Key
		//string_append_with_format(&cadena, " %s |", (char *)list_get(instruccion->parametros, 2)); //Value
		//string_append_with_format(&cadena, " %"PRIu64, (mseg_t)instruccion->timestamp); //Timestamp
		//string_append_with_format(&cadena, " porque alguien mas esta modificando la tabla");
		//*codOp = ERROR_INSERT;

		//list_add(listaParam, cadena);
		//return listaParam;
	}

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

	if (!existe_tabla(tabla)) {
		puts("No existe la tabla");
		char* cadena = string_from_format("No existe la tabla '%s'", tabla);
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(ERROR_SELECT, instruccion, listaParam, remitente);
		free(cadena);
		return;
	}

	if (!existe_mutex(tabla)) {
		//Provisorio hasta hacer mini-create de tablas pre-existentes
		char* cadena = string_from_format("No se pudo obtener el valor porque la tabla '%s' fue creada en otra sesion.\n", tabla);
		string_append_with_format(&cadena, "Por favor eliminela (DROP %s).", tabla);

		list_add(listaParam, cadena);
		imprimir_donde_corresponda(ERROR_SELECT, instruccion, listaParam, remitente);
		free(cadena);
		return;
	}

	sem_t mutex_tabla;
	int key = (uint16_t)atoi(obtener_parametro(instruccion, 1));
	t_list* registros_key;

	sem_wait(&mutex_dic_semaforos);
	int sem_value = obtener_mutex_tabla(tabla, &mutex_tabla);
	sem_post(&mutex_dic_semaforos);
	printf("Obtuve sem_value y es: %d\n", sem_value);

//	sem_wait(&mutex_tabla);
//	t_list* registros_key = obtener_registros_key(tabla, key);
//	sem_post(&mutex_tabla);

	if(!sem_trywait(&mutex_tabla)){	//Testing: si puede hacer wait, lo hace y devuelve 0
		registros_key = obtener_registros_key(tabla, key);
		sem_post(&mutex_tabla);
		puts("--Tremendos esos semaforos--");
	}
	else{
		registros_key = obtener_registros_key(tabla, key);
		puts("--Malisimos esos semaforos--");

		//Testing2: Si otro proceso lo bloquea y la cosa no funca
		//char* cadena = string_from_format("No se pudo obtener el dato porque alguien mas esta modificando la tabla");
		//list_add(listaParam, cadena);
		//imprimir_donde_corresponda(ERROR_SELECT, instruccion, listaParam, remitente);
		//free(cadena);
		//return;
	}

	if(list_is_empty(registros_key)) {
		puts("No hay registros de la key");
		char* cadena = string_from_format("No se encontraron registros con la key '%d'", key);
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(ERROR_SELECT, instruccion, listaParam, remitente);
		free(cadena);
		return;
	}

	puts("\nRegistro encontrado");

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

	if (!existe_tabla(tabla)) {
		char* cadena = string_from_format("No existe la tabla '%s'", tabla);
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(ERROR_DROP, instruccion, listaParam, remitente);
		free(cadena);
		return;
	}

	int resultadoDrop = -1;

	if (!existe_mutex(tabla)) {	//Provisorio hasta hacer mini-create de tablas pre-existentes
		resultadoDrop = eliminar_directorio(tabla);
		eliminar_tabla_de_mem(tabla);
	}
	else{
		sem_t mutex_tabla;
		sem_wait(&mutex_dic_semaforos);
		int sem_value = obtener_mutex_tabla(tabla, &mutex_tabla);
		sem_post(&mutex_dic_semaforos);
		printf("Obtuve sem_value y es: %d\n", sem_value);	//Testing: Imprime el valor del semaforo

		//sem_wait(&mutex_tabla);
		if(!sem_trywait(&mutex_tabla)){	//Testing: si puede hacer wait, lo hace y devuelve 0
			resultadoDrop = eliminar_directorio(tabla);
			//TODO: eliminar_numero_de_dump(tabla);
			eliminar_tabla_de_mem(tabla);
			eliminar_mutex_de_tabla(tabla);
			sem_post(&mutex_tabla);
			puts("--Tremendos esos semaforos--");
		}
		else{	//Testing hasta que funcione correctamente
			resultadoDrop = eliminar_directorio(tabla);
			//TODO: eliminar_numero_de_dump(tabla);
			eliminar_tabla_de_mem(tabla);
			eliminar_mutex_de_tabla(tabla);
			puts("--Malisimos esos semaforos--");

			//Testing2: Si otro proceso lo bloquea y la cosa no funca
			//char* cadena = string_from_format("No se pudo elimninar porque alguien mas esta modificando la tabla");
			//list_add(listaParam, cadena);
			//imprimir_donde_corresponda(ERROR_SELECT, instruccion, listaParam, remitente);
			//free(cadena);
			//return;
		}
	}

	if(resultadoDrop == 0){
		char* cadena = string_from_format("Se elimino correctamente la tabla '%s'", tabla);
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam, remitente);
		free(cadena);
	}
	else {
		char* cadena = string_from_format("La tabla '%s' no pudo ser eliminada", tabla);
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(ERROR_DROP, instruccion, listaParam, remitente);
		free(cadena);
	}
}

void execute_describe(instr_t* instruccion, char* remitente) {
	t_list* listaParam = list_create();
	int parametros_instr = list_size(instruccion->parametros);
	parametros_instr-=(!quien_pidio(instruccion))?1:0;
	if(parametros_instr == 0) { //DESCRIBE
		char* ruta = string_from_format("%s", g_ruta.tablas);
		printf("Ruta: %s\n", ruta);
		DIR* directorio = opendir(ruta);
		if (directorio == NULL) {
			printf("Error: No se puede abrir el directorio\n");
			//free(ruta);
			return;
		}

		struct dirent* directorio_leido;
		while((directorio_leido = readdir(directorio)) != NULL) {
			char* tabla = directorio_leido->d_name;
			if(!string_contains(tabla, ".")) { //readdir devuelve las entradas . y ..
				printf("\n\n\nLa ruta de la tabla es %s\n\n\n\n\n HOLAAAA\n\n",tabla);
				imprimirMetadata(tabla);
				char* consistencia = obtener_consistencia_metadata(tabla);
				char* particiones = string_itoa(obtener_part_metadata(tabla));
				char* tiempo_comp = string_itoa(obtener_tiempo_compactacion_metadata(tabla));

				list_add(listaParam, tabla);
				list_add(listaParam, consistencia);
				list_add(listaParam, particiones);
				list_add(listaParam, tiempo_comp);
			}
		}
		puts("Tablas leidas, enviando");
		imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam, remitente);
		free(ruta);
		closedir(directorio);

	}
	else { //DESCRIBE <NOMBRE_TABLA>
		char* tabla = obtener_nombre_tabla(instruccion);
		if(!existe_tabla(tabla)) {
			imprimir_donde_corresponda(ERROR_DESCRIBE, instruccion, listaParam, remitente);
			return;
		}
		char* consistencia = obtener_consistencia_metadata(tabla);
		char* particiones = string_itoa(obtener_part_metadata(tabla));
		char* tiempo_comp = string_itoa(obtener_tiempo_compactacion_metadata(tabla));
		imprimirMetadata(tabla);

		list_add(listaParam, tabla);
		list_add(listaParam, consistencia);
		list_add(listaParam, particiones);
		list_add(listaParam, tiempo_comp);
		imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam, remitente);
		//TODO: free
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
		//TODO: free
	}

	if(quien_pidio(instruccion) == CONSOLA_FS){
		resultadoInsert = list_duplicate(execute_insert(instruccion, &codOp));
		imprimir_donde_corresponda(codOp, instruccion, resultadoInsert, remitente);
		//return (int) codOp;
		//TODO: free
	}
	else{
		resultadoInsert = execute_insert(instruccion, &codOp);
		if(codOp == ERROR_INSERT){
			instruccion->codigo_operacion = CONSOLA_MEM_INSERT; //Para que el error se muestre en la memoria
			imprimir_donde_corresponda(codOp, instruccion, resultadoInsert, remitente);
		}
		//return (int) codOp;
		//TODO: free
	}
}


