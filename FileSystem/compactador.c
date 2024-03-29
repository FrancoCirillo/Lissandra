//--------compactador.c--------

#include "compactador.h"

void crear_hilo_compactador(char* tabla){
	pthread_t hilo_compactador_tabla;
	pthread_attr_t attr_tabla;
	pthread_attr_init(&attr_tabla);
	pthread_create(&hilo_compactador_tabla, &attr_tabla, &compactador, string_from_format(tabla));
	pthread_detach(hilo_compactador_tabla);
	loggear_trace(string_from_format("Se creo el hilo de compactacion de la tabla '%s'", tabla));
}

void compactar_todas_las_tablas() {

	DIR* directorio = abrir_directorio(g_ruta.tablas);

	struct dirent directorio_leido, *directorio_leido_p;
	compactation_locker = 1; //Se habilita una ultima compactacion

	while(readdir_r(directorio, &directorio_leido, &directorio_leido_p) == 0 && directorio_leido_p != NULL){
		char* tabla = directorio_leido.d_name;
		if(!string_contains(tabla, "."))
			compactador(string_from_format(tabla));
	}
	compactation_locker = 0;
	cerrar_directorio(directorio);

}

void* compactador(void* tab) {
	char* tabla = tab;


	char* tiempo = obtener_dato_metadata(tabla, "COMPACTATION_TIME");
	int tiempo_compactacion = atoi(tiempo);
	free(tiempo);

	char* cant = obtener_dato_metadata(tabla, "PARTITIONS");
	int cantidad_particiones = atoi(cant);
	free(cant);

	int cant_tmpc;


	sem_wait(&mutex_dic_semaforos);
	sem_t* mutex_tabla = obtener_mutex_tabla(tabla);
	sem_post(&mutex_dic_semaforos);

	int j;

	mseg_t ts_inicial;
	mseg_t ts_final;
	mseg_t duracion_compactacion;

	while(existe_tabla_en_mem(tabla) || compactation_locker) {
		if(existe_tabla_en_mem(tabla))
			usleep(tiempo_compactacion * 1000);

		sem_wait(mutex_tabla);
		if(existe_tabla_en_FS(tabla)) {

			ts_inicial = obtener_ts();

			cant_tmpc = pasar_a_tmpc(tabla);

			if(!cant_tmpc) {
				if(compactation_locker){
					sem_post(mutex_tabla);
					break;
				}
				else{
					sem_post(mutex_tabla);
					continue;
				}
			}
			else if(cant_tmpc == -1){
				sem_post(mutex_tabla);
				break;
			}

			t_list* particiones = list_create();
			for(int num = 0; num < cantidad_particiones; num++) {
				//inicia tantos diccionarios vacios como particiones tenga la tabla.
				t_dictionary* dic = dictionary_create();
				list_add(particiones, dic);
			}
			loggear_info(string_from_format("Estoy compactando la tabla '%s'", tabla));

			t_list* lista_archivos = listar_archivos(tabla);

			if(lista_archivos){

				for(int i = 0; i < list_size(lista_archivos); i++){
					char* lectura = (char*)list_get((t_list*)lista_archivos, i);
					agregar_registros_de_particion(particiones, lectura);
					//loggear_trace(string_from_format("Entre al for, en el ciclo %d\n", i));
				}
				//				puts("Ya agregue_registros_en_particion\n\n");

				sem_wait(&mutex_bitarray);
				list_iterate((t_list*)lista_archivos, &liberar_bloques);
				sem_post(&mutex_bitarray);
				//				puts("Ya libere los bloques\n\nENTRO AL FOR DE FINALIZAR COMPACTACION\n");


				for(j = 0; j< cantidad_particiones; j++){
					loggear_trace(string_from_format("Entre a finalizar_compactacion de la tabla %s, particion de indice: %d\n\n", tabla, j));
					finalizar_compactacion((t_dictionary*)list_get(particiones,j), tabla, j);
				}

				borrar_tmpcs(tabla); //Elimina los archivos tmpcs del directorio.

				loggear_trace(string_from_format("Si se llego hasta aca, se realizo la compactacion Exitosamente.\n\n\n"));

				vaciar_listas_registros(particiones); //Deja los diccionarios como nuevos.
				list_destroy_and_destroy_elements(lista_archivos,free);
			}
			else{
				list_destroy_and_destroy_elements(lista_archivos,free);
				sem_post(mutex_tabla);
				vaciar_listas_registros(particiones); //Deja los diccionarios como nuevos.
				break;
			}

			sem_post(mutex_tabla);

			ts_final = obtener_ts();
			duracion_compactacion = ts_final - ts_inicial;

			loggear_info(string_from_format("Duracion de compactacion: %" PRIu64 "\n", duracion_compactacion));

			if(compactation_locker) //Si debe hacerse por ultima vez porque se cierra el FS
				break;

		}
		else {
			sem_post(mutex_tabla);
			//sem_wait(&mutex_dic_semaforos);
			//eliminar_mutex_de_tabla(tabla); ESTO DA SEG
			//sem_post(&mutex_dic_semaforos);
			break;
		}

//		liberar_listas_registros(particiones);

	}
	free(tabla);
	return NULL;
}

