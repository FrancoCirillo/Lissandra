#include "fileSystem.h"

int main() {
	printf("PROCESO FILESYSTEM \n");

	//DIVIDIR ESTOS PROCESOS EN HILOS.

//	inicializarConfig();    //Tenerlo listo y borrar esto.
//	inicializar_Metadata_FS();		//Tenerlo listo y borrar.


//	inicializar_directorios();
	/* Esto lee el arch de config,
	 * el metadata del FS,
	 * inicializa los bloques con sus respectivos tamaños y la cantidad que sean.
	 * bitarrays en cero.
	 */

//	inicializar_memtable();
				/* Lista de colas con datos a dumpear, vacia.
	 	 	 	   iniciar cronometro del DUMP (iterativamente, siempre que tenga al menos un dato.)
	 	 	 	   es un nodo por tabla.
	 	 	  	   al hacer dumpeo, se toma una tabla y se guarda los datos en el .tmp de la tabla.
	 	 	 	 	 	 	 */

	crear_directorio("OtraTabla");
	crear_archivo_bin("Particion2", "OtraTabla");

	//HARDCODEO - NO MIRAR :P
	//loggear_FS("Arrancamos\n");
	//EJEMPLO CREATE;
	/*
	remitente_instr_t* mensaje;
	instr_t* instruccion_m;
	remitente_t* remitente_m;

	instruccion_m->timestamp= obtenertiempo();
	instruccion_m->codigoInstruccion = 5;
	instruccion_m->param1 = "Tabla1";
	instruccion_m->param2 = "SC" ;
	instruccion_m->param3 = "5";
	instruccion_m->param4 = "60000";

	mensaje->instruccion = instruccion_m;

	remitente_m->ip = IP_MEMORIA;
	remitente_m->puerto = PORT;

	mensaje->remitente = remitente_m;

	//	CREATE [TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]

			//Ver bien qué me llega, qué devuelvo.

	//Acá iria un hilo para cada instruccion que va llegando

	//evaluarInstruccion(mensaje);
*/

	return 0;

}
/*

void evaluar_instruccion(remitente_instr_t* mensaje){

	cod_instr resultado;

	switch(mensaje->instruccion->codigoInstruccion){

	case CODIGO_SELECT:

		break;

	case CODIGO_INSERT:

		break;

	case CODIGO_CREATE:

		char* nombre_tabla = mensaje->instruccion->param1;
		string_to_upper(nombre_tabla);

		if(!existe_Tabla(nombre_tabla)){
			execute_create(mensaje->instruccion);
			resultado = EXITO;
			char * nota = "La tabla %s se creó correctamente.\n", nombre_tabla
			loggear_FS("La tabla %s se creó correctamente.\n", nombre_tabla);
		}
		else{
			loggear_FS("La tabla %s se ya existe en el File System.\n", nombre_tabla);
			resultado = ERROR_CREATE;
		}

		break;

	case CODIGO_DESCRIBE:

		break;

	case CODIGO_DROP:

		break;

	default:
		//verrrr
	}
	//response(mensaje->remitente, resultado);

};



bool existe_Tabla(char * nombre_tabla){
	return false;
}

void execute_create(instr_t* inst){

	crear_directorio(inst->param1);
	crear_metadata();
	crear_particiones();
	asignar_bloques();
	response();
}
*/

void crear_directorio(char * nomb){	//FUNCIONA!

	char* ruta= malloc(sizeof(char)*(strlen(nomb) + strlen(RUTA_TABLAS)) +1);
	ruta = concat(RUTA_TABLAS, nomb);
	mkdir(ruta, S_IRWXU);
	printf("Se creó la carpeta: %s \n", nomb);
	free(ruta);
}


void crear_archivo_bin(char * nombre, char* tabla){		//Listo.
	char* ruta= malloc(sizeof(char)*(strlen(RUTA_TABLAS) + strlen(nombre) + strlen(tabla)) +1+1+4); //esto ultimo es por: \0, /, .bin
	ruta= concat(ruta, RUTA_TABLAS);
	ruta= concat(ruta, tabla);
	ruta= concat(ruta, "/");
	ruta= concat(ruta, nombre);
	ruta= concat(ruta, ".bin");
	FILE* f = fopen(ruta, "a");
	printf("Se creó la carpeta %s en la tabla %s", nombre, tabla); //Loggear..
	archivo_inicializar(ruta);
	free(ruta);
//	archivo_t* archivo = archivo_nuevo();
	free(f);
//	return archivo;
}

void archivo_inicializar(char * ruta){    //No funciona..
	FILE *f = fopen(ruta,"a+");
	char* bloque_num = "0";     //     ---> acá va cero. Pero desp devuelve un numero..int bloque_disponible(); TODO HACER FUNCION.
	char* cadena = concat("SIZE = \nBlocks = []\n");
	char* cadena = "SIZE= \nBlocks \= []\n";
	fwrite(cadena, 1, sizeof(cadena),f);
//	fflush(f);
	fclose(f);
	free(cadena);
	free(bloque_num);
}

void crear_archivo_tmp(char * nombre, char* tabla){		//Listo.
	char* ruta= malloc(sizeof(char)*(strlen(RUTA_TABLAS) + strlen(nombre) + strlen(tabla)) +1+1+4); //esto ultimo es por: \0, /, .bin
	ruta= concat(ruta, RUTA_TABLAS);
	ruta= concat(ruta, tabla);
	ruta= concat(ruta, "/");
	ruta= concat(ruta, nombre);
	ruta= concat(ruta, ".tmp");
	FILE* f = fopen(ruta, "a");
	printf("Se creó la carpeta %s en la tabla %s", nombre, tabla);
	free(ruta);
	free(f);
	//archivo_inicializar_tmp();    -->Esto le asigna el nombre correlativo al ultimo
	//tmp existente. Depende la cantidad de dumpeos del momento.

}


//TODO Terminarlo bien. Dejarlo listo.
void inicializarConfig(void){		//Valores random
	configuracion.PUNTO_MONTAJE = RUTA_PUNTO_MONTAJE;
	configuracion.PUERTO_ESCUCHA = 5555;
	configuracion.TAMANIO_VALUE = 5;
	configuracion.RETARDO = 20000;
	configuracion.TIEMPO_DUMP = 40000;
}

char* concat( char *s1, char *s2)		//Listo.
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
char* concat3( char *s1, char *s2, char * s3)		//Listo.
{
    char *result = malloc(strlen(s1) + strlen(s2) + strlen(s3) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    strcat(result, s3);
    return result;
}


void loggear_FS(char *valor) {	//Listo
		g_logger = log_create("Lissandra.log","File System", 1, LOG_LEVEL_INFO);
		log_info(g_logger, valor);
		log_destroy(g_logger);
	}

char* leer_config(char* clave) {	//Listo
	char* valor;
	g_config = config_create("Lissandra.config");
	valor = config_get_string_value(g_config, clave);
	config_destroy(g_config);
	return valor;
}
