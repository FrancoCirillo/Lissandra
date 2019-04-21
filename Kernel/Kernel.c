/*
 * kernel.c
 *
 *  Created on: 1 apr. 2019
 *      Author: rodrigo diaz
 */

#include "Kernel.h"
void* consola(void *param);
// Declaration of thread condition variable
pthread_cond_t cond_ejecutar = PTHREAD_COND_INITIALIZER;
// declaring mutex
pthread_mutex_t lock_ejecutar = PTHREAD_MUTEX_INITIALIZER;
int done = 1;

int main() {
	inicializarConfiguracion();
	instruccion_t i1={
			1,
			obtener_tiempo(),
			"Hilo 1 instr 1",
			"Param2",
			"Param3",
			"Param4",
			NULL
	};

	instruccion_t i2={
			2,
			obtener_tiempo(),
			"Hilo 1 instr 2",
			"Manzana",
			"Pera",
			"Limon",
			NULL
	};

	i1.sig=&i2;

	instruccion_t i3={
			3,
			obtener_tiempo(),
			"Hilo 1 instr 3",
			"Soy un parametro",
			"YO tambien",
			"Yo puede ser",
			NULL
	};

	i2.sig=&i3;

	proceso p1={
			&i1,
			&i1,
			NULL
	};

	instruccion_t i_1={
			1,
			obtener_tiempo(),
			"Hilo 2 instr 1",
			"Param2aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
			"Param3",
			"Param4",
			NULL
	};

	instruccion_t i_2={
			2,
			obtener_tiempo(),
			"Hilo 2 instr2",
			"Manzana",
			"Pera",
			"Limon",
			NULL
	};
	i_1.sig=&i_2;
	proceso p2={
			&i_1,
			&i_1,
			NULL
	};

	instruccion_t i__1={
			1,
			obtener_tiempo(),
			"Hilo 3 instr 1",
			"Param2aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
			"Param3",
			"Param4",
			NULL
	};

	instruccion_t i__2={
			2,
			obtener_tiempo(),
			"Hilo 3 instr2",
			"Manzana",
			"Pera",
			"Limon",
			NULL
	};
	i__1.sig=&i__2;
	proceso p3={
			&i__1,
			&i__1,
			NULL
	};


	loggear("Iniciando....");
	inicializar_semaforos();

	encolar_proceso(&p1);
	encolar_proceso(&p2);
	encolar_proceso(&p3);
	loggear("\n\n ENCOLADOS!\n\n");


/*	pthread_t un_hilo;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&un_hilo,&attr,ejecutar,NULL);
	pthread_detach(un_hilo);
	loggear("Continuando");
	continuar_ejecucion();*/

	sleep(100);
	loggear("FIN");

	//	pthread_t un_hilo;
	//	pthread_attr_t attr;
	//	pthread_create(&un_hilo,&attr,consola,NULL);
	//	pthread_join(un_hilo,NULL);
	return 0;
}
void* consola(void* param){
	while(1==1){
		sleep(1);
		char * cosa=readline(">");
		printf("La cosa es %s",cosa);
		return NULL;
	}

}
/*
void continuar_ejecucion(){
	loggear("Enviando senial en 2 segundos");
	sleep(2);
	pthread_mutex_lock(&lock_ejecutar);
	pthread_cond_signal(&cond_ejecutar);
	pthread_mutex_unlock(&lock_ejecutar);

	loggear("Senial enviada!");
}*/
int ejecutar(){
	proceso *p;
	/*
	printf("%p",p);
	while(1==1){
		//Espero a la senial para seguir
		loggear("Esperando!");
		pthread_mutex_lock(&lock_ejecutar);
		pthread_cond_wait(&cond_ejecutar,&lock_ejecutar);
		pthread_mutex_unlock(&lock_ejecutar);
		sleep(1);
		loggear("Espera finalizada");
		loggear("Ejecutando");
	 */
	while(hilos_disponibles()&&cola_ready!=NULL){//Se puede procesar
		loggear("Hay hilos y procesos!! Ejecutando...\n");
		p=obtener_sig_proceso();

		pthread_t un_hilo;
		pthread_attr_t attr;

		pthread_attr_init(&attr);
		pthread_create(&un_hilo,&attr,ejecutar_proceso,p);
		loggear("Se creo un hilo para atender la solicitud!");
		total_hilos++;
		//		pthread_join(un_hilo,NULL);
		pthread_detach(un_hilo);
		loggear("Hilo detacheado");
	}
	loggear("No hay mas hilos disponibles o procesos pendientes para ejecutar");

	return 1;
}
void* ejecutar_proceso(void* un_proceso){
	loggear("Ejecutando proceso....");
	proceso* p=(proceso*)un_proceso;
	instruccion_t* instruccion_obtenida;
	//sleep(1);
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
				printf("\n\n %d MENSAJE=%s",respuesta->codigo_instruccion,respuesta->param1);
			}
		}else{

			loggear("Ultima instruccion finalizada, fin de proceso");
			finalizar_proceso(p);

			total_hilos--;
			ejecutar();
			//ejecutar2();
			// Ejecutar aca!
			return NULL;
		}
	}
	loggear("Fin de quantum, encolando o finalizando");

	total_hilos--;

	encolar_o_finalizar_proceso(p);


	return NULL;
}
instruccion_t* ejecutar_instruccion(instruccion_t* i){
	loggear("Se ejecuta una instruccion");
//	sleep(1);
	instruccion_t* respuesta=enviar_i(i);
	return respuesta;
}
instruccion_t* enviar_i(instruccion_t* i){
	loggear("ENVIANDO INSTRUCCION FAKE");
	i->codigo_instruccion=0;
	i->timestamp=obtener_tiempo();
	printf("\n ##### \n Instruccion enviada %d, %s, %s, %s, %s\n",i->codigo_instruccion,i->param1,i->param2,i->param3,i->param4);
	return i;
}
void encolar_o_finalizar_proceso(proceso* p){
	if(p->current==NULL){//Pudo justo haber quedado parado al final
		finalizar_proceso(p);
	}else{

		encolar_proceso(p);

		//continuar_ejecucion();
			ejecutar();
	}

}
void finalizar_proceso(proceso* p){
	loggear("Se finalizo correctamente un proceso !!. Se libera su memoria");
}
void encolar_proceso(proceso *p){
	semaforo_wait(&semaforo_procesos_ready);
	loggear("Encolando proceso!");
	proceso *aux=cola_ready;
	if(aux==NULL){
		cola_ready=p;
	}else{
		while(aux->sig!=NULL){
			aux=aux->sig;
		}
		aux->sig=p;
	}
	p->sig=NULL;
	semaforo_signal(&semaforo_procesos_ready);
	loggear("Proceso encolado!");
	//Se ejecuta siempre que haya un hilo disponible y proceso para procesar.
	// Siempre implica un encolar, ya sea por agregar un hilo a la cola o porque un hilo se libero y disminuyo la cantidad corriendo.
	ejecutar();
	//continuar_ejecucion();
}
proceso* obtener_sig_proceso(){
	semaforo_wait(&semaforo_procesos_ready);
	proceso* aux=cola_ready;
	cola_ready=cola_ready->sig;
	aux->sig=NULL;
	semaforo_signal(&semaforo_procesos_ready);
	return aux;
}
instruccion_t* obtener_instruccion(proceso* p){
	instruccion_t* actual=p->current;
	if(actual!=NULL){
		p->current=p->current->sig;
	}else{
		loggear("No hay mas instrucciones");
	}
	return actual;
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
	configuracion.quantum = atoi(obtener_por_clave(rutaConfiguracion, "quantum"));
	configuracion.gradoMultiprocesamiento = atoi(obtener_por_clave(rutaConfiguracion, "gradoMultiprocesamiento"));
	configuracion.ip = obtener_por_clave(rutaConfiguracion, "IP");
	configuracion.puerto = obtener_por_clave(rutaConfiguracion, "PUERTO");
	configuracion.rutaLog = obtener_por_clave(rutaConfiguracion, "rutaLog");

}
void actualizar_configuracion(){
	char* rutaConfiguracion = "Kernel.config";
	configuracion.quantum = atoi(obtener_por_clave(rutaConfiguracion, "quantum"));
	configuracion.gradoMultiprocesamiento = atoi(obtener_por_clave(rutaConfiguracion, "gradoMultiprocesamiento"));

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
void inicializar_semaforos(){
	sem_init(&semaforo_procesos_ready,0,1);
	loggear("Semaforos inicializados");
}
void semaforo_wait(sem_t *semaforo){
	int n=sem_wait(semaforo);
	if(n!=0){
		loggear("Error al hacer wait");
	}
}
void semaforo_signal(sem_t *semaforo){
	int n=sem_post(semaforo);
	if(n!=0){
		loggear("Error al hacer signal");
	}
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



/*
void encolar_hilo(hilos_t *cola,hilos_t* nuevo){
	hilos_t* aux=cola;
	if(cola==NULL){
		cola=nuevo;
	}else{
		while(aux->sig!=NULL){
			aux=aux->sig;
		}
		aux->sig=nuevo;
		nuevo->sig=NULL;
	}
}
hilos_t* obtener_hilo(hilos_t *cola){
	hilos_t* aux=cola;
	cola=cola->sig;
	aux->sig=NULL;
	return aux;
}*/
