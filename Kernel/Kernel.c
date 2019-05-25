//---------kernel.c---------

#include "Kernel.h"

char * codigo_envio;

int main() {

	inicializar_semaforos();

	inicializarConfiguracion();

	iniciar_log();

	iniciar_ejecutador();

	//kernel_run("p1.lql");
	//kernel_run("p2.lql");
	//kernel_run("p3.lql");

	kernel_run("sleep.lql");

	loggear("Se encola instruccion, durmiendo 5 segundos!");

	sleep(6);
	recibi_respuesta_fake1();
	//iniciar_consola();
	loggear("Recibi respuesta fake1 ejecutado!");
	sleep(6);
	recibi_respuesta_fake2();
	loggear("Recibi respuesta fake2 ejecutado!");
	sleep(10);
	loggear("### KERNEL FINALIZADO ###");
	return 0;
}
void iniciar_consola(){
	sleep(1);
	loggear("Se inicia consola");
	pthread_t hilo_consola;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&hilo_consola,&attr,consola,NULL);
	pthread_join(hilo_consola,NULL);

}
void* consola(void* c){
	char* instruccion;
	while(1){
		instruccion=readline("\n>>");
		if(strcmp(instruccion,"close")==0){
			loggear("##### FINALIZANDO KERNEL.... ###### \n");
			return NULL;
		}

		printf("Procesando instruccion:: %s \n",instruccion);
		procesar_instruccion_consola(instruccion);

	}
}
void procesar_instruccion_consola(char *instruccion){
	loggear("Generando instruccion unitaria");
	t_list* instrucciones = list_create();
	proceso* p=malloc(sizeof(proceso));
	p->current=0;
	p->size=0;
	p->instrucciones=instrucciones;
	p->sig=NULL;
	instr_t* nueva_instruccion=leer_a_instruccion(instruccion,CONSOLA_KERNEL);
	list_add(p->instrucciones,nueva_instruccion);
	p->size++;
	loggear("Instruccion generada, encolando proceso...");
	encolar_proceso(p);

}
void iniciar_ejecutador(){
	loggear("Se inicia ejecutador");
	pthread_t hilo_ejecutador;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&hilo_ejecutador,&attr,ejecutar,NULL);
	pthread_detach(hilo_ejecutador);
	loggear("Ejecutador iniciado");
}
void kernel_run(char *nombre_archivo){
	loggear("RUN FILE!");
	FILE *f=fopen(nombre_archivo,"r");
	char line[64];
	t_list* instrucciones = list_create();
	proceso* p=malloc(sizeof(proceso));
	p->current=0;
	p->size=0;
	p->instrucciones=instrucciones;
	p->sig=NULL;
	instr_t* nueva_instruccion;
	loggear("Se ejecuto RUN, leyendo archivo!");

	while(fgets(line,sizeof(line),f)){
		nueva_instruccion=leer_a_instruccion(line,CONSOLA_KERNEL);
		char*  codigo=malloc(sizeof(char)*12);
		sprintf(codigo,"%d",obtener_codigo_request());
		//printf("EL codigo es %s",codigo);

		list_add(nueva_instruccion->parametros,codigo);

		list_add(p->instrucciones,nueva_instruccion);
		p->size++;
		loggear("Se agrego una instruccion!");
		//		print_instruccion(nueva_instruccion);
	}
	fclose(f);

	encolar_proceso(p);

}

int obtener_codigo_request(){
	sem_wait(&mutex_codigo_request);
	codigo_request++;
	int aux=codigo_request;
	sem_post(&mutex_codigo_request);
	return aux;
}

