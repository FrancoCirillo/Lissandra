//-------fileSystem.c-------

#include "fileSystem.h"

registro_t *crear_registro(mseg_t timestampNuevo, uint16_t keyNueva, char *valueNuevo) {

	registro_t registroCreado = {
		.timestamp = timestampNuevo,
		.key = keyNueva,
		.value = valueNuevo};

	registro_t *miReg = malloc(sizeof(registroCreado));
	memcpy(miReg, &registroCreado, sizeof(registroCreado));

	return miReg;
}

void prueba(registro_t* registro, char* ruta_bloque) {
	FILE* archivo_bloque = txt_open_for_append(ruta_bloque);
//	puts("Abri archivo");
	char* string_registro = formatear_registro(registro);
//	printf("Formatee Registro: %s Esto deberia aparecer abajo\n", string_registro);
	txt_write_in_file(archivo_bloque, string_registro);
//	puts("Escribi bloque");
	txt_close_file(archivo_bloque);
//	puts("Cerre archivo");
	free(string_registro);
//	puts("FIN FUNCION ESCRITURA");
}

t_list* pruebaLeer(char* ruta_archivo, uint16_t key, int tipo_archivo) {
	int nro_bloque = obtener_siguiente_bloque_archivo(ruta_archivo, -1);
	char* ruta_bloque = obtener_ruta_bloque(nro_bloque);
//	printf("Obtengo la ruta del bloque: %s\n", ruta_bloque);
	FILE* archivo_bloque = fopen(ruta_bloque, "r");
//	puts("Abro el bloque");
//	registro_t* reg;
	t_list* registros = crear_lista_registros();
	int status = 1;
	char* buffer = string_new();
	char caracter_leido;
	char* s_caracter;
//	puts("Creo un buffer y entro al while");
//	printf("El contenido del buffer es: %s\n", buffer);

	while(status) {
//		puts("Entro al while");
		caracter_leido = fgetc(archivo_bloque);
//		printf("Leo un caracter: %c\t", caracter_leido);

		switch(caracter_leido) {
		case '\n': //tengo un registro completo
			strcat(buffer, "\n");
//			puts("Obtuve un registro");
			registro_t* registro = obtener_reg(buffer);
//			imprimir_reg_fs(registro);

			printf("El contenido del buffer es: %s\n", buffer);
			strcpy(buffer, "");

			if(registro->key == key) {
//				puts("El registro tiene la key que busco");
//				puts("ENCONTRE UN REGISTRO");
				list_add(registros, registro); //lo agrego solo si tiene la key que busco
				status = tipo_archivo; //si es binario, se pone en 0 y corta el while
			} else
				puts("El registro que encontre no es de la key que busco\n");
//			free(registro);
			strcpy(buffer, "");
			printf("El contenido del buffer es: %s\n", buffer);
			break;

		case EOF: //se me acabo el archivo
//			puts("Llegue al final del archivo");
			fclose(archivo_bloque);
			free(ruta_bloque);
//			puts("Busco un nuevo archivo");
			int bloque_anterior = nro_bloque;
			nro_bloque = obtener_siguiente_bloque_archivo(ruta_archivo, bloque_anterior);

			if(nro_bloque >= 0) { //si es menor a cero, no hay mas bloques por leer
				puts("Encontre otro bloque");
				ruta_bloque = obtener_ruta_bloque(nro_bloque);
				archivo_bloque = fopen(ruta_bloque, "r");
			} else {
				status = 0; //corta el while
				puts("No hay mas bloques, quiero salir del while");
			}
			break;

		default:
			s_caracter = string_from_format("%c", caracter_leido);
			strcat(buffer, s_caracter);
//			printf("Concateno y me queda: %s\n", buffer);
//			free(s_caracter);
			break;
		}
	}
	return registros;
}