void finalizar_compactacion(t_dictionary* particion, char* tabla, int num_part) {
	//	printf("Estoy en FINALIZAR COMPACTACION \nDe la Particion: %d en tabla %s\n\n", num_part , tabla);

	char* ruta_archivo = string_from_format("%s%s/Part%d.bin", g_ruta.tablas, tabla, num_part);
	//	printf("RUTA: %s\n\n", ruta_archivo);

	int nro_bloque = inicializar_archivo(ruta_archivo);
	//	printf("BLOQUE nuevo de la Particion: %d es %d\n\n", num_part, nro_bloque);

	//	char* nom = string_from_format("Part%d", num);
	//	FILE* f = crear_archivo(tabla, nom, ".bin"); //Lo crea como nuevo.
	//	int nro_bloque = archivo_inicializar(f);
	//	printf("BLOQUE nuevo de la Particion: %d es %d\n\n", num ,nro_bloque);

	char* ruta_bloque;

	void bajar_registro(char* key, void* reg){
		//		printf("Estoy bajando_registro de key %s\n", key);
		registro_t* registro = reg;
		nro_bloque = obtener_ultimo_bloque(ruta_archivo);
		ruta_bloque = obtener_ruta_bloque(nro_bloque);
		loggear_trace(string_from_format("Num bloque del archivo escrito es: %d\n\n", nro_bloque));
		escribir_registro_bloque(registro, ruta_bloque, ruta_archivo);  //Esta funcion actualiza el nro de bloque si lo necesita.
		free(ruta_bloque);
	}

	loggear_trace(string_from_format("Empiezo con bajar_registros de la tabla %s\n", tabla));
	dictionary_iterator(particion, (void*) bajar_registro);
	free(ruta_archivo);

}


void agregar_registros_de_particion(t_list* particiones, char* ruta_archivo){
	//	puts("\n\n--------------estoy en agregar_registros_en_particion------------");
	//	loggear_error(string_from_format("Agregando registros al diccionario de particiones de la ruta:\n %s \n", ruta_archivo));

	if(!obtener_tam_archivo(ruta_archivo)) {
		loggear_error(string_from_format("No hay registros\n"));
		return;
	}

	int nro_bloque = obtener_siguiente_bloque_archivo(ruta_archivo, -1);
	char* ruta_bloque = obtener_ruta_bloque(nro_bloque);

	FILE* archivo_bloque = abrir_archivo(ruta_bloque, "r");
	int tamMax = 300; //Cantidad de caracteres maximos que va a leer
	char* lineaDeBloque = malloc(tamMax);
	char* bloqueCompleto = string_new();

	while (1) {
		while (fgets(lineaDeBloque, tamMax, archivo_bloque) != NULL) {
			//			loggear_error(string_from_format("Lei la linea %s del bloque %s", lineaDeBloque, ruta_bloque));
			string_append_with_format(&bloqueCompleto, "%s", lineaDeBloque);
			free(lineaDeBloque);
			lineaDeBloque = malloc(tamMax);
		}
		cerrar_archivo(archivo_bloque);
		free(ruta_bloque);
		int bloque_anterior = nro_bloque;
		nro_bloque = obtener_siguiente_bloque_archivo(ruta_archivo, bloque_anterior);
		//		loggear_error(string_from_format("\tBloque anterior: %d, Bloque siguiente %d",bloque_anterior, nro_bloque));
		if (nro_bloque >= 0) { //si es menor a cero, no hay mas bloques por leer
			ruta_bloque = obtener_ruta_bloque(nro_bloque);
			archivo_bloque = abrir_archivo(ruta_bloque, "r");
		} else{
			free(lineaDeBloque);
			break;
		}
	}
	agregar_bloque_por_ts(bloqueCompleto, particiones);
}

