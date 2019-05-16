//-------fileSystem.c-------

#include "fileSystem.h"

int main() {

	printf("PROCESO FILESYSTEM\n");


	inicializar_memtable();








	//	CREATE
	//[TABLA]
	//[TIPO_CONSISTENCIA]
	//[NUMERO_PARTICIONES]
	//[COMPACTION_TIME]



	instr_t* instruccion;
	instruccion->timestamp = obtener_ts();
	instruccion->codigo_operacion = CODIGO_CREATE;
	instruccion->parametros = list_create();

	list_add(instruccion->parametros,"Tabla A" );
	list_add(instruccion->parametros,"SC" );
	list_add(instruccion->parametros,"5" );
	list_add(instruccion->parametros,"60000" );

	remitente_t* remi;
	remi->ip = IP_MEMORIA;
	remi->puerto = PORT;

	remitente_instr_t* mensaje;
	mensaje->instruccion = instruccion;
	mensaje->remitente = remi;

	evaluar_instruccion(mensaje);





	//DIVIDIR ESTOS PROCESOS EN HILOS.

	//inicializarConfig();    //Tenerlo listo y borrar esto.
	//printf("%s\n\n" ,config_FS.puerto_escucha);
	//printf("%d\n\n" ,config_FS.tamanio_value);


	//config_destroy(g_config);


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



void inicializar_memtable(){
	memtable = list_create();

}

int execute_insert(instr_t* i){

	char* tabla = obtener_parametro(i,0);
	if(!existe_tabla(tabla)){
		return ERROR_INSERT;
	}
	if(mem_existe_tabla(tabla)){
		mem_agregar_reg(i);
	}

	else {

		mem_agregar_tabla(tabla);
		mem_agregar_reg(i);

	}
	return CODIGO_EXITO;
}

void mem_agregar_tabla(){
	t_list* t = list_create();
	list_add(memtable, t);

}

void mem_agregar_reg(){


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

p_create* instruccion_a_create(instr_t* instr){
	p_create* param;

	param->tabla= instr->parametros ;

	return

}

void dumpeo(){
	int tiempo_dump;
	while(1){

		actualizar_configuracion(); //con semaforos
		sem_wait(&mutex_tiempo_dump);
		tiempo_dump=config_FS.tiempo_dump;//Primero guardar variables y despues bloquearlas, y usarlas.
		sem_post(&mutex_tiempo_dump);
		sleep(tiempo_dump);

		dumpear();
	}

}



//ver si es puntero o pasado por valor.
void evaluar_instruccion(remitente_instr_t* mensaje){

	int respuesta;
	//respuesta->codigo_operacion = 0;
	printf("Me llego la instruccion: ");  //Todo se debe loggear.

	switch(mensaje->instruccion->codigo_operacion){

	case CODIGO_SELECT:
		printf("SELECT\n\n");
		break;

	case CODIGO_INSERT:
		printf("INSERT\n\n");
		respuesta=execute_insert(mensaje->instruccion);
		break;

	case CODIGO_CREATE:
		printf("CREATE\n\n");
		respuesta = execute_create(mensaje->instruccion);					//todos tienen que devolver un valor,

		break;

	case CODIGO_DESCRIBE:
		printf("DESCRIBE\n\n");
		break;

	case CODIGO_DROP:
		printf("DROP\n\n");
		break;

	default:
		//verrrr
		printf("No es una instruccion valida dentro del File System.\n\n");
	}

	mensaje->instruccion->codigo_operacion = respuesta;  //Esto pisa el parametro 1 con el mensaje a enviarle a memoria.
	//y el codigo de instr es el codigo de la respuesta. (0 es exito, o el num neg es el error que corresponde)


	contestar(mensaje);


	printf("Finalizo la instruccion.\n");
	//ver de hacer un free al mensaje si es necesario, o ver donde.



	//ESTO
}


void contestar(remitente_instr_t * i){
	//usa responder()
	//se contestar
	printf("Se contesta al remitente %s con %d \n",i->remitente->puerto,i->instruccion->codigo_operacion);;
	list_iterate(i->instruccion->parametros, free);
	free(i->instruccion->codigo_operacion);
	free(i->instruccion->timestamp);
	free(i->remitente->ip);
	free(i->remitente->puerto);
 //terminar de entender como liberar esto o cuando..

}




int execute_create(instr_t* instr){
	char* tabla=obtener_parametro(instr,0);
	if(!existe_Tabla(tabla)){
		crear_directorio(tabla);
		crear_metadata(instr);
		crear_particiones(tabla);
		printf("Se creo el directorio, el metadata y las particiones de la tabla: %s", tabla);
		return CODIGO_EXITO;   //Hardcodeo exito.
	}
	else{
		printf("Error al crear la tabla");
		return ERROR_CREATE;
	}
}
char* obtener_parametro(instr_t * i,int index){
	return (char*)list_get(i->parametros,index);
}

//Crea un directorio unicamente en TABLAS.

/*
void* compactar(instr_t* i){
	while(existe_tabla(i->param1)){
		sleep(atoi(i->param2));
		compactation(i->param1);
	}
	return NULL;
}
 */






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




