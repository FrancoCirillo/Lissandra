//---------kernel.c---------

#include "Kernel.h"
char * codigo_envio;

int main(int argc, char* argv[]) {



	inicializar_semaforos();

	inicializarConfiguracion();

	iniciar_log();

	check_inicial(argc, argv);

	inicializar_criterios();

	iniciar_metricas();

	iniciar_ejecutador();

	conexionesActuales = dictionary_create();

	iniciar_ejecutador_gossiping();

	iniciar_auto_describe();

	inicializar_kernel();
	//iniciar_consola();

//	loggear_debug(string_from_format("### FINALIZANDO KERNEL ###"));
//	sleep(2);
//	loggear_info(string_from_format("### KERNEL FINALIZADO ###"));
	return 0;
}
void iniciar_auto_describe(){
	loggear_info(string_from_format("Se inicia autodescribe"));
	pthread_t hilo_ejecutador;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_create(&hilo_ejecutador,&attr,auto_describe,NULL);
	pthread_detach(hilo_ejecutador);
	loggear_trace(string_from_format("Autodescribe iniciado"));
}
void inicializar_kernel(){

	callback = ejecutar_requestRecibido;
	auxiliarConexiones = dictionary_create();

	iniciar_servidor(miIPKernel, configuracion.puerto);
	// . es el directorio Actual - Tener en cuenta que lo corremos desde la carpeta padre a la que tiene el binario
	vigilar_conexiones_entrantes(callback, actualizar_config, memoria_desconectada, ".", CONSOLA_KERNEL);

}