void agregar_bloque_por_ts(char* bloqueCompleto, t_list* particiones){
	int cantParticiones = list_size(particiones);

	//	loggear_error(string_from_format("La tabla completa que se va a convertir en registro es:\n%s", bloqueCompleto));
	char** registros = string_split(bloqueCompleto, "\n");

	void agregar_registro_por_ts(char* registroString){
		//		loggear_error(string_from_format("El registroString es:\n%s", registroString));
		registro_t* registro = obtener_registro(registroString);
		imprimirRegistro(registro);
		int indice = registro->key % cantParticiones;
		loggear_trace(string_from_format("Pertenece al Diccionario-Particion nro: %d\n",index));
		t_dictionary* dic = list_get(particiones, indice);
		agregar_por_ts(dic, registro);
		free(registroString);
	}

	string_iterate_lines(registros, (void*)agregar_registro_por_ts);
	free(registros);
	free(bloqueCompleto);
}

void agregar_por_ts(t_dictionary* dic, registro_t* reg_nuevo){
	loggear_trace(string_from_format("Estoy en agregar_por_ts"));

	char* key_nueva = string_itoa(reg_nuevo->key);
	loggear_trace(string_from_format("\nLa key leida a agregar en el diccionario que corresponda es %s\n", key_nueva));

	registro_t* reg_viejo = (registro_t*)dictionary_remove((t_dictionary*)dic, key_nueva);

	if(!reg_viejo){
		dictionary_put((t_dictionary*)dic, key_nueva, reg_nuevo);
	} else if((reg_viejo) && (reg_viejo->timestamp < reg_nuevo->timestamp)){
		liberar_registro2(reg_viejo);
		dictionary_put((t_dictionary*)dic, key_nueva, reg_nuevo);
	} else {
		dictionary_put((t_dictionary*)dic, key_nueva, reg_viejo);
		liberar_registro2(reg_nuevo);
	}

	free(key_nueva);
	//TODO: Verificar si genera memory leaks al hacer el put !! No se si lo pisa o se pierde la referencia.
}


t_list* listar_archivos(char* tabla){
	loggear_trace(string_from_format("Listando archivos de la tabla '%s'\n", tabla));
	char* ruta_tabla =  obtener_ruta_tabla(tabla);
	DIR* directorio = opendir(ruta_tabla);

	if (directorio == NULL) {
		closedir(directorio);
		free(ruta_tabla);
		return NULL;
	}

	struct dirent directorio_leido, *directorio_leido_p;

	t_list* archivos = list_create();

	char* archivo;
	while(readdir_r(directorio, &directorio_leido, &directorio_leido_p) == 0 && directorio_leido_p != NULL){
		archivo = directorio_leido.d_name;
		if(string_contains(archivo, ".bin") || string_contains(archivo, ".tmpc")) {
			char* ruta_archivo = string_from_format("%s/%s", ruta_tabla, archivo);
			list_add(archivos, ruta_archivo);
			loggear_trace(string_from_format("La ruta del archivo leido es: %s", archivo));
		}
	}

	loggear_trace(string_from_format("\n\n **** Ya lei los archivos de la tabla %s ****\n\n", tabla));
	cerrar_directorio(directorio);
	free(ruta_tabla);
	return archivos;
}

