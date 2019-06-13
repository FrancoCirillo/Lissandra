/*COMPACTADOR*/

#include "compactador.h"


//Hablado el sabado: pseudocodigo
//maniana sigo

void compactador(){
	//t = obtener_tiempo_compactacion();
	while(1){
	//	sleep(t);
		//listar_y_marcar(tabla);//
	}
}
void compactar(){
	//pthread_create(hilo, tabla, compactar_tabla);

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


//compactacion por cada tabla.
/*
void* compactar(instr_t* i){
	while(existe_tabla(i->param1)){
		sleep(atoi(i->param2));
		compactation(i->param1);
	}
	return NULL;
}
 */