void ejecutar_requestRecibido(instr_t * instruccion,char* remitente){
	if(remitente==0){//Es nueva instruccion-CONSOLA
		t_list* instrucciones = list_create();
		proceso* p=malloc(sizeof(proceso));
		p->current=0;
		p->size=0;
		p->instrucciones=instrucciones;
		p->sig=NULL;
		list_add(p->instrucciones,instruccion);
		//Agrego código de instrucción
		int codigo_req=obtener_codigo_request();
		char*  codigo=malloc(sizeof(char)*(codigo_req/10+1)+1);
		loggear_info(string_from_format("Tamanio de request %d tamaio maloqueado %d",sizeof(codigo),(sizeof(char)*(codigo_req/10+1)+1)));
		sprintf(codigo,"%d",codigo_req);
		loggear_info(string_from_format("Tamanio de request %d tamaio maloqueado %d",sizeof(codigo),(sizeof(char)*(codigo_req/10+1)+1)));

		list_add(instruccion->parametros,codigo);

		p->size++;
		loggear_info(string_from_format("Instruccion generada, encolando proceso..."));
		encolar_proceso(p);
	}else{//Es una respuesta
		recibi_respuesta(instruccion, remitente);
	}

}
void procesar_instruccion_consola(char *instruccion){
	loggear_info(string_from_format("Generando instruccion unitaria"));
	t_list* instrucciones = list_create();
	proceso* p=malloc(sizeof(proceso));
	p->current=0;
	p->size=0;
	p->instrucciones=instrucciones;
	p->sig=NULL;
	instr_t* nueva_instruccion=leer_a_instruccion(instruccion,CONSOLA_KERNEL);
	char*  codigo=malloc(sizeof(char)*12);
	sprintf(codigo,"%d",obtener_codigo_request());
	list_add(nueva_instruccion->parametros,codigo);

	list_add(p->instrucciones,nueva_instruccion);
	p->size++;
	loggear_debug(string_from_format("Instruccion generada, encolando proceso..."));
	encolar_proceso(p);
}
void iniciar_ejecutador(){
	loggear_info(string_from_format("Se inicia ejecutador"));
	pthread_t hilo_ejecutador;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_create(&hilo_ejecutador,&attr,ejecutar,NULL);
	pthread_detach(hilo_ejecutador);
	loggear_trace(string_from_format("Ejecutador iniciado"));
}
void iniciar_metricas(){
	loggear_info(string_from_format("Iniciando metricas"));
	pthread_t hilo_metricas;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&hilo_metricas,&attr,&metricar,NULL);
	pthread_detach(hilo_metricas);
	loggear_trace(string_from_format("Metricas iniciadas"));

}
void *metricar(){
	while(1){
		sem_wait(&mutex_configuracion);
		int tiempo=configuracion.tiempoMetricas;
		sem_post(&mutex_configuracion);
		sleep(tiempo);
		metrics();

	}
}
void metrics(){//TODO Porcentaje de uso por memoria

	loggear_trace(string_from_format("METRICS:"));
	//printf(" \n\n PRUEBA METRICS %"PRIu64", PRUEBA UN DIGITO MENOS %"PRIu64"",obtener_ts()/100,obtener_ts()/1000);
	//Obtengo cantidad de instrucciones por tipo/tiempo para insert y select
	int cantidad_inserts=0;
	int cantidad_selects=0;
	double tiempo_inserts=0;
	double tiempo_selects=0;
	double promedio_insert=0;
	double promedio_selects=0;

	sem_wait(&mutex_lista_instrucciones_ejecutadas);
	for(int index=0;list_size(lista_instrucciones_ejecutadas)>0;index++){
		instr_t* i=(instr_t*)list_remove(lista_instrucciones_ejecutadas,0);
		if(i->codigo_operacion==CODIGO_INSERT){
			cantidad_inserts++;
			tiempo_inserts+=(double)i->timestamp/(double)1000;
			loggear_debug(string_from_format("El promedio del insert fue de %lf\n",tiempo_selects));
		}else if(i->codigo_operacion==CODIGO_SELECT){
			cantidad_selects++;
			tiempo_selects+=(double)i->timestamp/(double)1000;
			loggear_debug(string_from_format("El promedio del select fue de %lf\n",tiempo_selects));
		}
		liberar_instruccion(i);
	}
	list_destroy(lista_instrucciones_ejecutadas);
	lista_instrucciones_ejecutadas=list_create();
	sem_post(&mutex_lista_instrucciones_ejecutadas);

	if(cantidad_inserts)
		promedio_insert=(double)tiempo_inserts/(double)cantidad_inserts;
	if(cantidad_selects)
		promedio_selects=(double)tiempo_selects/(double)cantidad_selects;
	loggear_info(string_from_format("METRICS RESULTADO: \n \t Writes=%d Write Latency=%lf \n\t Reads=%d Reads Latency=%lf\n",cantidad_inserts,promedio_insert,cantidad_selects, promedio_selects));
	//TODO memload
}
void agregar_a_metricas(instr_t* i){
	sem_wait(&mutex_lista_instrucciones_ejecutadas);
	list_add(lista_instrucciones_ejecutadas,i);
	sem_post(&mutex_lista_instrucciones_ejecutadas);
}
instr_t* kernel_metrics(instr_t * i){
	loggear_debug(string_from_format("Ejecutando metricas"));
	metrics();

	instr_t * respuesta=malloc(sizeof(instr_t));
	respuesta->timestamp=i->timestamp;
	t_list * params=list_create();
	respuesta->parametros=params;
	char* mensaje=string_from_format("Metricas ejecutadas!");
	list_add(params,mensaje);
	respuesta->codigo_operacion=0;

	return respuesta;
}
instr_t* kernel_journal(instr_t* i){
	loggear_info(string_from_format("Ejecutando Journal"));

	void pedir_journal(char* nombreMemoria, identificador* ids){
		loggear_debug(string_from_format("Enviando Journal a %s", nombreMemoria));
		int conexion = obtener_fd_out_sin_diccionario(nombreMemoria);
		enviar_request_simple(i, conexion);
	}
	sem_wait(&mutex_diccionario_conexiones);
	dictionary_iterator(conexionesActuales, (void*)pedir_journal);
	sem_post(&mutex_diccionario_conexiones);

	instr_t * respuesta=malloc(sizeof(instr_t));
	respuesta->timestamp=i->timestamp;
	t_list * params=list_create();
	respuesta->parametros=params;
	char* mensaje=string_from_format("Journal ejecutado!");
	list_add(params,mensaje);
	respuesta->codigo_operacion=0;
	return respuesta;
}
instr_t* kernel_run(instr_t *i){
	loggear_info(string_from_format("Ejecutando run. Proceso en estado NUEVO"));
	char* nombre_archivo=obtener_parametroN(i,0);
	FILE *f=fopen(nombre_archivo,"r");
	instr_t * respuesta=malloc(sizeof(instr_t));
	respuesta->timestamp=i->timestamp;
	t_list * params=list_create();
	respuesta->parametros=params;

	if(!f){
		loggear_debug(string_from_format("Archivo no encontrado!"));
		respuesta->codigo_operacion=ERROR_RUN;
		char* mensaje=string_from_format(" ARCHIVO NO ENCONTRADO!");
		list_add(params,mensaje);
		return respuesta;

	}
	char line[64];
	t_list* instrucciones = list_create();
	proceso* p=malloc(sizeof(proceso));
	//free(p->instrucciones);
	p->current=0;
	p->size=0;
	p->instrucciones=instrucciones;
	p->sig=NULL;
	instr_t* nueva_instruccion;
	//loggear_debug(string_from_format("Se ejecuto RUN, leyendo archivo!");

	while(fgets(line,sizeof(line),f)){
		nueva_instruccion=leer_a_instruccion(line,CONSOLA_KERNEL);
		if(!nueva_instruccion){//Input invalido
			loggear_debug(string_from_format("El archivo posee inputs invalidos"));
			respuesta->codigo_operacion=ERROR_RUN;
			char* mensaje=string_from_format("EL ARCHIVO POSEE INPUTS INVALIDOS!");
			list_add(params,mensaje);
			return respuesta;
		}
		int cod=obtener_codigo_request();
		char*  codigo=malloc(sizeof(char)*(cod/10+1)+1);
		sprintf(codigo,"%d",cod);
		//printf("\n\n EL codigo es %d, el tamanio es %d y contenido es %s",cod,sizeof(codigo),codigo);

		list_add(nueva_instruccion->parametros,codigo);

		list_add(p->instrucciones,nueva_instruccion);
		p->size++;
		loggear_trace(string_from_format("Se agrego una instruccion!"));
		//		imprimir_instruccion(nueva_instruccion);
	}
	loggear_debug(string_from_format("\n\nSize %d\n\n",p->size));
	fclose(f);

	encolar_proceso(p);
	//RESPUESTA

	respuesta->codigo_operacion=0;
	char* mensaje=string_from_format("RUN EJECUTADO CORRECTAMENTE!");
	list_add(params,mensaje);
	loggear_trace(string_from_format("FIN RUN!"));
	return respuesta;

}
instr_t* kernel_add(instr_t* i){
	char* numero_memoria=string_from_format(obtener_parametroN(i,1));
	int codigo=obtener_codigo_criterio(obtener_parametroN(i,3));
	loggear_info(string_from_format("EJECUTANDO ADD PARA MEMORIA %s", numero_memoria));
	switch(codigo){
	case SC:
		loggear_info(string_from_format("Agregando memoria a criterio SC"));
		sem_wait(&criterio_strong_consistency->mutex_criterio);
		if(list_size(criterio_strong_consistency->lista_memorias)>0){
			instr_t * respuesta=malloc(sizeof(instr_t));
			respuesta->codigo_operacion=1007;
			char* mensaje=string_from_format("ERROR EN ADD, EL CRITERIO SC YA POSEE UNA MEMORIA ASIGNADA!");
			t_list * params=list_create();
			list_add(params,mensaje);
			respuesta->parametros=params;
			respuesta->timestamp=i->timestamp;
			loggear_trace(string_from_format("FIN ADD!"));
			sem_post(&criterio_strong_consistency->mutex_criterio);
			return respuesta;
		}
		list_add(criterio_strong_consistency->lista_memorias,numero_memoria);
		sem_post(&criterio_strong_consistency->mutex_criterio);
		break;

	case SHC:
		loggear_info(string_from_format("Agregando memoria a criterio SHC. Primero se envia journal a existentes"));
		instr_t* i=leer_a_instruccion("JOURNAL",CONSOLA_KERNEL);
		for(int i=0;i<list_size(criterio_strong_hash_consistency->lista_memorias);i++){
			char* alias_memoria=list_get(criterio_strong_hash_consistency->lista_memorias,i);
			char* nombre_memoria=krn_concat("Memoria_",alias_memoria);
			int conexionMemoria=obtener_fd_out(nombre_memoria);
			enviar_request_simple(i, conexionMemoria);
			//free(nombre_memoria);
		}
		liberar_instruccion(i);
		loggear_info(string_from_format("Se enviaron los JOURNAL!. Agregando"));
		sem_wait(&criterio_strong_hash_consistency->mutex_criterio);
		list_add(criterio_strong_hash_consistency->lista_memorias,numero_memoria);
		sem_post(&criterio_strong_hash_consistency->mutex_criterio);
		break;

	case EC:
		loggear_info(string_from_format("Agregando memoria a criterio EC"));
		sem_wait(&criterio_eventual_consistency->mutex_criterio);
		list_add(criterio_eventual_consistency->lista_memorias,numero_memoria);
		sem_post(&criterio_eventual_consistency->mutex_criterio);
		break;
	}

	instr_t * respuesta=malloc(sizeof(instr_t));
	respuesta->codigo_operacion=0;
	char* mensaje=string_from_format("ADD EJECUTADO CORRECTAMENTE!");
//	list_destroy(respuesta->parametros);
	t_list * params=list_create();
	list_add(params,mensaje);
	respuesta->parametros=params;
	respuesta->timestamp=i->timestamp;
	loggear_trace(string_from_format("FIN ADD!"));

	return respuesta;

}
int obtener_codigo_request(){
	sem_wait(&mutex_codigo_request);
	codigo_request++;
	int aux=codigo_request;
	sem_post(&mutex_codigo_request);
	return aux;
}

