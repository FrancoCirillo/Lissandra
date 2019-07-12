//-------fileSystem.c-------

#include "fileSystem.h"



int main(int argc, char* argv[]) {

	printf(COLOR_ANSI_CYAN "\n\n************ PROCESO FILESYSTEM ************\n\n" COLOR_ANSI_RESET);
	
	inicializar_FS(argc, argv);

	//ejemplo_aplanar();

//	pruebaDump();
//
//	compactation_locker = 1;
//

//	compactador("T2");

//	compactador("T1");

	inicializar_conexiones();

//	char* ruta0bin = "/home/utnso/lissandra-checkpoint/Tablas/TABLA3/Part0.bin";
//	imprimirContenidoArchivo(ruta0bin, loggear_trace);
//	char* ruta1bin = "/home/utnso/lissandra-checkpoint/Tablas/TABLA3/Part1.bin";
//	imprimirContenidoArchivo(ruta1bin, loggear_trace);
//	char* ruta2bin = "/home/utnso/lissandra-checkpoint/Tablas/TABLA3/Part2.bin";
//	imprimirContenidoArchivo(ruta2bin, loggear_trace);
//	char* ruta3bin = "/home/utnso/lissandra-checkpoint/Tablas/TABLA3/Part3.bin";
//	imprimirContenidoArchivo(ruta3bin, loggear_trace);

//	pruebaGeneral();
//	char* rutaDump1 = "/home/utnso/lissandra-checkpoint/Tablas/TABLA1/Dump1.tmp";
//	imprimirContenidoArchivo(rutaDump1, loggear_trace);

	return 0;
}


//------------------------TESTS------------------------

registro_t* crearRegistro(mseg_t timestampNuevo, uint16_t keyNueva, char *valueNuevo) {

	registro_t registroCreado = {
		.timestamp = timestampNuevo,
		.key = keyNueva,
		.value = valueNuevo};

	registro_t *miReg = malloc(sizeof(registroCreado));
	memcpy(miReg, &registroCreado, sizeof(registroCreado));

	return miReg;
}

//en el segundo parametro se le puede pasar loggear_loquesea (loggear_debug, loggear_info, etc) segun lo que se quiera loggear cuando se llama a la funcion
void imprimirContenidoArchivo(char* ruta, void (*funcion_log)(char *texto)) {
	loggear_trace(string_from_format("---LEO ARCHIVO COMPLETO---"));
	FILE* f = fopen(ruta, "r");
	if(f != NULL){
		char caracter_leido = fgetc(f);
		int leidos = 0;
		char *texto = string_new();

		while(caracter_leido != EOF){
			string_append_with_format(&texto, "%c", caracter_leido);
			caracter_leido = fgetc(f);
			leidos++;
		}
		funcion_log(texto);
		funcion_log(string_from_format("Leidos: %d\n", leidos));
	}
	else loggear_warning(string_from_format("No se pudo abrir el archivo %s", ruta));
}

//Se puede pasar el logger por parametro como en la funcion de arriba o directamente presetearle uno (si sabes que siempre vas a querer el mismo)
void imprimirRegistro(void* reg) {
	registro_t* registro = reg;
	char *texto = string_new();
	string_append_with_format(&texto, "REGISTRO:\n");
	string_append_with_format(&texto, "\tTimestamp: %"PRIu64"\n",registro->timestamp);
	string_append_with_format(&texto, "\tKey: %d\n", registro->key);
	string_append_with_format(&texto, "\tValue: %s\n", registro->value);
	loggear_info(texto);
}

void imprimirMetadata(char* tabla){
	loggear_trace(string_from_format("-------------------Entro a imprimir metadata-------------------"));
	char *texto = string_new();
	string_append_with_format(&texto, "Metadata de la tabla %s\n", tabla);
	int part = obtener_part_metadata(tabla);
	string_append_with_format(&texto, "\tParticiones: %d\n", part);
	char* consist = obtener_consistencia_metadata(tabla);
	string_append_with_format(&texto, "\tConsistencia: %s\n", consist);
	int tiempoComp = obtener_tiempo_compactacion_metadata(tabla);
	string_append_with_format(&texto, "\tTiempo de Compactacion: %d\n", tiempoComp);

	loggear_info(texto);
}

