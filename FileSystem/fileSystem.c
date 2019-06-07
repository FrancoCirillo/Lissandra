//-------fileSystem.c-------

#include "fileSystem.h"

int main() {

	printf("\n\n************PROCESO FILESYSTEM************\n\n");
	inicializar_FS();

	inicializar_conexiones();
	//testeo
	ejemplo_instr_create();
	//ejemplo_instr_insert();


	finalizar_FS();
	return 0;

}

void inicializar_FS() {
	iniciar_semaforos();
	iniciar_logger();
	loggear_FS("-----------INICIO PROCESO-----------");
	inicializar_configuracion();
	iniciar_rutas();
	inicializar_memtable();
	inicializar_directorios();
	puts("Directorios listossssssss");
	crear_bloques(); //inicializa tambien la var globlal de bloques disp.
	inicializar_bitmap();
	loggear_FS("-----------Fin inicialización LFS-----------");
}

void finalizar_FS() {
	config_destroy(g_config);
	//config_destroy(meta_config); No es necesario.
	finalizar_rutas();
	finalizar_memtable();  // Liberar memoria
	loggear_FS("-----------FIN PROCESO-----------");
}

void iniciar_semaforos() {
	sem_init(&mutex_tiempo_dump_config, 0, 1);
	sem_init(&mutex_tiempo_retardo_config, 0, 1);
	sem_init(&mutex_memtable, 0, 1);
	sem_init(&mutex_log, 0, 1);
	sem_init(&mutex_cant_bloques, 0, 1);
}

void iniciar_rutas(){
	g_ruta.tablas = concat(config_FS.punto_montaje, "Tablas/");
	g_ruta.bloques = concat(config_FS.punto_montaje, "Bloques/");
	g_ruta.metadata = concat(config_FS.punto_montaje, "Metadata/Metadata.bin");
	g_ruta.bitmap = concat(config_FS.punto_montaje, "Metadata/Bitmap.bin");
}

void finalizar_rutas(){
	free(g_ruta.tablas);
	free(g_ruta.bloques);
	free(g_ruta.metadata);
	free(g_ruta.bitmap);
}

//------------MANEJO INSTRUCCIONES-----------------

void evaluar_instruccion(instr_t* instr, char* remitente) {

	int respuesta = 0;
	int codigoNeto = instr->codigo_operacion %100; //Los primeros dos digitos son los posibles codigos de operacion
	//Aca pasar el nombre de la tabla a Mayuscula. y nos desligamos de esto en el resto de los pasos.

	switch (codigoNeto) {

	case CODIGO_CREATE:
		loggear_FS("Me llego una instruccion CREATE.");
		respuesta = execute_create(instr, remitente);
		break;

	case CODIGO_INSERT:
		loggear_FS("Me llego una instruccion INSERT.");
		respuesta = ejecutar_instruccion_insert(instr, remitente);
		//respuesta = execute_insert(mensaje->instruccion);
		break;

	case CODIGO_SELECT:
		loggear_FS("Me llego una instruccion SELECT.");
		respuesta = execute_select(instr, remitente);
		break;

	case CODIGO_DESCRIBE:
		loggear_FS("Me llego una instruccion DESCRIBE.");
		respuesta = execute_describe(instr, remitente);
		break;

	case CODIGO_DROP:
		loggear_FS("Me llego una instruccion DROP.");
		respuesta = execute_drop(instr, remitente);
		break;

	case CODIGO_VALUE:
			enviar_tamanio_value(remitente);
			break;

	default:
		//verrrr
		loggear_FS("Me llego una instruccion invalida dentro del File System.");
	}

	instr->codigo_operacion = respuesta; //Esto pisa el codigo de operacion del mensaje para enviarle a memoria.

	/*
	 contestar(mensaje);   ESTO SI VA ACA. Pero para pruebas lo comento.
	 //este hace los free de la instruccion completa.
	 */

	loggear_FS("Finalizó la ejecución de la instrucción.");
}