void continuar_ejecucion(){
	loggear_debug(string_from_format("Enviando senial"));
	//	sleep(1);
	pthread_mutex_lock(&lock_ejecutar);
	pthread_cond_signal(&cond_ejecutar);
	pthread_mutex_unlock(&lock_ejecutar);

	loggear_debug(string_from_format("Senial enviada!"));
}
int hay_procesos(){
	sem_wait(&semaforo_procesos_ready);
	int hay=cola_ready!=NULL;
	sem_post(&semaforo_procesos_ready);
	if(hay){
		//printf("\n HAY PRCOESOS\n\n");
	}
	else{
		//printf("\n NO HAY PROCESOS\n\n");
	}
	return hay;
}
int ejecutar(){
	proceso *p;

	while(1){
		//Espero a la senial para seguir
		loggear_debug(string_from_format("Esperando!"));
		pthread_mutex_lock(&lock_ejecutar);
		pthread_cond_wait(&cond_ejecutar,&lock_ejecutar);
		pthread_mutex_unlock(&lock_ejecutar);

		//sleep(1);
		loggear_info(string_from_format("Espera finalizada, ejecutando"));

		while(hilos_disponibles()&&hay_procesos()){//Se puede procesar

			loggear_trace(string_from_format("Hay hilos y procesos!! Ejecutando...\n"));
			p=obtener_sig_proceso();

			pthread_t un_hilo;
			pthread_attr_t attr;

			pthread_attr_init(&attr);
			pthread_create(&un_hilo,&attr,ejecutar_proceso,p);
			loggear_info(string_from_format("Se creo un hilo para atender la solicitud!"));

			subir_cantidad_hilos();

			//		pthread_join(un_hilo,NULL);
			pthread_detach(un_hilo);
			loggear_trace(string_from_format("Hilo detacheado"));
		}
		loggear_info(string_from_format("No hay mas hilos disponibles o procesos pendientes para ejecutar"));


	}
	return 1;
}
void* ejecutar_proceso(void* un_proceso){
	loggear_info(string_from_format("Ejecutando proceso. Proceso pasa a estado EXEC"));
	proceso* p=(proceso*)un_proceso;
	instr_t* instruccion_obtenida;

	sem_wait(&mutex_configuracion);
	int quantum = configuracion.quantum;
	sem_post(&mutex_configuracion);
	for(int i=0;i<quantum;/*i++*/){
		loggear_debug(string_from_format("Hay quantum!"));
		instruccion_obtenida=obtener_instruccion(p);
		//no se pudo obtener
		if(instruccion_obtenida!=NULL){

			instr_t* respuesta=ejecutar_instruccion(instruccion_obtenida);

			if(!respuesta->codigo_operacion){//Codigo 0-> OK, Codigo !=0 Error

				loggear_debug(string_from_format("Se ejecuto correctamente la instruccion!, respuesta:"));
				imprimir_instruccion(respuesta, loggear_debug);
				loggear_trace(string_from_format("Fin de instruccion"));
				//METRICS
				if(instruccion_obtenida->codigo_operacion==CODIGO_INSERT|| instruccion_obtenida->codigo_operacion==CODIGO_SELECT){
					instr_t* a_metricar=malloc(sizeof(instr_t));
					loggear_trace(string_from_format("Se agrega a metricas"));
					mseg_t tiempo_exec=respuesta->timestamp;
					loggear_trace(string_from_format("\n El tiempo que tardo en ejecutarse fue= %"PRIu64" milisegundos \n",tiempo_exec));
					a_metricar->timestamp=respuesta->timestamp;
					a_metricar->codigo_operacion=instruccion_obtenida->codigo_operacion;
					a_metricar->parametros=list_create();
					agregar_a_metricas(a_metricar);
				}else{
					if(instruccion_obtenida->codigo_operacion==CODIGO_DESCRIBE){
						actualizar_metadata_tablas(respuesta);
					}
				}
				liberar_instruccion(respuesta);

			}else{

				loggear_debug(string_from_format("ERROR al ejecutar la instruccion, finalizando proceso"));
				//printf("\n\n %d MENSAJE=%s",respuesta->codigo_operacion,obtener_parametroN(respuesta,0));
				//bajar_cantidad_hilos();
				imprimir_instruccion(respuesta, loggear_warning);
				liberar_instruccion(respuesta);
				finalizar_proceso(un_proceso);
				return NULL;
			}

		}else{

			loggear_info(string_from_format("Ultima instruccion finalizada, fin de proceso"));
			finalizar_proceso(p);

			return NULL;
		}
		i++;
		loggear_info(string_from_format("\n Fin de instruccion. Quantum restante: %d, Nro de instr: %d, Quantum: %d\n",configuracion.quantum-i,i,configuracion.quantum));
		sem_wait(&mutex_configuracion);
		int sleep_exec=configuracion.sleep_ejecucion;
		sem_post(&mutex_configuracion);
		usleep(sleep_exec);
	}
	loggear_trace(string_from_format("Fin de quantum, encolando o finalizando"));

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
	instr_t* respuesta;
	loggear_debug(string_from_format("#### Se ejecuta una instruccion"));
	imprimir_instruccion(i, loggear_trace);
	if(i->codigo_operacion==22){

		loggear_debug(string_from_format("#### FINALIZANDO KERNEL ####"));
		list_destroy(configuracion.IP_SEEDS);
		list_destroy(configuracion.PUERTO_SEEDS);
		liberar_instruccion(i);

		exit(0);
	}
	if(i->codigo_operacion==CODIGO_JOURNAL){
			return kernel_journal(i);
	}
	if(i->codigo_operacion==CODIGO_RUN){
		return kernel_run(i);
	}
	if(i->codigo_operacion==CODIGO_METRICS){
		return kernel_metrics(i);
	}
	if(i->codigo_operacion==CODIGO_ADD){
		return kernel_add(i);
	}
	if((respuesta=validar(i))!=NULL){
		return respuesta;
	}
	if(i->codigo_operacion==CODIGO_CREATE){
		agregar_tabla_a_criterio(i);
	}
	if(i->codigo_operacion==CODIGO_CREATE){
		if(!existe_tabla(obtener_parametroN(i,0))){
			loggear_debug(string_from_format("Agregando tabla %s a lista de tablas creadas", obtener_parametroN(i,0)));
			agregar_tabla(obtener_parametroN(i,0));
		}
	}
	respuesta=enviar_i(i);
	return respuesta;
}
instr_t *validar(instr_t * i){
	int codigo_error;
	if(i->codigo_operacion!=2&&i->codigo_operacion!=3 && i->codigo_operacion!=1){
		return NULL;
	}
	if(i->codigo_operacion==2 || i->codigo_operacion==1){
		if(existe_tabla(obtener_parametroN(i,0))){
			loggear_debug(string_from_format("Existe la tabla para el insert o select "));
			return NULL;
		}else{
			codigo_error=ERROR_INSERT;
			loggear_warning(string_from_format("La tabla %s no existe!",obtener_parametroN(i,0)));
			//mensaje="LA TABLA NO EXISTE!";
		}
	}
	if(i->codigo_operacion==3){
		if(!existe_tabla(obtener_parametroN(i,0))){
			loggear_debug(string_from_format("No existe tabla para el create"));
			return NULL;
		}else{
			//mensaje="LA TABLA YA EXISTE";
			codigo_error=ERROR_CREATE;
		}
	}
	char* mensaje=string_from_format("ERROR!");
	instr_t* mensaje_error=malloc(sizeof(instr_t));
	mensaje_error->codigo_operacion=codigo_error;
	mensaje_error->timestamp=i->timestamp;
	mensaje_error->parametros=list_create();
	list_add(mensaje_error->parametros,mensaje);
	return mensaje_error;
}

