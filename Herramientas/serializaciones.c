/*
 * serializaciones.c
 *
 *  Created on: 15 abr. 2019
 *      Author: utnso
 */

#include "serializaciones.h"


/*
 * Recibe la direccion de memoria de la instruccion a serealizar, y devuelve el paquete
 * junto con el tamanio total de la instruccion (No del paquete!):
 * tamanioTotalInstruccion = tamanioPaquete - sizeof(int) - 4*sizeof(int)
 */
void * serializar_instruccion(instr_t *instruccion, int* tamanioTotalInstruccion){

	int desplazamiento = 0;

	int tamanioParam1 = instruccion->param1 != NULL? strlen(instruccion->param1) + 1: 0;
	int tamanioParam2 = instruccion->param2 != NULL? strlen(instruccion->param2) + 1: 0;
	int tamanioParam3 = instruccion->param3 != NULL? strlen(instruccion->param3) + 1: 0;
	int tamanioParam4 = instruccion->param4 != NULL? strlen(instruccion->param4) + 1: 0;

	*tamanioTotalInstruccion = sizeof(time_t) + sizeof(int) + sizeof(char)*(tamanioParam1+tamanioParam2+tamanioParam3+tamanioParam4); //Timestamp, codigoInstruccion, parametros

	void*buffer=malloc(sizeof(int) + *tamanioTotalInstruccion + 4*sizeof(int));//Tam paquete, instruccion, tamanio parametros

	//Serializando tamanio total de la instruccion (sin contar los ints que indican sus tamanios)
	memcpy(buffer + desplazamiento, tamanioTotalInstruccion,sizeof(int));
	desplazamiento += sizeof(int);


	//Serializarndo instruccion
	memcpy(buffer + desplazamiento, &(instruccion->timestamp),sizeof(time_t));
	desplazamiento += sizeof(time_t);
	memcpy(buffer + desplazamiento, &(instruccion->codigoInstruccion),sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(buffer + desplazamiento, &(tamanioParam1),sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(buffer + desplazamiento,instruccion->param1,tamanioParam1);
	desplazamiento += tamanioParam1;
	memcpy(buffer + desplazamiento, &(tamanioParam2),sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(buffer + desplazamiento, instruccion->param2,tamanioParam2);
	desplazamiento += tamanioParam2;
	memcpy(buffer + desplazamiento,&(tamanioParam3),sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(buffer + desplazamiento,instruccion->param3,tamanioParam3);
	desplazamiento += tamanioParam3;
	memcpy(buffer + desplazamiento,&(tamanioParam4),sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(buffer + desplazamiento,instruccion->param4,tamanioParam4);
	desplazamiento += tamanioParam4;
	puts("Serializacion terminada!");
	return buffer;
}

/*
 * Recibe instruccion_a_parsear sin tipo y devuelve la instruccion ya con su tipo instr_t
 * Lo que recibe debe estar en formato timestamp-codigoOperacion-tam1-param1-tam2-param2-tam3-param3-tam4-param4
 * (No debe incluir el tamanio total de la instruccion que envia nuestro enviar_paquete())
 */
instr_t* deserializar_instruccion(void* instruccion_a_parsear){
	/*
	 * Los auxiliares van guardando los resultados y al final se le asignan a la instruccion que se devuelve
	 */
	int* auxiliar_codigo;
	int *auxiliar_tamanio1,*auxiliar_tamanio2,*auxiliar_tamanio3,*auxiliar_tamanio4;
	time_t* auxiliar_timestamp;
	char* auxiliar_param1,*auxiliar_param2,*auxiliar_param3,*auxiliar_param4;

	int desplazamiento = 0;

	/*
	 * Ya conocemos de antemano el tamanio del timestamp y codigoOperacion, por lo tanto no recibimos (ni deserializamos) sus tamanios
	 */
	void* auxiliar_buffer=malloc(sizeof(time_t));
	memcpy(auxiliar_buffer, instruccion_a_parsear + desplazamiento,sizeof(time_t));
	auxiliar_timestamp = (time_t*) auxiliar_buffer;
	desplazamiento+= sizeof(time_t);

	auxiliar_buffer=malloc(sizeof(int));
	memcpy(auxiliar_buffer,instruccion_a_parsear + desplazamiento,sizeof(int));
	auxiliar_codigo=(int*)auxiliar_buffer;
	desplazamiento+=sizeof(int);
	/*
	 * Siempre hay que hacer memcpy desde donde se leyo el parametro anterior+el tamanio del parametro anterior.
	 * (Notar como incremente instruccion_a_parser+desplazamiento)
	 *
	 * Obtener el tamanio del siguiente parametro, hacer malloc de ese tamanio y leer la cadena.
	 *
	 * Si bien cada vez que hacemos malloc se recibe una direccion de memoria nueva, la anterior no se pierde
	 * porque se guarda en los auxiliares.
	 */
	//Recibo primer parametro
	auxiliar_buffer=malloc(sizeof(int));
	memcpy(auxiliar_buffer, instruccion_a_parsear + desplazamiento, sizeof(int));
	auxiliar_tamanio1=(int*)auxiliar_buffer;
	desplazamiento+= sizeof(int);

	auxiliar_buffer=malloc(*auxiliar_tamanio1);
	memcpy(auxiliar_buffer,instruccion_a_parsear + desplazamiento, (*auxiliar_tamanio1)*sizeof(char));
	auxiliar_param1=(char*)auxiliar_buffer;
	desplazamiento+= *auxiliar_tamanio1*sizeof(char);

	//Recibo segundo parametro
	auxiliar_buffer=malloc(sizeof(int));
	memcpy(auxiliar_buffer,instruccion_a_parsear + desplazamiento, sizeof(int));
	auxiliar_tamanio2=(int*)auxiliar_buffer;
	desplazamiento+= sizeof(int);

	auxiliar_buffer=malloc(*auxiliar_tamanio2);
	memcpy(auxiliar_buffer, instruccion_a_parsear + desplazamiento, (*auxiliar_tamanio2)*sizeof(char));
	auxiliar_param2=(char*)auxiliar_buffer;
	desplazamiento+= *auxiliar_tamanio2*sizeof(char);

	//Recibo tercer parametro
	auxiliar_buffer=malloc(sizeof(int));
	memcpy(auxiliar_buffer, instruccion_a_parsear + desplazamiento, sizeof(int));
	auxiliar_tamanio3=(int*)auxiliar_buffer;
	desplazamiento+= sizeof(int);

	auxiliar_buffer=malloc(*auxiliar_tamanio3);
	memcpy(auxiliar_buffer, instruccion_a_parsear + desplazamiento, (*auxiliar_tamanio3)*sizeof(char));
	auxiliar_param3=(char*)auxiliar_buffer;
	desplazamiento+= *auxiliar_tamanio3*sizeof(char);

	//Recibo cuarto parametro
	auxiliar_buffer=malloc(sizeof(int));
	memcpy(auxiliar_buffer, instruccion_a_parsear + desplazamiento, sizeof(int));
	auxiliar_tamanio4=(int*)auxiliar_buffer;
	desplazamiento+= sizeof(int);

	auxiliar_buffer=malloc(*auxiliar_tamanio4);
	memcpy(auxiliar_buffer, instruccion_a_parsear + desplazamiento, (*auxiliar_tamanio4)*sizeof(char));
	auxiliar_param4=(char*)auxiliar_buffer;
	desplazamiento+= *auxiliar_tamanio4*sizeof(char);


	int tamanio_parametros = (*auxiliar_tamanio4 + *auxiliar_tamanio3 +*auxiliar_tamanio2+*auxiliar_tamanio1)*sizeof(char);

	instr_t* instruccion_recibida = malloc(sizeof(time_t)+sizeof(int)+tamanio_parametros); //timestamp + codOperacion + parametros

	instruccion_recibida->timestamp = * auxiliar_timestamp;
	instruccion_recibida->codigoInstruccion = * auxiliar_codigo;
	instruccion_recibida->param1 = strdup(auxiliar_param1);
	instruccion_recibida->param2 = strdup(auxiliar_param2);
	instruccion_recibida->param3 = strdup(auxiliar_param3);
	instruccion_recibida->param4 = strdup(auxiliar_param4);

	return instruccion_recibida;
}

//guarda lo recibido en * instruccion
int recibir_paquete(int socket_cliente, instr_t ** instruccion){
	t_log* logger_serial = log_create("serializaciones.log", "Serializaciones", 1, LOG_LEVEL_DEBUG);
	int tamanioInstruccion;
	void * buffer;

	puts("Recibiendo paquete");

	//Lee el primer int que dice cuanto ocupa la instruccion en si (Sin contar los auxilares)
	int resultadoRecepcionTamanio = recv(socket_cliente, &tamanioInstruccion, sizeof(int), MSG_WAITALL);

	//recv devuelve -1 si es error
	if(resultadoRecepcionTamanio<0){
		log_error(logger_serial, "Error al recibir el tamanio de la instruccion");
		return -1;
	}
	buffer= malloc(tamanioInstruccion+ 4*sizeof(int));
	*instruccion = malloc(tamanioInstruccion);

	//Lee el resto del paquete
	int resultadoRecepcionInstruccion = recv(socket_cliente, buffer, tamanioInstruccion + 4*sizeof(int), MSG_WAITALL);

	//recv devuelve -1 si es error
	if(resultadoRecepcionInstruccion<0){
		log_error(logger_serial, "Error al recibir la instruccion");
		return -1;
	}
	//Si alguno de los dos es 0 (recv devuelve 0 si se desconecto)
	if((resultadoRecepcionTamanio && resultadoRecepcionInstruccion)==0){
		log_error(logger_serial, "El cliente se desconecto");
		close(socket_cliente);
		return -1;
//        FD_CLR(i, &master); // PARA SACARLO DEL SELECTTT
	}
	*instruccion = deserializar_instruccion(buffer);
//	free(buffer);
	return 0;
}

/*
 * Serializa la instruccion y la envia al socket_cliente
 *
 */
void enviar_instruccion_socket(instr_t* instruccion, int socket_cliente){

	int tamanioTotalInstruccion;
	void * a_enviar = serializar_instruccion (instruccion, &tamanioTotalInstruccion);
	puts("Instruccion serializada");
	if(send(socket_cliente, a_enviar, sizeof(int) + tamanioTotalInstruccion + 4*sizeof(int), 0)<0){
		//TODO: log_error
		printf("ERROR: No se pudo enviar la instruccion\n");
	}
	puts("Instruccion enviada");
	//free(a_enviar);
	//eliminar_instruccion(instruccion);
}


void eliminar_instruccion(instr_t* instruccion){
	free(instruccion->timestamp);
	free(instruccion->codigoInstruccion);
	free(instruccion->param1);
	free(instruccion->param2);
	free(instruccion->param3);
	free(instruccion->param4);
}

//Para debug
void print_instruccion(instr_t* instruccion){
	printf("timestamp: %lld\n", (long long)instruccion->timestamp);
	printf("codigoInstruccion: %d\n", instruccion->codigoInstruccion);
	printf("Param1: %s\n", instruccion->param1);
	printf("Param2: %s\n", instruccion->param2);
	printf("Param3: %s\n", instruccion->param3);
	printf("Param4: %s\n\n", instruccion->param4);

}
void leerConsola() {
	char *actual, *comando, *param1, *param2, *param3, *param4;
	comando = param1 = param2 = param3 = param4 = NULL;

	printf("\n----------Esperando requests----------\n");
	printf("Cuando desee finalizar, presione ENTER\n\n");
	char* request = readline("> ");

	while(strncmp(request, "", 1) != 0) {
		actual = strtok (request, " ");

		for(int i=0; actual != NULL; i++)
		{
			switch(i){
			case 0: comando = strdup(actual); break;
			case 1: param1 = actual != NULL? strdup(actual): actual; break;
			case 2: param2 = actual != NULL? strdup(actual): actual; break;
			case 3: param3 = actual != NULL? strdup(actual): actual; break;
			case 4: param4 = actual != NULL? strdup(actual): actual; break;
			}

			if(i==2 && strcmp(comando, "INSERT")==0){
				actual = strtok (NULL, "");
				param3 = strdup(actual);
				break;
			}

			actual = strtok (NULL, " ");
		}

		free(request);

		reconocerRequest(comando, param1, param2, param3, param4);
		comando = param1 = param2 = param3 = param4 = NULL;

		request = readline("> ");
	}

	free(request);
}


void reconocerRequest(char* comando, char* param1, char* param2, char* param3, char* param4) {

	if (strcmp(comando, "SELECT")==0) {
		printf ("Se detecto comando 'SELECT'\n");
		crearInstruccion(CODIGO_SELECT, param1, param2, param3, param4);
	}
	else if (strcmp(comando, "INSERT")==0) {
		printf ("Se detecto comando 'INSERT'\n");
		crearInstruccion(CODIGO_INSERT, param1, param2, param3, param4);
	}
	else if (strcmp(comando, "CREATE")==0) {
		printf ("Se detecto comando 'CREATE'\n");
		crearInstruccion(CODIGO_CREATE, param1, param2, param3, param4);
	}
	else if (strcmp(comando, "DESCRIBE")==0) {
		printf ("Se detecto comando 'DESCRIBE'\n");
		crearInstruccion(CODIGO_DESCRIBE, param1, param2, param3, param4);
	}
	else if (strcmp(comando, "DROP")==0) {
		printf ("Se detecto comando 'DROP'\n");
		crearInstruccion(CODIGO_DROP, param1, param2, param3, param4);
	}
	else if (strcmp(comando, "JOURNAL")==0) {
		printf ("Se detecto comando 'JOURNAL'\n");
		crearInstruccion(CODIGO_JOURNAL, param1, param2, param3, param4);
	}
	else
		printf ("Comando invalido\n\n");
}

instr_t *crearInstruccion(int codigoRequest, char* p1, char* p2, char* p3, char* p4) {
	time_t nuevoTSmp = obtener_tiempo();

	instr_t instruccionCreada ={
		.timestamp = nuevoTSmp,
		.codigoInstruccion = codigoRequest,
		.param1 = p1 != NULL? strdup(p1): p1,
		.param2 = p2 != NULL? strdup(p2): p2,
		.param3 = p3 != NULL? strdup(p3): p3,
		.param4 = p4 != NULL? strdup(p4): p4
	};

/*
	instr_t *miInstr = malloc(sizeof(instruccionCreada));
	memcpy(miInstr, &instruccionCreada, sizeof(instruccionCreada));
	print_instruccion(miInstr);
	return miInstr;
*/

	print_instruccion(&instruccionCreada);

	return &instruccionCreada;
}
