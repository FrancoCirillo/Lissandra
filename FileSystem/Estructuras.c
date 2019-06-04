/*
 * Estructuras.c
 *
 *  Created on: 16 may. 2019
 *      Author: utnso
 */

#include "Estructuras.h"

//Todo
/*
 * existe tabla
 * eliminar carpeta
 * eliminar directorio
 * copiar archivo (para los tmpc)
 * archivo_modificar_size();	//escribir nuevo numero en el archivo.
 * archivo_agregar_bloque();    //en el array de un archivo
 * archivo_liberar_bloque();	//en el array de un archivo
 * bloque disponible (bits arrays)
 * semaforos!
 *
 *NO FUNCIONA: string_to_upper(nomb);   //ver como hacer en crear_directorio().
 * ver actualizacion RETARDO del Archivo de config. Se actualiza en cada fin de instruccion?
 * \*/

//Pendientes Dai:
//1) Ver si el g_logger global no hace problemas..
//	 Si se crea una vez sola o ahi en el loggear_FS esta bien.
//2) Ver TS. Tipo de dato??
//3) chequear semaforizacion de todo esto..
//4) Ver validaciones de errores.. tests?
//5) validaciones de errores, aca o en las funciones en las que lo llaman?

int obtener_tiempo_dump_config() {
	return (int) config_FS.tiempo_dump * 1000;
}

int cant_bloques_disp(){
	return 100;   // TODO Ver con el bit array.
}

//---------------------------METADATA---------------------------
char* obtener_path_metadata(char* tabla) {
	return string_from_format("%s/%s/Metadata", RUTA_TABLAS, tabla);
}

t_config* obtener_metadata(char* tabla) {
	char* path = string_new();
	path = obtener_path_metadata(tabla);
	return config_create(path); //hay que hacer config_destroy
}

int obtener_part_metadata(char* tabla) {
	t_config* metadata = obtener_metadata(tabla);
	return config_get_int_value(metadata, "PARTITIONS");
}

char* obtener_consistencia_metadata(char* tabla) {
	t_config* metadata = obtener_metadata(tabla);
	return config_get_string_value(metadata, "CONSISTENCY");
}

int obtener_tiempo_compactacion_metadata(char* tabla) {
	t_config* metadata = obtener_metadata(tabla);
	return config_get_int_value(metadata, "COMPACTATION_TIME");
}


//---------------------------BLOQUES---------------------------
char* formatear_registro(registro_t* registro) {
	uint16_t key = registro->key;
	char* value = registro->value;
	char* ts = mseg_a_string(registro->timestamp);
	return string_from_format("%s;%d;%s", ts, key, value);
}

char* obtener_ruta_bloque(int nro_bloque) {
	return string_from_format("%s/%d.bin", RUTA_BLOQUES, nro_bloque);
}

void escribir_registro_bloque(registro_t* registro, char* ruta_bloque) {
	FILE* bloque = txt_open_for_append(ruta_bloque);
	char* string_registro = formatear_registro(registro);
	//if(hay espacio para el registro en el bloque):
		txt_write_in_file(bloque, string_registro);
	//else
	//escribir lo que se pueda en el bloque (txt_write_in_file)
	//int nro_bloque = siguiente_bloque_disponible();
	//char* nuevo_bloque = obtener_ruta_bloque(nro_bloque);
	//FILE* otro_bloque = txt_open_for_append(nuevo_bloque)
	//escribir lo que queda en el bloque (txt_write_in_file)
	//txt_close_file_(otro_bloque);
	txt_close_file(bloque);
}

//---------------------------BITARRAY---------------------------
int maximo_bloques(){
	return Metadata_FS.blocks;
}

void crear_bitarray() {
	int bloques_en_bytes = ceil(maximo_bloques()/8);
	char* miBitarray = string_repeat(0, bloques_en_bytes);
	bitarray = bitarray_create_with_mode(miBitarray, bloques_en_bytes, LSB_FIRST);
}

void eliminar_bitarray(){
	bitarray_destroy(bitarray);
}

int bloque_esta_ocupado(int nro_bloque) {
	return  bitarray_test_bit(bitarray, nro_bloque);
}

int siguiente_bloque_disponible() {
	int nro_bloque = 0;
	while(nro_bloque < maximo_bloques() && bloque_esta_ocupado(nro_bloque))
		nro_bloque++;
	return nro_bloque;
}