instr_t* enviar_i(instr_t* i){

	loggear_info(string_from_format(" ENVIANDO INSTRUCCION CODIGO %s\n",(char*)obtener_ultimo_parametro(i)));
	//imprimir_instruccion(i);



	loggear_debug(string_from_format("Determinando memoria para request"));
	int conexionMemoria = obtener_fd_memoria(i);
	if(conexionMemoria<0){
		loggear_debug(string_from_format("No se pudo encontrar una memoria para el criterio indicado"));
		char* mensaje=string_from_format("No existe una memoria asignada para dicha instruccion");
		instr_t* respuesta=malloc(sizeof(instr_t));
		respuesta->timestamp=i->timestamp;
		respuesta->codigo_operacion=1000;
		t_list* params=list_create();
		list_add(params,mensaje);
		list_add(params,obtener_ultimo_parametro(i));
		respuesta->parametros=params;
		return respuesta;
	}

	hilo_enviado* h=malloc(sizeof(hilo_enviado));
	pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
	h->cond_t=&cond;
	h->mutex_t=&lock;

	loggear_trace(string_from_format("Agrego hilo a diccionario"));

	sem_wait(&mutex_diccionario_enviados);
	//printf("EL VALOR DEL CODIGO ES DE %s \n",(char *)obtener_ultimo_parametro(i));
	dictionary_put(diccionario_enviados, obtener_ultimo_parametro(i), h);
	sem_post(&mutex_diccionario_enviados);
	loggear_trace(string_from_format("ENVIANDO INSTRUCCION.  "));
	enviar_request_simple(i, conexionMemoria);

	loggear_trace(string_from_format("\n##### Instruccion enviada, esperando respuesta###\n"));
	loggear_debug(string_from_format("Bloqueando hilo hasta recibir respuesta!"));
	pthread_mutex_lock(h->mutex_t);
	pthread_cond_wait(h->cond_t,h->mutex_t);
	pthread_mutex_unlock(h->mutex_t);

	loggear_debug(string_from_format("Hilo despierto!"));
	instr_t * rta=h->respuesta;
	h->respuesta->timestamp=obtener_ts()-i->timestamp;
	free(h);


	return rta;
}

int obtener_fd_memoria(instr_t *i){
	char* memoria="Memoria_";
	char* alias_memoria="";
	int codigo_criterio;
	//SIEMPRE el primer parametro es el nombre de la tabla que uso como key para determinar criterio

	sem_wait(&mutex_diccionario_criterios);
	if(i->codigo_operacion==4){//SI ES DESCRIBE A SECAS USO SC
		codigo_criterio=0;
	}else{
		codigo_criterio=*(int*)dictionary_get(diccionario_criterios,(char*)obtener_parametroN(i,0));
	}sem_post(&mutex_diccionario_criterios);

	//Obtengo la memoria segun el criterio
	loggear_trace(string_from_format("Codigo de criterio es : %d \n",codigo_criterio));
	loggear_info(string_from_format("Determinando criterio de tabla"));
	switch(codigo_criterio){
	case SC:
		sem_wait(&criterio_strong_consistency->mutex_criterio);
		alias_memoria=list_get(criterio_strong_consistency->lista_memorias,0);
		sem_post(&criterio_strong_consistency->mutex_criterio);
		break;
	case SHC:
		sem_wait(&criterio_strong_hash_consistency->mutex_criterio);
		if(i->codigo_operacion==CODIGO_SELECT || i->codigo_operacion==CODIGO_INSERT){//EL criterio requiere que para insert y selects depende de la key
			int key=atoi(obtener_parametroN(i,1));
			alias_memoria=list_get(criterio_strong_hash_consistency->lista_memorias,key % list_size(criterio_strong_hash_consistency->lista_memorias));
		}
		else{
			alias_memoria=list_get(criterio_strong_hash_consistency->lista_memorias,0);
		}

		sem_post(&criterio_strong_hash_consistency->mutex_criterio);
		break;
	case EC:
		sem_wait(&criterio_eventual_consistency->mutex_criterio);
		sem_wait(&mutex_contador_ec);
		alias_memoria=list_get(criterio_eventual_consistency->lista_memorias,contador_ec % list_size(criterio_eventual_consistency->lista_memorias));
		contador_ec++;
		sem_post(&mutex_contador_ec);
		sem_post(&criterio_eventual_consistency->mutex_criterio);
		break;
	default:
		return -100;
		break;
	}//Ya obtuve el alias de la memoria

	if(alias_memoria==NULL){
		loggear_warning(string_from_format("NO se pudo obtener la memoria para dicha instruccion"));
		return -100;
	}

	char* nombre_memoria=krn_concat(memoria,alias_memoria);
	loggear_info(string_from_format("Memoria obtenida: %s", nombre_memoria));
	int fd=obtener_fd_out(nombre_memoria);
	free(nombre_memoria);
	return fd;
}
void borrar_memoria_de_criterio(char* numero_memoria, criterio* crit){
	sem_wait(&(crit->mutex_criterio));
	for(int i=0;i<list_size(crit->lista_memorias);i++){
		char* mem =list_get(crit->lista_memorias,i);
		if(!strcmp(mem,numero_memoria)){
			loggear_info(string_from_format("Memorio encontrada y removida en posicion %d",i));
			list_remove(crit->lista_memorias,i);
			free(mem);
		}
	}

	sem_post(&(crit->mutex_criterio));
}
void memoria_desconectada(char* nombre_memoria){
	char* numero=nombre_memoria+8;
	loggear_info(string_from_format("Se desconecto la memoria %s, se borra de criterios",numero));
	borrar_memoria_de_criterio(numero,criterio_strong_consistency);
	borrar_memoria_de_criterio(numero,criterio_eventual_consistency);
	borrar_memoria_de_criterio(numero,criterio_strong_hash_consistency);

}
char* krn_concat(char* s1,char* s2){
	char* rdo=(char*)malloc(1+strlen(s1)+strlen(s2));
	strcpy(rdo,s1);
	strcat(rdo,s2);
	loggear_debug(string_from_format(rdo));
	return rdo;
}
void agregar_tabla_a_criterio(instr_t* i){//EN create
	loggear_trace(string_from_format("Agregando tabla a criterio"));
	int* codigo_criterio=malloc(sizeof(int));
	*codigo_criterio=obtener_codigo_criterio(obtener_parametroN(i,1));
	sem_wait(&mutex_diccionario_criterios);
	dictionary_put(diccionario_criterios,obtener_parametroN(i,0),codigo_criterio);
	sem_post(&mutex_diccionario_criterios);
	loggear_info(string_from_format("Se agrega la tabla %s al criterio codigo: %d\n\n",(char*)obtener_parametroN(i,0),*codigo_criterio));
}
int obtener_codigo_criterio(char* criterio){
	if(!strcmp(criterio,"SC")){
		return SC;
	}else if(!strcmp(criterio,"SHC")){
		return SHC;
	}else if(!strcmp(criterio,"EC")){
		return EC;
	}else return 4;

}
void enviar_a(instr_t* i,char* destino){
	enviar_request_simple(i,obtener_fd_out(destino));
}