t_list* listaRegistros() {
	t_list* registros = crear_lista_registros();

	registro_t* registro1 = obtener_registro("43242345;253;registro1\n");
	registro_t* registro2 = obtener_registro("43242345;43;registro2\n");
	registro_t* registro3 = obtener_registro("43242345;54;registro3\n");
	registro_t* registro4 = obtener_registro("43242345;5;registro4\n");
	registro_t* registro5 = obtener_registro("99992999;5;registro5\n");
	registro_t* registro6 = obtener_registro("33242345;34;registro6\n");
	registro_t* registro7 = obtener_registro("43224345;64;registro7\n");
	registro_t* registro8 = obtener_registro("43224345;11;registro8\n");
	registro_t* registro9 = obtener_registro("43224345;34;registro9\n");
	registro_t* registro10 = obtener_registro("23224345;5;registro10\n");


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

t_list* listaRegistros2() {
	t_list* registros = crear_lista_registros();

	registro_t* registro1 = obtener_registro("55242345;253;registro1-2\n");
	registro_t* registro2 = obtener_registro("11242345;43;registro2-2\n");
	registro_t* registro3 = obtener_registro("66242345;54;registro3-2\n");
	registro_t* registro4 = obtener_registro("66242345;5;registro4-2\n");
	registro_t* registro5 = obtener_registro("22992999;5;registro5-2\n");
	registro_t* registro6 = obtener_registro("88242345;34;registro6-2\n");
	registro_t* registro7 = obtener_registro("88224345;64;registro7-2\n");
	registro_t* registro8 = obtener_registro("11224345;11;registro8-2\n");
	registro_t* registro9 = obtener_registro("66224345;34;registro9-2\n");
	registro_t* registro10 = obtener_registro("93224345;5;registro10-2\n");


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


//Testeo de prueba de dumpear_tabla(); //Tiene el mismo codigo.
//Lo adapto para usar en compactacion. Genera tmps que uso para testear.
void pruebaDump() {
	void dump(char* tabla, void* registros) {
		loggear_trace(string_from_format("-------------------Entre a dump-------------------"));
		int nro_dump = 2;
		loggear_trace(string_from_format("Numero de Dump: %d\n", nro_dump));
		char* nombre_tmp = string_from_format("Dump%d", nro_dump);
		char* ruta_tmp = string_from_format("%s%s/%s.tmp", g_ruta.tablas, tabla, nombre_tmp);
		loggear_debug(string_from_format("Ruta Temporal: %s\n", ruta_tmp));
		FILE* temporal = crear_archivo(tabla, nombre_tmp, ".tmp");
		int nro_bloque = archivo_inicializar(temporal); //TODO
		loggear_debug(string_from_format("Al temporal %s se le asigno el bloque %d\n", nombre_tmp, nro_bloque));

		void escribir_reg_en_tmp(void* registro) {
			loggear_trace(string_from_format("-------------------Entro a escribir_reg_en_tmp-------------------"));
			imprimirRegistro((registro_t*)registro);
			int bloque = obtener_ultimo_bloque(ruta_tmp);
			char* ruta_bloque = obtener_ruta_bloque(bloque);
			printf("Ruta Bloque: %s\nRuta TMP: %s\n", ruta_bloque, ruta_tmp);

			escribir_registro_bloque((registro_t*)registro, ruta_bloque, ruta_tmp);

		//	loggear_trace(string_from_format("Escribi el registro en bloque"));
		}

		list_iterate((t_list*)registros, &escribir_reg_en_tmp);

		free(ruta_tmp);
		free(nombre_tmp);
		fclose(temporal);
		loggear_trace(string_from_format("Cierro el temporal"));
	}

	t_dictionary* mockMem = dictionary_create();

	//--agrego una tabla con registros---
	t_list* reg = listaRegistros2();
	dictionary_put(mockMem, "T1", reg);

	loggear_info(string_from_format("Se agrego la tabla en la memtable."));

	//--agrego otra tabla--
	t_list* registros = crear_lista_registros();
	dictionary_put(mockMem, "T2", registros);

	loggear_info(string_from_format("Se agrego la tabla en la memtable."));

	//--creo registros--
	registro_t* registro2 = crearRegistro(4324234, 10, "ChauVoyEnT2");
//	loggear_info(string_from_format("Cree un registro:"));
	imprimirRegistro(registro2);

	registro_t* registro = crearRegistro(4324234, 3, "ChauTmbVoyEnT2");
//	loggear_info(string_from_format("Cree un registro:"));
	imprimirRegistro(registro);

	//--agrego registros--
	t_list* registros_tabla = dictionary_get(mockMem, "T2");
	list_add(registros_tabla, registro);
	list_add(registros_tabla, registro2);

	loggear_info(string_from_format("Se inserto el registro en la tabla T2 en la memtable."));

	//--hago dump--
	dictionary_iterator(mockMem, &dump);

}

void pruebaTmp() {
		char* ruta_archivo = "/home/utnso/lissandra-checkpoint/Tablas/TABLA1/Dump1.tmp";

//		---prueba de blocks archivo---
		agregar_bloque_archivo(ruta_archivo, 6);
		agregar_bloque_archivo(ruta_archivo, 35);
		int cant = cantidad_bloques_usados(ruta_archivo);
		loggear_debug(string_from_format("Bloques: %d\n", cant));

		int primero = obtener_siguiente_bloque_archivo(ruta_archivo, -1);
		loggear_debug(string_from_format("PRIMER BLOQUE: %d\n", primero));
		int sig = obtener_siguiente_bloque_archivo(ruta_archivo, 0);
		loggear_debug(string_from_format("SIGUIENTE BLOQUE: %d\n", sig));
		int ultimo = obtener_siguiente_bloque_archivo(ruta_archivo, 35);
		loggear_debug(string_from_format("ULTIMO BLOQUE: %d\n", ultimo));

		//---prueba de size archivo---
		t_config* archivo = config_create(ruta_archivo);
		loggear_debug(string_from_format("Config create"));
		char* nro = string_itoa(10);
		config_set_value(archivo, "SIZE", nro);
		config_save(archivo);
		loggear_debug(string_from_format("Set value SIZE = 10"));

		int size = config_get_int_value(archivo, "SIZE");
		loggear_debug(string_from_format("Tam archivo: %d\n", size));
}

void pruebaGeneral() {
	pruebaDump();

	puts("Ya termino el dumpeo");

//	char* bloque0 = obtener_ruta_bloque(0);
//	printf("\n\nRuta bloque 0: %s\n", bloque0);
//	imprimirContenidoArchivo(bloque0);
//
//	char* bloque1 = obtener_ruta_bloque(1);
//	printf("\n\nRuta bloque 1: %s\n", bloque1);
//	imprimirContenidoArchivo(bloque1);
//
//	char* bloque2 = obtener_ruta_bloque(2);
//	printf("\n\nRuta bloque 2: %s\n", bloque2);
//	imprimirContenidoArchivo(bloque2);

	char* ruta = "/home/utnso/lissandra-checkpoint/Tablas/TABLA1/Dump1.tmp";

	t_list* registros = buscar_key_en_bloques(ruta, 34, 1);
	loggear_debug(string_from_format("Estos son los registros que encontre:"));
	list_iterate(registros, &imprimirRegistro);
	char* value = obtener_registro_mas_reciente(registros);
	loggear_debug(string_from_format("\nVALUE DEL REGISTRO MAS RECIENTE: %s\n", value));
}

//------------INICIALIZACION Y FINALIZACION-----------------

void inicializar_FS(int argc, char* argv[]) {
	iniciar_semaforos();
	inicializar_configuracion();
	iniciar_logger();

	if(argc == 1 || strlen(argv[1])<2){
		puts("IP 127.0.0.2");
		miIP = IP_FILESYSTEM;
	}
	else{
		printf("IP %s\n", argv[1]);
		miIP = argv[1];
	}

	loggear_info(string_from_format("-----------INICIO PROCESO-----------"));
	iniciar_rutas();
	inicializar_memtable();
	iniciar_dumpeo();
	compactation_locker = 0; //Se inicializa var. global
	inicializar_directorios();
	crear_bloques();
	inicializar_bitmap();
	inicializar_bitarray();
	inicializar_bloques_disp();

	loggear_info(string_from_format("-----------Fin inicialización LFS-----------"));

}

void finalizar_FS(instr_t* instruccion) {
	list_destroy_and_destroy_elements(instruccion->parametros, free);
	free(instruccion);

	finalizar_memtable();
	finalizar_bitarray();
	config_destroy(g_config);
	log_destroy(g_logger);

	finalizar_rutas();
	finalizar_tablas_nro_dump();
	finalizar_dic_semaforos_tablas();
	finalizar_diccionarios_conexiones();

	printf(COLOR_ANSI_CYAN "\n\n************ FIN PROCESO ************\n\n" COLOR_ANSI_RESET);
	exit(0);
}

void iniciar_semaforos() {
	crear_dic_semaforos_FS();
	sem_init(&mutex_tiempo_dump_config, 0, 1);
	sem_init(&mutex_tiempo_retardo_config, 0, 1);
	sem_init(&mutex_memtable, 0, 1);
	sem_init(&mutex_log, 0, 1);
	sem_init(&mutex_bitarray, 0, 1);
	sem_init(&mutex_cant_bloques, 0, 1);
	sem_init(&mutex_tablas_nro_dump, 0, 1);
	sem_init(&mutex_diccionario_conexiones, 0, 1);
	tablas_nro_dump = dictionary_create();
}

void iniciar_rutas(){
	g_ruta.tablas = string_from_format("%sTablas/", config_FS.punto_montaje);
	g_ruta.bloques = string_from_format("%sBloques/", config_FS.punto_montaje);
	g_ruta.carpeta_metadata = string_from_format("%sMetadata/", config_FS.punto_montaje);
	g_ruta.metadata = string_from_format("%sMetadata/Metadata.bin", config_FS.punto_montaje);
	g_ruta.bitmap = string_from_format("%sMetadata/Bitmap.bin", config_FS.punto_montaje);
}

void finalizar_rutas(){
	free(g_ruta.tablas);
	free(g_ruta.bloques);
	free(g_ruta.carpeta_metadata);
	free(g_ruta.metadata);
	free(g_ruta.bitmap);
}

void finalizar_diccionarios_conexiones(){
	dictionary_destroy_and_destroy_elements(conexionesActuales, free);
	dictionary_destroy_and_destroy_elements(auxiliarConexiones, (void*)free);
}


//------------FUNCIONES DE BLOQUES------------

t_list* leer_binario(char* tabla, uint16_t key) {
	loggear_trace(string_from_format("---Buscando en el binario---"));
	int particion = obtener_particion_key(tabla, key);
	char* ruta_bin = string_from_format("%s%s/Part%d.bin", g_ruta.tablas, tabla, particion);
	imprimirContenidoArchivo(ruta_bin, loggear_debug); //Aca se podria pasar loggear_trace
	t_list* registro_key = buscar_key_en_bloques(ruta_bin, key, 0);
	free(ruta_bin);
	if(registro_key != NULL)
		loggear_debug(string_from_format("Tam de lista binarios: %d\n", list_size(registro_key)));
	return registro_key;
}

t_list* leer_archivos_temporales(char* tabla, uint16_t key) {
	loggear_trace(string_from_format("---Buscando en los temporales---"));
	t_list* registros = crear_lista_registros();
	char* ruta_tabla = string_from_format("%s%s/", g_ruta.tablas, tabla);
	DIR* directorio = opendir(ruta_tabla);
	if (directorio == NULL) {
		loggear_error(string_from_format("Error: No se puede abrir el directorio %s\n", ruta_tabla));
		free(ruta_tabla);
		closedir(directorio);
		return NULL;
	}

	struct dirent* directorio_leido;
	while((directorio_leido = readdir(directorio)) != NULL) {
		loggear_debug(string_from_format("Directorio leido: %s\n", directorio_leido->d_name));
		char* nombre_archivo = directorio_leido->d_name;
		if(string_ends_with(nombre_archivo, "tmp") || string_ends_with(nombre_archivo, "tmpc")) {
			char* ruta_tmp = string_from_format("%s%s", ruta_tabla, nombre_archivo);
			loggear_info(string_from_format("RUTA:%s\n", ruta_tmp));
//			imprimirContenidoArchivo(ruta_tmp, loggear_trace);
			t_list* registros_tmp = buscar_key_en_bloques(ruta_tmp, key, 1);
			list_add_all(registros, registros_tmp);
		}
	}
	free(ruta_tabla);
	closedir(directorio);
	loggear_debug(string_from_format("Tam de lista temporales: %d\n", list_size(registros)));
	return registros;
}

char* obtener_registro_mas_reciente(t_list* registros_de_key) {
	list_sort(registros_de_key, &es_registro_mas_reciente);
	registro_t* registro = list_get(registros_de_key, 0);
	return registro->value;
}

t_list* obtener_registros_key(char* tabla, uint16_t key) {
	sem_wait(&mutex_dic_semaforos);
	sem_t* mutex_tabla = obtener_mutex_tabla(tabla);
	sem_post(&mutex_dic_semaforos);

	sem_wait(mutex_tabla);
	t_list* registros_mem = obtener_registros_mem(tabla, key);
	t_list* registros_temp = leer_archivos_temporales(tabla, key);
	t_list* registro_bin = leer_binario(tabla, key);
	sem_post(mutex_tabla);

	t_list* registros_totales = crear_lista_registros(); //free
	list_add_all(registros_totales, registros_mem);
	list_add_all(registros_totales, registros_temp);
	if(registro_bin != NULL)
	{
		list_add_all(registros_totales, registro_bin);
	}
	else {
		//TODO FREES?
		return NULL;
	}
//	borrar_lista_registros(registros_mem);
//	borrar_lista_registros(registros_temp);
//	borrar_lista_registros(registro_bin);
	return registros_totales;
}


//------------FUNCIONES AUXILIARES------------

char* obtener_nombre_tabla(instr_t* instr) {
	return obtener_parametro(instr, 0);
}

char* obtener_parametro(instr_t * instr, int index) {
	return (char*) list_get(instr->parametros, index);
}

int obtener_particion_key(char* tabla, int key) {
	int cant_particiones = obtener_part_metadata(tabla);
	return key % cant_particiones;
}

_Bool es_registro_mas_reciente(void* un_registro, void* otro_registro){
	mseg_t ts_un_registro = ((registro_t*)un_registro)->timestamp;
	mseg_t ts_otro_registro = ((registro_t*)otro_registro)->timestamp;
	return (_Bool)(ts_un_registro > ts_otro_registro);
}


//------------CONEXIONES----------------


void inicializar_conexiones() {
	loggear_trace(string_from_format("Inicializando conexiones"));
	conexionesActuales = dictionary_create();
	auxiliarConexiones = dictionary_create();
	callback = evaluar_instruccion;
	fd_out_inicial = 0;
	iniciar_servidor(miIP, config_FS.puerto_escucha);
	// . es el directorio Actual - Tener en cuenta que lo corremos desde la carpeta padre a la que tiene el binario
	vigilar_conexiones_entrantes(callback, actualizar_config, proceso_desconectado, ".", CONSOLA_FS);
}

void enviar_tamanio_value(instr_t* instruccion, char* remitente) {
	int conexionMemoriaN = obtener_fd_out(remitente);
	t_list *listaParam = list_create();
    char* tamanioValue = string_from_format("%d", config_FS.tamanio_value);
    list_add(listaParam, tamanioValue);
    char* puntoMontaje = string_from_format("%s", config_FS.punto_montaje);
    list_add(listaParam, puntoMontaje);

    instr_t* miInstruccion = crear_instruccion(obtener_ts(), CODIGO_VALUE, listaParam);
    enviar_liberando_request(miInstruccion, conexionMemoriaN);
    loggear_trace(string_from_format("Tamanio del value y punto de montaje enviados"));

	list_destroy(instruccion->parametros);
	free(instruccion);
}

void responderHandshake(identificador *idsConexionEntrante) {
	t_list *listaParam = list_create();
	list_add(listaParam, "FileSystem");
	list_add(listaParam, miIP);
	list_add(listaParam, config_FS.puerto_escucha);
	instr_t *miInstruccion = miInstruccion = crear_instruccion(obtener_ts(), CODIGO_HANDSHAKE, listaParam);

	int fd_saliente = crear_conexion(idsConexionEntrante->ip_proceso, idsConexionEntrante->puerto, miIP, 1);
	enviar_request(miInstruccion, fd_saliente);
	idsConexionEntrante->fd_out = fd_saliente;
}

int obtener_fd_out(char *proceso) {
	sem_wait(&mutex_diccionario_conexiones);
	identificador *idsProceso = (identificador *)dictionary_get(conexionesActuales, proceso);
	sem_post(&mutex_diccionario_conexiones); //quizas en todo obtener_fd_out iria

	if(idsProceso == NULL){
		loggear_error(string_from_format("Se desconoce completamente el proceso %s", proceso));
		return 0;
	}
	if (idsProceso->fd_out == 0) {
		loggear_trace(string_from_format("Es la primera vez que se quiere enviar algo al proceso"));
		responderHandshake(idsProceso);
	}
	return idsProceso->fd_out;
}

void imprimir_donde_corresponda(cod_op codigoOperacion, instr_t* instruccion, t_list* listaParam, char* remitente) {
	void *ultimoParametro = obtener_ultimo_parametro(instruccion);

	instr_t *miInstruccion;

	switch (quien_pidio(instruccion)) {
	case CONSOLA_KERNEL:
		puts("La instrucicon la pidio la consola del Kernel");
		if (ultimoParametro != NULL){
				list_add(listaParam, (char*)ultimoParametro);
		}
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion + BASE_CONSOLA_KERNEL, listaParam);
		int conexionReceptor1 = obtener_fd_out(remitente);
		t_list* listaABorrar = list_duplicate(miInstruccion->parametros);
		enviar_request(miInstruccion, conexionReceptor1);
		//list_remove(listaABorrar, list_size(listaABorrar)-1);
		list_destroy_and_destroy_elements(listaABorrar, free);
		loggear_trace(string_from_format("Parametros freed"));
		break;

	case CONSOLA_MEMORIA:
		puts("La instrucicon la pidio la consola de Memoria");
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion + BASE_CONSOLA_MEMORIA, listaParam);
		int conexionReceptor2 = obtener_fd_out(remitente);
		enviar_liberando_request(miInstruccion, conexionReceptor2);
		break;

	default: //Consola file system
		puts("La instrucicon la pidio la consola del FS");
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion, listaParam);

		if (codigoOperacion == DEVOLUCION_SELECT) //Se pidio desde la consola de FS
			imprimir_registro(miInstruccion, loggear_info);

		if (codigoOperacion == CODIGO_EXITO)
			loggear_exito_proceso(miInstruccion);

		if (codigoOperacion > BASE_COD_ERROR)
			loggear_error_proceso(miInstruccion);

		list_destroy_and_destroy_elements(miInstruccion->parametros, free);
		free(miInstruccion);

		break;
	}
	free(instruccion);
}

