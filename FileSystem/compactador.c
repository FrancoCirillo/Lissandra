//--------compactador.c--------

#include "compactador.h"

void crear_hilo_compactador(char* tabla){
	pthread_t hilo_compactador_tabla;
	pthread_attr_t attr_tabla;
	pthread_attr_init(&attr_tabla);
	pthread_create(&hilo_compactador_tabla, &attr_tabla, &compactador, tabla);
	pthread_detach(hilo_compactador_tabla);
	loggear_trace(string_from_format("Se creo el hilo de compactacion de la tabla '%s'", tabla));
}

void compactar_todas_las_tablas() {
	DIR* directorio = opendir(g_ruta.tablas);
	if (directorio != NULL){
		struct dirent* directorio_leido;
		compactation_locker = 1; //Se habilita una ultima compactacion
		while((directorio_leido = readdir(directorio)) != NULL) {
			char* tabla = directorio_leido->d_name;
			if(!string_contains(tabla, "."))
				compactador(tabla);
		}
		compactation_locker = 0;
	}
	closedir(directorio);
}

void* compactador(void* tab) {
	char* tabla = tab;

	t_list* particiones = list_create();

	char* tiempo = obtener_dato_metadata(tabla, "COMPACTATION_TIME");
	int tiempo_compactacion = atoi(tiempo); //en segundos
	free(tiempo);

	char* cant = obtener_dato_metadata(tabla, "PARTITIONS");
	int cantidad_particiones = atoi(cant);
	free(cant);

	int cant_tmpc;

	for(int num = 0; num < cantidad_particiones; num++) {
		//inicia tantos diccionarios vacios como particiones tenga la tabla.
		t_dictionary* dic = dictionary_create();
		list_add(particiones, dic);
	}

	sem_wait(&mutex_dic_semaforos);
	sem_t* mutex_tabla = obtener_mutex_tabla(tabla);
	sem_post(&mutex_dic_semaforos);

	//int i=0;
	int j;

	mseg_t ts_inicial;
	mseg_t ts_final;
	mseg_t duracion_compactacion;

	while(existe_tabla_en_mem(tabla) || compactation_locker) {
		if(existe_tabla_en_mem(tabla))
			usleep(tiempo_compactacion);

		if(existe_mutex(tabla)) {

			sem_wait(mutex_tabla);

			ts_inicial = obtener_ts();
			//i++;
			//printf("Compactacion nro %d de la tabla %s\n", i, tabla);

			cant_tmpc = pasar_a_tmpc(tabla);

			if(!cant_tmpc) {
				if(compactation_locker){
					sem_post(mutex_tabla);
					break;
				}
				else continue;
			}

			loggear_info(string_from_format("Estoy compactando la tabla %s", tabla));
			t_list* lista_archivos = listar_archivos(tabla);

			if(lista_archivos){

				for(int i = 0; i < list_size(lista_archivos); i++){
					char* lectura = (char*)list_get((t_list*)lista_archivos, i);
					agregar_registros_en_particion(particiones, lectura);
					//loggear_trace(string_from_format("Entre al for, en el ciclo %d\n", i));
				}
				puts("Ya agregue_registros_en_particion\n\n");

				list_iterate((t_list*)lista_archivos, &liberar_bloques);
				puts("Ya libere los bloques\n\nENTRO AL FOR DE FINALIZAR COMPACTACION\n");

				for(j = 0; j< cantidad_particiones; j++){
					loggear_trace(string_from_format("Entre a finalizar_compactacion de la tabla %s, particion de indice: %d\n\n", tabla, j));
					finalizar_compactacion((t_dictionary*)list_get(particiones,j), tabla, j);
				}

				borrar_tmpcs(tabla); //Elimina los archivos tmpcs del directorio.

				loggear_trace(string_from_format("Si se llego hasta aca, se realizo la compactacion Exitosamente.\n\n\n"));

				list_iterate(particiones, &vaciar_diccionario);
			}

			sem_post(mutex_tabla);

			if(compactation_locker)	//Si debe hacerse por ultima vez porque se cierra el FS
				break;

			ts_final = obtener_ts();
			duracion_compactacion = dif_timestamps(ts_inicial, ts_final);

			loggear_info(string_from_format("Duracion de compactacion: %" PRIu64 "\n", duracion_compactacion));

			//i++;
			puts("FIN de 1 while de la compactacion.");
		}
		else
			break;
	}

	list_destroy_and_destroy_elements(particiones,dictionary_destroy);
	//liberar_recursos(particiones);
	//free(de todo lo que use);
}