int obtener_fd_out_sin_diccionario(char* proceso){
	identificador *idsProceso = (identificador *)dictionary_get(conexionesActuales, proceso);
	if(idsProceso == NULL){
		loggear_warning(string_from_format("Se desconoce completamente el proceso %s. fd_out devuelto incorrecto.", proceso));
		return 0;
	}
	if (idsProceso->fd_out == 0)
	{ //Es la primera vez que se le quiere enviar algo a proceso
		responderHandshake(idsProceso);
	}
	return idsProceso->fd_out;
}
int obtener_fd_out(char *proceso)
{
	sem_wait(&mutex_diccionario_conexiones);
	identificador *idsProceso = (identificador *)dictionary_get(conexionesActuales, proceso);
	sem_post(&mutex_diccionario_conexiones);
	if(idsProceso == NULL){
		loggear_warning(string_from_format("Se desconoce completamente el proceso %s. fd_out devuelto incorrecto.", proceso));
		return 0;
	}
	if (idsProceso->fd_out == 0)
	{ //Es la primera vez que se le quiere enviar algo a proceso
		responderHandshake(idsProceso);
	}
	return idsProceso->fd_out;
}
void responderHandshake(identificador *idsConexionEntrante)
{

	instr_t *miInstruccion = mis_datos(CODIGO_HANDSHAKE);

	int fd_saliente = crear_conexion(idsConexionEntrante->ip_proceso, idsConexionEntrante->puerto, miIPKernel, 1);
	enviar_request(miInstruccion, fd_saliente);
	idsConexionEntrante->fd_out = fd_saliente;
}
void recibi_respuesta(instr_t* respuesta, char* remitente){
	loggear_debug(string_from_format("Recibi una respuesta con el Codigo %d de %s", respuesta->codigo_operacion, remitente));
	imprimir_instruccion(respuesta, loggear_trace);

	if(respuesta->codigo_operacion==PETICION_GOSSIP)
		devolver_gossip(respuesta, remitente);
	else if (respuesta->codigo_operacion==RECEPCION_GOSSIP)
		actualizar_tabla_gossiping(respuesta);
	else{
		//	list_add(respuesta->parametros,"1");
		sem_wait(&mutex_diccionario_enviados);
		loggear_trace(string_from_format("El ultimo parametro de la instruccion es %s\n", (char*)obtener_ultimo_parametro(respuesta)));
		hilo_enviado* h=dictionary_remove(diccionario_enviados,obtener_ultimo_parametro(respuesta));
		if(h!=NULL) loggear_trace(string_from_format("Hilo obtenido y removido del diccionario!"));
		else loggear_warning(string_from_format("El hilo no existe"));
		sem_post(&mutex_diccionario_enviados);

		loggear_debug(string_from_format("Asigno respuesta y revivo hilo"));
		h->respuesta=respuesta;

		pthread_mutex_lock(h->mutex_t);
		pthread_cond_signal(h->cond_t);
		pthread_mutex_unlock(h->mutex_t);

		loggear_trace(string_from_format("Hilo revivido!"));
		//printf("La cantidad de waiters es de: %d el lock es de %d\n\n",h->cond_t.__data.__nwaiters, h->cond_t.__data.__lock);
		//printf("La cantidad de waiters en el principal es de:%d \n\n",cond_ejecutar.__data.__nwaiters);
	}
}
void encolar_o_finalizar_proceso(proceso* p){
	if(p->current==p->size){//Pudo justo haber quedado parado al final
		loggear_debug(string_from_format("FIN DE PROCESO"));
		finalizar_proceso(p);

	}else{
		encolar_proceso(p);
	}

}
void liberar_instruccion(instr_t* instruccion){

	if(instruccion->codigo_operacion == CODIGO_DESCRIBE){
		list_destroy(instruccion->parametros);
	}
	else{
		list_destroy_and_destroy_elements(instruccion->parametros,free);
	}
	free(instruccion);
}
void finalizar_proceso(proceso* p){
	loggear_info(string_from_format("Proceso pasa a estado FINALIZADO. Se libera su memoria"));
	for(int i=0;list_size(p->instrucciones)>0;i++){
		instr_t* instruccion=list_get(p->instrucciones,0);
		loggear_trace(string_from_format("Se libera la memoria para la instruccion %d",i));
		//imprimir_instruccion(instruccion, loggear_debug);
		liberar_instruccion(instruccion);
		list_remove(p->instrucciones,0);
	}
	list_destroy(p->instrucciones);

	p->sig=NULL;
	free(p);

	loggear_trace(string_from_format("Memoria liberada!"));

	bajar_cantidad_hilos();
	continuar_ejecucion();
}
void encolar_proceso(proceso *p){

	sem_wait(&semaforo_procesos_ready);
	loggear_info(string_from_format("Encolando proceso. Proceso pasa a estado READY"));
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
	loggear_trace(string_from_format("Proceso encolado!"));
	//Se ejecuta siempre que haya un hilo disponible y proceso para procesar.
	// Siempre implica un encolar, ya sea por agregar un hilo a la cola o porque un hilo se libero y disminuyo la cantidad corriendo.
	bajar_cantidad_hilos();
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
	sem_wait(&mutex_configuracion);
	int hay=configuracion.gradoMultiprocesamiento>total_hilos;
	sem_post(&mutex_configuracion);
	sem_post(&mutex_cantidad_hilos);
	if(hay){
		//printf("\n HAY PRCOESOS\n\n");
	}
	else{
		//printf("\n NO HAY PROCESOS\n\n");
	}
	return hay;
}