void ocupar_bloque(int nro_bloque) {
	bitarray_set_bit(bitarray, nro_bloque);
}

void liberar_bloque(int nro_bloque) {
	bitarray_clean_bit(bitarray, nro_bloque);
}



//*****************************************************************
//De aca para abajo estan corregidos y funcionan sin memory leaks.
//*****************************************************************

int crear_particiones(instr_t* i) {

	int cantidad = atoi(obtener_parametro(i, 2));

	if  (cant_bloques_disp()<cantidad){
		loggear_FS_error("Error al crear las particiones. No hay suficientes bloques disponibles",i);
		return 0;
	}

	FILE* f;
	char * tabla = obtener_parametro(i, 0);
	char * nomb_part;
	char* num;

	for (int i = 1; i <= cantidad; i++) {
		num = string_itoa(i);
		nomb_part = concat("Part_", num);
		f = crear_archivo(nomb_part, tabla, ".bin");
		archivo_inicializar(f);
		free(nomb_part);
		free(num);
		fclose(f);
	}

	loggear_FS("Se crearon las particiones de la tabla correctamente.");
	return 1;
}

void crear_metadata(instr_t* instr) {
	char* tabla = obtener_parametro(instr, 0);
	FILE* f = crear_archivo("Metadata", tabla, "");
	metadata_inicializar(f, instr);
	fclose(f);
	char* mensaje = malloc(sizeof(char) * (40 + strlen(tabla)));

	sprintf(mensaje, "Se creó el metadata en la tabla \"%s\".", tabla);
	loggear_FS(mensaje);
	free(mensaje);
}

void metadata_inicializar(FILE* f, instr_t* instr) {
	char* consist = obtener_parametro(instr, 1);
	char* part = obtener_parametro(instr, 2);
	char* time = obtener_parametro(instr, 3);
	fprintf(f, "%s%s%s%s%s%s%s", "CONSISTENCY=", consist, "\nPARTITIONS=", part, "\nCOMPACTATION_TIME=", time, "\n");
}

//Inicializa con size = 0, y el array de Blocks con un bloque asignado.
void archivo_inicializar(FILE* f) {
	int bloque_num = 1; //bloque_disponible();
	fprintf(f, "%s%d%s%d%s", "SIZE = ", 0, "\nBlocks = [", bloque_num, "]\n");

	//Si hay un bloque disp se valida cuando se llama a esta funcion. Solo pasa en CREATE.
	// No hace el fclose(f);
}

FILE* crear_archivo(char* nombre, char* tabla, char* ext) {
	char* ruta = malloc(sizeof(char) * (strlen(RUTA_TABLAS) + strlen(nombre) + strlen(tabla)) + 1 + 1 + strlen(ext));
	sprintf(ruta, "%s%s%s%s%s", RUTA_TABLAS, tabla, "/", nombre, ext);
	FILE* f = fopen(ruta, "w+"); //Modo: lo crea vacio para lectura y escritura. Si existe borra lo anterior.

	char* mje = malloc(sizeof(char) * (strlen(nombre) + strlen(tabla) + 50));
	sprintf(mje, "Se creó el archivo %s%s en la tabla \"%s\".", nombre, ext, tabla);
	loggear_FS(mje);
	free(mje);
	free(ruta);
	return f;
}

void crear_directorio(char* ruta, char * nomb) {

	//string_to_upper(nomb);   //No me funciona (??) TODO ver
	char* ruta_dir = concat(ruta, nomb);
	char* mje = malloc(sizeof(char) * (60 + strlen(ruta_dir)));
	if (!mkdir(ruta_dir, S_IRWXU)) {
		sprintf(mje, "Se creó correctamente la carpeta \"%s\" en el directorio %s", nomb, ruta);
		loggear_FS(mje);
		free(ruta_dir);
		free(mje);
	} else {
		sprintf(mje, "No se creó la carpeta \"%s\". Ya existe.", nomb);
		loggear_FS(mje);
		free(ruta_dir);
		free(mje);
	}
	//Concat hace un malloc. Aca tiene que haber un free
}

void crear_bloques() {  //Los bloques van a partir del numero 0.

	int cantidad = Metadata_FS.blocks;
	char* num;
	for (int i = 0; i <= cantidad; i++) {
		num = string_itoa(i);
		crear_bloque(num);
		free(num);
	}
	loggear_FS("Se crearon los bloques del File System.");
}

