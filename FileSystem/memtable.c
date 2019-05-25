/*MEMTABLE*/

#include "memtable.h"

//Nada de esto esta probado.

void inicializar_memtable() {
	memtable = list_create();
	loggear_FS("Se inicializó la memtable.");
}

void finalizar_memtable() {
	//list_iterate();  //Liberar todos los registros y tablas.
	list_destroy(memtable);
}

void dumpear(t_list* mem) {

	//list_iterate(mem, bajar_tabla());
	//
	//de cada tabla de esa mem, que no es la mem que sigue usando, bajar a los .tmp correspondientes.
	//Ver bien como hacer esto..
}

void dumpeo() {
	sem_wait(&mutex_tiempo_dump_config);
	int tiempo_dump = obtener_tiempo_dump_config();
	sem_post(&mutex_tiempo_dump_config);

	t_list* mem;

	while (1) {
		sleep(tiempo_dump);

		mem = memtable;

		sem_wait(&mutex_memtable);
		mem_limpiar();  //la deja como nueva. sin tablas.
		sem_post(&mutex_memtable);

		//Ver si aca hay que crear hilo, o espera a que termina el dumpeo de las tablas.
		//Preguntar en el foro si se hacen dumpeos paralelos.
		dumpear(mem);

		sem_wait(&mutex_tiempo_dump_config);
		tiempo_dump = obtener_tiempo_dump_config();	//Primero guardar variables y despues bloquearlas, y usarlas.
		sem_post(&mutex_tiempo_dump_config);
	}
}

void mem_limpiar() {
	//la vacia
}

void mem_agregar_tabla(char* tabla) {
	mem_tabla_t* m_tabla = malloc(sizeof(mem_tabla_t));
	m_tabla->tabla = tabla;
	m_tabla->registros = list_create();

	sem_wait(&mutex_memtable);
	list_add(memtable, m_tabla);
	sem_post(&mutex_memtable);
}

void mem_agregar_reg(instr_t* instr) { //crea el struct registro_t y lo agrega a los registros de la tabla correspondiente
	registro_t* nuevo_reg = malloc(sizeof(registro_t));
	nuevo_reg->key = atoi(obtener_parametro(instr, 1));
	nuevo_reg->value = obtener_parametro(instr, 2);
	nuevo_reg->timestamp = obtener_parametro(instr, 3); //mseg_t?

	//TODO obtener_nodo_tabla --> obtiene el nodo de la tabla a la que le quiero agregar un registro
//	mem_tabla_t* nodo_tabla = mem_obtener_nodo_tabla(tabla); //list_find(es_nodo_tabla)

	//list_add(nodo_tabla registros, nuevo_reg); o algo asi
}

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

void bajar_tabla(mem_tabla_t* t) {
	//char* i = string_itoa(obtener_num_sig_dumpeo(t->tabla));
	//i = contat(i, ".tmp");
	pasar_a_archivo(t->tabla, t->registros, "tmp");
}

void escribir(FILE* f, t_list* algo){
	//no se que hace
}

int obtener_num_sig_dumpeo(char* tabla) {
	//busqueda ultimo dump.tmp en tabla.
	return 1;
}

int mem_existe_tabla(char* tabla){
	//list_any_satisfy?
	return 1;
}

int es_nodo_tabla(char* tabla, mem_tabla_t* nodo_tabla){
	char* nombre_nodo;
	strcpy(nombre_nodo, nodo_tabla->tabla);
	return strcmp(nombre_nodo, tabla);
}



