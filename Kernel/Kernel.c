/*
 * kernel.c
 *
 *  Created on: 1 apr. 2019
 *      Author: rodrigo diaz
 */

#include "Kernel.h"

int main() {
	inicializarConfiguracion();
	instruccion_t i1={
			1,
			obtener_tiempo(),
			"Param1",
			"Param2",
			"Param3",
			"Param4",
			NULL
	};
	proceso p={
			&i1,
			&i1,
			NULL
	};
	loggear("Iniciando....");
	loggear(obtener_tiempo_cadena());
	encolar_proceso(&p);
	ejecutar();
	return 0;
}
int ejecutar(){
	loggear("Ejecutando");
	while(hilos_disponibles()&&cola_ready!=NULL){//Se puede procesar
		loggear("Hay hilos!! Ejecutando");
		proceso *p=obtener_sig_proceso();

		pthread_t un_hilo;
		pthread_attr_t attr;

		pthread_attr_init(&attr);
		pthread_create(&un_hilo,&attr,ejecutar_proceso,p);
		loggear("Se creo un hilo para atender la solicitud!");
		pthread_join(un_hilo,NULL);
		loggear("Hilo finalizado");
		total_hilos++;

	}
	loggear("No hay mas hilos disponibles o procesos pendientes");
	return 1;
}
void* ejecutar_proceso(void* un_proceso){
	proceso* p=(proceso*)un_proceso;
	instruccion_t* instruccion_obtenida;
	for(int i=0;i<configuracion.quantum;i++){
		loggear("Hay quantum!");
		instruccion_obtenida=obtener_instruccion(p);
		//no se pudo obtener
		if(instruccion_obtenida!=NULL){

			instruccion_t* respuesta=ejecutar_instruccion(instruccion_obtenida);

			if(!respuesta->codigo_instruccion){//Codigo 0-> OK, Codigo !=0 Error

				loggear("Se ejecuto correctamente la instruccion!, Respuesta=");
				loggear(respuesta->param1);
				loggear("Fin de instruccion");
			}else{

				loggear("ERROR al ejecutar la instruccion, Codigo=");
				printf("\n\n %d",respuesta->codigo_instruccion);
			}
		}else{

			loggear("Ultima instruccion finalizada, fin de proceso");
			finalizar_proceso(p);

			total_hilos--;

			return NULL;
		}
	}
	loggear("Fin de quantum, encolando");

	total_hilos--;

	encolar_o_finalizar_proceso(p);
	return NULL;
}
instruccion_t* ejecutar_instruccion(instruccion_t* i){
	loggear("Se ejecuta una instruccion");
	instruccion_t* respuesta=enviar_i(i);
	return respuesta;
}
instruccion_t* enviar_i(instruccion_t* i){
	loggear("ENVIANDO INSTRUCCION ");
	i->codigo_instruccion=0;
	i->timestamp=obtener_tiempo();
	return i;
}
void encolar_o_finalizar_proceso(proceso* p){
	if(p->current==NULL){//Pudo justo haber quedado parado al final
		finalizar_proceso(p);
	}else{
		encolar_proceso(p);
	}
}
void finalizar_proceso(proceso* p){
	loggear("Se finalizo correctamente un proceso !!. Se libera su memoria");
}
void encolar_proceso(proceso *p){
	proceso *aux=cola_ready;
	if(aux==NULL){
		cola_ready=p;
	}else{
		while(aux->sig!=NULL){
			aux=aux->sig;
		}
		aux->sig=p;
		p->sig=NULL;
	}
}

