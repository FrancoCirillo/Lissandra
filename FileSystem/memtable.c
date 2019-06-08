/*MEMTABLE*/

#include "memtable.h"


void inicializar_memtable() {
	memtable = dictionary_create();
	loggear_FS("Se inicializó la memtable.");
}

void finalizar_memtable() { //Borra y libera todos los registros y tablas.
	//IMPORTANTE: la variable global no se puede usar a menos que hagan otro diccionary_create
	dictionary_destroy_and_destroy_elements(memtable, &borrar_lista_registros);
}

//////////////////////////////



void borrar_lista_registros(void* registros) {
	list_destroy_and_destroy_elements((t_list*)registros, &borrar_registro);
}

void borrar_registro(void* registro){
	//free(((registro_t*)registro)->value);
	free((registro_t*)registro);
}

void borrar_registros(char* tabla, void* registros) {
	list_destroy_and_destroy_elements((t_list*)registros, &borrar_registro);
	agregar_tabla(tabla);
	//dictionary_put inserta un nuevo par (key->data) al diccionario
	//en caso de ya existir la key actualiza la data.
}

void limpiar_memtable() {
	dictionary_iterator(memtable, &borrar_registros);
}

int existe_tabla(char* tabla) {
	return dictionary_has_key(memtable, tabla);
}

void eliminar_tabla_de_mem(char* tabla){
	dictionary_remove_and_destroy(memtable, tabla, &borrar_lista_registros);
}

t_list* crear_lista_registros() {
	return list_create();
}

void agregar_tabla(char* tabla) {
	t_list* registros = crear_lista_registros();
	dictionary_put(memtable, tabla, registros);
	loggear_FS("Se agrego la tabla en la memtable.");
}

void agregar_registro(char* tabla, registro_t* registro) {
	t_list* registros_tabla = dictionary_get(memtable, tabla);
	list_add(registros_tabla, registro);
	loggear_FS("Se inserto el registro en la memtable.");
}

t_list* obtener_registros_mem(char* tabla, uint16_t key) {
	t_list* registros_tabla = dictionary_get(memtable, tabla);

	_Bool es_key_registro(void* registro){
		uint16_t key_registro = ((registro_t*)registro)->key;
		return key_registro == key;
	}

	return list_filter(registros_tabla, &es_key_registro);
}

//TODO borrar cuando se resuelva
//registro_t* obtener_registro_mas_reciente(t_list* registros_de_key) {
//	list_sort(registros_de_key, &es_registro_mas_reciente);
//	return list_get(registros_de_key, 0);
//}
//
//_Bool es_registro_mas_reciente(void* un_registro, void* otro_registro) {
//	mseg_t ts_un_registro = ((registro_t*)un_registro)->timestamp;
//	mseg_t ts_otro_registro = ((registro_t*)otro_registro)->timestamp;
//	return (_Bool)(ts_un_registro > ts_otro_registro);
//}
//
//registro_t* leer_binario(char* tabla, uint16_t key) { //TODO hacer
//	registro_t* unRegistro;
//	return unRegistro;
//}

registro_t* pasar_a_registro(instr_t* instr) {
	registro_t* registro = malloc(sizeof(registro_t));
	registro->key = (uint16_t) atoi(obtener_parametro(instr, 1));
	registro->value = obtener_parametro(instr, 2); //tengo que hacer algun malloc?
	registro->timestamp = instr->timestamp;
	return registro;
}

//NUMERO DE DUMP
void resetear_numero_dump(char* tabla) {
	int * rdo;
	sem_wait(&mutex_tablas_nro_dump);
	rdo=dictionary_get(tablas_nro_dump, tabla);
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

void agregar_a_contador_dumpeo(char* nombre_tabla) {//SE DEBE LLAMAR AL CREAR LA TABLA!
	int* valor_inicial = malloc(sizeof(int));
	*valor_inicial = 0;
	sem_wait(&mutex_tablas_nro_dump);
	dictionary_put(tablas_nro_dump,nombre_tabla,valor_inicial);
	sem_post(&mutex_tablas_nro_dump);
}

void ejemplo_nro_dump(){
	iniciar_semaforos();
	agregar_a_contador_dumpeo("Tabla1");
	agregar_a_contador_dumpeo("Tabla2");
	agregar_a_contador_dumpeo("Tabla53");
	printf("El siguiente numero de dump para la tabla1 es %d\n",siguiente_nro_dump("Tabla1"));
	printf("El siguiente numero de dump para la tabla2 es %d\n",siguiente_nro_dump("Tabla2"));
	printf("El siguiente numero de dump para la tabla2 es %d\n",siguiente_nro_dump("Tabla2"));
	printf("El siguiente numero de dump para la tabla1 es %d\n",siguiente_nro_dump("Tabla1"));
	resetear_numero_dump("Tabla1");
	printf("Se resetea numero de dump\n");
	printf("El siguiente numero de dump para la tabla1 es %d\n",siguiente_nro_dump("Tabla1"));

}

void dumpear_tabla(char* tabla, void* registros) {

	int nro_dump = siguiente_nro_dump(tabla);
	char* nombre_tmp = string_from_format("Dump%d", nro_dump);
	char* ruta_tmp = string_from_format("%s%s/%s.tmp", g_ruta.tablas, tabla, nombre_tmp);
	FILE* temporal = crear_tmp(tabla, nombre_tmp);
	int nro_bloque = archivo_inicializar(temporal);
	fclose(temporal);
	char* ruta_bloque = obtener_ruta_bloque(nro_bloque);

	void escribir_reg_en_tmp(void* registro) {
	escribir_registro_bloque((registro_t*)registro, ruta_bloque, ruta_tmp); //este warnings es porque esta comentado en estructuras.h
	}

	free(ruta_tmp);
	free(nombre_tmp);
	free(ruta_bloque);
	list_iterate((t_list*)registros, &escribir_reg_en_tmp);
}

void dumpear(t_dictionary* mem) {
	dictionary_iterator(memtable, &dumpear_tabla);

	//de cada tabla de esa mem, que no es la mem que sigue usando, bajar a los .tmp correspondientes.
	//Ver bien como hacer esto..
}

void dumpeo() {    //Version sin uso del diccionario.
	sem_wait(&mutex_tiempo_dump_config);
	int tiempo_dump = obtener_tiempo_dump_config();
	sem_post(&mutex_tiempo_dump_config);

	t_dictionary* mem;

	while (1) {
		sleep(tiempo_dump);

		mem = memtable;

		dumpear(mem);

		sem_wait(&mutex_memtable);
		limpiar_memtable();  //la deja como nueva. sin tablas.
		sem_post(&mutex_memtable);

		sem_wait(&mutex_tiempo_dump_config);
		tiempo_dump = obtener_tiempo_dump_config();	//Primero guardar variables y despues bloquearlas, y usarlas.
		sem_post(&mutex_tiempo_dump_config);
	}
}


void pasar_a_archivo(char* tabla, t_list* registros, char* ext) {
	if (!strcmp(ext, "tmp")) {
		char* n = string_itoa(siguiente_nro_dump(tabla));
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