t_list* listaRegistros() {
	t_list* registros = crear_lista_registros();

	registro_t* registro1 = obtener_reg("43242345;253;registro1\n");
	registro_t* registro2 = obtener_reg("43242345;43;registro2\n");
	registro_t* registro3 = obtener_reg("43242345;54;registro3\n");
	registro_t* registro4 = obtener_reg("43242345;5;registro4\n");
	registro_t* registro5 = obtener_reg("99992999;5;registro5\n");
	registro_t* registro6 = obtener_reg("33242345;34;registro6\n");
	registro_t* registro7 = obtener_reg("43224345;64;registro7\n");
	registro_t* registro8 = obtener_reg("43224345;11;registro8\n");
	registro_t* registro9 = obtener_reg("43224345;34;registro9\n");
	registro_t* registro10 = obtener_reg("23224345;5;registro10\n");

	list_add(registros, registro1);
	list_add(registros, registro2);
	list_add(registros, registro3);
	list_add(registros, registro4);
	list_add(registros, registro5);
	list_add(registros, registro6);
	list_add(registros, registro7);
	list_add(registros, registro8);
	list_add(registros, registro9);
	list_add(registros, registro10);

	return registros;
}

void pruebaEscrituraYLecturaBloques() {

	char* ruta_bloque = obtener_ruta_bloque(5);

	registro_t* registro2 = crear_registro(12345678, 25, "HolaSoyUnaPrueba");
	prueba(registro2, ruta_bloque);

	registro_t* registro1 = crear_registro(87654321, 32, "HolaSoyOtraPrueba");
	prueba(registro1, ruta_bloque);

//	registro_t* registro2 = malloc(sizeof(registro_t));
//	registro2->key = 25;
//	registro2->timestamp = 4324234;
//	registro2->value = "HolaSoyOtraPrueba";
//	prueba(registro2, ruta_bloque);
//
//	registro_t* registro = malloc(sizeof(registro_t));
//	registro->key = 32;
//	registro->timestamp = 4324234;
//	registro->value = "HolaSoyUnaPrueba";
//	prueba(registro, ruta_bloque);

	puts("");
	imprimirContenidoArchivo(ruta_bloque);

	printf("\n\nRUTA BLOQUE: %s\n\n\n", ruta_bloque);

//		registro_t* reg = pruebaLeer(32, 0, 5);

		puts("Voy a imprimir registros");
//		imprimir_reg_fs(reg);
}

int main(int argc, char* argv[]) {

	printf("\n\n************PROCESO FILESYSTEM************\n\n");
	inicializar_FS(argc, argv);

	// DESCOMENTAR LO COMENTADO DE LAS CONEXIONES DE FRAN!

//	un_num_bloque = 0; //da bloques provisorios. bitmap no esta desarrollado.

//	inicializar_conexiones();
	//ejemplo_instr_create();
	//ejemplo_instr_insert();
//	pruebaEscrituraYLecturaBloques();
	pruebaDump();

	char* bloque0 = obtener_ruta_bloque(0);
	printf("\n\nRuta bloque 0: %s\n", bloque0);
	imprimirContenidoArchivo(bloque0);

	char* bloque1 = obtener_ruta_bloque(1);
	printf("\n\nRuta bloque 1: %s\n", bloque1);
	imprimirContenidoArchivo(bloque1);

	char* bloque2 = obtener_ruta_bloque(2);
	printf("\n\nRuta bloque 2: %s\n", bloque2);
	imprimirContenidoArchivo(bloque2);

	char* ruta = "/home/utnso/lissandra-checkpoint/Tablas/TABLA1/Dump1.tmp";
	t_list* registros = pruebaLeer(ruta, 5, 1);
	list_iterate(registros, &imprimir_reg_fs);

	//finalizar_FS();
	return 0;

}