void actualizar_config(){

	t_config * auxConfig;
	while((auxConfig = config_create(rutaConfiguracion)) == NULL||
			!config_has_property(auxConfig, "RETARDO")			||
			!config_has_property(auxConfig, "TIEMPO_DUMP")		||
			!config_has_property(auxConfig, "LOG_LEVEL")){
		config_destroy(auxConfig);
	}

	sem_wait(&mutex_tiempo_retardo_config);
	config_FS.retardo = (mseg_t)config_get_int_value(auxConfig, "RETARDO");
	sem_post(&mutex_tiempo_retardo_config);

	sem_wait(&mutex_tiempo_dump_config);
	config_FS.tiempo_dump = (mseg_t)config_get_int_value(auxConfig, "TIEMPO_DUMP");
	sem_post(&mutex_tiempo_dump_config);

	config_FS.LOG_LEVEL = log_level_from_string(config_get_string_value(auxConfig, "LOG_LEVEL"));
	sem_wait(&mutex_log);
	actualizar_log_level();
	sem_post(&mutex_log);
	config_destroy(auxConfig);

	loggear_info(string_from_format("Se ha actualizado el archivo de config.\n"
									"Retardo: %" PRIu64 "\n"
									"Tiempo de Dump: %" PRIu64
									"\nLog level: %s",
									config_FS.retardo, config_FS.tiempo_dump, log_level_as_string(config_FS.LOG_LEVEL)));

	printf("\n"COLOR_ANSI_MAGENTA ">" COLOR_ANSI_RESET);
	fflush(stdout);

}

void actualizar_log_level(){
	log_destroy(g_logger);
	g_logger = log_create("Lissandra.log", "FileSystem", 1, config_FS.LOG_LEVEL);
}

void proceso_desconectado(char* nombreProceso){}
