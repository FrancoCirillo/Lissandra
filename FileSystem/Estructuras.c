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
 * eliminar carpeta
 * eliminar directorio
 * metadata_modificar_size();	//escribir nuevo numero en el archivo.
 * metadata_agregar_bloque();    //en el array
 * metadata_liberar_bloque();	//en el array
 * bloque disponible (bits arrays)
 * archivo de config, actualizar.
 * semaforos!
 *
 * */


int crear_directorio(char * nomb){

	//string_to_upper(nomb);   No me funciona (??)
	char*ruta = concat(RUTA_TABLAS, nomb);
	if(!mkdir(ruta, S_IRWXU)){
		printf("Se creó correctamente la carpeta: %s \n", nomb);
		free(ruta);
		return 1;
	}
	else{
		printf("No se pudo crear la carpeta: %s \n", nomb);
		free(ruta);
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
	sprintf(cadena, "%s%s%s%s%s","SIZE = ",string_itoa(5), "\nBlocks = [", bloque_num_s, "]\n");
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

//Lo viejo
//char* cadena = malloc(sizeof(char)*(50+strlen(consist) + strlen(part) + strlen(time))+1);
//sprintf(cadena, "%s%s%s%s%s%s%s","CONSISTENCY = ",consist, "\nPARTITIONS = ", part, "\nCOMPACTATION_TIME = ", time, "\n");
//fwrite(cadena, sizeof(char), strlen(cadena)+1,f);
//free(cadena);



// poner semaforos
void loggear_FS(char *valor) {
	g_logger = log_create("Lissandra.log","File System", 1, LOG_LEVEL_INFO);

	log_info(g_logger, valor);

	log_destroy(g_logger);
}

char* leer_config(char* clave) {
	char* valor;
	g_config = config_create("Lissandra.config");
	valor = config_get_string_value(g_config, clave);
	config_destroy(g_config);
	return valor;
}


//TODO FALTA Terminarlo bien. Dejarlo listo.
void inicializarConfig(void){
	g_config = config_create("Lissandra.config");
	config_FS.punto_montaje = config_get_string_value(g_config, "PUNTO_MONTAJE");
	config_FS.puerto_escucha = config_get_string_value(g_config, "PUERTO_ESCUCHA");
	config_FS.tamanio_value = config_get_int_value(g_config, "TAMANIO_VALUE");
	config_FS.retardo = config_get_int_value(g_config, "RETARDO");
	config_FS.tiempo_dump = config_get_int_value(g_config, "TIEMPO_DUMP");
}

void actualizar_configuracion(){

	g_config = config_create("Lissandra.config");

	//sem_wait(&mutex_tiempo_retardo);
	config_FS.retardo = config_get_int_value(g_config, "RETARDO");
	//sem_post(&mutex_tiempo_retardo);

	//sem_wait(&mutex_tiempo_dump);
	config_FS.tiempo_dump = config_get_int_value(g_config, "TIEMPO_DUMP");
	//sem_post(&mutex_tiempo_dump);
	//Hacer
	//tener cuidado con los memory leaks, cuando reemplazo strings.
}
//void iniciar_semaforos(){
//	sem_init(&mutex_tiempo_dump,0,1);
//	sem_init(&mutex_tiempo_retardo,0,1);
//}

