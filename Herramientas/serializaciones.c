/*
 * serializaciones.c
 *
 *  Created on: 15 abr. 2019
 *      Author: utnso
 */

#include "serializaciones.h"


//Serializa una instruccion con parametros de tamanio varible y timestamp y codigoInstruccion con tamanios fijos
void* serializar_instruccion(instr_t* instruccion, int tamanioParam1, int tamanioParam2, int tamanioParam3, int tamanioParam4) {
	int desplazamiento = 0;
	void* paqueteInstr;

	int tamanioInstruccion = sizeof(time_t) + sizeof(int) + tamanioParam1 + tamanioParam2 + tamanioParam3 + tamanioParam4;

	agregar_a_paquete(&paqueteInstr, &tamanioInstruccion, sizeof(int), &desplazamiento);

//	Agregando la instruccion:
	agregar_a_paquete(&paqueteInstr, &instruccion->timestamp, sizeof(time_t), &desplazamiento); //Se hace el sizeof asi al deserealizar se usa el mismo sizeof
	agregar_a_paquete(&paqueteInstr, &instruccion->codigoInstruccion, sizeof(instruccion->codigoInstruccion), &desplazamiento);

	agregar_a_paquete(&paqueteInstr, &tamanioParam1, sizeof(tamanioParam1), &desplazamiento);
	agregar_a_paquete(&paqueteInstr, instruccion->param1, tamanioParam1, &desplazamiento);
	agregar_a_paquete(&paqueteInstr, &tamanioParam2, sizeof(tamanioParam2), &desplazamiento);
	agregar_a_paquete(&paqueteInstr, instruccion->param2, tamanioParam2, &desplazamiento);
	agregar_a_paquete(&paqueteInstr, &tamanioParam3, sizeof(tamanioParam1), &desplazamiento);
	agregar_a_paquete(&paqueteInstr, instruccion->param3, tamanioParam3, &desplazamiento);
	agregar_a_paquete(&paqueteInstr, &tamanioParam4, sizeof(tamanioParam2), &desplazamiento);
	agregar_a_paquete(&paqueteInstr, instruccion->param4, tamanioParam4, &desplazamiento);

	return paqueteInstr;
}

//A diferencia del agregar_a_paquete del tp0, aca lo unico que hacemos es copiar datos
void agregar_a_paquete(void** paquete, void* valor, int tamanio, int* desplazamiento) {
	if(tamanio !=0){
		*paquete = realloc(*paquete, *desplazamiento+ tamanio);
		memcpy(*paquete + *desplazamiento, valor, tamanio);
		*desplazamiento+= tamanio;
	}
}


//guarda lo recibido en * instruccion
int recibir_paquete(int socket_cliente, instr_t ** instruccion){
	int tamanio;
	void * buffer;

	int resultadoRecepcionTamanio = recv(socket_cliente, tamanio, sizeof(int), MSG_WAITALL);
	buffer= malloc(tamanio);
	*instruccion=malloc(tamanio);
//TODO: conociendo el tamanio de antemano no hace falta hacer los realloc de deserializar_instruccion
	int resultadoRecepcionInstruccion = recv(socket_cliente, buffer, tamanio, MSG_WAITALL);

	if((resultadoRecepcionInstruccion||resultadoRecepcionTamanio)==0){ //recv devuelve la cantidad recibida
		//TODO: log_error, usar errno?
			printf("ERROR: el cliente se desconecto\n");
			close(socket);
			return -1;
	}
	if(resultadoRecepcionTamanio<0){ //recv devuelve -1 si es error
		//TODO: log_error, usar errno?
		printf("ERROR: error al recibir la instruccion\n");
		return -1;
	}
	deserializar_instruccion(buffer, instruccion);
	free(buffer);

	return 0;
}

//TODO: fijarse si tendria que pasar &
void deserializar_instruccion(void* instruccion, instr_t** instruccionFinal){

	int desplazamineto = 0;

	time_t timestamp;
	int codInstruccion;

	int tamanioP1, tamanioP2, tamanioP3, tamanioP4;

	//Copia en instruccionFinal el timestamp y codigo instruccion, que ya conoce cuanto ocupan
	memcpy(&(*instruccionFinal)->timestamp, instruccion + desplazamineto, sizeof(timestamp));
	desplazamineto += sizeof(timestamp);
	memcpy(&(*instruccionFinal)->codigoInstruccion, instruccion + desplazamineto, sizeof(codInstruccion));
	desplazamineto += sizeof(codInstruccion);

	//Para copiar los parametros(de tamanio variable), recibimos en ints sus respectivos tamanios
	memcpy(&tamanioP1, instruccion + desplazamineto, sizeof(int));
	desplazamineto += sizeof(int);
	memcpy(&(*instruccionFinal)->param1, instruccion + desplazamineto, tamanioP1);
	desplazamineto += tamanioP1;

	memcpy(&tamanioP2, instruccion + desplazamineto, sizeof(int));
	desplazamineto += sizeof(int);
	memcpy(&(*instruccionFinal)->param2, instruccion + desplazamineto, tamanioP2);
	desplazamineto += tamanioP2;

	memcpy(&tamanioP3, instruccion + desplazamineto, sizeof(int));
	desplazamineto += sizeof(int);
	memcpy(&(*instruccionFinal)->param3, instruccion + desplazamineto, tamanioP3);
	desplazamineto += tamanioP3;

	memcpy(&tamanioP4, instruccion + desplazamineto, sizeof(int));
	desplazamineto += sizeof(int);
	memcpy(&(*instruccionFinal)->param4, instruccion + desplazamineto, tamanioP4);

}

void enviar_instruccion(instr_t* instruccion, int socket_cliente){
	//TODO: Hacer esto bien (No tiene sentido solo poder usar selializar a traves de enviar_instruccion)
	int tamanioParam1 = instruccion->param1 != NULL? strlen(instruccion->param1) + 1: 0;
	int tamanioParam2 = instruccion->param2 != NULL? strlen(instruccion->param2) + 1: 0;
	int tamanioParam3 = instruccion->param3 != NULL? strlen(instruccion->param3) + 1: 0;
	int tamanioParam4 = instruccion->param4 != NULL? strlen(instruccion->param4) + 1: 0;

	int tamanioTotal = sizeof(time_t) + sizeof(int) + tamanioParam1 + tamanioParam2 + tamanioParam3 + tamanioParam4; //Asi xq sizeof(estructura) != suma de el tamanio de los elementos

	void * a_enviar = serializar_instruccion (instruccion, tamanioParam1, tamanioParam2, tamanioParam3, tamanioParam4);

	if(send(socket_cliente, a_enviar, tamanioTotal, 0)<0){
		//TODO: log_error
		printf("ERROR: No se pudo enviar la instruccion\n");
	}

	free(a_enviar);
	eliminar_instruccion(instruccion);
}

void eliminar_instruccion(instr_t* instruccion){
	free(instruccion->timestamp);
	free(instruccion->codigoInstruccion);
	free(instruccion->param1);
	free(instruccion->param2);
	free(instruccion->param3);
	free(instruccion->param4);

}