void inicializarConfiguracion() {
	rutaConfiguracion = "Kernel.config";
	g_config = config_create(rutaConfiguracion);
	configuracion.quantum = atoi(obtener_por_clave("quantum"));
	configuracion.gradoMultiprocesamiento = atoi(obtener_por_clave("gradoMultiprocesamiento"));
	configuracion.ip = obtener_por_clave("IP");
	configuracion.puerto = obtener_por_clave("PUERTO");
	configuracion.rutaLog = obtener_por_clave("rutaLog");
	configuracion.sleep_ejecucion = config_get_int_value(g_config, "SLEEP_EJECUCION");
	configuracion.metadata_refresh = config_get_int_value(g_config, "METADATA_REFRESH");

	char** ip_seeds = config_get_array_value(g_config, "IP_SEEDS");
	configuracion.IP_SEEDS = string_array_to_list(ip_seeds);
	free(ip_seeds);

	char** PUERTO_SEEDS=config_get_array_value(g_config, "PUERTO_SEEDS");
	configuracion.PUERTO_SEEDS = string_array_to_list(PUERTO_SEEDS);
	free(PUERTO_SEEDS);
	imprimir_config_actual();

	configuracion.RETARDO_GOSSIPING = atoi(obtener_por_clave("RETARDO_GOSSIPING"));
	configuracion.tiempoMetricas = atoi(obtener_por_clave("tiempoMetricas"));
	configuracion.LOG_LEVEL =  log_level_from_string(obtener_por_clave("LOG_LEVEL"));


}
void iniciar_log(){
	g_logger = log_create(configuracion.rutaLog,"kernel", 1, configuracion.LOG_LEVEL);
}

char* obtener_por_clave(char* key) {
	char* valor;
	valor = config_get_string_value(g_config, key);
//	printf("-----------\nGenerando config, valor obtenido para %s, es:   %s \n ---------\n",key,valor);
	return valor;
}

void inicializar_semaforos(){
	sem_init(&mutex_log,0,1);
	sem_init(&semaforo_procesos_ready,0,1);
	sem_init(&mutex_cantidad_hilos,0,1);
	sem_init(&mutex_diccionario_enviados,0,1);
	sem_init(&mutex_codigo_request,0,1);
	sem_init(&mutex_diccionario_conexiones,0,1);
	sem_init(&mutex_diccionario_criterios,0,1);
	sem_init(&mutex_contador_ec,0,1);
	sem_init(&mutex_lista_tablas,0,1);
	sem_init(&mutex_configuracion,0,1);
	sem_init(&mutex_lista_instrucciones_ejecutadas,0,1);
	sem_init(&mutex_diccionario_conexiones,0,1);
	diccionario_enviados=dictionary_create();
	lista_tablas=list_create();
	lista_instrucciones_ejecutadas=list_create();
	puts("Semaforos inicializados");
}
void inicializar_criterios(){
	diccionario_criterios=dictionary_create();
	sem_t sem[3];
	for(int i=0;i<3;i++){
		sem_init(&sem[i],0,1);
	}
	criterio_eventual_consistency=malloc(sizeof(criterio));
	criterio_eventual_consistency->codigo_criterio=0;
	criterio_eventual_consistency->lista_memorias=list_create();
	criterio_eventual_consistency->mutex_criterio=sem[0];

	criterio_strong_consistency=malloc(sizeof(criterio));
	criterio_strong_consistency->codigo_criterio=1;
	criterio_strong_consistency->lista_memorias=list_create();
	criterio_strong_consistency->mutex_criterio=sem[1];

	criterio_strong_hash_consistency=malloc(sizeof(criterio));
	criterio_strong_hash_consistency->codigo_criterio=2;
	criterio_strong_hash_consistency->lista_memorias=list_create();
	criterio_strong_hash_consistency->mutex_criterio=sem[2];
	loggear_trace(string_from_format("Criterios inicializados!"));
}
void check_inicial(int argc, char* argv[])
{
	if(argc>2){
		log_error(g_logger, "Uso: kernel <IP>, IP vacio => IP = 127.0.0.4");
		exit(0);
	}
	if(argc ==1 || strlen(argv[1])<2 ){
		puts("IP 127.0.0.4");
		miIPKernel = IP_KERNEL;
	}
	else if(argc==2){
		printf("IP %s\n", argv[1]);
		miIPKernel = argv[1];
	}
}
void agregar_tabla(char* tabla){

	sem_wait(&mutex_lista_tablas);
	list_add(lista_tablas,tabla);
	sem_post(&mutex_lista_tablas);
	loggear_debug(string_from_format("Tabla agregada"));
}
bool existe_tabla(char* tablaBuscada){
	//puts("A");
	bool existe(char*tabla){
		return strcmp(tablaBuscada,tabla)==0;
	}
	//puts("B");
	sem_wait(&mutex_lista_tablas);
	char *rdo=list_find(lista_tablas,(void*)existe);
	sem_post(&mutex_lista_tablas);
	//puts("C");
	return rdo!=NULL;
}
void iniciar_consola(){
	sleep(1);
	loggear_trace(string_from_format("Se inicia consola"));
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
			loggear_debug(string_from_format("##### FINALIZANDO KERNEL.... ###### \n"));
			return NULL;
		}
		printf("Procesando instruccion:: %s \n",instruccion);
		procesar_instruccion_consola(instruccion);
	}

}

void iniciar_ejecutador_gossiping(){
	loggear_debug(string_from_format("Iniciando ejecutador gossiping"));
	pthread_t hilo_ejecutador_gossiping;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&hilo_ejecutador_gossiping,&attr, &ejecutar_gossiping, NULL);
	pthread_detach(hilo_ejecutador_gossiping);
	loggear_trace(string_from_format("Ejecutador gossiping iniciado"));
}