//------------------------TESTS------------------------
void imprimirContenidoArchivo(char* ruta) {
	puts("---LEO ARCHIVO COMPLETO---");
	FILE* f = fopen(ruta, "r");
	char caracter_leido = fgetc(f);
	int leidos = 0;
	while(caracter_leido != EOF){
		printf("%c", caracter_leido);
		caracter_leido = fgetc(f);
		leidos++;
	}
	printf("\nLeidos: %d\n", leidos);
}

void imprimir_reg_fs(registro_t* registro) {
	puts("REGISTRO:");
	printf("Timestamp: %"PRIu64"\n",registro->timestamp);
	printf("Key: %d\n", registro->key);
	printf("Value: %s\n", registro->value);
	puts("");
}

void imprimirMetadata(char* tabla){
	puts("-------------------Entro a imprimir metadata-------------------");
	printf("Metadata de la tabla %s\n", tabla);
	int part = obtener_part_metadata(tabla);
	printf("Particiones: %d\n", part);
	char* consist = obtener_consistencia_metadata(tabla);
	printf("Consistencia: %s\n", consist);
	int tiempoComp = obtener_tiempo_compactacion_metadata(tabla);
	printf("Tiempo de Compactacion: %d\n", tiempoComp);
}

void pruebaDump() {
	void dump(char* tabla, void* registros) {
		puts("-------------------Entre a dump-------------------");
		int nro_dump = 1;
		printf("Numero de Dump: %d\n", nro_dump);
		char* nombre_tmp = string_from_format("Dump%d", nro_dump);
		char* ruta_tmp = string_from_format("%s%s/%s.tmp", g_ruta.tablas, tabla, nombre_tmp);
		printf("Ruta Temporal: %s\n", ruta_tmp);
		FILE* temporal = crear_tmp(tabla, nombre_tmp);
		int nro_bloque = archivo_inicializar(temporal);
		printf("Al temporal %s se le asigno el bloque %d\n", nombre_tmp, nro_bloque);

		void escribir_reg_en_tmp(void* registro) {
			puts("-------------------Entro a escribir_reg_en_tmp-------------------");
			imprimir_reg_fs((registro_t*)registro);
			int bloque = obtener_ultimo_bloque(ruta_tmp);
			char* ruta_bloque = obtener_ruta_bloque(bloque);
//			printf("Ruta Bloque: %s\nRuta TMP: %s\n", ruta_bloque, ruta_tmp);

			escribir_registro_bloque((registro_t*)registro, ruta_bloque, ruta_tmp);
			puts("Escribi el registro en bloque");
		}

		list_iterate((t_list*)registros, &escribir_reg_en_tmp);

		free(ruta_tmp);
		free(nombre_tmp);
		fclose(temporal);
		puts("Cierro el temporal");
	}

	t_dictionary* mockMem = dictionary_create();

	//--agrego una tabla con registros---
	t_list* reg = listaRegistros();
	dictionary_put(mockMem, "TABLA1", reg);
	puts("Se agrego la tabla en la memtable.");

	//--agrego otra tabla--
	t_list* registros = crear_lista_registros();
	dictionary_put(mockMem, "TABLA2", registros);
	puts("Se agrego la tabla en la memtable.");

	//--creo registros--
	registro_t* registro2 = crear_registro(4324234, 25, "HolaSoyOtraPrueba");
	puts("Cree un registro:");
	imprimir_reg_fs(registro2);

	registro_t* registro = crear_registro(4324234, 32, "HolaSoyUnaPrueba");
	puts("Cree un registro:");
	imprimir_reg_fs(registro);

	//--agrego registros--
	t_list* registros_tabla = dictionary_get(mockMem, "TABLA2");
	list_add(registros_tabla, registro);
	list_add(registros_tabla, registro2);
	puts("Se inserto el registro en la memtable.");

	//--hago dump--
	dictionary_iterator(mockMem, &dump);
}