int execute_create(instr_t* instruccion, char* remitente) {
	char* tabla = obtener_nombre_tabla(instruccion);
	//TODO string_to_upper(tabla);
	t_list *listaParam = list_create();
	if (!existe_tabla(tabla)) {
		if(!puede_crear_particiones(instruccion)) {
			char* cadena = string_from_format("No hay bloques disponibles para crear las particiones de la tabla'%s'.", tabla); //TODO: free
			list_add(listaParam, cadena);
			imprimir_donde_corresponda(ERROR_CREATE, instruccion, listaParam, remitente);
			return ERROR_CREATE;
		}
		agregar_tabla(tabla); //la agrega a la mem
		crear_directorio(g_ruta.tablas, tabla);
		crear_particiones(instruccion);
		crear_metadata(instruccion);
		char* cadena = string_from_format("Se creo el directorio, el metadata y las particiones de la tabla: %s", tabla);
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam, remitente);
		return CODIGO_EXITO;
	} else {
		char* cadena = string_from_format("Error al crear la tabla '%s', ya existe en el FS.", tabla); //TODO: free
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(ERROR_CREATE, instruccion, listaParam, remitente);
		return ERROR_CREATE;
	}
}


t_list* execute_insert(instr_t* instruccion, cod_op* codOp) { //no esta chequeado
	t_list *listaParam = list_create();
	char* tabla = obtener_nombre_tabla(instruccion);
	//string_to_upper(tabla);
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
//NOTA DAI: Una tabla existe si en la mem hay un nodo de esa tabla vacia.
//es decir, no hay que validar en la mem ademas de en el FS.


int execute_select(instr_t* instruccion, char* remitente) {
	char* tabla = obtener_nombre_tabla(instruccion);
	t_list *listaParam = list_create();
	//string_to_upper(tabla);
	if (!existe_tabla(tabla)) {
		char* cadena = string_from_format("No existe la tabla '%s'", tabla); //TODO: free
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(ERROR_SELECT, instruccion, listaParam, remitente);
		return ERROR_SELECT;
	}
	registro_t* registro = pasar_a_registro(instruccion);
	int key = registro->key;
	t_list* registros_key = obtener_registros_key(tabla, key);
	if(list_is_empty(registros_key)){
		char* cadena = string_from_format("No se encontraron registros con la key '%s'", (char *)list_get(instruccion->parametros, 1)); //TODO: free
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(ERROR_SELECT, instruccion, listaParam, remitente);
		return ERROR_SELECT;
	}
	registro_t* registro_reciente = obtener_registro_mas_reciente(registros_key); //respuesta del select
	list_add(listaParam, (char *)list_get(instruccion->parametros, 0)); //Tabla //TODO: Usar los campos de registro_reciente (declaratividad)
	list_add(listaParam, (char *)list_get(instruccion->parametros, 1)); //Key
	list_add(listaParam, registro_reciente->value);						//Value
	imprimir_donde_corresponda(DEVOLUCION_SELECT, instruccion, listaParam, remitente);
	return CODIGO_EXITO;
}

int execute_drop(instr_t* instruccion, char* remitente) {
	char* tabla = obtener_nombre_tabla(instruccion);
	t_list *listaParam = list_create();


	//string_to_upper(tabla);
	if (!existe_tabla(tabla)) {
		char* cadena = string_from_format("No existe la tabla '%s'", tabla); //TODO: free
		imprimir_donde_corresponda(ERROR_DROP, instruccion, listaParam, remitente);
		list_add(listaParam, cadena);
		return ERROR_DROP;
	}
	eliminar_tabla_de_mem(tabla);
	eliminar_directorio(tabla); //adentro tiene un eliminar_archivos(tabla)
	char* cadena = string_from_format("Se eliminpo correctamente la tabla '%s'", tabla); //TODO: free
	imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam, remitente);
	list_add(listaParam, cadena);
	return CODIGO_EXITO;
}

char* obtener_nombre_tabla(instr_t* instr) { //Azul: aca se puede hacer el string_to_upper();
	return obtener_parametro(instr, 0);
}

char* obtener_parametro(instr_t * instr, int index) {
	return (char*) list_get(instr->parametros, index);
}

int obtener_particion_key(char* tabla, int key) {
	int cant_particiones = obtener_part_metadata(tabla);
	return key % cant_particiones;
}

t_list* obtener_registros_key(char* tabla, uint16_t key) {
	t_list* registros_mem = obtener_registros_mem(tabla, key);
	t_list* registros_temp = leer_archivos_temporales(tabla, key);
	registro_t* registro_bin = leer_binario(tabla, key); // int particion = obtener_particion_key(tabla, key);

	t_list* registros_totales = crear_lista_registros(); //free
	list_add_all(registros_totales, registros_mem);
	list_add_all(registros_totales, registros_temp);
	list_add(registros_totales, registro_bin);
	return registros_totales;
}