void *ejecutar_gossiping()
{
	fd_out_inicial = 0;
	while(1)
	{
		ejecutar_instruccion_gossip();
		loggear_debug(string_from_format("Fin gossiping programado"));
		sem_wait(&mutex_configuracion);
		int retardoGossiping = configuracion.RETARDO_GOSSIPING;
		sem_post(&mutex_configuracion);
		usleep(retardoGossiping * 1000);
	}
}
void auto_describe(){
	while(1){
		sem_wait(&mutex_configuracion);
		int tiempo=configuracion.metadata_refresh;
		sem_post(&mutex_configuracion);
		usleep(tiempo*1000);
		loggear_info(string_from_format("Realizando autodescribe"));
		instr_t* req=leer_a_instruccion("DESCRIBE",CONSOLA_KERNEL);
		req->parametros=create_list();
		int cod=obtener_codigo_request();
		char*  codigo=malloc(sizeof(char)*(cod/10+1)+1);
		sprintf(codigo,"%d",cod);
		list_add(req->parametros,codigo);
		req->codigo_operacion=CODIGO_DESCRIBE;
		instr_t* rta= enviar_i(req);

		actualizar_metadata_tablas(rta);
		liberar_instruccion(req);
		liberar_instruccion(rta);
	}
}
void devolver_gossip(instr_t *instruccion, char *remitente){
	loggear_info(string_from_format("Devolviendo el gossip a %s", remitente));
//	loggear_trace(string_from_format("Enviando datos a %s", remitente));
	int conexionRemitente = obtener_fd_out(remitente);
//	loggear_trace(string_from_format("Datos enviados"));
	sem_wait(&mutex_diccionario_conexiones);
	t_list* tablaGossiping = conexiones_para_gossiping();
	sem_post(&mutex_diccionario_conexiones);
	instr_t* miInstruccion = crear_instruccion(obtener_ts(), RECEPCION_GOSSIP, tablaGossiping);


	loggear_debug(string_from_format("Envio mi tabla de datos al que me arranco el gossiping"));
	enviar_request(miInstruccion, conexionRemitente);
//	loggear_trace(string_from_format("Envio mi tabla de datos enviada"));


	actualizar_tabla_gossiping(instruccion);

}

void actualizar_tabla_gossiping(instr_t* instruccion){

	loggear_debug(string_from_format("Actualizando tabla de gossiping"));

	int saltearProximos = 0;
	int i = 0;
	char* nombre;
	char* ip;
	char* puerto;

	void acutalizar_tabla(char* parametro){
		saltearProximos--;
		//loggear_trace(string_from_format("Saltear Proximos = %d\n", saltearProximos));
		if(saltearProximos <= 0){
			if(i % 3 == 0){
				sem_wait(&mutex_diccionario_conexiones);
				if(strcmp("Kernel", parametro)!=0 && !dictionary_has_key(conexionesActuales, parametro)){
					sem_post(&mutex_diccionario_conexiones);
					nombre = strdup(parametro);
					loggear_info(string_from_format("No conocia al proceso %s", parametro));
					i++;
					saltearProximos = 0;
				}
				else{
					sem_post(&mutex_diccionario_conexiones);
					loggear_trace(string_from_format("Ya tenia el proceso %s en la tabla", parametro));
					i+=3;
					saltearProximos = 3;
				}
			}
				else if(i % 3 == 1){
					ip = strdup(parametro);
//					loggear_trace(string_from_format("Su ip es %s", ip));
					i++;
				}
					else if(i % 3 == 2){
						puerto = strdup(parametro);
//						loggear_trace(string_from_format("Su puerto es %s", puerto));
						identificador identificadores = {
								.fd_out = 0,
								.fd_in = 0
						};

						strcpy(identificadores.ip_proceso, ip);
						strcpy(identificadores.puerto, puerto);

						identificador* idsConexionesActuales = malloc(sizeof(identificadores));
						memcpy(idsConexionesActuales, &identificadores, sizeof(identificadores));

						loggear_trace(string_from_format("Se agrego al proceso %s al diccionario de conexiones conocidas", nombre));
						sem_wait(&mutex_diccionario_conexiones);
						dictionary_put(conexionesActuales,nombre, idsConexionesActuales);
						sem_post(&mutex_diccionario_conexiones);
						sem_wait(&mutex_configuracion);
						list_add(configuracion.IP_SEEDS, ip);
						list_add(configuracion.PUERTO_SEEDS, puerto);
						sem_post(&mutex_configuracion);
						i++;
					}
		}
		free(parametro);
	}

	list_iterate(instruccion->parametros, (void*)acutalizar_tabla);

	loggear_info(string_from_format("Tabla actualizada:"));
	sem_wait(&mutex_diccionario_conexiones);
	imprimir_conexiones(conexionesActuales, loggear_info);
	sem_post(&mutex_diccionario_conexiones);

	list_destroy(instruccion->parametros);
	free(instruccion);
//	imprimir_config_actual();
}

t_list *conexiones_para_gossiping(){

	t_list *tablaGossiping = list_create();

	void juntar_ip_y_puerto(char* nombre, identificador* ids){
		if(strcmp(nombre, "Kernel")!=0){
			//loggear_trace(string_from_format("Agregando nombre %s", nombre));
			list_add(tablaGossiping, nombre);
			//loggear_trace(string_from_format("Agregando IP %s", ids->ip_proceso));
			list_add(tablaGossiping, ids->ip_proceso);
			//loggear_trace(string_from_format("Agregando puerto %s", ids->puerto));
			list_add(tablaGossiping, ids->puerto);
		}
	}

	dictionary_iterator(conexionesActuales, (void *)juntar_ip_y_puerto);

	return tablaGossiping;

}
void enviar_lista_gossiping(char* nombreProceso){
	if(strcmp(nombreProceso, "Kernel")!=0){
		int conexionVieja = obtener_fd_out_sin_diccionario(nombreProceso);
		loggear_debug(string_from_format("Enviando lista de Gossiping a %s", nombreProceso));
		t_list* listaGossiping = conexiones_para_gossiping();
		instr_t* miInstruccion = crear_instruccion(obtener_ts(), PETICION_GOSSIP, listaGossiping);
		enviar_request(miInstruccion, conexionVieja);
	}
}

void ejecutar_instruccion_gossip(){

	loggear_info(string_from_format("Ejecutando instruccion Gossip"));

	gossipear_con_conexiones_actuales();

	gossipear_con_procesos_desconectados();

}

void gossipear_con_conexiones_actuales(){

	loggear_trace(string_from_format("Gossipeando conexiones actuales"));
	void su_nombre(char* nombre, identificador* ids){
		enviar_lista_gossiping(nombre);
	}
	sem_wait(&mutex_diccionario_conexiones);
	dictionary_iterator(conexionesActuales, (void *)su_nombre);
	sem_post(&mutex_diccionario_conexiones);
}



