/*
 * kernel.h
 *
 *  Created on: 1 apr. 2019
 *      Author: rodrigo diaz
 */

#include "Kernel.h"

int main()
{
	inicializarConfiguracion();

	inicializarMetricas();

	inicializarMemorias();

	informarMetricas();

	leerProcesosDesdeConsola();

	//escucharYEncolarProcesos();

	//ejecutar();



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
			inicializarConfiguracion();
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
	loguear("Metricas informadas");
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

		response=enviar_instruccion(instruccion);//responde con el tipo de instruccion enviada

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
void inicializarConfiguracion(){
	char* rutaConfiguracion="configuracion.config";
	config configuracion;
	configuracion.quantum=atoi(getByKey(rutaConfiguracion,"quantum"));
	configuracion.codigoError=atoi(getByKey(rutaConfiguracion,"codigoError"));
	configuracion.gradoMultiprocesamiento=atoi(getByKey(rutaConfiguracion,"gradoMultiprocesamiento"));
	configuracion.ip=getByKey(rutaConfiguracion,"ip");
	configuracion.puerto=getByKey(rutaConfiguracion,"puerto");
	configuracion.rutaLog=getByKey(rutaConfiguracion,"rutaLog");
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
response* enviar_instruccion(instruccion *i){
	response *r;
	return r;
}

memoria obtenerMemoria(instruccion* instr){
	memoria m;
	return m;
}
void metricarInsert(clock_t tiempoRespuesta){
	m.tiempoInsert=(m.tiempoInsert*m.cantidadInsert+tiempoRespuesta)/(m.cantidadInsert++);
}

void loguear(char *valor){
	g_logger = log_create(configuracion.rutaLog,"un logueo" , 1, LOG_LEVEL_INFO);
	log_info(g_logger, valor);
	log_destroy(g_logger);
}
void inicializarMemorias(){
	loguear("Memorias inicializadas");
}
char* getByKey(char* ruta, char* key){
	char* valor;

	//ahora, vamos a abrir el archivo de configuracion "tp0.config"
	g_config = config_create(ruta);

	//tenemos que levantar en valor asociado a la clave "CLAVE" del archivo y asignarselo a la variable valor
	valor=config_get_string_value(g_config,key);
	loguear("Obteniendo datos");
	loguear(valor);
	//config_destroy(g_config);
	return valor;

}
void inicializarMetricas(){
	loguear("Metricas inicializadas");
}