void continuar_ejecucion(){
	loggear("Enviando senial en 1 segundo");
	sleep(1);
	pthread_mutex_lock(&lock_ejecutar);
	pthread_cond_signal(&cond_ejecutar);
	pthread_mutex_unlock(&lock_ejecutar);

	loggear("Senial enviada!");
}
int hay_procesos(){
	sem_wait(&semaforo_procesos_ready);
	int hay=cola_ready!=NULL;
	sem_post(&semaforo_procesos_ready);
	return hay;
}
int ejecutar(){
	proceso *p;

	while(1){
		//Espero a la senial para seguir
		loggear("Esperando!");
		pthread_mutex_lock(&lock_ejecutar);
		pthread_cond_wait(&cond_ejecutar,&lock_ejecutar);
		pthread_mutex_unlock(&lock_ejecutar);

		//sleep(1);
		loggear("Espera finalizada");
		loggear("Ejecutando");

		while(hilos_disponibles()&&hay_procesos()){//Se puede procesar

			loggear("Hay hilos y procesos!! Ejecutando...\n");
			p=obtener_sig_proceso();

			pthread_t un_hilo;
			pthread_attr_t attr;

			pthread_attr_init(&attr);
			pthread_create(&un_hilo,&attr,ejecutar_proceso,p);
			loggear("Se creo un hilo para atender la solicitud!");

			subir_cantidad_hilos();

			//		pthread_join(un_hilo,NULL);
			pthread_detach(un_hilo);
			loggear("Hilo detacheado");
		}
		loggear("No hay mas hilos disponibles o procesos pendientes para ejecutar");


	}
	return 1;
}
void* ejecutar_proceso(void* un_proceso){
	loggear("Ejecutando proceso....");
	proceso* p=(proceso*)un_proceso;
	instr_t* instruccion_obtenida;
	for(int i=0;i<configuracion.quantum;/*i++*/){
		loggear("Hay quantum!");
		instruccion_obtenida=obtener_instruccion(p);
		//no se pudo obtener
		if(instruccion_obtenida!=NULL){

			instr_t* respuesta=ejecutar_instruccion(instruccion_obtenida);

			if(!respuesta->codigo_operacion){//Codigo 0-> OK, Codigo !=0 Error

				loggear("Se ejecuto correctamente la instruccion!, Respuesta=");
				loggear(obtener_parametroN(respuesta,0));
				loggear("Fin de instruccion");
			}else{

				loggear("ERROR al ejecutar la instruccion, Codigo=");
				printf("\n\n %d MENSAJE=%s",respuesta->codigo_operacion,obtener_parametroN(respuesta,0));
			}
		}else{

			loggear("Ultima instruccion finalizada, fin de proceso");
			finalizar_proceso(p);

			bajar_cantidad_hilos();

			return NULL;
		}
		i++;
		printf("\n Fin de instruccion. Quantum restante: %d, Nro de instr: %d, Quantum: %d\n",configuracion.quantum-i,i,configuracion.quantum);

	}
	loggear("Fin de quantum, encolando o finalizando");

	bajar_cantidad_hilos();

	encolar_o_finalizar_proceso(p);


	return NULL;
}
void subir_cantidad_hilos(){
	sem_wait(&mutex_cantidad_hilos);
	total_hilos++;
	sem_post(&mutex_cantidad_hilos);
}
void bajar_cantidad_hilos(){
	sem_wait(&mutex_cantidad_hilos);
	total_hilos--;
	sem_post(&mutex_cantidad_hilos);
}
char* obtener_parametroN(instr_t* i,int index){
	return (char*)list_get(i->parametros,index);
}
instr_t* ejecutar_instruccion(instr_t* i){
	loggear("Se ejecuta una instruccion");
	//	sleep(1);
	if(i->codigo_operacion==CODIGO_RUN){
		kernel_run(obtener_parametroN(i,0));
	}
	instr_t* respuesta=enviar_i(i);
	return respuesta;
}

instr_t* enviar_i(instr_t* i){

	loggear("ENVIANDO INSTRUCCION:  ");
	print_instruccion(i);

	hilo_enviado* h=malloc(sizeof(hilo_enviado));
	pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
	h->cond_t=&cond;
	h->mutex_t=&lock;

	//Fake init
	loggear("Seteo time verdadero");
	codigo_envio=list_get(i->parametros,list_size(i->parametros)-1);
	printf("EL VALOR DEL CODIGO  ES DE %s",codigo_envio);
	//Fake end

	loggear("Agrego a diccionario");

	sem_wait(&mutex_diccionario_enviados);
	printf("EL VALOR DEL CODIGO ES DE %s \n",(char *)list_get(i->parametros,list_size(i->parametros)-1));
	dictionary_put(diccionario_enviados,list_get(i->parametros,list_size(i->parametros)-1),h);
	sem_post(&mutex_diccionario_enviados);


	printf("\n##### Instruccion enviada, esperando respuesta###\n");



	loggear("Hilo bloqueado hasta recibir respuesta!");
	pthread_mutex_lock(h->mutex_t);
	pthread_cond_wait(h->cond_t,h->mutex_t);
	pthread_mutex_unlock(h->mutex_t);

	loggear("Hilo despierto!");

	return h->respuesta;
}