void gossipear_con_procesos_desconectados(){

	int i = 0;
	void enviar_tabla_gossiping(char* unaIP){
		sem_post(&mutex_configuracion);
		loggear_trace(string_from_format("IP seed: %s\n", unaIP));
		loggear_trace(string_from_format("Puerto seed: %s\n", (char*)list_get(configuracion.PUERTO_SEEDS,i)));
		sem_wait(&mutex_configuracion);
		char* nombreProceso = nombre_para_ip_y_puerto(unaIP, (char*)list_get(configuracion.PUERTO_SEEDS,i));
		sem_post(&mutex_configuracion);
		if(nombreProceso == NULL || ((identificador*)dictionary_get(conexionesActuales, nombreProceso))->fd_out==0)
		{
			loggear_trace(string_from_format("El IP %s y Puerto %s no estaban en las conexiones conocidas. Creando conexion", unaIP, (char*)list_get(configuracion.PUERTO_SEEDS,i)));
			sem_wait(&mutex_configuracion);
			int conexion = crear_conexion(unaIP, (char*)list_get(configuracion.PUERTO_SEEDS,i), miIPKernel, 0);
			sem_post(&mutex_configuracion);
			if(conexion != -1){
//				puts("Conexion creada");
				loggear_trace(string_from_format("conexion creada"));
				fd_out_inicial = conexion;
				instr_t * miInstruccion = mis_datos(CODIGO_HANDSHAKE);
				enviar_request(miInstruccion, conexion);
				instr_t * peticionDeSuTabla = mis_datos(PETICION_GOSSIP);
				enviar_request(peticionDeSuTabla, conexion);
			}
		}
		i++;
		sem_wait(&mutex_configuracion);
	}
	sem_wait(&mutex_configuracion);
	list_iterate(configuracion.IP_SEEDS, (void*)enviar_tabla_gossiping);
	sem_post(&mutex_configuracion);
}



bool contiene_IP_y_puerto(identificador *ids, char *ipBuscado, char *puertoBuscado){
	return (strcmp(ids->ip_proceso, ipBuscado)==0) && (strcmp(ids->puerto, puertoBuscado)==0);
}

char* nombre_para_ip_y_puerto(char *ipBuscado, char* puertoBuscado){

	char* nombreEncontrado = NULL;
	void su_nombre(char* nombre, identificador* ids){
//		loggear_trace(string_from_format("Buscando nombre para ip %s y puerto %s\n", ipBuscado, puertoBuscado));
		if(contiene_IP_y_puerto(ids, ipBuscado, puertoBuscado)){
			nombreEncontrado = strdup(nombre);
//			loggear_trace(string_from_format("Nombre encontrado! : %s\n", nombreEncontrado));
		}
	}
	sem_wait(&mutex_diccionario_conexiones);
	dictionary_iterator(conexionesActuales, (void *)su_nombre);
	sem_post(&mutex_diccionario_conexiones);
	return nombreEncontrado;
}

void imprimir_config_actual(){

	char *texto = string_new();
	string_append_with_format(&texto, "Config actual:\n");
	
	int i=0;
	void imprimir(char* valor){
		string_append_with_format(&texto,"IP: %s\n", valor);
		string_append_with_format(&texto,"Puerto: %s\n", (char*)list_get(configuracion.PUERTO_SEEDS, i));
		i++;
	}
	list_iterate(configuracion.IP_SEEDS, (void*)imprimir);
	printf("%s",texto); //No log xq lo usamos al levantar el config
	free(texto);
}

instr_t* mis_datos(cod_op codigoOperacion){
	t_list *listaParam = list_create();
	list_add(listaParam, "Kernel");
	list_add(listaParam, miIPKernel);
	sem_wait(&mutex_configuracion);
	list_add(listaParam, configuracion.puerto);
	sem_post(&mutex_configuracion);
	return crear_instruccion(obtener_ts(), codigoOperacion, listaParam);
}

void actualizar_config(){
	t_config *auxConfig;
	puts("llego aca");
	while((auxConfig = config_create(rutaConfiguracion)) == NULL||
			!config_has_property(auxConfig, "quantum") 			||
			!config_has_property(auxConfig, "RETARDO_GOSSIPING")||
			!config_has_property(auxConfig, "tiempoMetricas")	||
			!config_has_property(auxConfig, "LOG_LEVEL")){
		config_destroy(auxConfig);
		puts("Reintentandooooo");
	}

	puts("Va a tomar el semaforo");
	sem_wait(&mutex_configuracion);
	puts("Tomo el semaforo");
	configuracion.quantum = config_get_int_value(auxConfig, "quantum");
	configuracion.RETARDO_GOSSIPING = config_get_int_value(auxConfig, "RETARDO_GOSSIPING");
	configuracion.tiempoMetricas = config_get_int_value(auxConfig, "tiempoMetricas");
	configuracion.LOG_LEVEL =  log_level_from_string(config_get_string_value(auxConfig, "LOG_LEVEL"));
	configuracion.sleep_ejecucion = config_get_int_value(auxConfig, "SLEEP_EJECUCION");
	configuracion.metadata_refresh = config_get_int_value(auxConfig, "METADATA_REFRESH");
	sem_wait(&mutex_log);
	actualizar_log_level();
	sem_post(&mutex_log);
	loggear_info(string_from_format(
			"Config actualizado!\n"
			"Quantum: %d\n"
			"Tiempo de Gossiping: %d\n"
			"Tiempo de Metricas: %d\n"
			"Log level: %s",
			configuracion.quantum, configuracion.RETARDO_GOSSIPING, configuracion.tiempoMetricas , log_level_as_string(configuracion.LOG_LEVEL)));
	sem_post(&mutex_configuracion);
	config_destroy(auxConfig);
	printf("\n"COLOR_ANSI_MAGENTA ">" COLOR_ANSI_RESET);
	fflush(stdout);
}

void actualizar_log_level(){
	log_destroy(g_logger);
	g_logger = log_create(configuracion.rutaLog,"kernel", 1, configuracion.LOG_LEVEL);
}


void actualizar_metadata_tablas(instr_t* respuesta){

	int i = 0;
	int ultimo = list_size(respuesta->parametros) - 2;


	void agregar_metadata_tablas(char* valor){

		if(i%4 ==0 && i < ultimo){
			char* tabla = string_from_format(valor); //Así se le puede hacer free a respuesta->parametros
			agregar_tabla(tabla);
			char* criterio = string_from_format("%s", list_get(respuesta->parametros, i+1));
			agregar_tabla_a_su_criterio(valor, criterio);
			printf("\n\nSe agrego la tabla %s con el criterio %s", valor, criterio);
			free(criterio);
		}
		i++;
	}

	list_iterate(respuesta->parametros, (void*) agregar_metadata_tablas);
}

void agregar_tabla_a_su_criterio(char* tabla, char* criterio){
	int* codigo_criterio=malloc(sizeof(int));
	*codigo_criterio=obtener_codigo_criterio(criterio);
	sem_wait(&mutex_diccionario_criterios);
	dictionary_put(diccionario_criterios,tabla ,codigo_criterio);
	sem_post(&mutex_diccionario_criterios);
}
