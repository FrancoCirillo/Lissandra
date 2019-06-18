//--------compactador.c--------

#include "compactador.h"

void compactador(char* tabla){
	//iniciar con detach a partir del CREATE o de la lectura cuando se inicia el FS.

	t_list* particiones = malloc(sizeof(t_list));

	int tiempo_compactacion = obtener_tiempo_compactacion_metadata(tabla)/1000; //en segundos

	int cantidad_particiones = obtener_part_metadata(tabla);

	char* nomb_part;

	for(int num = 0; num < cantidad_particiones; num++) {
		nomb_part = string_from_format("Part%d.bin", num);
		list_add(particiones, dictionary_create());
		free(nomb_part);
	} //inicia tantos diccionarios vacios como particiones tenga la tabla.



	void finalizar_compactacion(t_dictionary* particion){
	//	char* bloques_nuevos = escribir_en_nuevos_bloques(particion); //Que retorne el array para escribirlo en el archivo.
	//	actualizar_bloques_archivo(particion, bloques_nuevos); //escribe el nuevo array. Para esta altura los bloques ya estan liberados.
	//	int nuevo_size = calcular_size(particion);
	//	actualizar_size_archivo(particion, nuevo_size); //mechar con lo existente.

		//TODO
	}


	while(existe_tabla(tabla)){
		sleep(tiempo_compactacion);

		if(hay_tmp(tabla)){
			//Iniciar un cronometro: Enunciado dice que hay que registrar tiempo de la duracion de la compactacion.
			//sem_wait(todos los tmp);
			pasar_a_tmpc(tabla);
			resetear_numero_dump(tabla);
			//sem_post(todos los tmp); //TODO
		}
		else{
			continue;
		}

		t_list* lista_archivos = listar_archivos(tabla);
		for(int i = 0; i< list_size(lista_archivos);i++){
			agregar_registros_en_particion(particiones, list_get(lista_archivos, i));
		}

//		bloquear(tabla);//Todo
		liberar_todos_los_bloques(tabla);
		list_iterate(particiones, &finalizar_compactacion);

//		desbloquear(tabla);//TODO

//		vaciar_listas_registros(particiones);//TODO deja los diccionarios como nuevos.

//		finalizar_tmpcs(tabla); //Elimina los archivos tmpcs del directorio.

		//loggear duracion de la compactacion.
	}



//	liberar_recursos(particiones);
	//free(de todo lo que use);
}


void liberar_todos_los_bloques(char* tabla){
	char* ruta_tabla = string_from_format("%s%s/", g_ruta.tablas, tabla);
	int cantidad_particiones = obtener_part_metadata(tabla);

	char*ruta;

	for(int i = 0; i<cantidad_particiones;i++){
		ruta = string_from_format("%s%s/Part%d.bin", g_ruta.tablas, tabla, i);
		liberar_bloques(ruta);
		free(ruta);
	}
	free(ruta_tabla);

}


void agregar_registros_en_particion(t_list* particiones, char* ruta_archivo){

	int cant_particiones= list_size(particiones);
	int index;

	int nro_bloque = obtener_siguiente_bloque_archivo(ruta_archivo, -1);
	char* ruta_bloque = obtener_ruta_bloque(nro_bloque);
	FILE* archivo_bloque = fopen(ruta_bloque, "r");

	char* buffer= string_new();
	char caracter_leido;
	int status=1;
	char* s_caracter;

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
				imprimirContenidoArchivo(ruta_bloque);
				archivo_bloque = fopen(ruta_bloque, "r");
				} else
				status = 0; //corta el while
				break;


			case '\n': //registro completo.
				strcat(buffer, "\n");
				registro_t* registro = obtener_reg(buffer);
				strcpy(buffer, "");

				index = registro->key % cant_particiones;
				t_dictionary* dic = list_get(particiones, index);
				agregar_por_ts(dic, registro);

				break;

			default:
				s_caracter = string_from_format("%c", caracter_leido);
				strcat(buffer, s_caracter);
			break;

			}

			caracter_leido = fgetc(archivo_bloque);
			}
}

void agregar_por_ts( t_dictionary* dic, registro_t* reg_nuevo){

	registro_t* reg_viejo= (registro_t*)dictionary_get( dic,reg_nuevo->key);
	if( (!reg_viejo) || ((reg_viejo != NULL) && (reg_viejo->timestamp < reg_nuevo->timestamp) ))
	dictionary_put(dic, reg_nuevo->key, reg_nuevo);

}


int hay_tmp(char* tabla){
	char* ruta_tabla = string_from_format("%s%s/", g_ruta.tablas, tabla);
	DIR* directorio = opendir(ruta_tabla);
	puts("Entre a hay_tmp");
	printf("de la tabla %s\n", tabla);
	struct dirent* directorio_leido;
	char* archivo;
	while((directorio_leido = readdir(directorio)) != NULL) {
			archivo = directorio_leido->d_name;
			if(string_contains(archivo, ".tmp")) {
				free(archivo);
				printf("Hay un tmp en la tabla: %s", tabla);
				return 1;
				//closedir(); ?? TODO ver
				}
			free(archivo);
		}
	free(archivo);
	free(ruta_tabla);
	return 0;

}


t_list* listar_archivos(char* tabla){

	char* ruta_tabla = string_from_format("%s%s/", g_ruta.tablas, tabla);
	DIR* directorio = opendir(ruta_tabla);
	puts("Entre a listar_archivos");
	printf("de la tabla %s\n", tabla);
	struct dirent* directorio_leido;

	t_list* archivos = list_create();
	char* archivo;
	while((directorio_leido = readdir(directorio)) != NULL) {
		archivo = directorio_leido->d_name;
		if(string_contains(archivo, ".")) {
			char* ruta_archivo = string_from_format("%s%s",ruta_tabla, archivo);
			list_add(archivos,ruta_archivo);
			printf("la ruta del archivo leido es: %s", archivo);
			}
		free(archivo);
	}
	free(archivo);
	free(ruta_tabla);
	//No hago free(ruta_tabla porque es el que se usa en la lista.).. ver que no rompa.
	return archivos;
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
	char* ruta_tabla = string_from_format("%s%s", g_ruta.tablas, tabla);
	printf("RUTA TABLA: %s\n", ruta_tabla);
	DIR* directorio = opendir(ruta_tabla);
	if (directorio == NULL) {
		printf("Error: No se puede abrir el directorio\n");
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


