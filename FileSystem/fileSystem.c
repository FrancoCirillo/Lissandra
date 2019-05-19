//-------fileSystem.c-------

#include "fileSystem.h"

int main() {

	printf("\n\n************PROCESO FILESYSTEM************\n\n");
	inicializar_FS();





	//	CREATE
	//[TABLA]
	//[TIPO_CONSISTENCIA]
	//[NUMERO_PARTICIONES]
	//[COMPACTION_TIME]
	loggear_FS("Ejecutamos Instruccion CREATE");
	instr_t* instr=malloc(sizeof(instr_t));
	instr->timestamp = obtener_ts();
	instr->codigo_operacion = CODIGO_CREATE;
	instr->parametros = list_create();



	list_add(instr->parametros,"Una Tablita chica" );
	list_add(instr->parametros,"SC" );
	list_add(instr->parametros,"7" );
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

	crear_metadata(instr);
	int cc= atoi(c);
	int p = crear_particiones(a,cc);
		if(p>0){
				puts("exito en las particiones");
		}
		else puts("fallaron");


	contestar(mensaje);  //Libera memoria del mje

	printf("------\n");
	printf("Se libero la memoria\n");



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

		// este es el que va.

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







int execute_create(instr_t* instr){
	char* tabla=obtener_parametro(instr,0);
	if(!existe_Tabla(tabla)){     //existe_tabla esta hardcodeado
		if(!crear_directorio(RUTA_TABLAS,tabla))
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




void inicializar_FS(){

	iniciar_semaforos();
	loggear_FS("-----------INICIO PROCESO-----------");
	inicializar_configuracion();
	inicializar_memtable();
	inicializar_directorios();
	crear_bloques();
	loggear_FS("-----------Fin inicialización LFS-----------");

}

void finalizar_FS(){

	config_destroy(g_config);
	config_destroy(meta_config);

	finalizar_memtable();// Liberar memoria

	loggear_FS("-----------FIN PROCESO-----------");
}


void iniciar_semaforos(){
	sem_init(&mutex_tiempo_dump_config,0,1);
	sem_init(&mutex_tiempo_retardo_config,0,1);
	sem_init(&mutex_memtable,0,1);
	sem_init(&mutex_log,0,1);

}



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




