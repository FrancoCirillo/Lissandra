//--------memtable.c--------

#include "memtable.h"

void inicializar_memtable() {
	memtable = dictionary_create();
	DIR* directorio = opendir(g_ruta.tablas);
	if (directorio != NULL){
		levantar_tablas_directorio(directorio);
		loggear_info(string_from_format("Se inicializó la memtable."));
	}
}

void finalizar_memtable() { //Borra y libera todos los registros y tablas.
	//IMPORTANTE: la variable global no se puede usar a menos que hagan otro diccionary_create
	dictionary_destroy_and_destroy_elements(memtable, &borrar_registros);
}

void levantar_tablas_directorio(DIR* directorio) {
	loggear_debug(string_from_format("Levantando tablas del directorio"));
	struct dirent* directorio_leido;
	while((directorio_leido = readdir(directorio)) != NULL) {
		char* tabla = directorio_leido->d_name;
		if(!string_contains(tabla, ".")) {
			t_list* registros = crear_lista_registros();
			dictionary_put(memtable, tabla, registros);
			inicializar_semaforo_tabla(tabla);
			agregar_a_contador_dumpeo(tabla);
		}
	}
	closedir(directorio);
}

int existe_tabla_en_mem(char* tabla) {
	return dictionary_has_key(memtable, tabla);
}

char* obtener_ruta_tabla(char* tabla) {
	return string_from_format("%s%s", g_ruta.tablas, tabla);
}

t_list* crear_lista_registros() {
	return list_create();
}

void borrar_lista_registros(void* lista_registros) {
	list_destroy((t_list*)lista_registros);
}

void eliminar_tabla_de_mem(char* tabla){
	dictionary_remove_and_destroy(memtable, tabla, &borrar_registros);
}

void borrar_registros(void* registros) {
	list_destroy_and_destroy_elements((t_list*)registros, &borrar_registro);
}

void borrar_registro(void* registro){
	free(((registro_t*)registro)->value);
	free((registro_t*)registro);
}

void limpiar_memtable() {
	dictionary_iterator(memtable, &limpiar_registros);
}

void limpiar_registros(char* tabla, void* registros) {
	list_destroy_and_destroy_elements((t_list*)registros, &borrar_registro);
	agregar_tabla_a_mem(tabla);
}

void agregar_tabla_a_mem(char* tabla) {
	t_list* registros = crear_lista_registros();
	dictionary_put(memtable, tabla, registros);
	loggear_info(string_from_format("Se agrego la tabla en la memtable."));
}

void agregar_registro(char* tabla, registro_t* registro) {
	t_list* registros_tabla = dictionary_get(memtable, tabla);
	list_add(registros_tabla, registro);
	loggear_debug(string_from_format("Se inserto el registro en la memtable."));
}

t_list* obtener_registros_mem(char* tabla, uint16_t key) {

	loggear_trace(string_from_format("---Buscando en la memtable---"));
	t_list* registros_tabla = dictionary_get(memtable, tabla);

	_Bool es_key_registro(void* registro){
		uint16_t key_registro = ((registro_t*)registro)->key;
		return key_registro == key;
	}
	t_list* registros = list_filter(registros_tabla, &es_key_registro);
	loggear_trace(string_from_format("Tam de lista mem: %d\n", list_size(registros)));
	return registros;
}

registro_t* obtener_registro(char* buffer) {

	char* bufferCopy = strdup(buffer);
	registro_t* registro = malloc(sizeof(registro_t));
	char* valor;

	char* actual = strtok(bufferCopy, ";");
	valor = strdup(actual);
	registro->timestamp = string_a_mseg(valor);
	free(valor);   //IMPORTANTE agrego este free

	actual = strtok(NULL, ";");
	valor = strdup(actual);
	registro->key = (uint16_t)atoi(valor);
	free(valor);	//IMPORTANTE agrego este free


	actual = strtok(NULL, "\n");
	valor = strdup(actual);
	registro->value = valor;

	free(bufferCopy);
	return registro;
}

