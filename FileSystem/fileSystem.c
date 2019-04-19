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


	//HARDCODEO - NO MIRAR :P

	//EJEMPLO CREATE;
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

	while(true){

	evaluarInstruccion(mensaje);

	//Ver como llevar esto..
	}

/*    VISTO CON RODRI: pseudocódigo
      * intruccion
      * la memoria
      * -Servidor debe recibir mensaje ya como instruccion-memoria; crea hilo con funcion procesar
      * -Hacer struct que contenga instruccion y memoria
      * procesar(struct instruccion-memoria){
      *     Interpreto intruccion y utilizo response
      *             if(codigo==CODIGO_CREATE)
      *                     hacer_create(instruccion)// piso codigo de instruccion por codigo de error/Respuesta y parametro 1 por respuesta
      *             responder(instruccion-memoria);
      *
      *             RECORDAR EL JOIN;
      *             Y MALLOC.
      *
      *
      *             Cola de colas/ Castear.
	*/
	return 0;
}


void evaluar_instruccion(remitente_instr_t* mensaje){

	switch(mensaje->instruccion->codigoInstruccion){

	case CODIGO_SELECT:

		break;

	case CODIGO_INSERT:

		break;

	case CODIGO_CREATE:
		char* nombre_tabla = mensaje->instruccion->param1;
		string_to_upper(nombre_tabla);

		cod_instr resultado;

		if(!existe_Tabla(nombre_tabla)){
			execute_create(mensaje->instruccion);
			response(mensaje->remitente, 0);
			loggear("La tabla %s se creó correctamente.\n", nombre_tabla);
		}
		else{
			loggear("La tabla %s se ya existe en el File System.\n", nombre_tabla);
			response(mensaje->remitente, ERROR_CREATE);
		}

		break;

	case CODIGO_DESCRIBE:

		break;

	case CODIGO_DROP:

		break;

	default:
		//verrrr
	}

};



bool existe_Tabla(nombre_tabla){
	return true;
}

void execute_create(instr_t* inst){



}



//TODO Terminarlo bien. Dejarlo listo.
void inicializarConfig(void){
	configuracion.PUNTO_MONTAJE = strcat(RUTA_PUNTO_MONTAJE,"Configuracion_LFS.config");
	configuracion.PUERTO_ESCUCHA = 5555;
	configuracion.TAMANIO_VALUE = 5;
	configuracion.RETARDO = 20000;
	configuracion.TIEMPO_DUMP = 40000;
}

void loggear(char *valor) {
		g_logger = log_create(RUTA_PUNTO_MONTAJE_3,"File System", 1, LOG_LEVEL_INFO);
		log_info(g_logger, valor);
		log_destroy(g_logger);
	}

char* leer_config(char* clave) {
	char* valor;
	g_config = config_create(RUTA_PUNTO_MONTAJE);
	valor = config_get_string_value(g_config, clave);
	config_destroy(g_config);
	return valor;
}