void pruebaTmp() {
		char* ruta_archivo = "/home/utnso/lissandra-checkpoint/Tablas/TABLA1/Dump1.tmp";

//		---prueba de blocks archivo---
		agregar_bloque_archivo(ruta_archivo, 6);
		agregar_bloque_archivo(ruta_archivo, 35);
		int cant = cantidad_bloques_usados(ruta_archivo);
		printf("Bloques: %d\n", cant);

		int primero = obtener_siguiente_bloque_archivo(ruta_archivo, -1);
		printf("PRIMER BLOQUE: %d\n", primero);
		int sig = obtener_siguiente_bloque_archivo(ruta_archivo, 0);
		printf("SIGUIENTE BLOQUE: %d\n", sig);
		int ultimo = obtener_siguiente_bloque_archivo(ruta_archivo, 35);
		printf("ULTIMO BLOQUE: %d\n", ultimo);

		//---prueba de size archivo---
		t_config* archivo = config_create(ruta_archivo);
		puts("Config create");
		char* nro = string_itoa(10);
		config_set_value(archivo, "SIZE", nro);
		config_save(archivo);
		puts("Set value SIZE = 10");

		int size = config_get_int_value(archivo, "SIZE");
		printf("Tam archivo: %d\n", size);
}


//------------INICIALIZACION Y FINALIZACION-----------------
void inicializar_FS(int argc, char* argv[]) {
	iniciar_semaforos();
	iniciar_logger();

	if(strlen(argv[1])<2){
		puts("IP 127.0.0.2");
		miIP = IP_FILESYSTEM;
	}
	else{
		printf("IP %s\n", argv[1]);
		miIP = argv[1];
	}

	loggear_FS("-----------INICIO PROCESO-----------");
	inicializar_configuracion();
	iniciar_rutas();
	inicializar_memtable();
	inicializar_directorios();
	crear_bloques(); //inicializa tambien la var globlal de bloques disp.
	inicializar_bitmap();
	loggear_FS("-----------Fin inicialización LFS-----------");

}

void finalizar_FS() {
	config_destroy(g_config);
	finalizar_rutas();
	finalizar_memtable();
	loggear_FS("-----------FIN PROCESO-----------");
}

void iniciar_semaforos() {
	sem_init(&mutex_tiempo_dump_config, 0, 1);
	sem_init(&mutex_tiempo_retardo_config, 0, 1);
	sem_init(&mutex_memtable, 0, 1);
	sem_init(&mutex_log, 0, 1);
	sem_init(&mutex_cant_bloques, 0, 1);
	sem_init(&mutex_tablas_nro_dump,0,1);
	tablas_nro_dump = dictionary_create();
}

void iniciar_rutas(){
	g_ruta.tablas = string_from_format("%sTablas/", config_FS.punto_montaje);
	g_ruta.bloques = string_from_format("%sBloques/", config_FS.punto_montaje);
	g_ruta.metadata = string_from_format("%sMetadata/Metadata.bin", config_FS.punto_montaje);
	g_ruta.bitmap = string_from_format("%sMetadata/Bitmap.bin", config_FS.punto_montaje);
}

void finalizar_rutas(){
	free(g_ruta.tablas);
	free(g_ruta.bloques);
	free(g_ruta.metadata);
	free(g_ruta.bitmap);
}