instruccion_t* obtener_instruccion(proceso* p){
	instruccion_t* actual=p->current;
	if(actual!=NULL){
		p->current=p->current->sig;
	}else{
		loggear("No mas instrucciones");
	}
	return actual;
}
proceso* obtener_sig_proceso(){
	proceso* aux=cola_ready;
	cola_ready=cola_ready->sig;
	return aux;
}
int hilos_disponibles(){
	//Compara el total de config con la cantidad creada;
	return configuracion.gradoMultiprocesamiento>total_hilos;
}
void inicializarMemorias() {
	loggear("Memorias inicializadas");
}
void inicializarConfiguracion() {
	char* rutaConfiguracion = "Kernel.config";
	configuracion.quantum=malloc(sizeof(int));
	configuracion.quantum = atoi(obtener_por_clave(rutaConfiguracion, "quantum"));
	configuracion.gradoMultiprocesamiento = atoi(obtener_por_clave(rutaConfiguracion, "gradoMultiprocesamiento"));
	configuracion.ip = obtener_por_clave(rutaConfiguracion, "IP");
	configuracion.puerto = obtener_por_clave(rutaConfiguracion, "PUERTO");
	configuracion.rutaLog = obtener_por_clave(rutaConfiguracion, "rutaLog");

}
memoria obtenerMemoria(instruccion_t* instr) {
	memoria m;
	return m;
}
void loggear(char *valor) {
	g_logger = log_create(configuracion.rutaLog,"kernel", 1, LOG_LEVEL_INFO);
	log_info(g_logger, valor);
	log_destroy(g_logger);
}
char* obtener_por_clave(char* ruta, char* key) {
	char* valor;
	g_config = config_create(ruta);
	valor = config_get_string_value(g_config, key);
	printf("-----------\nGenerando config, valor obtenido para %s, es:   %s \n ---------",key,valor);
	//config_destroy(g_config);
	return valor;
}
void inicializarMetricas() {
	loggear("Metricas inicializadas");
}
/*
instruccion* obtenerInstruccion(proceso *unProceso) {
	listaInstruccion* aux = unProceso->listaInstrucciones;
	if (aux == NULL) {
		unProceso->estadoActual = EXIT;
		unProceso->current++;
		return NULL;
	}
	for (int i = 0; i < unProceso->current; i++) {
		if (aux->sig != NULL) {
			aux = aux->sig;
		} else {
			unProceso->estadoActual = EXIT;
			unProceso->current++;
		}
	}
	return &aux->instruccion;

}
 */

/*
int ejecutar() {
	proceso* p;
	while (1) {
		p = popProceso();
		if (p == NULL) {
			//sleep
			loggear("No hay nada que correr");
		} else {
			p->estadoActual = READY;

			if (!realizar_proceso(p)) {
				return 0;
			}
			//agrega el P al final de la cola luego de enviarlo(y este internamente lleva la cuenta de por cual va en current)
			if (p->estadoActual == EXIT) {
				return 1;
			} else {
				p->estadoActual = READY;
				agregarAlFinal(cola_ready, p);
			}
			//sleep;
			inicializarConfiguracion();
		}
	}

}
proceso* popProceso() {
	proceso unProceso = cola_ready->p;
	cola_ready = cola_ready->sig;
	return unProceso;
}
void encolarProceso(listadoProcesos* listado, proceso unProceso) {
	listadoProcesos* aux = listado;
	if (aux == NULL) {
		aux = malloc(sizeof(listadoProcesos));
		aux->p = unProceso;
		aux->sig = NULL;
		return;
	}
	while (aux->sig != NULL) {
		aux = aux->sig;
	}
	aux->sig = malloc(sizeof(listadoProcesos));
	aux->sig->p = unProceso;
	aux->sig->sig = NULL;

}
void escucharYEncolarProcesos() {
	//va recibiendo y encolando al final procesos estado new;

}
void informarMetricas() {
	loggear("Metricas informadas");
	//logueo
	//reinicio valores
}
void leerProcesosDesdeConsola() {
	loggear("Leyendo desde consola");
	//leo creo nuevo proceso y agrego a lista
}
int realizar_proceso(proceso *unProceso) {
	response* response;
	instruccion* instruccion;
	unProceso->estadoActual=EXEC;
	for (int i = 0; i < configuracion.quantum; i++) {

		//Se asume que todos tienen por lo menos 1 instruccion

		instruccion = obtenerInstruccion(unProceso);

		instruccion->tiempoEnvio = clock();

		response = enviar_instruccion(instruccion);	//responde con el tipo de instruccion enviada

		instruccion->tiempoRespuesta = instruccion->tiempoEnvio - clock();

		if (response->tipoEnviado == configuracion.codigoInsert) {
			m.cantidadInsert++;
			metricarInsert(instruccion->tiempoRespuesta);
		}
		if (response->codigoRespuesta == configuracion.codigoError) {
			loggear("ERRROR:");
			loggear(response->codigoRespuesta);
			//0 si pincha
			return 0;
		}
		if (unProceso->estadoActual == EXIT) {
			return 1;
		}

	}
	return 1;
}*/