void finalizar_compactacion(t_dictionary* particion, char* tabla, int num) {
	printf("Estoy en FINALIZAR COMPACTACION \nDe la Particion: %d en tabla %s\n\n", num , tabla);
	sleep(1);
	char* nom = string_from_format("Part%d", num);
	FILE* f = crear_archivo(tabla, nom, ".bin"); //Lo crea como nuevo.

	int nro_bloque = archivo_inicializar(f);
	printf("BLOQUE nuevo de la Particion: %d es %d\n\n", num ,nro_bloque);

	char* ruta_bloque ;

	char* ruta_archivo = string_from_format("%s%s/Part%d.bin", g_ruta.tablas, tabla, num);
	printf("LA RUTA ES: %s\n\n", ruta_archivo);



	void bajar_registro(char* key, void* reg){
		printf("Estoy bajando_registro de key %s\n", key);
		registro_t* registro = reg;
		nro_bloque = obtener_ultimo_bloque(ruta_archivo);
		ruta_bloque = obtener_ruta_bloque(nro_bloque);
		loggear_trace(string_from_format("Num bloque del archivo escrito es: %d\n\n", nro_bloque));
		escribir_registro_bloque(registro, ruta_bloque, ruta_archivo);  //Esta funcion actualiza el nro de bloque si lo necesita.
		printf("Ya termine de bajar el reg de key %s\n",key );
		sleep(2);
		free(ruta_bloque);
	}
	loggear_trace(string_from_format("Empiezo con bajar_registros de la tabla %s\n", tabla));
	sleep(2);
	dictionary_iterator((t_dictionary*)particion, &bajar_registro);
//	config_destroy(archivo);
	fclose(f);
	free(nom);
	printf("\n\nFIN FINALIZAR_COMPACTACION de la tabla %s particion %d \n\n", tabla, num);
}

//int ultimo_bloque(t_config* archivo){
//	char* lista_bloques = config_get_string_value(archivo, "BLOCKS");
//	char* s_ult_bloque = string_substring(lista_bloques, strlen(lista_bloques)-2, 1);
//	int ult_bloque = atoi(s_ult_bloque);
//	free(lista_bloques);
//	return ult_bloque;
//}

void agregar_registros_en_particion(t_list* particiones, char* ruta_archivo){
	puts("\n\n--------------estoy en agregar_registros_en_particion------------");
	printf("Agregando registros al diccionario de particiones de la ruta:\n %s \n", ruta_archivo);

	if(!obtener_tam_archivo(ruta_archivo)) {
		loggear_trace(string_from_format("No hay registros\n"));
		return;
	}

	int cant_particiones = list_size(particiones);
	int index;

	int nro_bloque = obtener_siguiente_bloque_archivo(ruta_archivo, -1);
	char* ruta_bloque = obtener_ruta_bloque(nro_bloque);

	FILE* archivo_bloque = fopen(ruta_bloque, "r");
//	loggear_trace(string_from_format("El archivo es: %s\n", ruta_bloque));
	//int cant_letras_ts = strlen(mseg_a_string(obtener_ts()));
	//char* buffer = malloc(sizeof(char*)*(cant_letras_ts + 4 + config_FS.tamanio_value + 10)); //   +4 por: \n ; ; \0 //Le mando 10 de cabeza. es para que sobre.
	//strcpy(buffer, "");
	char* buffer = string_new();
	char caracter_leido;
	int status = 1;

	while(status) {
		caracter_leido = fgetc(archivo_bloque);
		switch(caracter_leido){

			case EOF: //se me acabo el archivo
				fclose(archivo_bloque);
				free(ruta_bloque);
				int bloque_anterior = nro_bloque;
				nro_bloque = obtener_siguiente_bloque_archivo(ruta_archivo, bloque_anterior);
				if(nro_bloque >= 0) { //si es menor a cero, no hay mas bloques por leer
					ruta_bloque = obtener_ruta_bloque(nro_bloque);
					archivo_bloque = fopen(ruta_bloque, "r");
				} else
					status = 0; //corta el while
				break;

			case '\n': //registro completo.
				//strcat(buffer, "\n");
				string_append(&buffer, "\n");
				registro_t* registro = obtener_registro(buffer);
				imprimirRegistro(registro);
				free(buffer);
				buffer = string_new();
				index = registro->key % cant_particiones;
				loggear_trace(string_from_format("Pertenece al Diccionario-Particion nro: %d\n", index));
				t_dictionary* dic = list_get(particiones, index);
				agregar_por_ts(dic, registro);
				//borrar_registro(registro); //NO, genera invalid read
				break;

			default:
				string_append_with_format(&buffer, "%c", caracter_leido);
				break;
			}

	}    //ESTO FUNCA!
}

