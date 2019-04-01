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
	config configuracion=obtenerConfiguracion();
	while(1){
		config configuracion=obtenerConfiguracion();

		proceso p=pop(listaProcesos);
		if(p=null){
			//sleep
			loggear("No hay nada que correr");
		}else{
			p->estadoActual=EXEC;

			if(!realizar_proceso(p,configuracion)){
				return 0;
			}
			//agrega el P al final de la cola luego de enviarlo(y este internamente lleva la cuenta de por cual va en current)
			if(p->estadoActual==EXIT){
				return 1;
			}else{
				p->estadoActual==NEW;
				push(listaProcesos,p);
			}
			//sleep;
		}
	}

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
int realizar_proceso(proceso unProceso , config configuracion){
	response respuesta;
	char* instruccion;



	for(int i=0, i<configuracion->quantum,i++){

		//Se asume que todos tienen por lo menos 1 instruccion

		instruccion=obtenerInstruccion(unProceso,i);

		instruccion->tiempoEnvio=clock();

		response=enviar_mensaje(instruccion,configuracion);//responde con el tipo de instruccion enviada

		instruccion->tiempoRespuesta=clock();

		if(response->tipoEnviado==configuracion->codigoInsert){
			m->cantidadInsert++;
			push (tiemposInserts,instruccion->tiempoRespuesta-instruccion->tiempoEnvio);
		}
		if(response->codigoRespuesta==configuracion->codigoError){
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
config obtenerConfiguracion(){
	char* rutaConfiguracion="configuracion.config";
	config c;
	c->quantum=getByKey(rutaConfiguracion,"quantum");
	c->codigoError=getByKey(rutaConfiguracion,"codigoError");
	c->gradoMultiprocesamiento=getByKey(rutaConfiguracion,"gradoMultiprocesamiento");
	c->ip=getByKey(rutaConfiguracion,"ip");
	c->puerto=getByKey(rutaConfiguracion,"puerto");
}
char* obtenerInstruccion(proceso unProceso,posicion){
	intruccion i=unProceso->listaInstrucciones[unProceso->current];
	if(i==null){
		unProceso->estadoActual=EXIT;
	}
	unProceso->current++;
	return i;

}
void enviar_mensaje(instruccion instr,config configuracion){
	//utilizo datos de conexion para enviar_mensaje

	//utilizo criterios para definir memoria
	memoria m=obtenerMemoria(instr);

}
memoria obtenerMemoria(instruccion instr){

}