//------------MANEJO INSTRUCCIONES-----------------

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
			char* cadena = string_from_format("No hay bloques disponibles para crear las particiones de la tabla'%s'.", tabla); //TODO: free
			list_add(listaParam, cadena);
			free(cadena);
			imprimir_donde_corresponda(ERROR_CREATE, instruccion, listaParam, remitente);
		}
		agregar_tabla(tabla); //la agrega a la mem
		agregar_a_contador_dumpeo(tabla);
		crear_directorio(g_ruta.tablas, tabla);
		crear_particiones(instruccion);
		crear_metadata(instruccion);
		char* cadena = string_from_format("Se creo el directorio, el metadata y las particiones de la tabla: %s", tabla);
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam, remitente);

	} else {
		char* cadena = string_from_format("Error al crear la tabla '%s', ya existe en el FS.", tabla); //TODO: free
		list_add(listaParam, cadena);
		free(cadena);
		imprimir_donde_corresponda(ERROR_CREATE, instruccion, listaParam, remitente);
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

	} else {
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
		char* cadena = string_from_format("No existe la tabla '%s'", tabla); //TODO: free
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(ERROR_SELECT, instruccion, listaParam, remitente);
		//return ERROR_SELECT;
	}
	puts("Existe tabla");
	registro_t* registro = pasar_a_registro(instruccion);
	puts("Pasar a resgistro");
	int key = registro->key;
	t_list* registros_key = obtener_registros_key(tabla, key);
	puts("obtener registros key");
	if(list_is_empty(registros_key)) {
		puts("No hay registros en la key");
		char* cadena = string_from_format("No se encontraron registros con la key '%s'", (char *)list_get(instruccion->parametros, 1)); //TODO: free
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(ERROR_SELECT, instruccion, listaParam, remitente);
		//borrar_lista_registros(registros_key);
		//return ERROR_SELECT;
	}

//	puts("Select realizado");
//	printf("Tabla %s\n", (char*)list_get(instruccion->parametros, 0));
//	printf("Tabla %s\n", (char*)list_get(instruccion->parametros, 1));
	char* value_registro_reciente = obtener_registro_mas_reciente(registros_key);//respuesta del select, TODO: no anda
	printf("%s", value_registro_reciente);
//	list_add(listaParam, (char*)list_get(instruccion->parametros, 0)); //Tabla //TODO: Usar los campos de registro_reciente (declaratividad)
//	list_add(listaParam, (char*)list_get(instruccion->parametros, 1)); //Key
//	list_add(listaParam, "V");	// cambiar (cuando ande) por value_registro_reciente; //Value
//	imprimir_donde_corresponda(DEVOLUCION_SELECT, instruccion, listaParam, remitente);
	//borrar_lista_registros(registros_key);
	//return CODIGO_EXITO;
}

void execute_drop(instr_t* instruccion, char* remitente) {
	char* tabla = obtener_nombre_tabla(instruccion);
	t_list *listaParam = list_create();

	if (!existe_tabla(tabla)) {
		char* cadena = string_from_format("No existe la tabla '%s'", tabla); //TODO: free
		imprimir_donde_corresponda(ERROR_DROP, instruccion, listaParam, remitente);
		list_add(listaParam, cadena);
		//return ERROR_DROP;
	}
	eliminar_tabla_de_mem(tabla);
	eliminar_directorio(tabla); //adentro tiene un eliminar_archivos(tabla)
	char* cadena = string_from_format("Se eliminpo correctamente la tabla '%s'", tabla); //TODO: free
	imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam, remitente);
	list_add(listaParam, cadena);
	//return CODIGO_EXITO;
}

void execute_describe(instr_t* instruccion, char* remitente) {
	t_list* listaParam = list_create();
	int parametros_instr = list_size(instruccion->parametros);

	if(!parametros_instr) { //DESCRIBE
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
		imprimir_donde_corresponda(CODIGO_DESCRIBE, instruccion, listaParam, remitente);
		free(ruta);
		closedir(directorio);

	} else { //DESCRIBE tabla1
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
		imprimir_donde_corresponda(CODIGO_DESCRIBE, instruccion, listaParam, remitente);
	}
}

char* to_upper(char* str){
	char* upr = "";
	char ch;
	int tam = strlen(str);
	for(int i = 0; i < tam; i++) {
//		printf("%c\n", *(str+i));
		ch = toupper((char)*(str+i));
		upr = string_from_format("%s%c", upr, ch);
//		printf("%s\n", upr);
	}
	return upr;
}

char* obtener_nombre_tabla(instr_t* instr) {
	char* tabla = obtener_parametro(instr, 0);
	return to_upper(tabla);
}

