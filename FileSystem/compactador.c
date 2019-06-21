//--------compactador.c--------

#include "compactador.h"

void compactador(char* tabla){
	//iniciar con detach a partir del CREATE o de la lectura cuando se inicia el FS.

	t_dictionary* particiones = malloc(sizeof(t_dictionary));

	int tiempo_compactacion = 10000;//(int)obtener_tiempo_compactacion(tabla); //en segundos

	int cantidad_particiones = obtener_part_metadata(tabla);

	char* nomb_part;

	for(int num = 0; num < cantidad_particiones; num++) {
		nomb_part = string_from_format("Part%d.bin", num);
		dictionary_put(particiones, nomb_part, dictionary_create());
		free(nomb_part);
	} //inicia tantos diccionarios vacios como particiones tenga la tabla.





	//////////Viejo
//	void actualizar_registros_de_la_particion(char* particion, t_list* registros_tmpc){
//			char* bloques= bloques_particion(particion, tabla); // no se si ya existe una funcion similar. TODO
//			//dev el array.
//
//			t_list* registros_bin = leer_registros(bloques);
//			t_list* registros_nuevos = comparar_registro_a_registro(registros_tmpc, registros_bin);
//
//			list_clean(registros_tmpc); //liberar todos los registros viejos, para pisar con los nuevos. Esto hace el free bien?
//			registros_tmpc = registros_nuevos;
//			list_destroy(registros_bin);
//	}

	//////////Viejo
//	void finalizar_compactacion(char* particion, t_list* registros_tmpc){
//		//registros_tmpc tiene a esta altura los registros nuevos a escribir.
//
//		//Yo escribiria primero los nuevos bloques, despues los libero (para asegurar persistencia)
//			// El enunciado dice q primero los liberemos..
//		char* bloques = escribir_nuevos_bloques(registros_tmpc);
//		//retorna los bloques nuevos que se usaron.
//
//		liberar_bloques(particion);
//
//		actualizar_bloques_archivo(particion, bloques); //escribe el nuevo array
//		int nuevo_size = calcular_size(bloques);
//		actualizar_size_archivo(particion, nuevo_size); //mechar con lo existente.
//
//		free(bloques);
//
//	}


	while(existe_tabla(tabla)){
		sleep(tiempo_compactacion);
//TODO
//		if(hay_tmp(tabla)){
//			//Iniciar un cronometro: Enunciado dice que hay que registrar tiempo de la duracion de la compactacion.
//			//sem_wait(todos los tmp);
//			pasar_a_tmpc(tabla);
//			resetear_numero_dump(tabla);
//			//sem_post(todos los tmp);
//		}
//		else{
//			continue;
//		}

		t_list* lista_archivos = listar_archivos(tabla);
		for(int i = 0; i< list_size(lista_archivos);i++){
//			agregar_registros_en_particion(particiones, list_get(lista_archivos, i));
		}

//		bloquear(tabla);//Todo
//
//		dictionary_iterator(particiones, &finalizar_compactacion); //TODO actualizar
//
//		desbloquear(tabla);//TODO
//
//		vaciar_listas_registros(particiones);
//
//		finalizar_tmpcs(tabla); //Libera bloques, y elimina los archivos tmpcs en el directorio.

		//loggear duracion de la compactacion.
	}



//	liberar_recursos(particiones);
	//free(de todo lo que use);
}

//void agregar_registros_en_particion(t_dictionary* particiones, char* ruta_archivo){
//	t_list* registros = levantar_registros(ruta_archivo);//TODO o hacer una iteracion por los bloques que tenga ese archivo.
//	list_iterate(registros, );
//
//
//}


t_list* listar_archivos(char* tabla){
	printf("---Entre a listar_archivos de la tabla %s---\n", tabla);
	char* ruta_tabla =  obtener_ruta_tabla(tabla);

	DIR* directorio = opendir(ruta_tabla);
	if (directorio == NULL) {
		puts("Error: No se pudo abrir el directorio");
		//free(ruta_tabla);
		exit(2);
	}
	else{
		struct dirent* directorio_leido;
		t_list* archivos = list_create();

		while((directorio_leido = readdir(directorio)) != NULL) {
			char* archivo = directorio_leido->d_name;
			if(string_contains(archivo, ".")) {
				char* ruta_archivo = string_from_format("%s/%s", ruta_tabla, archivo);
				list_add(archivos, ruta_archivo);
				printf("la ruta del archivo leido es: %s", archivo);
			}
			free(archivo);
		}
		free(ruta_tabla);
		//free(ruta_archivo) No lo hago porque es el que se usa en la lista... ver que no rompa.
		return archivos;
	}
}






//Para probar detach que no segui intentando..
void compactar2(){
	//pthread_create(hilo, tabla, compactar_tabla);
	while(1){
		puts("estoy compactando cada 2 segundos\n");
	}
}

void compactar5(){
	//pthread_create(hilo, tabla, compactar_tabla);
	while(1){
		puts("estoy compactando cada 5 segundos\n");

	}

}



void pasar_a_tmpc(char* tabla) {
	char* ruta_tabla = obtener_ruta_tabla(tabla);
	printf("RUTA TABLA: %s\n", ruta_tabla);
	DIR* directorio = opendir(ruta_tabla);
	if (directorio == NULL) {
		puts("Error: No se pudo abrir el directorio");
		//free(ruta_tabla);
		exit(2);
	}
	struct dirent* directorio_leido;
	while((directorio_leido = readdir(directorio)) != NULL) {
		char* nombre_archivo = directorio_leido->d_name;
		printf("Nombre archivo: %s\n", nombre_archivo);
		if(string_ends_with(nombre_archivo, ".tmp")) {
			puts("Es un tmp");
			char* nombre_viejo = string_from_format("%s/%s", ruta_tabla, nombre_archivo);
			printf("Nombre viejo: %s\n", nombre_viejo);
			int length  = strlen(nombre_archivo);
			printf("Letras del nombre: %d\n", length);
			char* nombre_sin_ext = string_substring_until(nombre_archivo, length-4);
			printf("Nombre sin extension: %s\n", nombre_sin_ext);
			char* nuevo_nombre = string_from_format("%s/%s.tmpc", ruta_tabla, nombre_sin_ext);
			printf("Nuevo nombre: %s\n", nuevo_nombre);
			int status = rename(nombre_viejo, nuevo_nombre);
			printf("Status: %d\n", status);
		}
	}
	//resetear_numero_dump(tabla);
}