registro_t* pasar_a_registro(instr_t* instr) {
	registro_t* registro = malloc(sizeof(registro_t));
	registro->key = (uint16_t) atoi(obtener_parametro(instr, 1));
	registro->value = obtener_parametro(instr, 2); //tengo que hacer algun malloc?
	registro->timestamp = instr->timestamp;
	return registro;
}

//DUMPEO
void iniciar_dumpeo() {
	pthread_t hilo_dumpeo;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&hilo_dumpeo, &attr, &dumpeo, NULL);
	pthread_detach(hilo_dumpeo);
	loggear_trace(string_from_format("Dumpeo iniciado"));
}

void* dumpeo() {
	sem_wait(&mutex_tiempo_dump_config);
	int tiempo_dump = obtener_tiempo_dump_config();
	sem_post(&mutex_tiempo_dump_config);

	while(1){
		sleep(tiempo_dump);
		loggear_debug(string_from_format("Realizando dumpeo programado."));

		dumpear_memtable();

		sem_wait(&mutex_memtable);
		limpiar_memtable();  //La deja como nueva. Sin tablas.
		sem_post(&mutex_memtable);

		sem_wait(&mutex_tiempo_dump_config);
		tiempo_dump = obtener_tiempo_dump_config();
		sem_post(&mutex_tiempo_dump_config);
	}
}

void dumpear_memtable() {
	dictionary_iterator(memtable, &dumpear_tabla);
}

void dumpear_tabla(char* tabla, void* registros) {

	sem_wait(&mutex_dic_semaforos);
	sem_t* mutex_tabla = obtener_mutex_tabla(tabla);
	sem_post(&mutex_dic_semaforos);

	sem_wait(mutex_tabla);

	if(!list_is_empty((t_list*)registros)) { //Si se hicieron inserts
		int nro_dump = siguiente_nro_dump(tabla);
		char* nombre_tmp = string_from_format("Dump%d", nro_dump);
		char* ruta_tmp = string_from_format("%s%s/%s.tmp", g_ruta.tablas, tabla, nombre_tmp);
		FILE* temporal = crear_archivo(tabla, nombre_tmp, ".tmp");

		loggear_trace(string_from_format("Creando temporal"));
		int nro_bloque = archivo_inicializar(temporal);
		loggear_trace(string_from_format("Inicializando temporal"));

		char* ruta_bloque;

		void escribir_reg_en_tmp(void* registro) {
			nro_bloque = obtener_ultimo_bloque(ruta_tmp);
			ruta_bloque = obtener_ruta_bloque(nro_bloque);
			escribir_registro_bloque((registro_t*)registro, ruta_bloque, ruta_tmp);
			free(ruta_bloque);
		}

		list_iterate((t_list*)registros, &escribir_reg_en_tmp);

		free(ruta_tmp);
		free(nombre_tmp);
		fclose(temporal);
	}

	sem_post(mutex_tabla);
}

void agregar_a_contador_dumpeo(char* nombre_tabla) {
	int* valor_inicial = malloc(sizeof(int));
	*valor_inicial = 0;
	sem_wait(&mutex_tablas_nro_dump);
	dictionary_put(tablas_nro_dump, nombre_tabla, valor_inicial);
	sem_post(&mutex_tablas_nro_dump);
}

void resetear_numero_dump(char* tabla) {
	int* rdo;
	sem_wait(&mutex_tablas_nro_dump);
	rdo = dictionary_get(tablas_nro_dump, tabla);
	(*rdo) = 0;
	sem_post(&mutex_tablas_nro_dump);
}

int siguiente_nro_dump(char* tabla) {
	int* rdo;
	sem_wait(&mutex_tablas_nro_dump);
	rdo = dictionary_get(tablas_nro_dump, tabla);
	(*rdo)++;
	sem_post(&mutex_tablas_nro_dump);

	return *rdo;
}

void eliminar_nro_dump_de_tabla(char* tabla) {
	sem_wait(&mutex_tablas_nro_dump);
	dictionary_remove_and_destroy(tablas_nro_dump, tabla, free);
	sem_post(&mutex_tablas_nro_dump);
}

void finalizar_tablas_nro_dump() {
	dictionary_destroy_and_destroy_elements(tablas_nro_dump, (void*)free);
}