//registro_t* obtener_registro_mas_reciente(t_list* registros_de_key){
//	list_sort(registros_de_key, &es_registro_mas_reciente);
//	return list_get(registros_de_key, 0);
//}
//
//_Bool es_registro_mas_reciente(void* un_registro, void* otro_registro){
//	mseg_t ts_un_registro = ((registro_t*)un_registro)->timestamp;
//	mseg_t ts_otro_registro = ((registro_t*)otro_registro)->timestamp;
//	return (_Bool)(ts_un_registro > ts_otro_registro);
//}

t_list* leer_archivos_temporales(char* tabla, uint16_t key) { //TODO hacer
	return crear_lista_registros();
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

	list_add(instr->parametros, "ALUMNOS");
	list_add(instr->parametros, "1234");
	list_add(instr->parametros, "Hola");
	//list_add(instr->parametros, "60000"); //timestamp lo recibimos en la instr

	evaluar_instruccion(instr, 0);

	contestar(instr);  //Libera memoria del mje

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

	//testeo
	//	crear_metadata(instr);
	//	int cc= atoi(c);
	//	int p = crear_particiones(a,cc);
	//		if(p>0){
	//			puts("exito en las particiones");
	//			}
	//			else puts("fallaron");

	contestar(instr);  //Libera memoria del mje

	loggear_FS("Se libero la memoria de la instruccion");
}

char* concat(char *s1, char *s2) {
	char *result = malloc(strlen(s1) + strlen(s2) + 1);
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}

char* concat3(char *s1, char *s2, char * s3) {
	char *result = malloc(strlen(s1) + strlen(s2) + strlen(s3) + 1);
	strcpy(result, s1);
	strcat(result, s2);
	strcat(result, s3);
	return result;
}

void contestar(instr_t * instr) {
	//usa responder()
	list_clean(instr->parametros);
	free(instr->parametros);
	free(instr);
}

void inicializar_conexiones(){
	puts("Inicializando conexiones");
	conexionesActuales = dictionary_create();
	callback = evaluar_instruccion;
	puts("callback creado");
	int listenner = iniciar_servidor(IP_FILESYSTEM, PORT, g_logger, &mutex_log);
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
	list_add(listaParam, IP_FILESYSTEM);
	list_add(listaParam, config_FS.puerto_escucha); //TODO configuracion.PUERTO
	instr_t *miInstruccion = miInstruccion = crear_instruccion(obtener_ts(), CODIGO_HANDSHAKE, listaParam);

	int fd_saliente = crear_conexion(idsConexionEntrante->ip_proceso, idsConexionEntrante->puerto, IP_FILESYSTEM, g_logger, &mutex_log);
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

void imprimir_donde_corresponda(cod_op codigoOperacion, instr_t *instruccion, t_list *listaParam, char *remitente)
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

int ejecutar_instruccion_insert(instr_t* instruccion, char* remitente){
	cod_op codOp;
	t_list* resultadoInsert;

	if(quien_pidio(instruccion) == CONSOLA_FS){
		puts("Ejecutando instruccion Insert");
		resultadoInsert = list_duplicate(execute_insert(instruccion, &codOp));
		void imprimir(char* a){
			puts("aasassasaas");
			printf("%s\n",a);
		}
		list_iterate(resultadoInsert, (void*)imprimir);
		puts("assdfasldfjas");
		imprimir_donde_corresponda(codOp, instruccion, resultadoInsert, remitente);
		return (int) codOp;
	}
	else{
		puts("No lo pidio FS");
		resultadoInsert = execute_insert(instruccion, &codOp);
		if(codOp == ERROR_INSERT){
			instruccion->codigo_operacion = CONSOLA_MEM_INSERT; //Para que el error se mustre en la memoria
			imprimir_donde_corresponda(codOp, instruccion, resultadoInsert, remitente);
		}
			return (int) codOp;
	}
}

int execute_describe(instr_t* instruccion, char* remitente){
	t_list *listaParam = list_create();
	char* tabla = obtener_nombre_tabla(instruccion);
	char* blockSize = string_from_format("%d", 12);
	char* blocks = string_from_format("%d", 64);
	char* magicNumber = string_from_format("%s", "LISSANDRA");

	list_add(listaParam, tabla);
	list_add(listaParam, blockSize);
	list_add(listaParam, blocks);

	list_add(listaParam, magicNumber);
	imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam, remitente);
	return CODIGO_EXITO;
}