void agregar_por_ts(t_dictionary* dic, registro_t* reg_nuevo){
	loggear_trace(string_from_format("Estoy en agregar_por_ts"));

	char* key_nueva = string_itoa(reg_nuevo->key);

	loggear_trace(string_from_format("\nLa key leida a agregar en el diccionario que corresponda es %s\n", key_nueva));

	registro_t* reg_viejo = (registro_t*)dictionary_get( (t_dictionary*)dic, key_nueva);

	if( (!reg_viejo) || ((reg_viejo != NULL) && (reg_viejo->timestamp < reg_nuevo->timestamp) ))
		dictionary_put((t_dictionary*)dic, key_nueva, reg_nuevo);
	//TODO: Verificar si genera memory leaks al hacer el put !! No se si lo pisa o se pierde la referencia.
}


t_list* listar_archivos(char* tabla){
	loggear_trace(string_from_format("Listando archivos de la tabla '%s'\n", tabla));
	char* ruta_tabla =  obtener_ruta_tabla(tabla);

	DIR* directorio = opendir(ruta_tabla);
	if (directorio == NULL) {
		loggear_error(string_from_format("Error: No se pudo abrir el directorio"));
		closedir(directorio);
		free(ruta_tabla);
		return NULL;
	}

	struct dirent* directorio_leido;
	t_list* archivos = list_create();

	char* archivo;
	while((directorio_leido = readdir(directorio)) != NULL) {
		archivo = directorio_leido->d_name;
		if(string_contains(archivo, ".bin") || string_contains(archivo, ".tmpc")) {
			char* ruta_archivo = string_from_format("%s/%s", ruta_tabla, archivo);
			list_add(archivos, ruta_archivo);
			loggear_trace(string_from_format("La ruta del archivo leido es: %s", archivo));
		}
	}

	loggear_trace(string_from_format("\n\n **** Ya lei los archivos de la tabla %s ****\n\n", tabla));
	closedir(directorio);
	free(ruta_tabla);
	//free(ruta_archivo) No lo hago porque es el que se usa en la lista... ver que no rompa.
	return archivos;
}

int pasar_a_tmpc(char* tabla) {

	char* ruta_tabla = obtener_ruta_tabla(tabla);
	//loggear_trace(string_from_format("RUTA TABLA: %s\n", ruta_tabla));
	DIR* directorio = opendir(ruta_tabla);
	if (directorio == NULL) {
		loggear_error(string_from_format("Error: No se pudo abrir el directorio"));
		closedir(directorio);
		free(ruta_tabla);
		return 0;
	}
	struct dirent* directorio_leido;
	int contador = 0;
	while((directorio_leido = readdir(directorio)) != NULL) {
		char* nombre_archivo = directorio_leido->d_name;
		//loggear_trace(string_from_format("Nombre archivo: %s\n", nombre_archivo));
		if(string_ends_with(nombre_archivo, ".tmp")) {
			//loggear_trace(string_from_format("Es un tmp"));
			char* nombre_viejo = string_from_format("%s/%s", ruta_tabla, nombre_archivo);
			//loggear_trace(string_from_format("Nombre viejo: %s\n", nombre_viejo));
			int length  = strlen(nombre_archivo);
			//loggear_trace(string_from_format("Letras del nombre: %d\n", length));
			char* nombre_sin_ext = string_substring_until(nombre_archivo, length-4);
			//loggear_trace(string_from_format("Nombre sin extension: %s\n", nombre_sin_ext));
			char* nuevo_nombre = string_from_format("%s/%s.tmpc", ruta_tabla, nombre_sin_ext);
			loggear_trace(string_from_format("Nuevo nombre: %s\n", nuevo_nombre));
			rename(nombre_viejo, nuevo_nombre);

			contador++;
			//loggear_trace(string_from_format("Status: %d\n", status));
		}
	}
	closedir(directorio);
//	loggear_trace(string_from_format("Se pasaron a tmpc los archivos de la tabla %s\n", tabla));
	resetear_numero_dump(tabla);   //Importante esto!
	//loggear_trace(string_from_format("Se reseteo el numero de dump\n"));

	return contador;

}

void borrar_tmpcs(char* tabla){
	char* ruta_tabla = obtener_ruta_tabla(tabla);
	DIR* directorio = opendir(ruta_tabla);
	struct dirent* directorio_leido;

	char* archivo;
	while((directorio_leido = readdir(directorio)) != NULL) {
		archivo = directorio_leido->d_name;
		if(string_contains(archivo, ".tmpc")) {
			char* ruta_archivo = string_from_format("%s/%s", ruta_tabla, archivo);
			eliminar_archivo(ruta_archivo);
			//loggear_trace(string_from_format("El archivo leido es: %s \n Y fue ELIMINADO :(\n\n", archivo));
		}
	}

//	loggear_trace(string_from_format("\n\n **** Ya lei los archivos de la tabla %s ****\n\n", tabla));
	closedir(directorio);
	free(ruta_tabla);

	//FUNCA
}

void eliminar_archivo(char* ruta_archivo){
	remove(ruta_archivo);
}   //FUNCA

void vaciar_diccionario(void* dic){
	dictionary_clean((t_dictionary*)dic);
}
