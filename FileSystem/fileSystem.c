//-------fileSystem.c-------

#include "fileSystem.h"

int main() {

	printf("\n\n************PROCESO FILESYSTEM************\n\n");
	inicializar_FS();

	//testeo
	ejemplo_instr_create();

	//////////////////////////////////////////////

	//	int listenner = iniciar_servidor(IP_FILESYSTEM, PORT);

	//	vigilar_conexiones_entrantes(listenner);

	//////////////////////////////////////////////

	//Esto va dentro del create, pero estoy probando.

	//////////////////////////////////////
	//FUNCIONA.
//		crear_directorio(RUTA_TABLAS,"Tablota");
//		FILE* f = crear_archivo("Part_3", "Tablota", ".bin");
//		int i= archivo_inicializar(f);
//		if(i>0){
//			puts("exito");
//		}
//		else puts("fallo");
	//////////////////////////////////////

	//Funciona
	//actualizar_tiempo_retardo_config(600);
	//printf("El valor es: %d.",(int)config_FS.retardo);
	//Joyita.
	//printf("%ld",config_FS.tiempo_dump);
	//actualizar_tiempo_retardo_config();

	//////////////////////////////////////

	finalizar_FS();
	return 0;

}

void inicializar_FS() {
	iniciar_semaforos();
	loggear_FS("-----------INICIO PROCESO-----------");
	inicializar_configuracion();
	inicializar_memtable();
	inicializar_directorios();
	crear_bloques();
	loggear_FS("-----------Fin inicialización LFS-----------");
}

void finalizar_FS() {
	config_destroy(g_config);
	config_destroy(meta_config);
	finalizar_memtable();  // Liberar memoria
	loggear_FS("-----------FIN PROCESO-----------");
}

void iniciar_semaforos() {
	sem_init(&mutex_tiempo_dump_config, 0, 1);
	sem_init(&mutex_tiempo_retardo_config, 0, 1);
	sem_init(&mutex_memtable, 0, 1);
	sem_init(&mutex_log, 0, 1);
}

//------------MANEJO INSTRUCCIONES-----------------

void evaluar_instruccion(instr_t* instr) {

	int respuesta = 0;
	loggear_FS("Me llego la instruccion: ");  //Todo se debe loggear.

	switch (instr->codigo_operacion) {

	case CODIGO_CREATE:
			loggear_FS("Me llego una instruccion CREATE.");
			respuesta = execute_create(instr);	//todos tienen que devolver un valor,
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
		respuesta = execute_describe(instr);
		break;

	case CODIGO_DROP:
		loggear_FS("Me llego una instruccion DROP.");
		respuesta = execute_drop(instr);
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
	char* tabla = obtener_parametro(instr, 0); //consigue nombre de la tabla
	if (!existe_tabla(tabla)) { //existe_tabla esta hardcodeado. TODO hacer funcion existe_tabla
		if (!crear_directorio(RUTA_TABLAS, tabla))
			return ERROR_CREATE; //log?
		if (!crear_metadata(instr))
			return ERROR_CREATE;
		int cantidad_part = atoi(obtener_parametro(instr, 2));
		//crear_particiones(tabla, cantidad_part);
		printf("Se creo el directorio, el metadata y las particiones de la tabla: %s", tabla);
		return CODIGO_EXITO;   //Hardcodeo exito.
	} else {
		printf("Error al crear la tabla, la tabla ya existe\n");
		return ERROR_CREATE;
	}
}

int execute_insert(instr_t* instr) { //no esta chequeado
	char* tabla = obtener_parametro(instr, 0); //nombre tabla
	if (!existe_tabla(tabla)) {
		return ERROR_INSERT; //log
	}
	metadata_t* metadata_tabla = malloc(sizeof(metadata_t));
	leer_metadata_tabla(tabla, metadata_tabla); //no se para que quieren la metadata.
	if (mem_existe_tabla(tabla)) {
		//Verificar si existe en memoria una lista de datos a dumpear. De no existir, alocar dicha memoria.
		mem_agregar_reg(instr);
	} else {
		mem_agregar_tabla(tabla);
		mem_agregar_reg(instr);
	}
	return CODIGO_EXITO;
}

int execute_select(instr_t* instr) {
	char* tabla = obtener_parametro(instr, 0); //consigue nombre de la tabla
	if (!existe_tabla(tabla)) {
			return ERROR_SELECT; //log
	}
	//algo = leer_metadata_tabla(tabla); //no se para que quieren la metadata.
	//int cant_particiones = algo->particiones;
	//part = key % cant_particiones;
	//leer archivos temporales (key)
	//leer binarios (part, key)
	//mem_buscar_key(key, tabla);
	//comparar
	return CODIGO_EXITO;
}

char* obtener_parametro(instr_t * instr, int index) {
	return (char*) list_get(instr->parametros, index);
}

//------------EJEMPLO INSTRUCCIONES----------------

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

	list_add(instr->parametros, "Una Tablita chica");
	list_add(instr->parametros, "SC");
	list_add(instr->parametros, "7");
	list_add(instr->parametros, "60000");

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

	printf("------\n");
	printf("Se libero la memoria\n");
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

void contestar(instr_t * instr) { //remitente_instr_t*
	//usa responder()  Lo hacen los chicos
	list_clean(instr->parametros);
	free(instr->parametros);
	free(instr);
}

