//---------kernel.c---------

#include "Kernel.h"
char * codigo_envio;
t_list* lista_tablas;
sem_t mutex_lista_tablas;
void agregar_tabla(char* tabla){

	sem_wait(&mutex_lista_tablas);
	list_add(lista_tablas,tabla);
	sem_post(&mutex_lista_tablas);
	loggear("Tabla agregada");
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

int main(int argc, char* argv[]) {



	inicializar_semaforos();

	inicializarConfiguracion();

	iniciar_log();

	check_inicial(argc, argv);

	inicializar_criterios();

	iniciar_ejecutador();

	inicializar_kernel();
	//iniciar_consola();

	loggear("### FINALIZANDO KERNEL ###");
	sleep(2);
	loggear("### KERNEL FINALIZADO ###");
	return 0;

	//	/*sleep(6);
	//	recibi_respuesta_fake1();
	//	//iniciar_consola();
	//	loggear("Recibi respuesta fake1 ejecutado!");
	//	sleep(6);
	//	recibi_respuesta_fake2();
	//	loggear("Recibi respuesta fake2 ejecutado!");

}


void inicializar_kernel(){
	conexionesActuales = dictionary_create();
	callback = ejecutar_requestRecibido;

	conectar_nueva_memoria(configuracion.MEMORIA_1_IP, configuracion.PUERTO_MEMORIA, "Memoria_1");
	conectar_nueva_memoria(configuracion.MEMORIA_8_IP, configuracion.PUERTO_MEMORIA, "Memoria_3");


	int listenner = iniciar_servidor(miIPKernel, configuracion.puerto, g_logger, &mutex_log);
	vigilar_conexiones_entrantes(listenner, callback, conexionesActuales, CONSOLA_KERNEL, g_logger, &mutex_log);

}

void conectar_nueva_memoria(char* IPMemoria, char* PuertoMemoria, char* NombreMemoria){
	t_list *listaParam = list_create();
	list_add(listaParam, "Kernel");
	list_add(listaParam, miIPKernel);
	list_add(listaParam, configuracion.puerto);
	instr_t *miInstruccion = crear_instruccion(obtener_ts(), CODIGO_HANDSHAKE, listaParam);


	int conexion_con_memoria_3 = crear_conexion(IPMemoria, PuertoMemoria, miIPKernel, g_logger, &mutex_log);

	enviar_request(miInstruccion, conexion_con_memoria_3);

	identificador *idsNuevasConexiones = malloc(sizeof(identificador));
	idsNuevasConexiones->fd_in = 0; //Por las moscas
	strcpy(idsNuevasConexiones->puerto, PuertoMemoria);
	strcpy(idsNuevasConexiones->ip_proceso, IPMemoria);
	idsNuevasConexiones->fd_out = conexion_con_memoria_3;

	sem_wait(&mutex_conexiones_actuales);
	dictionary_put(conexionesActuales, NombreMemoria, idsNuevasConexiones);
	sem_post(&mutex_conexiones_actuales);

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
		char*  codigo=malloc(sizeof(char)*12);
		sprintf(codigo,"%d",obtener_codigo_request());
		list_add(instruccion->parametros,codigo);

		p->size++;
		loggear("Instruccion generada, encolando proceso...");
		encolar_proceso(p);
	}else{//Es una respuesta
		recibi_respuesta(instruccion);
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
	char*  codigo=malloc(sizeof(char)*12);
	sprintf(codigo,"%d",obtener_codigo_request());
	list_add(nueva_instruccion->parametros,codigo);

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
instr_t* kernel_run(instr_t *i){
	loggear("EJECUTANDO RUN!");
	char* nombre_archivo=obtener_parametroN(i,0);
	FILE *f=fopen(nombre_archivo,"r");
	char line[64];
	t_list* instrucciones = list_create();
	proceso* p=malloc(sizeof(proceso));
	p->current=0;
	p->size=0;
	p->instrucciones=instrucciones;
	p->sig=NULL;
	instr_t* nueva_instruccion;
	//loggear("Se ejecuto RUN, leyendo archivo!");

	while(fgets(line,sizeof(line),f)){
		nueva_instruccion=leer_a_instruccion(line,CONSOLA_KERNEL);
		char*  codigo=malloc(sizeof(char)*12);
		sprintf(codigo,"%d",obtener_codigo_request());
		//		printf("EL codigo es %s",codigo);

		list_add(nueva_instruccion->parametros,codigo);

		list_add(p->instrucciones,nueva_instruccion);
		p->size++;
		loggear("Se agrego una instruccion!");
		//		imprimir_instruccion(nueva_instruccion);
	}
	printf("\n\nSize %d\n\n",p->size);
	fclose(f);

	encolar_proceso(p);
	//RESPUESTA
	instr_t * respuesta=malloc(sizeof(instr_t));
	respuesta->codigo_operacion=0;
	char* mensaje="RUN EJECUTADO CORRECTAMENTE!";
	t_list * params=list_create();
	list_add(params,mensaje);
	respuesta->parametros=params;
	respuesta->timestamp=i->timestamp;
	loggear("FIN RUN!");
	return respuesta;

}
instr_t* kernel_add(instr_t* i){
	char* numero_memoria=obtener_parametroN(i,1);
	int codigo=obtener_codigo_criterio(obtener_parametroN(i,3));
	loggear("EJECUTANDO ADD PARA MEMORIA:");
	loggear(numero_memoria);
	switch(codigo){
	case SC:
		loggear("Agregando memoria a criterio SC");
		sem_wait(&criterio_strong_consistency->mutex_criterio);
		if(list_size(criterio_strong_consistency->lista_memorias)>0){
			instr_t * respuesta=malloc(sizeof(instr_t));
			respuesta->codigo_operacion=1007;
			char* mensaje="ERROR EN ADD, EL CRITERIO SC YA POSEE UNA MEMORIA ASIGNADA!";
			t_list * params=list_create();
			list_add(params,mensaje);
			respuesta->parametros=params;
			respuesta->timestamp=i->timestamp;
			loggear("FIN ADD!");
			sem_post(&criterio_strong_consistency->mutex_criterio);
			return respuesta;
		}
		list_add(criterio_strong_consistency->lista_memorias,numero_memoria);
		sem_post(&criterio_strong_consistency->mutex_criterio);
		break;

	case SHC:
		loggear("Agregando memoria a criterio SHC");
		sem_wait(&criterio_strong_hash_consistency->mutex_criterio);
		list_add(criterio_strong_hash_consistency->lista_memorias,numero_memoria);
		sem_post(&criterio_strong_hash_consistency->mutex_criterio);
		break;

	case EC:
		loggear("Agregando memoria a criterio EC");
		sem_wait(&criterio_eventual_consistency->mutex_criterio);
		list_add(criterio_eventual_consistency->lista_memorias,numero_memoria);
		sem_post(&criterio_eventual_consistency->mutex_criterio);
		break;
	}

	instr_t * respuesta=malloc(sizeof(instr_t));
	respuesta->codigo_operacion=0;
	char* mensaje="ADD EJECUTADO CORRECTAMENTE!";
	t_list * params=list_create();
	list_add(params,mensaje);
	respuesta->parametros=params;
	respuesta->timestamp=i->timestamp;
	loggear("FIN ADD!");

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
	loggear("Enviando senial en 1 segundo");
	//	sleep(1);
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

			//METRICS


			if(!respuesta->codigo_operacion){//Codigo 0-> OK, Codigo !=0 Error

				loggear("Se ejecuto correctamente la instruccion!, Respuesta=");
				loggear(obtener_parametroN(respuesta,0));
				loggear("Fin de instruccion");
			}else{

				loggear("ERROR al ejecutar la instruccion, finalizando proceso");
				//printf("\n\n %d MENSAJE=%s",respuesta->codigo_operacion,obtener_parametroN(respuesta,0));
				bajar_cantidad_hilos();
				imprimir_instruccion(respuesta);
				return NULL;
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
	instr_t* respuesta;
	loggear("#### Se ejecuta una instruccion");
	imprimir_instruccion(i);
	if(i->codigo_operacion==CODIGO_RUN){
		return kernel_run(i);
	}
	if(i->codigo_operacion==CODIGO_ADD){
		return kernel_add(i);
	}if(i->codigo_operacion==CODIGO_CREATE){
		agregar_tabla_a_criterio(i);
	}
	if((respuesta=validar(i))!=NULL){
		return respuesta;
	}
	if(i->codigo_operacion==CODIGO_CREATE){
		if(!existe_tabla(obtener_parametroN(i,0))){
			loggear("Agregando tabla a lista");
			loggear(obtener_parametroN(i,0));
			agregar_tabla(obtener_parametroN(i,0));
		}
	}
	respuesta=enviar_i(i);
	return respuesta;
}
instr_t *validar(instr_t * i){
	instr_t* mensaje_error=malloc(sizeof(instr_t));
	mensaje_error->timestamp=i->timestamp;
	char* mensaje="ERROR!";
	if(i->codigo_operacion!=2&&i->codigo_operacion!=3){
		return NULL;
	}
	if(i->codigo_operacion==2){
		if(existe_tabla(obtener_parametroN(i,0))){
			loggear("Existe la tabla para el insert ");
			return NULL;
		}else{
			mensaje_error->codigo_operacion=ERROR_INSERT;
			printf("\nLa tabla %s no existe!\n",obtener_parametroN(i,0));
			//mensaje="LA TABLA NO EXISTE!";
		}
	}
	if(i->codigo_operacion==3){
		if(!existe_tabla(obtener_parametroN(i,0))){
			loggear("No existe tabla para el create");
			return NULL;
		}else{
			//mensaje="LA TABLA YA EXISTE";
			mensaje_error->codigo_operacion=ERROR_CREATE;
		}
	}
	mensaje_error->parametros=list_create();
	list_add(mensaje_error->parametros,mensaje);

	return mensaje_error;
}

instr_t* enviar_i(instr_t* i){

	printf(" ENVIANDO INSTRUCCION CODIGO %s\n",(char*)obtener_ultimo_parametro(i));
	//imprimir_instruccion(i);

	hilo_enviado* h=malloc(sizeof(hilo_enviado));
	pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
	h->cond_t=&cond;
	h->mutex_t=&lock;

	loggear("Agrego a diccionario");

	sem_wait(&mutex_diccionario_enviados);
	//printf("EL VALOR DEL CODIGO ES DE %s \n",(char *)obtener_ultimo_parametro(i));
	dictionary_put(diccionario_enviados, obtener_ultimo_parametro(i), h);
	sem_post(&mutex_diccionario_enviados);


	loggear("Determinando memoria para request");
	int conexionMemoria = obtener_fd_memoria(i);

	loggear("ENVIANDO INSTRUCCION:  ");
	enviar_request(i, conexionMemoria);


	printf("\n##### Instruccion enviada, esperando respuesta###\n");

	loggear("Hilo bloqueado hasta recibir respuesta!");
	pthread_mutex_lock(h->mutex_t);
	pthread_cond_wait(h->cond_t,h->mutex_t);
	pthread_mutex_unlock(h->mutex_t);

	loggear("Hilo despierto!");

	return h->respuesta;
}

int obtener_fd_memoria(instr_t *i){
	char* memoria="Memoria_";
	char* alias_memoria="";
	//SIEMPRE el primer parametro es el nombre de la tabla que uso como key para determinar criterio
	sem_wait(&mutex_diccionario_criterios);
	int codigo_criterio=*(int*)dictionary_get(diccionario_criterios,(char*)obtener_parametroN(i,0));
	sem_post(&mutex_diccionario_criterios);
	//Obtengo la memoria segun el criterio
	printf("Codigo de criterio es : %d \n",codigo_criterio);
	loggear("Determinando criterio de tabla");
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
	}//Ya obtuve el alias de la memoria

	loggear("Memoria obtenida:");
	//loggear(krn_concat(memoria,alias_memoria));

	return obtener_fd_out(krn_concat(memoria,alias_memoria));
}
char* krn_concat(char* s1,char* s2){
	char* rdo=(char*)malloc(1+strlen(s1)+strlen(s2));
	strcpy(rdo,s1);
	strcat(rdo,s2);
	loggear(rdo);
	return rdo;
}
void agregar_tabla_a_criterio(instr_t* i){//EN create
	loggear("Agregando tabla a criterio");
	int* codigo_criterio=malloc(sizeof(int));
	*codigo_criterio=obtener_codigo_criterio(obtener_parametroN(i,1));
	sem_wait(&mutex_diccionario_criterios);
	dictionary_put(diccionario_criterios,obtener_parametroN(i,0),codigo_criterio);
	sem_post(&mutex_diccionario_criterios);
	printf("Se agrega la tabla %s, al criterio codigo: %d\n\n",(char*)obtener_parametroN(i,0),*codigo_criterio);
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
	enviar_request(i,obtener_fd_out(destino));
}
int obtener_fd_out(char *proceso)
{
	identificador *idsProceso = (identificador *)dictionary_get(conexionesActuales, proceso);
	if (idsProceso->fd_out == 0)
	{ //Es la primera vez que se le quiere enviar algo a proceso
		responderHandshake(idsProceso);
	}
	return idsProceso->fd_out;
}
void responderHandshake(identificador *idsConexionEntrante)
{
	t_list *listaParam = list_create();
	list_add(listaParam, "Kernel");  //Tabla
	list_add(listaParam, miIPKernel); //Key
	list_add(listaParam, configuracion.puerto);
	instr_t *miInstruccion = miInstruccion = crear_instruccion(obtener_ts(), CODIGO_HANDSHAKE, listaParam);

	int fd_saliente = crear_conexion(idsConexionEntrante->ip_proceso, idsConexionEntrante->puerto, miIPKernel, g_logger, &mutex_log);
	enviar_request(miInstruccion, fd_saliente);
	idsConexionEntrante->fd_out = fd_saliente;
}
void recibi_respuesta(instr_t* respuesta){
	loggear("Instruccion recibida: ");
	imprimir_instruccion(respuesta);

	//	list_add(respuesta->parametros,"1");
	sem_wait(&mutex_diccionario_enviados);
	printf("El ultimo parametro de la instruccion es %s\n", (char*)obtener_ultimo_parametro(respuesta));
	hilo_enviado* h=dictionary_remove(diccionario_enviados,obtener_ultimo_parametro(respuesta));
	if(h!=NULL) loggear("Hilo obtenido y removido del diccionario!");
	else loggear("El hilo no existe");
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
		loggear("FIN DE PROCESO");
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
	configuracion.MEMORIA_1_IP = obtener_por_clave(rutaConfiguracion, "MEMORIA_1_IP");
	configuracion.MEMORIA_8_IP = obtener_por_clave(rutaConfiguracion, "MEMORIA_8_IP");
	configuracion.MEMORIA_9_IP = obtener_por_clave(rutaConfiguracion, "MEMORIA_9_IP");
	configuracion.PUERTO_MEMORIA = obtener_por_clave(rutaConfiguracion, "PUERTO_MEMORIA");

}
void actualizar_configuracion(){
	char* rutaConfiguracion = "Kernel.config";
	configuracion.quantum = atoi(obtener_por_clave(rutaConfiguracion, "quantum"));
	configuracion.gradoMultiprocesamiento = atoi(obtener_por_clave(rutaConfiguracion, "gradoMultiprocesamiento"));

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
	sem_init(&mutex_conexiones_actuales,0,1);
	sem_init(&mutex_diccionario_criterios,0,1);
	sem_init(&mutex_contador_ec,0,1);
	sem_init(&mutex_lista_tablas,0,1);
	diccionario_enviados=dictionary_create();
	diccionario_criterios=dictionary_create();
	lista_tablas=list_create();
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
	loggear("Criterios inicializados!");
}
/*void recibi_respuesta_fake1(){
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
}*/
/*
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
}*/

void check_inicial(int argc, char* argv[])
{
	if(argc>2){
		log_error(g_logger, "Uso: kernel <IP>, IP vacio => IP = 127.0.0.4");
		exit(0);
	}
	if(strlen(argv[1])<2){
		puts("IP 127.0.0.4");
		miIPKernel = IP_KERNEL;
	}
	else if(argc==2){
		printf("IP %s\n", argv[1]);
		miIPKernel = argv[1];
	}
}
