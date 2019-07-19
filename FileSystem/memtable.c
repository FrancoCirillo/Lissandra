//--------memtable.c--------

#include "memtable.h"

void inicializar_memtable() {
	memtable = dictionary_create();
	DIR* directorio = opendir(g_ruta.tablas);
	if(directorio ==NULL){
		closedir(directorio);
		loggear_info(string_from_format("No hay tablas en el File System."));
		return;
	}
	struct dirent directorio_leido, *directorio_leido_p;

	char* tabla;
	while(readdir_r(directorio, &directorio_leido, &directorio_leido_p) == 0 && directorio_leido_p != NULL){
		tabla = directorio_leido.d_name;
		if(!string_contains(tabla, ".")) {
			agregar_tabla_a_mem(tabla);
			inicializar_semaforo_tabla(tabla);
			agregar_a_contador_dumpeo(tabla);
			crear_hilo_compactador(tabla);
		}
	}
	loggear_info(string_from_format("Se inicializó la memtable."));
	cerrar_directorio(directorio);

}

void finalizar_memtable() {
	sem_wait(&mutex_memtable);
	dumpear_memtable(); //Esto limpia lo ultimo de la mem antes de cerrar el FS.

	dictionary_clean_and_destroy_elements(memtable, &borrar_registros); //Borra y libera todos los registros y tablas.
	//No se destruye el diccionario por la comprobacion de la compactacion.

	compactar_todas_las_tablas();
	//Esto compacta todos los .tmpc que hayan antes de cerrar el FS.

	dictionary_destroy_and_destroy_elements(memtable, free);
	sem_post(&mutex_memtable);
}

_Bool existe_tabla_en_FS(char* tabla){
	char* ruta_tabla = obtener_ruta_tabla(tabla);
	DIR* directorio = opendir(ruta_tabla);
	_Bool existe = directorio != NULL;
	closedir(directorio);
	free(ruta_tabla);
	return existe;
}

_Bool existe_tabla_en_mem(char* tabla) {
	return dictionary_has_key(memtable, tabla);
}

char* obtener_ruta_tabla(char* tabla) {
	return string_from_format("%s%s", g_ruta.tablas, tabla);
}

void borrar_lista_registros(void* lista_registros) {
	list_destroy((t_list*)lista_registros);
}

void eliminar_tabla_de_mem(char* tabla){
	dictionary_remove_and_destroy(memtable, tabla, &borrar_registros);
}

void borrar_registros(void* registros) {
	list_destroy_and_destroy_elements((t_list*)registros, free);
}

void limpiar_memtable() {
	dictionary_iterator(memtable, &limpiar_registros);
}
void liberar_trucho(registro_t* t){
	free(t);
}

void limpiar_registros(char* tabla, void* registros) {
	list_destroy_and_destroy_elements((t_list*)registros, (void*)liberar_trucho);
	agregar_tabla_a_mem(tabla);
}

void agregar_tabla_a_mem(char* tabla) {
	t_list* registros = list_create();
	dictionary_put(memtable, tabla, registros);
	//loggear_info(string_from_format("Se agrego la tabla en la memtable."));
}

void agregar_registro(char* tabla, registro_t* registro) {
	sem_wait(&mutex_memtable);
	t_list* registros_tabla = dictionary_get(memtable, tabla);
	list_add(registros_tabla, registro);
	sem_post(&mutex_memtable);
//	loggear_warning(string_from_format("Se inserto el registro en la memtable."));
//	loggear_error(string_from_format(registro_a_string(registro)));
}

t_list* obtener_registros_mem(char* tabla, uint16_t key) {

	loggear_trace(string_from_format("---Buscando en la memtable---"));

	sem_wait(&mutex_memtable);
	t_list* registros_tabla = dictionary_get(memtable, tabla);
	sem_post(&mutex_memtable);

	_Bool es_key_registro(void* registro){
		uint16_t key_registro = ((registro_t*)registro)->key;
		return key_registro == key;
	}
	t_list* registros = list_filter(registros_tabla, &es_key_registro);
	loggear_trace(string_from_format("Tam de lista mem: %d\n", list_size(registros)));
	return registros;
}

