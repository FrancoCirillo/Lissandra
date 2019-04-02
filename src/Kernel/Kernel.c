/*
 * kernel.h
 *
 *  Created on: 1 apr. 2019
 *      Author: rodrigo diaz
 */

#include "Kernel.h"

int main()
{

	inicializarMetricas();

	inicializarMemorias();

	informarMetricas();

	leerProcesosDesdeConsola();

	escucharYEncolarProcesos();

	ejecutar(listaProcesos,m);


	return 0;
}

int ejecutar(){
	proceso* p;
	while(1){
		p=popProceso();
		if(p==NULL){
			//sleep
			loggear("No hay nada que correr");
		}else{
			p->estadoActual=EXEC;

			if(!realizar_proceso(p)){
				return 0;
			}
			//agrega el P al final de la cola luego de enviarlo(y este internamente lleva la cuenta de por cual va en current)
			if(p->estadoActual==EXIT){
				return 1;
			}else{
				p->estadoActual==NEW;
				agregarAlFinal(listaProcesos,p);
			}
			//sleep;
			obtenerConfiguracion();
		}
	}

}
proceso* popProceso(){
	proceso unProceso=listaProcesos->p;
	listaProcesos=listaProcesos->sig;
	return &unProceso;
}
void agregarAlFinal(listadoProcesos* listado,proceso unProceso){
	listadoProcesos* aux=listado;
	if(aux==NULL){
			aux=malloc(sizeof(listadoProcesos));
			aux->p=unProceso;
			aux->sig=NULL;
			return;
		}
	while(aux->sig!=NULL){
		aux=aux->sig;
	}

	aux->sig=malloc(sizeof(listadoProcesos));
	aux->sig->p=unProceso;
	aux->sig->sig=NULL;

}
void escucharYEncolarProcesos(){
	//va recibiendo y encolando al final procesos estado new;

}
void informarMetricas(){
	//logueo
	//reinicio valores
}
void leerProcesosDesdeConsola(){
	//leo creo nuevo proceso y agrego a lista
}
int realizar_proceso(proceso *unProceso){
	response* response;
	instruccion* instruccion;



	for(int i=0; i<configuracion.quantum;i++){

		//Se asume que todos tienen por lo menos 1 instruccion

		instruccion=obtenerInstruccion(unProceso);

		instruccion->tiempoEnvio=clock();

		response=enviar_mensaje(*instruccion);//responde con el tipo de instruccion enviada

		instruccion->tiempoRespuesta=instruccion->tiempoEnvio-clock();

		if(response->tipoEnviado==configuracion.codigoInsert){
			m.cantidadInsert++;
			metricarInsert (instruccion->tiempoRespuesta);
		}
		if(response->codigoRespuesta==configuracion.codigoError){
			loggear("ERRROR:");
			loggear(response->codigoRespuesta);
			//0 si pincha
			return 0;
		}
		if(unProceso->estadoActual==EXIT){
			return 1;
		}

	}
	return 1;
}
void obtenerConfiguracion(){
	char* rutaConfiguracion="configuracion.config";
	config configuracion;
	configuracion.quantum=atoi(getByKey(rutaConfiguracion,"quantum"));
	configuracion.codigoError=atoi(getByKey(rutaConfiguracion,"codigoError"));
	configuracion.gradoMultiprocesamiento=atoi(getByKey(rutaConfiguracion,"gradoMultiprocesamiento"));
	configuracion.ip=getByKey(rutaConfiguracion,"ip");
	configuracion.puerto=getByKey(rutaConfiguracion,"puerto");
}
instruccion* obtenerInstruccion(proceso *unProceso){
	listaInstruccion* aux=unProceso->listaInstrucciones;
	if(aux==NULL){
		unProceso->estadoActual=EXIT;
		unProceso->current++;
		return NULL;
	}
	for(int i=0;i<unProceso->current;i++){
		if(aux->sig!=NULL){
			aux=aux->sig;
		}else{
			unProceso->estadoActual=EXIT;
			unProceso->current++;
		}
	}
	return &aux->instruccion;

}
response* enviar_mensaje(instruccion instr){
	//utilizo datos de conexion para enviar_mensaje
	memoria m;
	response r;
	//utilizo criterios para definir memoria
	m=obtenerMemoria(instr);
	return &r;
}
memoria obtenerMemoria(instruccion instr){
	memoria m;
	return m;
}
void metricarInsert(clock_t tiempoRespuesta){
	m.tiempoInsert=(m.tiempoInsert*m.cantidadInsert+tiempoRespuesta)/(m.cantidadInsert++);
}
void loggear(char* mensaje){
	puts(mensaje);
}
void inicializarMemorias(){

}
char* getByKey(char* ruta, char* buscado){
	return "123";
}