void crear_bloque(char * nombre) {
	char* ruta = malloc(sizeof(char) * (strlen(RUTA_BLOQUES) + strlen(nombre) + strlen(".bin")) + 1);
	sprintf(ruta, "%s%s%s", RUTA_BLOQUES, nombre, ".bin");
	FILE* f = fopen(ruta, "w+");
	free(ruta);
	fclose(f);
}

void loggear_FS(char *valor) {
	g_logger = log_create("Lissandra.log", "File System", 1, LOG_LEVEL_INFO);

	sem_wait(&mutex_log);
	log_info(g_logger, valor);
	sem_post(&mutex_log);

	printf("--------------\n");

	log_destroy(g_logger);
}

void loggear_FS_error(char *valor, instr_t* i) {
	g_logger = log_create("Lissandra.log", "File System", 1, LOG_LEVEL_INFO);

	sem_wait(&mutex_log);
	log_error(g_logger, valor);
	sem_post(&mutex_log);

	list_replace(i->parametros, 0, valor);

	printf("--------------\n");

	log_destroy(g_logger);
	//Esto no genera memory leak, pisa el parametro bien. testeado!
}


char* obtener_por_clave(char* ruta, char* clave) {
	t_config* c = config_create(ruta);
	char* valor;
	valor = config_get_string_value(c, clave);
	config_destroy(c);
	return valor;
}


void inicializar_configuracion(void) {
	g_config = config_create("Lissandra.config");
	config_FS.punto_montaje = config_get_string_value(g_config, "PUNTO_MONTAJE");
	config_FS.puerto_escucha = config_get_string_value(g_config, "PUERTO_ESCUCHA");
	config_FS.tamanio_value = config_get_int_value(g_config, "TAMANIO_VALUE");
	config_FS.retardo = (mseg_t)config_get_int_value(g_config, "RETARDO");
	config_FS.tiempo_dump = (mseg_t)config_get_int_value(g_config, "TIEMPO_DUMP");

	loggear_FS("Se leyó el archivo de configuración");
}

void actualizar_tiempo_dump_config(mseg_t value) {

	char* val = string_itoa((int) value);

	sem_wait(&mutex_tiempo_dump_config);
	config_set_value(g_config, "TIEMPO_DUMP", val);
	sem_post(&mutex_tiempo_dump_config);

	config_save(g_config);

	char* mje = concat("Se actualizo el tiempo de dumpeo en el archivo de configuracion. Nuevo valor: ", val);
	loggear_FS(mje);
	free(mje);
	free(val);
}

void actualizar_tiempo_retardo_config(mseg_t value) {

	char* val = string_itoa( value);

	sem_wait(&mutex_tiempo_retardo_config);
	config_set_value(g_config, "TIEMPO_RETARDO", val);
	sem_post(&mutex_tiempo_retardo_config);

	config_save(g_config);

	char* mje = concat("Se actualizo el tiempo de retardo en el archivo de configuracion. Nuevo valor: ", val);
	loggear_FS(mje);
	free(mje);
	free(val);

}

void inicializar_directorios() {

	crear_directorio("mnj/Lissandra_FS/", "Tablas");
	crear_directorio("mnj/Lissandra_FS/", "Bloques"); //vacias

	loggear_FS("Directorios listos.");

	leer_metadata_FS();

}

void leer_metadata_FS() {  //esto se lee una vez.

	char* ruta = "mnj/Lissandra_FS/Metadata/Metadata.bin";
	meta_config = config_create(ruta);
	Metadata_FS.block_size = config_get_int_value(meta_config, "BLOCK_SIZE");
	Metadata_FS.blocks = config_get_int_value(meta_config, "BLOCKS");
	Metadata_FS.magic_number = config_get_string_value(meta_config, "MAGIC_NUMBER");

	char* mensaje = malloc(sizeof(char) * 200);
	sprintf(mensaje, "%s%s%d%s%d%s%s",
			"Metadata leído. Los datos son:\n",
			"BLOCK_SIZE = ",
			Metadata_FS.block_size,
			"\nBLOCKS = ",
			Metadata_FS.blocks,
			"\nMAGIC_NUMBER = ",
			Metadata_FS.magic_number);

	loggear_FS(mensaje);
	free(mensaje);
}

