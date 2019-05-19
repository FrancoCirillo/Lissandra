/*
 * Estructuras.c
 *
 *  Created on: 16 may. 2019
 *      Author: utnso
 */

#include "Estructuras.h"

int existe_Tabla(char * nombre_tabla){
	return 0;    //hardcodeado. ver como implementar.
}

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
 * */


int crear_directorio(char* ruta,char * nomb){

	//string_to_upper(nomb);   No me funciona (??)
	char*ruta_dir = concat(ruta, nomb);
	if(!mkdir(ruta_dir, S_IRWXU)){
		printf("Se creó correctamente la carpeta %s en el directorio %s\n", nomb, ruta);
		free(ruta_dir);
		return 1;
	}
	else{
		printf("No se pudo crear la carpeta: %s \n", nomb);
		free(ruta_dir);
		return 0;
	}
	 //Concat hace un malloc. Aca tiene que haber un free
}



FILE* crear_archivo(char * nombre, char* tabla, char * ext){
	char* ruta= malloc(sizeof(char)*(strlen(RUTA_TABLAS) + strlen(nombre) + strlen(tabla)) +1+1+strlen(ext));
	sprintf(ruta, "%s%s%s%s%s", RUTA_TABLAS, tabla, "/", nombre, ext);
	FILE* f = fopen(ruta, "w");
	printf("Se creó el archivo %s%s en la tabla %s.\n", nombre, ext, tabla);
	free(ruta);
	return f;
}


void crear_particiones(char * tabla){
	FILE* f;
	int cantidad = 10;   //Esto lo debe leer del metadata de ESA TABLA.
	for(int i = 1; i <= cantidad; i++){
		//creo que este char de abajo esta mal, hace memory leak en cada iteracion.. arreglar.
		char * nomb_part = concat("Part_", string_itoa(i));
		f = crear_archivo(nomb_part, tabla, ".bin");
		archivo_inicializar(f);
	}
	fclose(f);
}

void crear_bloque(char * nombre){
	char* ruta= malloc(sizeof(char)*(strlen(RUTA_BLOQUES) + strlen(nombre) + strlen(".bin"))+1);
	sprintf(ruta, "%s%s%s", RUTA_BLOQUES, nombre, ".bin");
	FILE* f = fopen(ruta, "w");
	free(ruta);
	fclose(f);
}

void crear_bloques(){    //Los bloques van a partir del numero 1. Tener presente.
	int cantidad = 10; //leer del metadata del FS, la cantidad que sean.
	char* num;
	for(int i = 1; i<= cantidad; i++){
		num = string_itoa(i);
		crear_bloque(num);
	}
	printf("Se crearon los bloques del 1 al %d\n", cantidad);
}


//TODO: asignar un bloque disponible!! Hardcodeado. Crear luego de ver los bitarrays.

//Inicializa con size = 0, y el array de Blocks con un bloque asignado.
void archivo_inicializar(FILE* f){
	int bloque_num = 1;     //  int bloque_num = bloque_disponible(); Devuelve un bloque libre.
	//TODO VALIDAR esto si no llega a haber un bloque disponible.

	char* bloque_num_s = string_itoa(bloque_num);
	char* cadena = malloc(sizeof(char)*(20+strlen(bloque_num_s))+1);
	sprintf(cadena, "%s%s%s%s%s","SIZE = ",string_itoa(0), "\nBlocks = [", bloque_num_s, "]\n");
	fwrite(cadena, sizeof(char), strlen(cadena)+1,f);
	free(cadena);
	free(bloque_num_s);
	printf("Se inicializó el archivo.\n");
}

int crear_metadata(instr_t* i){
	char* tabla=obtener_parametro(i,0);
	FILE* f = crear_archivo("Metadata", tabla, "");
	metadata_inicializar(f, i);
	fclose(f);
	printf("Se creó el metadata en la tabla %s. \n", tabla);

	return 1;  //Ver si hay que validar algo mas aca.. donde puede fallar?
}


void metadata_inicializar(FILE* f, instr_t* i){
	char* consist= obtener_parametro(i,1);
	char* part= obtener_parametro(i,2);
	char* time= obtener_parametro(i,3);
	char* cadena=malloc(sizeof(char)*(50+strlen(consist) + strlen(part) + strlen(time))+1);
	sprintf(cadena, "%s%s%s%s%s%s%s","CONSISTENCY = ",consist, "\nPARTITIONS = ", part, "\nCOMPACTATION_TIME = ", time, "\n");
	fwrite(cadena, sizeof(char), strlen(cadena)+1,f);
	free(cadena);
}


// poner semaforos
void loggear_FS(char *valor) {
	g_logger = log_create("Lissandra.log","File System", 1, LOG_LEVEL_INFO);

	log_info(g_logger, valor);

	log_destroy(g_logger);
}


char* obtener_por_clave(char* ruta, char* clave) {
	char* valor;
	//g_config = config_create(ruta); // ya esta creado el archivo de config.
	valor = config_get_string_value(g_config, clave);

	return valor;
}

//ver tipo de dato TS
void inicializar_configuracion(void){
	g_config = config_create("Lissandra.config");
	config_FS.punto_montaje = config_get_string_value(g_config, "PUNTO_MONTAJE");
	config_FS.puerto_escucha = config_get_string_value(g_config, "PUERTO_ESCUCHA");
	config_FS.tamanio_value = config_get_int_value(g_config, "TAMANIO_VALUE");
	config_FS.retardo = config_get_int_value(g_config, "RETARDO");
	config_FS.tiempo_dump = config_get_int_value(g_config, "TIEMPO_DUMP");
}


void actualizar_tiempo_dump_config(char* value){

	config_set_value(g_config, "TIEMPO_DUMP", value);
	sem_wait(&mutex_tiempo_dump_config);
	config_FS.tiempo_dump = config_get_int_value(g_config, "TIEMPO_DUMP");
	sem_post(&mutex_tiempo_dump_config);

}

void actualizar_tiempo_retardo_config(char* value){

	config_set_value(g_config, "TIEMPO_RETARDO", value);
	sem_wait(&mutex_tiempo_retardo_config);
	config_FS.retardo = config_get_int_value(g_config, "RETARDO");
	sem_post(&mutex_tiempo_retardo_config);

}


void inicializar_directorios(){

	crear_directorio("mnj/Lissandra_FS/","Tablas");
	crear_directorio("mnj/Lissandra_FS/","Bloques"); //vacias
	leer_metadata_FS();

}

void leer_metadata_FS(){

	char*ruta= "mnj/Lissandra_FS/Metadata/Metadata.bin";
	meta_config = config_create(ruta);
	Metadata_FS.block_size= config_get_int_value(meta_config, "BLOCK_SIZE");
	Metadata_FS.blocks= config_get_int_value(meta_config, "BLOCKS");
	Metadata_FS.magic_number= config_get_string_value(meta_config, "MAGIC_NUMBER");

}


int obtener_tiempo_dump_config(){
	return config_FS.tiempo_dump *1000;
}

