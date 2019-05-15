#include "fileSystem.h"

int main() {

	printf("PROCESO FILESYSTEM\n");

//	instr_t instruccion;
//	instruccion.timestamp = get_timestamp();
//	instruccion.codigo_operacion = CODIGO_CREATE;
//	instruccion.parametros = list_create();
//	instruccion.parametros


	//DIVIDIR ESTOS PROCESOS EN HILOS.

		//inicializarConfig();    //Tenerlo listo y borrar esto.
		//printf("%s\n\n" ,config_FS.puerto_escucha);
		//printf("%d\n\n" ,config_FS.tamanio_value);


		//config_destroy(g_config);

/////////////////////////////////////////////////

	//	int listenner = iniciar_servidor(IP_FILESYSTEM, PORT);

	//	vigilar_conexiones_entrantes(listenner);






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

//	crear_directorio("Tabla A");
//	FILE* f = crear_archivo("Part_1", "Tabla A", ".tmp");
//	archivo_inicializar(f);
//	fclose(f);
//	crear_metadata("OtraTabla", "SC", "5", string_itoa(obtener_tiempo()));      //No me funciona el obtener_tiempo, debo tener algo mal.
//	//Todo lo paso a int para probar que funcione el crear_Metadata.
//	//crear_metadata("Tabla A", "SC", 10, 333666999);
//	crear_bloques();
//	crear_particiones("Tabla A");

	/*
	//HARDCODEO
	loggear_FS("Arrancamos\n");
	//EJEMPLO CREATE;

	remitente_instr_t* mensaje;
	instr_t* instruccion_m;
	remitente_t* remitente_m;

	instruccion_m->timestamp= obtenertiempo();
	instruccion_m->codigoInstruccion = 5;
	instruccion_m->param1 = "TablaABC";
	instruccion_m->param2 = "SC" ;
	instruccion_m->param3 = "5";
	instruccion_m->param4 = "60000";

	mensaje->instruccion = instruccion_m;

	remitente_m->ip = IP_MEMORIA;
	remitente_m->puerto = PORT;

	mensaje->remitente = remitente_m;
	printf("Se creo la estructura correctamente.\n");

	evaluar_instruccion(mensaje);

	//	CREATE [TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]

			//Ver bien qué me llega, qué devuelvo.

	//Acá iria un hilo para cada instruccion que va llegando

	//evaluarInstruccion(mensaje);

*/
	return 0;

}

/*
instr_t* create(instr_t * instruccion){

	char* nombre_tabla = instruccion->param1;
	instr_t* respuesta;
	string_to_upper(nombre_tabla);
	char *nota;

	if(!existe_Tabla(nombre_tabla)){
		execute_create(instruccion);
		respuesta->codigoInstruccion = 0;	//EXITO
		nota = concat3("La tabla ",instruccion->param1," se creó correctamente.\n");
		loggear_FS(nota);
	}

	else{
		respuesta->codigoInstruccion = ERROR_CREATE;
		nota = concat3("La tabla ",instruccion->param1," ya existe en el File System.\n");
		loggear_FS(nota);
	}

	respuesta->param1=nota; //El mje para enviar se guarda en el param 1.
	respuesta->timestamp=instruccion->timestamp;  //convencion: se responde con una instruccion con el timestamp propio, asi sabe el que lo recibe de instruccion se trata.
	return respuesta;
}

*/