void recibi_respuesta_fake1(){
	instr_t* rta=malloc(sizeof(instr_t));
	t_list * params=list_create();
	char* frase="Mensaje de respuesta MENSAJE 1";
	rta->timestamp=123;
	rta->codigo_operacion=0;

	list_add(params,frase);
	list_add(params,codigo_envio);
	rta->parametros=params;

	recibi_respuesta(rta);
}
void recibi_respuesta_fake2(){
	instr_t* rta=malloc(sizeof(instr_t));
	t_list * params=list_create();
	char* frase="Mensaje de respuesta MENSAJE 2";
	rta->timestamp=456;
	rta->codigo_operacion=0;

	list_add(params,frase);
	list_add(params,codigo_envio);
	rta->parametros=params;

	recibi_respuesta(rta);
}
void recibi_respuesta(instr_t* respuesta){
	loggear("Instruccion recibida: ");
	print_instruccion(respuesta);

	sem_wait(&mutex_diccionario_enviados);
	hilo_enviado* h=dictionary_remove(diccionario_enviados,list_get(respuesta->parametros,list_size(respuesta->parametros)-1));
	loggear("Hilo obtenido y removido del diccionario!");
	sem_post(&mutex_diccionario_enviados);

	loggear("Asigno respuesta y revivo hilo");
	h->respuesta=respuesta;

	pthread_mutex_lock(h->mutex_t);
	pthread_cond_signal(h->cond_t);
	pthread_mutex_unlock(h->mutex_t);

	loggear("Hilo revivido!");
	//printf("La cantidad de waiters es de: %d el lock es de %d\n\n",h->cond_t.__data.__nwaiters, h->cond_t.__data.__lock);
		//printf("La cantidad de waiters en el principal es de:%d \n\n",cond_ejecutar.__data.__nwaiters);

}
void encolar_o_finalizar_proceso(proceso* p){
	if(p->current==p->size){//Pudo justo haber quedado parado al final
		finalizar_proceso(p);
	}else{
		encolar_proceso(p);
	}

}

void finalizar_proceso(proceso* p){
	//	instruccion_t* aux;
	//	loggear("finalizando");
	//	while((aux=p->instrucciones)!=NULL){
	//		p->instrucciones=p->instrucciones->sig;
	//		free(&aux);
	//	}
	loggear("Se finalizo correctamente un proceso !!. Se libera su memoria");
	continuar_ejecucion();
}
void encolar_proceso(proceso *p){
	sem_wait(&semaforo_procesos_ready);
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
	sem_post(&semaforo_procesos_ready);
	loggear("Proceso encolado!");
	//Se ejecuta siempre que haya un hilo disponible y proceso para procesar.
	// Siempre implica un encolar, ya sea por agregar un hilo a la cola o porque un hilo se libero y disminuyo la cantidad corriendo.
	//ejecutar();
	continuar_ejecucion();
}
proceso* obtener_sig_proceso(){
	sem_wait(&semaforo_procesos_ready);
	proceso* aux=cola_ready;
	cola_ready=cola_ready->sig;
	aux->sig=NULL;
	sem_post(&semaforo_procesos_ready);
	return aux;
}
instr_t* obtener_instruccion(proceso* p){
	if(p->current==p->size){
		return NULL;
	}else{
		instr_t* actual=(instr_t*)list_get(p->instrucciones,p->current);
		p->current++;
		return actual;
	}
}

int hilos_disponibles(){
	sem_wait(&mutex_cantidad_hilos);
	//Compara el total de config con la cantidad creada;
	int hay=configuracion.gradoMultiprocesamiento>total_hilos;
	sem_post(&mutex_cantidad_hilos);
	return hay;
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
memoria obtenerMemoria(instr_t* instr) {
	memoria m;
	return m;
}
void iniciar_log(){
	g_logger = log_create(configuracion.rutaLog,"kernel", 1, LOG_LEVEL_INFO);

}
void loggear(char *valor) {
	sem_wait(&mutex_log);
	log_info(g_logger, valor);
	sem_post(&mutex_log);
}
char* obtener_por_clave(char* ruta, char* key) {
	char* valor;
	g_config = config_create(ruta);
	valor = config_get_string_value(g_config, key);
	printf("-----------\nGenerando config, valor obtenido para %s, es:   %s \n ---------\n",key,valor);
	//config_destroy(g_config);
	return valor;
}
void inicializarMetricas() {
	loggear("Metricas inicializadas");
}
void inicializar_semaforos(){
	sem_init(&mutex_log,0,1);
	sem_init(&semaforo_procesos_ready,0,1);
	sem_init(&mutex_cantidad_hilos,0,1);
	sem_init(&mutex_diccionario_enviados,0,1);
	sem_init(&mutex_codigo_request,0,1);
	diccionario_enviados=dictionary_create();
	puts("Semaforos inicializados");
}


