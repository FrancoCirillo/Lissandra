//--------compactador.c--------

#include "compactador.h"

void compactador(char* tabla){

	//iniciar con detach a partir del CREATE o de la lectura cuando se inicia el FS.

	t_list* particiones = list_create();

	int tiempo_compactacion = obtener_tiempo_compactacion_metadata(tabla)/1000; //en segundos
	int cantidad_particiones = obtener_part_metadata(tabla);
	int cant_tmpc;

	for(int num = 0; num < cantidad_particiones; num++) {
		//inicia tantos diccionarios vacios como particiones tenga la tabla.
		list_add(particiones, dictionary_create());
	}

	while(existe_tabla(tabla)){
		sleep(tiempo_compactacion);
		//sem_wait(mutex_tabla);

		cant_tmpc = pasar_a_tmpc(tabla);

		//sem_post(mutex_tabla); //TODO

		if(!cant_tmpc)
			continue;


		t_list* lista_archivos = listar_archivos(tabla);
		for(int i = 0; i< list_size(lista_archivos);i++){
			agregar_registros_en_particion(particiones, list_get(lista_archivos, i));
		}

		//sem_wait(mutex_tabla);
		list_iterate((t_list*)lista_archivos,&liberar_bloques);
		for(int j = 0; j< cantidad_particiones;j++){
			finalizar_compactacion(list_get(particiones,j), tabla, j);
		}
//		borrar_tmpcs(tabla); //Elimina los archivos tmpcs del directorio.

		//sem_post(mutex_tabla);


//		vaciar_listas_registros(particiones);//TODO deja los diccionarios como nuevos.


		//loggear duracion de la compactacion.

	}



//	liberar_recursos(particiones);
	//free(de todo lo que use);
}



void finalizar_compactacion(t_dictionary* particion, char* tabla, int num){
	char* nom = string_from_format("Part%d", num);
	FILE* f = crear_archivo(nom, tabla,".bin"); //lo crea como nuevo.

	int ult_bloque;
	int nro_bloque = archivo_inicializar(f);
	char* ruta_bloque = obtener_ruta_bloque(nro_bloque);
	char* ruta_archivo = string_from_format("%s%s/Part%d.bin", g_ruta.tablas, tabla, num);
	t_config* archivo = config_create(ruta_archivo);

	void bajar_registro(char* key, registro_t* reg){
		if(nro_bloque != (ult_bloque = ultimo_bloque(archivo))){
			nro_bloque = ult_bloque;
			free(ruta_bloque);
			ruta_bloque = obtener_ruta_bloque(nro_bloque);
		}

		escribir_registro_bloque(reg, ruta_bloque, ruta_archivo);  //Esta funcion actualiza el nro de bloque si lo necesita.
	}

	dictionary_iterator((t_dictionary*)particion, &bajar_registro);
	config_destroy(archivo);
	fclose(f);
	free(nom);
}


int ultimo_bloque(t_config* archivo){
	char* lista_bloques = config_get_string_value(archivo, "BLOCKS");
	char* s_ult_bloque = string_substring(lista_bloques,strlen(lista_bloques)-2,1);
	int ult_bloque = atoi(s_ult_bloque);
	free(s_ult_bloque);
	free(lista_bloques);
	return ult_bloque;
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
				imprimirContenidoArchivo(ruta_bloque, loggear_debug);
				archivo_bloque = fopen(ruta_bloque, "r");
				} else
				status = 0; //corta el while
				break;


			case '\n': //registro completo.
				strcat(buffer, "\n");
				registro_t* registro = obtener_registro(buffer);
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

void agregar_por_ts( t_dictionary* dic, registro_t* reg_nuevo){  //Esto me tiro mil warnings..
	char* key_nueva = string_itoa(reg_nuevo->key);
	registro_t* reg_viejo= (registro_t*)dictionary_get( (t_dictionary*)dic,key_nueva);
	if( (!reg_viejo) || ((reg_viejo != NULL) && (reg_viejo->timestamp < reg_nuevo->timestamp) ))
	dictionary_put((t_dictionary*)dic, key_nueva, reg_nuevo);
	//TODO Verificar si genera memory leaks al hacer el put !! No se si lo pisa o se pierde la referencia.
}


t_list* listar_archivos(char* tabla){
	loggear_trace(string_from_format("---Entre a listar_archivos de la tabla %s---\n", tabla));
	char* ruta_tabla =  obtener_ruta_tabla(tabla);

	DIR* directorio = opendir(ruta_tabla);
	if (directorio == NULL) {
		loggear_error(string_from_format("Error: No se pudo abrir el directorio"));
		//free(ruta_tabla);
		exit(2);
	}
	else{
		struct dirent* directorio_leido;
		t_list* archivos = list_create();

		char* archivo;
		while((directorio_leido = readdir(directorio)) != NULL) {
			archivo = directorio_leido->d_name;
			if(string_contains(archivo, ".")) {
				char* ruta_archivo = string_from_format("%s/%s", ruta_tabla, archivo);
				list_add(archivos, ruta_archivo);
				loggear_debug(string_from_format("la ruta del archivo leido es: %s", archivo));
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



int pasar_a_tmpc(char* tabla) {

	char* ruta_tabla = obtener_ruta_tabla(tabla);
	loggear_trace(string_from_format("RUTA TABLA: %s\n", ruta_tabla));
	DIR* directorio = opendir(ruta_tabla);
	if (directorio == NULL) {
		loggear_error(string_from_format("Error: No se pudo abrir el directorio"));
		//free(ruta_tabla);
		exit(2);
	}
	struct dirent* directorio_leido;
	int contador =0;
	while((directorio_leido = readdir(directorio)) != NULL) {
		char* nombre_archivo = directorio_leido->d_name;
		loggear_debug(string_from_format("Nombre archivo: %s\n", nombre_archivo));
		if(string_ends_with(nombre_archivo, ".tmp")) {
			loggear_trace(string_from_format("Es un tmp"));
			char* nombre_viejo = string_from_format("%s/%s", ruta_tabla, nombre_archivo);
			loggear_trace(string_from_format("Nombre viejo: %s\n", nombre_viejo));
			int length  = strlen(nombre_archivo);
			loggear_trace(string_from_format("Letras del nombre: %d\n", length));
			char* nombre_sin_ext = string_substring_until(nombre_archivo, length-4);
			loggear_trace(string_from_format("Nombre sin extension: %s\n", nombre_sin_ext));
			char* nuevo_nombre = string_from_format("%s/%s.tmpc", ruta_tabla, nombre_sin_ext);
			loggear_info(string_from_format("Nuevo nombre: %s\n", nuevo_nombre));
			int status = rename(nombre_viejo, nuevo_nombre);

			contador++;
			loggear_trace(string_from_format("Status: %d\n", status));
		}
	}
	return contador;
	//resetear_numero_dump(tabla);
}


