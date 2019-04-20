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

//Esto va dentro del create, pero estoy probando.

	crear_directorio("Holanda");
	FILE* f = crear_archivo("Particion4", "Holanda", ".tmp");
	archivo_inicializar(f);
	fclose(f);
	//crear_metadata("OtraTabla", "SC", 5, obtener_tiempo());      //No me funciona el obtener_tiempo, debo tener algo mal.
																	//Todo lo paso a int para probar que funcione el crear_Metadata.
	crear_metadata("Holanda", "HC", 9, 333666999);

	//HARDCODEO
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
//Crea un directorio unicamente en TABLAS.
void crear_directorio(char * nomb){

	char* ruta= malloc(sizeof(char)*(strlen(nomb) + strlen(RUTA_TABLAS)) +1);
	ruta = concat(RUTA_TABLAS, nomb);
	mkdir(ruta, S_IRWXU);
	printf("Se creó la carpeta: %s \n", nomb);
	free(ruta);

}


FILE* crear_archivo(char * nombre, char* tabla, char * ext){
	char* ruta= malloc(sizeof(char)*(strlen(RUTA_TABLAS) + strlen(nombre) + strlen(tabla)) +1+1+strlen(ext));
	sprintf(ruta, "%s%s%s%s%s", RUTA_TABLAS, tabla, "/", nombre, ext);
	FILE* f = fopen(ruta, "w");
	printf("Se creó el archivo %s%s en la tabla %s.\n", nombre, ext, tabla);
	free(ruta);
	return f;
}

//void crear_metadata(char * tabla,char * consistency, int particiones,time_t timestamp){     <-- int x time_t para probar
void crear_metadata(char * tabla,char * consistency, int particiones,int timestamp){
	FILE* f = crear_archivo("Metadata", tabla, "");
	metadata_inicializar(f, consistency, particiones, timestamp);
	fclose(f);
	printf("Se creó el metadata en la tabla %s. \n", tabla);
}


//TODO: asignar un bloque disponible!! Hardcodeado. Crear luego de ver los bitarrays.

//Inicializa con size = 0, y el array de Blocks con un bloque asignado.
void archivo_inicializar(FILE* f){
	int bloque_num = 1;     //  int bloque_num = bloque_disponible();
	//TODO ¿Cómo valido esto si no llega a haber un bloque disponible??
	char* bloque_num_s = string_itoa(bloque_num);
	char* cadena = malloc(sizeof(char)*(20+strlen(bloque_num_s))+1);
	sprintf(cadena, "%s%s%s%s%s","SIZE = ",string_itoa(5), "\nBlocks = [", bloque_num_s, "]\n");  //Delegar..
	fwrite(cadena, sizeof(char), strlen(cadena)+1,f);
	free(cadena);
	printf("Se inicializó el archivo.\n");
}

//void metadata_inicializar(FILE* f, char * consistency, int particiones,time_t tiempo){    //FUNCIONA.     //     ---> acá va uno. Pero desp devuelve un numero de bloque..int bloque_disponible(); TODO HACER FUNCION.
void metadata_inicializar(FILE* f, char * consistency, int particiones,int tiempo_comp){    //FUNCIONA.     //     ---> acá va uno. Pero desp devuelve un numero de bloque..int bloque_disponible(); TODO HACER FUNCION.
	char* part = string_itoa(particiones);
	char* tiempo_c = string_itoa(tiempo_comp);
	char* cadena = malloc(sizeof(char)*(50+strlen(consistency) + strlen(part) + strlen(tiempo_c))+1);
	sprintf(cadena, "%s%s%s%s%s%s%s","CONSISTENCY = ",consistency, "\nPARTITIONS = ", part, "\nCOMPACTATION_TIME = ", tiempo_c, "\n");  //Delegar..
	fwrite(cadena, sizeof(char), strlen(cadena)+1,f);
	free(cadena);
	free(part);
	free(tiempo_c);
}		//Esto esta horrible.. ver de delegar un poco..si es necesario.

//TODO:
	/*modificar_Size();
	 * agregar_bloque();
	 * liberar_bloque();
	 * */


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