registro_t* obtener_registro(char* buffer) {

	registro_t* registro = malloc(sizeof(registro_t));
	char* copiaBuffer = string_duplicate(buffer);
	string_trim(&copiaBuffer);//Para sacarle el \n, si tiene
//	loggear_error(string_from_format("El buffer es %s", copiaBuffer));
	char** registroSeparado = string_split(copiaBuffer,";");
//	printf("El registro separado es %s, ", registroSeparado[0]);
//	printf("%s,", registroSeparado[1]);
//	printf("%s", registroSeparado[2]);
	sscanf(registroSeparado[0], "%" SCNu64, &(registro->timestamp));
	registro->key = (uint16_t)atoi(registroSeparado[1]);
	registro->value = string_from_format(registroSeparado[2]);

	free(registroSeparado[0]);
	free(registroSeparado[1]);
	free(registroSeparado);
	free(copiaBuffer);
	return registro;
}

registro_t* pasar_a_registro(instr_t* instr) {
	registro_t* registro = malloc(sizeof(registro_t));
	registro->key = (uint16_t) atoi(obtener_parametro(instr, 1));
	registro->value = string_from_format("%s", obtener_parametro(instr, 2));
	registro->timestamp = instr->timestamp;
	return registro;
}

char* registro_a_string(registro_t* registro) {
	char* ts = mseg_a_string(registro->timestamp);
	char* key = string_from_format("%d",registro->key);
	char* value = string_from_format("%s", registro->value);
	char* reg_string = string_from_format("%s;%s;%s\n", ts, key, value);
	free(ts);
	free(key);
	free(value);
//	loggear_warning(string_from_format("El registro que se va a escribir es %s", reg_string));
	return reg_string;
}

//DUMPEO
void iniciar_dumpeo() {
	pthread_t hilo_dumpeo;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&hilo_dumpeo, &attr, &dumpeo, NULL);
	pthread_detach(hilo_dumpeo);
	loggear_info(string_from_format("Dumpeo iniciado."));
}

void* dumpeo() {

	sem_wait(&mutex_config);
	mseg_t tiempo_dump = obtener_tiempo_dump_config();
	sem_post(&mutex_config);

	while(1){
		usleep(tiempo_dump);
		loggear_debug(string_from_format("Realizando dumpeo programado."));

		dumpear_memtable();

		sem_wait(&mutex_memtable);
		limpiar_memtable();  //La deja como nueva. Sin tablas.
		sem_post(&mutex_memtable);

		sem_wait(&mutex_config);
		tiempo_dump = obtener_tiempo_dump_config();
		sem_post(&mutex_config);
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
		loggear_info(string_from_format("Estoy dumpeando la tabla %s", tabla));
		int nro_dump = siguiente_nro_dump(tabla);
		char* ruta_tmp = string_from_format("%s%s/Dump%d.tmp", g_ruta.tablas, tabla, nro_dump);

		int nro_bloque = inicializar_archivo(ruta_tmp);
		loggear_trace(string_from_format("Temporal inicializado"));

		char* ruta_bloque;

		void escribir_reg_en_tmp(void* registro) {
			nro_bloque = obtener_ultimo_bloque(ruta_tmp);
			ruta_bloque = obtener_ruta_bloque(nro_bloque);
//			loggear_error(string_from_format("ESTOY DUMPLEANDO Y EN ESTE MOMENTO EL REGISTRO ES %s", registro_a_string((registro_t*)registro)));
			escribir_registro_bloque((registro_t*)registro, ruta_bloque, ruta_tmp);
			free(ruta_bloque);
		}

		list_iterate((t_list*)registros, &escribir_reg_en_tmp);

		free(ruta_tmp);
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