char* obtener_parametro(instr_t * instr, int index) {
	return (char*) list_get(instr->parametros, index);
}

int obtener_particion_key(char* tabla, int key) {
	int cant_particiones = obtener_part_metadata(tabla);
	return key % cant_particiones;
}

t_list* leer_binario(char* tabla, uint16_t key) {
	int particion = obtener_particion_key(tabla, key);
	char* ruta_bin = string_from_format("%s%s/%d.bin", g_ruta.tablas, tabla, particion);
	t_list* registro_key = buscar_key_en_bloques(ruta_bin, key, 0);
	free(ruta_bin);
	return registro_key;
}

t_list* leer_archivos_temporales(char* tabla, uint16_t key) {
	t_list* registros = crear_lista_registros();
	char* ruta_tabla = string_from_format("%s%s/", g_ruta.tablas, tabla);
	DIR* directorio = opendir(ruta_tabla);
	if (directorio == NULL) {
	 printf("Error: No se puede abrir el directorio\n");
	 exit(2);
	 }

	struct dirent* directorio_leido;
	while((directorio_leido = readdir(directorio)) != NULL) {
//		printf("%d\t%d\t%d\t%s\n", directorio_leido->d_ino,
//								   directorio_leido->d_off,
//								   directorio_leido->d_reclen,
//								   directorio_leido->d_name);
		char* nombre_archivo = directorio_leido->d_name;
		if(string_ends_with(nombre_archivo, "tmp")) {
			char* ruta_tmp = string_from_format("%s%s", ruta_tabla, nombre_archivo);
			t_list* registros_tmp = buscar_key_en_bloques(ruta_tmp, key, 0);
			list_add_all(registros, registros_tmp);
		}
	}
	free(ruta_tabla);
	closedir(directorio);
	return registros;
}

t_list* obtener_registros_key(char* tabla, uint16_t key) {
	t_list* registros_mem = obtener_registros_mem(tabla, key);
	t_list* registros_temp = leer_archivos_temporales(tabla, key);
	t_list* registro_bin = leer_binario(tabla, key);

	t_list* registros_totales = crear_lista_registros(); //free
	list_add_all(registros_totales, registros_mem);
	list_add_all(registros_totales, registros_temp);
	list_add_all(registros_totales, registro_bin);

	borrar_lista_registros(registros_mem);
	borrar_lista_registros(registros_temp);
	borrar_lista_registros(registro_bin);
	return registros_totales;
}

char* obtener_registro_mas_reciente(t_list* registros_de_key) {
	list_sort(registros_de_key, &es_registro_mas_reciente);
	registro_t* registro = list_get(registros_de_key, 0);
	return registro->value;
}

_Bool es_registro_mas_reciente(void* un_registro, void* otro_registro){
	mseg_t ts_un_registro = ((registro_t*)un_registro)->timestamp;
	mseg_t ts_otro_registro = ((registro_t*)otro_registro)->timestamp;
	return (_Bool)(ts_un_registro > ts_otro_registro);
}


//------------EJEMPLO INSTRUCCIONES----------------
void ejemplo_instr_insert() {

	//INSERT
	//[TABLA] [KEY] “[VALUE]” [Timestamp]

	loggear_FS("Ejecutamos Instruccion INSERT");
	instr_t* instr = malloc(sizeof(instr_t));
	instr->timestamp = obtener_ts();
	instr->codigo_operacion = CODIGO_INSERT;
	instr->parametros = list_create();

	list_add(instr->parametros, "Como PCs en el agua");
	list_add(instr->parametros, "1234");
	list_add(instr->parametros, "Hola");

	evaluar_instruccion(instr, 0);

	liberar_memoria_instr(instr);  //Libera memoria del mje

	loggear_FS("Se libero la memoria de la instruccion");
}