//void evaluar_instruccion(remitente_instr_t* mensaje){
//
//	instr_t* respuesta = 0;
//	printf("Me llego la instruccion: ");
//
//	switch(mensaje->instruccion->codigoInstruccion){
//
//	case CODIGO_SELECT:
//		printf("SELECT\n\n");
//		break;
//
//	case CODIGO_INSERT:
//		printf("INSERT\n\n");
//		break;
//
//	case CODIGO_CREATE:
//		printf("CREATE\n\n");
//		//respuesta = i_create(mensaje->instruccion);					//todos tienen que devolver un valor,
//
//		break;
//
//	case CODIGO_DESCRIBE:
//		printf("DESCRIBE\n\n");
//		break;
//
//	case CODIGO_DROP:
//		printf("DROP\n\n");
//		break;
//
//	default:
//	//verrrr
//		printf("No es una instruccion valida dentro del File System.\n\n");
//	}
//
//	mensaje->instruccion=respuesta;  //Esto pisa el parametro 1 con el mensaje a enviarle a memoria.
//	//y el codigo de instr es el codigo de la respuesta. (0 es exito, o el num neg es el error que corresponde)
//
//	/*TODO
//	responder(mensaje);
//	*/
//
//	printf("Finalizo la instruccion.\n");
//	//ver de hacer un free al mensaje si es necesario, o ver donde.
//
//	free(mensaje->instruccion);
//	free(mensaje->remitente);
//	free(mensaje); //terminar de entender como liberar esto o cuando..
//};
//

int existe_Tabla(char * nombre_tabla){
	return false;
}

//void execute_create(instr_t* inst){
//
//	crear_directorio(inst->param1);
//	crear_metadata(inst->param1,inst->param2,inst->param3,inst->param4);
//	crear_particiones(inst->param1);
//	printf("Se creo el directorio, el metadata y las particiones de la tabla: %s", inst->param1);
//}

//Crea un directorio unicamente en TABLAS.
void crear_directorio(char * nomb){

	char* ruta= malloc(sizeof(char)*(strlen(nomb) + strlen(RUTA_TABLAS)) +1);
	ruta = concat(RUTA_TABLAS, nomb);
	mkdir(ruta, S_IRWXU);
	printf("Se creó la carpeta: %s \n", nomb);
	free(ruta);

}
/*
void* compactar(instr_t* i){
	while(existe_tabla(i->param1)){
		sleep(atoi(i->param2));
		compactation(i->param1);
	}
	return NULL;
}
*/

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

void crear_bloques(){
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

void crear_metadata(char * tabla,char * consistency, char* particiones,char* timestamp){
	FILE* f = crear_archivo("Metadata", tabla, "");
	metadata_inicializar(f, consistency, particiones, timestamp);
	fclose(f);
	printf("Se creó el metadata en la tabla %s. \n", tabla);
}

void metadata_inicializar(FILE* f, char * consist, char* part,char* time){
	char* cadena = malloc(sizeof(char)*(50+strlen(consist) + strlen(part) + strlen(time))+1);
	sprintf(cadena, "%s%s%s%s%s%s%s","CONSISTENCY = ",consist, "\nPARTITIONS = ", part, "\nCOMPACTATION_TIME = ", time, "\n");
	fwrite(cadena, sizeof(char), strlen(cadena)+1,f);
	free(cadena);
}

//TODO:
/*metadata_modificar_size();
 * metadata_agregar_bloque();
 * metadata_liberar_bloque();
 * */

char* concat( char *s1, char *s2){
	char *result = malloc(strlen(s1) + strlen(s2) + 1);
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}

char* concat3( char *s1, char *s2, char * s3){
	char *result = malloc(strlen(s1) + strlen(s2) + strlen(s3) + 1);
	strcpy(result, s1);
	strcat(result, s2);
	strcat(result, s3);
	return result;
}

// poner semaforo
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
	char* ruta_config="FileSystem.config";
	config_FS.punto_montaje = config_get_string_value(g_config, "PUNTO_MONTAJE");
	config_FS.puerto_escucha = config_get_string_value(g_config, "PUERTO_ESCUCHA");
	config_FS.tamanio_value = config_get_int_value(g_config, "TAMANIO_VALUE");
	config_FS.retardo = config_get_int_value(g_config, "RETARDO");
	config_FS.tiempo_dump = config_get_int_value(g_config, "TIEMPO_DUMP");
}

char* obtener_clave(char* ruta, char* key) {
	char* valor;
	g_config = config_create(ruta);
	valor = config_get_string_value(g_config, key);
	printf("-----------\nGenerando config, valor obtenido para %s, es:   %s \n ---------",key,valor);
	//config_destroy(g_config);
	return valor;
}


