//--------compactador.c--------

#include "compactador.h"
//
//void compactador(){// ver que conviene que reciba de parametro
//	//iniciar con detach a partir del CREATE o de la lectura cuando se inicia el FS.
//
//	t_dictionary* reg_x_particion= malloc(sizeof(t_dictionary));
//
//	int t = (int)obtener_tiempo_compactacion(tabla); //en segundos
//
//	//Esto no se si dejarlo aca o que entre al while, asi no ocupa memoria si es que no hay nada para compactar.. TODO!
//	int cantidad_particiones = obtener_particiones(tabla); //dev la cantidad de particiones
//	char* nomb_part;
//	for(int num = 0; num < cantidad_particiones; num++) {
//		nomb_part = string_from_format("Part%d", num);
//		dictionary_put(reg_x_particion, nomb_part, list_create());
//		free(nomb_part);
//	} //deja los registros vacios..
//
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
//
//	}
//
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
//
//	while(existe_tabla(tabla)){
//		sleep(t);
//
//		//Iniciar cronometro: Enunciado dice que hay que registrar tiempo de la duracion de la compactacion.
//
//
//		if(hay_tmpc(tabla)){
//			//sem_wait(todos los tmp);
//			renombrar_todos_tmp_a_tmpc(tabla);
//			formatear_contador_dump(tabla); //esta ya existe
//			//sem_post(todos los tmp);
//		}
//		else{
//			continue;
//		}
//
//		char *bloques_a_compactar = bloques_totales_tmpc(tabla); //devuelve el array con todos los num de bloques que debe leer de los tmpc
//
//		t_list* registros_totales = leer_registros(bloques_a_compactar); //junta todos los registros de todos los tmpc.
//
//		separar_registros_x_part(reg_x_particion, registros_totales);//Segun el modulo de la key y la particion, las agrupa.
//
//		dictionary_iterator(reg_x_particion, &filtrar_keys_repetidas);
//		//filtro para despues tratar cada registro individualmente y preocuparme solo por actualizar o insertar.
//
//		dictionary_iterator(reg_x_particion, &actualizar_registros_de_la_particion);
//		//ya tendria todo en reg_x_particion.
//
//		bloquear(tabla);
//
//		dictionary_iterator(reg_x_particion, &finalizar_compactacion);
//
//		desbloquear(tabla);
//
//		vaciar_listas_registros(reg_x_particion);
//
//		finalizar_tmpcs(tabla); //Libera bloques, y borra los archivos en el directorio.
//
//		//loggear duracion de la compactacion.
//	}
//
//
//
//	liberar_recursos(tabla);
//	//free(de todo lo que use);
//}


t_list* comparar_registro_a_registro(t_list* registros_tmpc, t_list* registros_bin){

	return NULL;
}

void filtrar_keys_repetidas(char* particion, t_list* registros_tmpc){


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
