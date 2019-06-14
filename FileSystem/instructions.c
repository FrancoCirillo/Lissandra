//--------instructions.c--------

#include "instructions.h"

void evaluar_instruccion(instr_t* instr, char* remitente) {

	int codigoNeto = instr->codigo_operacion %100; //Los primeros dos digitos son los posibles codigos de operacion

	//TODO Aca pasar el nombre de la tabla a Mayuscula. y nos desligamos de esto en el resto de los pasos.

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

	case CODIGO_VALUE:
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
		}
		agregar_tabla(tabla); //la agrega a la mem
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
		//TODO: free
		char* cadena = string_from_format("No se pudo insertar %s |", (char *)list_get(instruccion->parametros, 0)); //Tabla
		string_append_with_format(&cadena, " %s |", (char *)list_get(instruccion->parametros, 1)); //Key
		string_append_with_format(&cadena, " %s |", (char *)list_get(instruccion->parametros, 2)); //Value
		string_append_with_format(&cadena, " %"PRIu64, (mseg_t)instruccion->timestamp); //Timestamp
		*codOp = ERROR_INSERT;

		list_add(listaParam, cadena);
		return listaParam;
	}
	else {
		agregar_registro(tabla, registro);

		char* cadena = string_from_format("Se inserto %s |", (char *)list_get(instruccion->parametros, 0)); //Tabla
		string_append_with_format(&cadena, "%s |", (char *)list_get(instruccion->parametros, 1)); //Key
		string_append_with_format(&cadena, "%s |", (char *)list_get(instruccion->parametros, 2)); //Value
		string_append_with_format(&cadena, " %"PRIu64, (mseg_t)instruccion->timestamp); //Timestamp
		*codOp = CODIGO_EXITO;

		list_add(listaParam, cadena);
		return listaParam;
	}
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
	}
	puts("Existe tabla");
	int key = (uint16_t)atoi(obtener_parametro(instruccion, 1));
	t_list* registros_key = obtener_registros_key(tabla, key);

	if(list_is_empty(registros_key)) {
		puts("No hay registros de la key");
		char* cadena = string_from_format("No se encontraron registros con la key '%d'", key);
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(ERROR_SELECT, instruccion, listaParam, remitente);
		free(cadena);
	}
	else {
		puts("Registro encontrados");
		char* value_registro_reciente = obtener_registro_mas_reciente(registros_key);//respuesta del select, TODO: no anda
//		printf("Value %s\n", value_registro_reciente);

		list_add(listaParam, tabla);
		list_add(listaParam, string_itoa(key));
		list_add(listaParam, value_registro_reciente);
		imprimir_donde_corresponda(DEVOLUCION_SELECT, instruccion, listaParam, remitente);
		free(value_registro_reciente);
	}
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

	eliminar_tabla_de_mem(tabla);
	int resultadoDrop = eliminar_directorio(tabla);

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

	if(parametros_instr == 0) { //DESCRIBE
		char* ruta = string_from_format("%s", g_ruta.tablas);
		printf("Ruta: %s\n", ruta);
		DIR* directorio = opendir(ruta);
		if (directorio == NULL) {
			printf("Error: No se puede abrir el directorio\n");
			exit(2);
		}

		struct dirent* directorio_leido;
		while((directorio_leido = readdir(directorio)) != NULL) {
			char* tabla = directorio_leido->d_name;
			if(!string_contains(tabla, ".")) { //readdir devuelve las entradas . y ..
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
	}

	if(quien_pidio(instruccion) == CONSOLA_FS){
		resultadoInsert = list_duplicate(execute_insert(instruccion, &codOp));

		imprimir_donde_corresponda(codOp, instruccion, resultadoInsert, remitente);
		//return (int) codOp;
	}
	else{
		resultadoInsert = execute_insert(instruccion, &codOp);
		if(codOp == ERROR_INSERT){
			instruccion->codigo_operacion = CONSOLA_MEM_INSERT; //Para que el error se mustre en la memoria
			imprimir_donde_corresponda(codOp, instruccion, resultadoInsert, remitente);
		}
			//return (int) codOp;
	}
}


