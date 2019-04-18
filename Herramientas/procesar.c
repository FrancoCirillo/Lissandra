/*
 * procesar.c
 *
 *  Created on: 15 abr. 2019
 *      Author: rodrigo diaz
 */
//
//#include "procesar.h"
//
//const cod_proceso soy=KERNEL;
//
//inbox el_inbox=NULL;
//
//procesar(remitente_instr_t *mensaje){
//
//	instr_t* instruccion=mensaje->instruccion;
//	remitente_t* remitente=mensaje->remitente;
//	if(soy==MEMORIA){
//		/*--CASO MEMORIA*/
//		//Al recibir si el codigo es >0 viene a procesar. Si el codigo es <=0 Lo mandas al inbox y revivis el hilo correspondiente
//		//En caso de que necesite consultar al filesystem.
//		remitente_t filesystem;
//		if(instruccion->codigoInstruccion>0){
//			instr_t respuesta=procesar_instruccion(instruccion);
//			if(respuesta==NULL)
//				enviar_intruccion(instruccion,filesystem);
//			while((respuesta=chequear_inbox(instruccion))==NULL){
//				agregar_a_dormir();
//				dormir();
//			}
//			mostrar_respuesta(instruccion,respuesta);
//			devolverA(respuesta,remitente);
//		}else{
//			agregar_a_inbox(instruccion);
//			despertar(instruccion);
//		}
//
//	}
//	if(soy==KERNEL){
//		/*CASO KERNEL*/
//		//En caso de que necesite consultar a una memoria.
//		if(instruccion->codigoInstruccion>0){
//			remitente_t memoria=obtener_memoria(instruccion);
//			enviar_instruccion(instruccion,memoria);
//			instr_t respuesta;
//			while((respuesta=chequear_inbox(instruccion))==NULL){
//				agregar_a_dormir();
//				dormir();
//			}
//			mostrar_respuesta(instruccion,respuesta);
//		}else{
//			agregar_a_inbox(instruccion);
//			despertar(instruccion);
//		}
//	}
//	if(soy==FILESYSTEM){
//		/*CASO FILESYSTEM*/
//		instr_t respuesta=procesar_instruccion(instruccion);
//		devolverA(respuesta,remitente);
//	}
//}
//instr_t chequear_inbox(instr_t* instruccion){
//	inbox aux=el_inbox;
//	inbox reemplazador;
//	while(aux!=NULL& &&aux->instruccion->timestamp!=instruccion->timestamp){
//		reemplazador=aux;
//		aux=aux->sig;
//	}
//	if(aux!=NULL){
//		//REMUEVO AUX DE LA LISTA
//		reemplazador->sig=aux->sig;
//	}
//	return aux;
//}
//void agregar_a_inbox(instr_t* instruccion){
//	inbox aux=malloc(sizeof(inbox));
//	aux->instruccion=instruccion;
//	aux->sig=el_inbox;
//	el_inbox=aux;
//}
//instr_t procesar_instruccion(instr_t * instruccion){
//	//HACER ALGO O DEVOLVER NULL
//	return instruccion;
//}
//void mostrar_respuesta(instr_t * instruccion,instr_t* respuesta){
//	printf("Se recibio la respuesta %s para el pedido de instruccion %d",respuesta->param1,instruccion->codigoInstruccion);
//}
//remitente_t* obtener_memoria(instr_t*instruccion){
//	remitente_t* un_remitente=malloc(sizeof(remitente_t));
//	un_remitente->ip="1.1.1.";
//	un_remitente->puerto="444";
//	return un_remitente;
//}
//
