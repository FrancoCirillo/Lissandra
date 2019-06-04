/*MEMTABLE*/

#include "memtable.h"

//Nada de esto esta probado.

void inicializar_memtable() {
	memtable = dictionary_create();
	loggear_FS("Se inicializó la memtable.");
}

void finalizar_memtable() { //Borra y libera todos los registros y tablas.
	//IMPORTANTE: la variable global no se puede usar a menos que hagan otro diccionary_create
	dictionary_destroy_and_destroy_elements(memtable, &borrar_lista_registros);;
}

void borrar_lista_registros(void* lista){
	list_destroy_and_destroy_elements((t_list*)lista, &borrar_registro);
}

void borrar_registro(void* registro){ //no tengo ni idea de lo que estoy haciendo
	free(((registro_t*)registro)->value);
	free((registro_t*)registro);
}

void limpiar_memtable(){
	dictionary_clean_and_destroy_elements(memtable, &borrar_lista_registros);
}

int existe_tabla(char* tabla){
	return dictionary_has_key(memtable, tabla);
}

void eliminar_tabla_de_mem(char* tabla){
	dictionary_remove_and_destroy(memtable, tabla, &borrar_lista_registros);
}

t_list* crear_lista_registros(){
	return list_create();
}

void agregar_tabla(char* tabla){
	t_list* registros = crear_lista_registros();
	dictionary_put(memtable, tabla, registros);
}

void agregar_registro(char* tabla, registro_t* registro){
	t_list* registros_tabla = dictionary_get(memtable, tabla);
	list_add(registros_tabla, registro);
	loggear_FS("Se inserto el registro en la memtable.");
}

t_list* obtener_registros_mem(char* tabla, uint16_t key){
	t_list* registros_tabla = dictionary_get(memtable, tabla);

	_Bool es_key_registro(void* registro){
		uint16_t key_registro = ((registro_t*)registro)->key;
		return key_registro == key;
	}

	return list_filter(registros_tabla, &es_key_registro);
}

//TODO borrar cuando se resuelva
registro_t* obtener_registro_mas_reciente(t_list* registros_de_key){
	list_sort(registros_de_key, &es_registro_mas_reciente);
	return list_get(registros_de_key, 0);
}

_Bool es_registro_mas_reciente(void* un_registro, void* otro_registro){
	mseg_t ts_un_registro = ((registro_t*)un_registro)->timestamp;
	mseg_t ts_otro_registro = ((registro_t*)otro_registro)->timestamp;
	return (_Bool)(ts_un_registro > ts_otro_registro);
}

registro_t* pasar_a_registro(instr_t* instr){
	registro_t* registro = malloc(sizeof(registro_t));
	registro->key = (uint16_t) atoi(obtener_parametro(instr, 1));
	registro->value = obtener_parametro(instr, 2); //tengo que hacer algun malloc?
	registro->timestamp = instr->timestamp; //se supone que la instr siempre tiene ts y nos abstrae de validar si el ts viene en los parametros
	return registro;
}



void dumpear_tabla(char* tabla, void* registros){
	crear_tmp(tabla);
	void escribir_reg_en_tmp(void* registro){
		int nro_bloque = siguiente_bloque_disponible();
		char* ruta_bloque = obtener_ruta_bloque(nro_bloque);
		//escribir nro de bloque en tmp
		escribir_registro_bloque((registro_t*)registro, ruta_bloque); //este warnings es porque esta comentado en estructuras.h
	}

	list_iterate((t_list*)registros, &escribir_reg_en_tmp);
}

void dumpear(t_dictionary* mem) { //tengo otra forma de hacerlo, si esta no gusta
	dictionary_iterator(memtable, &dumpear_tabla);

	//de cada tabla de esa mem, que no es la mem que sigue usando, bajar a los .tmp correspondientes.
	//Ver bien como hacer esto..
}

void dumpeo() {
	sem_wait(&mutex_tiempo_dump_config);
	int tiempo_dump = obtener_tiempo_dump_config();
	sem_post(&mutex_tiempo_dump_config);

	t_dictionary* mem;

	while (1) {
		sleep(tiempo_dump);

		mem = memtable;

		dumpear(mem); //tiene que dumpear antes de limpiar

		sem_wait(&mutex_memtable);
		limpiar_memtable();  //la deja como nueva. sin tablas.
		sem_post(&mutex_memtable);

		//Ver si aca hay que crear hilo, o espera a que termina el dumpeo de las tablas.
		//Preguntar en el foro si se hacen dumpeos paralelos.
		//dumpear(mem);

		sem_wait(&mutex_tiempo_dump_config);
		tiempo_dump = obtener_tiempo_dump_config();	//Primero guardar variables y despues bloquearlas, y usarlas.
		sem_post(&mutex_tiempo_dump_config);
	}
}

//esto no compila.
	//TODO obtener_nodo_tabla --> obtiene el nodo de la tabla a la que le quiero agregar un registro
	//mem_tabla_t* nodo_tabla = mem_obtener_nodo_tabla(tabla);

	//list_add(nodo_tabla->registros, nuevo_reg);
	//loggear_FS("Se inserto el registro en la memtable.");
	//Ver si agregamos mas info del registro en el log.
//}

//Lo que falta es mem_obtener_nodo_tabla().
//mem_tabla_t* mem_obtener_nodo_tabla(char*){
//	int i = memtable->elements_count;
//	int j=0;
//	int signal =;
//	while(j <= i && )
//
//	return NULL;
//}


void pasar_a_archivo(char* tabla, t_list* registros, char* ext) {
	if (!strcmp(ext, "tmp")) {
		char* n = string_itoa(obtener_num_sig_dumpeo(tabla));
		n = concat("Dump_", n);
		FILE * f = crear_archivo(n, tabla, ext);
		//escribir(f,registros);
		//Escribir tiene que hacer el fclose(f);
		//hacer eso cuando lo implementemos.
		fclose(f);
		free(n);
	} else if (!strcmp(ext, "tmpc")) { //ver si sirve de algo hacerlo polimorfico asi.
		//
	} else {
		//ver en compactacion como podemos reutilizar esto..
	}
}

//void bajar_tabla(mem_tabla_t* t) {
//	//char* i = string_itoa(obtener_num_sig_dumpeo(t->tabla));
//	//i = contat(i, ".tmp");
//	pasar_a_archivo(t->tabla, t->registros, "tmp");
//}

void escribir(FILE* f, t_list* algo){
	//no se que hace
}

int obtener_num_sig_dumpeo(char* tabla) {
	//busqueda ultimo dump.tmp en tabla.
	return 1;
}




