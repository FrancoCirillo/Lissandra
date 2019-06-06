//-------fileSystem.c-------

#include "fileSystem.h"

int main() {

	printf("\n\n************PROCESO FILESYSTEM************\n\n");
	inicializar_FS();


	//testeo
	ejemplo_instr_create();
	//ejemplo_instr_insert();


	finalizar_FS();
	return 0;

}

void inicializar_FS() {
	iniciar_semaforos();
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

void evaluar_instruccion(instr_t* instr) {

	int respuesta = 0;

	//Aca pasar el nombre de la tabla a Mayuscula. y nos desligamos de esto en el resto de los pasos.

	switch (instr->codigo_operacion) {

	case CODIGO_CREATE:
		loggear_FS("Me llego una instruccion CREATE.");
		respuesta = execute_create(instr);
		break;

	case CODIGO_INSERT:
		loggear_FS("Me llego una instruccion INSERT.");
		respuesta = execute_insert(instr);
		//respuesta = execute_insert(mensaje->instruccion);
		break;

	case CODIGO_SELECT:
		loggear_FS("Me llego una instruccion SELECT.");
		respuesta = execute_select(instr);
		break;

	case CODIGO_DESCRIBE:
		loggear_FS("Me llego una instruccion DESCRIBE.");
		//respuesta = execute_describe(instr);
		break;

	case CODIGO_DROP:
		loggear_FS("Me llego una instruccion DROP.");
		//respuesta = execute_drop(instr);
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

int execute_create(instr_t* instr) {
	char* tabla = obtener_nombre_tabla(instr);
	//TODO string_to_upper(tabla);
	if (!existe_tabla(tabla)) {
		if(!puede_crear_particiones(instr)) {
			loggear_FS_error("No hay bloques disponibles para crear las particiones.", instr);
			return ERROR_CREATE;
		}
		agregar_tabla(tabla); //la agrega a la mem
		crear_directorio(g_ruta.tablas, tabla);
		crear_particiones(instr);
		crear_metadata(instr);
		char* mensaje = string_from_format("Se creo el directorio, el metadata y las particiones de la tabla: %s", tabla);
//		char* mensaje = malloc(sizeof(char)* 80);
//		sprintf(mensaje, "Se creo el directorio, el metadata y las particiones de la tabla: %s", tabla);
		loggear_FS(mensaje);
		free(mensaje);
		return CODIGO_EXITO;
	} else {
		loggear_FS_error("Error al crear la tabla, la tabla ya existe en el FS.", instr);
		return ERROR_CREATE;
	}
}


int execute_insert(instr_t* instr) { //no esta chequeado
	char* tabla = obtener_nombre_tabla(instr);
	//string_to_upper(tabla);
	registro_t* registro = pasar_a_registro(instr); //VALIDAR SI TAM_VALUE ES MAYOR AL MAX_TAM_VALUE
	if (!existe_tabla(tabla)) {
		loggear_FS_error("La tabla no existe en el File System.", instr);
		return ERROR_INSERT;
	} else {
		agregar_registro(tabla, registro);
	}
	return CODIGO_EXITO;
}
//NOTA DAI: Una tabla existe si en la mem hay un nodo de esa tabla vacia.
//es decir, no hay que validar en la mem ademas de en el FS.


int execute_select(instr_t* instr) {
	char* tabla = obtener_nombre_tabla(instr);
	//string_to_upper(tabla);
	if (!existe_tabla(tabla)) {
		return ERROR_SELECT; //TODO log: no existe tabla
	}
	registro_t* registro = pasar_a_registro(instr);
	int key = registro->key;
	t_list* registros_key = obtener_registros_key(tabla, key);
	if(list_is_empty(registros_key)){
		return ERROR_SELECT;
		//TODO log: no se encontraron registros con esa key
	}
	char* value_registro_reciente = obtener_registro_mas_reciente(registros_key); //respuesta del select
	return CODIGO_EXITO;
}

int execute_drop(instr_t* instr) {
	char* tabla = obtener_nombre_tabla(instr);
	//string_to_upper(tabla);
	if (!existe_tabla(tabla)) {
		return ERROR_DROP; //TODO log:no existe tabla
	}
	eliminar_tabla_de_mem(tabla);
	eliminar_directorio(tabla); //adentro tiene un eliminar_archivos(tabla)
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

registro_t* leer_binario(char* p1, uint16_t p2){}

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

	evaluar_instruccion(instr);

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

	evaluar_instruccion(instr);

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