void ejemplo_instr_create() {

	//CREATE
	//[TABLA]
	//[TIPO_CONSISTENCIA]
	//[NUMERO_PARTICIONES]
	//[COMPACTION_TIME]

	loggear_FS("Ejecutamos Instruccion CREATE");
	instr_t* instr = malloc(sizeof(instr_t));
	instr->timestamp = obtener_ts();
	instr->codigo_operacion = CODIGO_CREATE;
	instr->parametros = list_create();

	list_add(instr->parametros, "Como PCs en el agua");
	list_add(instr->parametros, "SC");
	list_add(instr->parametros, "5");
	list_add(instr->parametros, "20000");

	evaluar_instruccion(instr, 0);

	liberar_memoria_instr(instr);  //Libera memoria del mje

	loggear_FS("Se libero la memoria de la instruccion");
}

void liberar_memoria_instr(instr_t * instr) {  //Para testeo. Fran libera todo con sus conexiones.
	list_clean(instr->parametros);
	free(instr->parametros);
	free(instr);
}

void inicializar_conexiones(){
	puts("Inicializando conexiones");
	conexionesActuales = dictionary_create();
	callback = evaluar_instruccion;
	puts("callback creado");
	int listenner = iniciar_servidor(miIP, config_FS.puerto_escucha, g_logger, &mutex_log);
	puts("Servidor iniciado");
	vigilar_conexiones_entrantes(listenner, callback, conexionesActuales, CONSOLA_FS, g_logger, &mutex_log);
}

void enviar_tamanio_value(char* remitente){
	int conexionMemoriaN = obtener_fd_out(remitente);
    t_list *listaParam = list_create();
    char* tamanioValue = string_from_format("%d", config_FS.tamanio_value);
    list_add(listaParam, tamanioValue);
    instr_t* miInstruccion = crear_instruccion(obtener_ts(), CODIGO_VALUE, listaParam);
	puts("Enviando tamanio del value");
    enviar_request(miInstruccion, conexionMemoriaN);
    puts("Tamanio del value enviado");
}

void responderHandshake(identificador *idsConexionEntrante)
{
	t_list *listaParam = list_create();
	list_add(listaParam, "FileSystem");
	list_add(listaParam, miIP);
	list_add(listaParam, config_FS.puerto_escucha);
	instr_t *miInstruccion = miInstruccion = crear_instruccion(obtener_ts(), CODIGO_HANDSHAKE, listaParam);

	int fd_saliente = crear_conexion(idsConexionEntrante->ip_proceso, idsConexionEntrante->puerto, miIP, g_logger, &mutex_log);
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

void imprimir_donde_corresponda(cod_op codigoOperacion, instr_t* instruccion, t_list* listaParam, char* remitente)
{
	list_add(listaParam, obtener_ultimo_parametro(instruccion));
	instr_t *miInstruccion;
	switch (quien_pidio(instruccion))
	{

	case CONSOLA_KERNEL:
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion + BASE_CONSOLA_KERNEL, listaParam);
		int conexionReceptor1 = obtener_fd_out(remitente);
		enviar_request(miInstruccion, conexionReceptor1);
		break;
	case CONSOLA_MEMORIA:
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion + BASE_CONSOLA_MEMORIA, listaParam);
		int conexionReceptor2 = obtener_fd_out(remitente);
		enviar_request(miInstruccion, conexionReceptor2);
		break;
	default:
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion, listaParam);
		//Se pidio desde la consola de FS
		if (codigoOperacion == DEVOLUCION_SELECT)
		{
			imprimir_registro(miInstruccion);
		}
		if (codigoOperacion == CODIGO_EXITO)
		{
			loggear_exito_proceso(miInstruccion, g_logger, &mutex_log);
		}
		if (codigoOperacion > BASE_COD_ERROR)
		{
			loggear_error_proceso(miInstruccion, g_logger, &mutex_log);
		}
		break;
	}
}

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