int pasar_a_tmpc(char* tabla) {   //Listo sin leaks.

	char* ruta_tabla = obtener_ruta_tabla(tabla);
	DIR* directorio = opendir(ruta_tabla);

	if(directorio == NULL) {
		closedir(directorio);
		free(ruta_tabla);
		return -1;
	}

	struct dirent directorio_leido, *directorio_leido_p;
	int contador = 0;
	while(readdir_r(directorio, &directorio_leido, &directorio_leido_p) == 0 && directorio_leido_p != NULL){

		char* nombre_archivo = directorio_leido.d_name;
		if(string_ends_with(nombre_archivo, ".tmp")) {
			//loggear_trace(string_from_format("Es un tmp"));
			char* nombre_viejo = string_from_format("%s/%s", ruta_tabla, nombre_archivo);
			//loggear_trace(string_from_format("Nombre viejo: %s\n", nombre_viejo));
			int length  = strlen(nombre_archivo);
			//loggear_trace(string_from_format("Letras del nombre: %d\n", length));
			char* nombre_sin_ext = string_substring_until(nombre_archivo, length-4);
			//loggear_trace(string_from_format("Nombre sin extension: %s\n", nombre_sin_ext));
			char* nuevo_nombre = string_from_format("%s/%s.tmpc", ruta_tabla, nombre_sin_ext);
			//loggear_trace(string_from_format("Nuevo nombre: %s\n", nuevo_nombre));
			rename(nombre_viejo, nuevo_nombre);

			free(nombre_viejo);
			free(nombre_sin_ext);
			free(nuevo_nombre);

			contador++;

		}
	}
	free(ruta_tabla);
	cerrar_directorio(directorio);
	resetear_numero_dump(tabla);

	return contador;

}

void borrar_tmpcs(char* tabla){

	char* ruta_tabla = obtener_ruta_tabla(tabla);
	DIR* directorio = abrir_directorio(ruta_tabla);

	if(directorio!=NULL){
		struct dirent directorio_leido, *directorio_leido_p;

		char* archivo;
		while(readdir_r(directorio, &directorio_leido, &directorio_leido_p) == 0 && directorio_leido_p != NULL){
			archivo = directorio_leido.d_name;
			if(string_contains(archivo, ".tmpc")) {
				char* ruta_archivo = string_from_format("%s/%s", ruta_tabla, archivo);
				remove(ruta_archivo);
				free(ruta_archivo);
			}
		}
	}

	cerrar_directorio(directorio);
	free(ruta_tabla);
}
void liberar_registro2(registro_t* registro){
	free(registro-> value);
	free(registro);
}

void vaciar_listas_registros(t_list* particiones){

	void vaciar_diccionario(void* dic){
		dictionary_destroy_and_destroy_elements((t_dictionary*)dic,(void*)liberar_registro);
	}
	list_iterate(particiones, &vaciar_diccionario);
	list_destroy(particiones);
}


void liberar_registro(registro_t* registro){
	free(registro);
}

void liberar_listas_registros(t_list* particiones){

	void liberar_diccionario(void* dic){
		loggear_info(string_from_format("Se libera un diccionario"));
		dictionary_destroy_and_destroy_elements((t_dictionary*)dic, (void*)liberar_registro);
	}
	list_iterate(particiones, &liberar_diccionario);
	list_destroy_and_destroy_elements(particiones, (void*)dictionary_destroy);
	//list_destroy_and_destroy_elements(particiones,liberar_diccionario);

}

//Ejemplo de uso. agregar un struct tipo * y eso en el
void archivos_de_un_dir(char* ruta){

	DIR *d;
	struct dirent de, *dep;
	d = opendir(ruta);

	if (d == NULL) {

	}

	while (readdir_r(d, &de, &dep) == 0 && dep != NULL) {
		printf("La entrada del directorio es %s\n", de.d_name);
	}

	if (closedir(d) == -1) {
		perror("closedir");
	}
}

//Esto se debe implementar  en donde se usen directorios.
//	DIR* directorio = abrir_directorio(ruta_tabla);
//	struct dirent directorio_leido, *directorio_leido_p;
//	while(readdir_r(directorio, &directorio_leido, &directorio_leido_p) == 0 && directorio_leido_p != NULL){
//	char* nombre_archivo = directorio_leido.d_name;
//	cerrar_directorio(directorio);
