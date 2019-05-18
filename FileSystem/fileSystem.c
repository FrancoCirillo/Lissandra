//-------fileSystem.c-------

#include "fileSystem.h"

int main() {

	printf("PROCESO FILESYSTEM\n");

	// Empezar a loggear

	inicializar_memtable();




	/*PRUEBAS*/

	//	CREATE
	//[TABLA]
	//[TIPO_CONSISTENCIA]
	//[NUMERO_PARTICIONES]
	//[COMPACTION_TIME]

	instr_t* instr=malloc(sizeof(instr_t));
	instr->timestamp = obtener_ts();
	instr->codigo_operacion = CODIGO_CREATE;
	instr->parametros = list_create();

	list_add(instr->parametros,"Tabla A" );
	list_add(instr->parametros,"SC" );
	list_add(instr->parametros,"5" );
	list_add(instr->parametros,"60000" );

	remitente_t* remi=malloc(sizeof(remitente_t));
	remi->ip = IP_MEMORIA;
	remi->puerto = PORT;

	remitente_instr_t* mensaje = malloc(sizeof(remitente_instr_t));
	mensaje->instruccion = instr;
	mensaje->remitente = remi;

	evaluar_instruccion(mensaje);

	printf("\nTIMESTAMP: %ld \n",mensaje->instruccion->timestamp );

	char * a= obtener_parametro(mensaje->instruccion, 0);
	char * b= obtener_parametro(mensaje->instruccion, 1);
	char * c= obtener_parametro(mensaje->instruccion, 2);
	char * d= obtener_parametro(mensaje->instruccion, 3);

	//Pruebas de lectura nomas..
	printf("cod_op: %d\n",mensaje->instruccion->codigo_operacion);
	printf("tabla: %s\n",a);
	printf("consistencia: %s\n",b);
	printf("part: %s\n",c);
	printf("t_dump: %s\n",d);

	printf("\n");
	printf("------\n");

	contestar(mensaje);  //Libera memoria del mje

	printf("------\n");
	printf("Se libero la memoria\n");




	//////////////////////////////////////////////




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

//int execute_insert(instr_t* i){
//
//	char* tabla = obtener_parametro(i,0);
//	if(!existe_tabla(tabla)){
//		return ERROR_INSERT;
//	}
//	if(mem_existe_tabla(tabla)){
//		mem_agregar_reg(i);
//	}
//
//	else {
//
//		mem_agregar_tabla(tabla);
//		mem_agregar_reg(i);
//
//	}
//	return CODIGO_EXITO;
//}

void mem_agregar_tabla(){
	t_list* t = list_create();
	list_add(memtable, t);

}

void mem_agregar_reg(){


}




void dumpeo(){
	int tiempo_dump;
	while(1){

		actualizar_configuracion(); //con semaforos
		//sem_wait(&mutex_tiempo_dump);
		tiempo_dump=config_FS.tiempo_dump;//Primero guardar variables y despues bloquearlas, y usarlas.
		//sem_post(&mutex_tiempo_dump);
		sleep(tiempo_dump);

	//	dumpear();
	}

}

int execute_create(instr_t* instr){
	char* tabla=obtener_parametro(instr,0);
	if(!existe_Tabla(tabla)){     //existe_tabla esta hardcodeado
		if(!crear_directorio(tabla))
			return ERROR_CREATE;
		if(!crear_metadata(instr))
			return ERROR_CREATE;

		//crear_particiones(tabla);
		printf("Se creo el directorio, el metadata y las particiones de la tabla: %s", tabla);
		return CODIGO_EXITO;   //Hardcodeo exito.
	}
	else{
		printf("Error al crear la tabla, la tabla ya existe\n");
		return ERROR_CREATE;
	}
}

char* obtener_parametro(instr_t * i,int index){
	return (char*)list_get(i->parametros,index);
}




void evaluar_instruccion(remitente_instr_t* mensaje){

	int respuesta= 0;
	printf("Me llego la instruccion: ");  //Todo se debe loggear.

	switch(mensaje->instruccion->codigo_operacion){

	case CODIGO_SELECT:
		printf("SELECT\n\n");
		break;

	case CODIGO_INSERT:
		printf("INSERT\n\n");
		//respuesta=execute_insert(mensaje->instruccion);
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

	mensaje->instruccion->codigo_operacion = respuesta;  //Esto pisa el codigo de operacion del mensaje para enviarle a memoria.

	/*
	contestar(mensaje);   ESTO SI VA ACA. Pero para pruebas lo comento.
		//este hace los free de la instruccion completa.
	*/

	printf("Finalizo la instruccion.\n");


}


void contestar(remitente_instr_t * i){
	//usa responder()  Lo hacen los chicos
	//se contesta
	printf("Se contesta al remitente %s con %d \n",i->remitente->ip,i->instruccion->codigo_operacion);
	list_clean(i->instruccion->parametros);
	free(i->instruccion->parametros);
	free(i->remitente);
	free(i->instruccion);
	free(i);

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




