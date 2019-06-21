//--------estructuras.c--------

#include "Estructuras.h"


//Todo
/*
 * eliminar carpeta
 * eliminar directorio
 * copiar archivo (para los tmpc)
 * archivo_modificar_size();	//escribir nuevo numero en el archivo.
 * archivo_agregar_bloque();    //en el array de un archivo
 * archivo_liberar_bloque();	//en el array de un archivo
 * bloque disponible (bits arrays)
 * semaforos!
 *
 * ver actualizacion RETARDO del Archivo de config. Se actualiza en cada fin de instruccion?
 * \*/

//Pendientes Dai:
//1) Ver si el g_logger global no hace problemas..
//	 Si se crea una vez sola o ahi en el loggear_FS esta bien.


int obtener_tiempo_dump_config() {
	return (int) config_FS.tiempo_dump * 1000;
}

////---------------------------SEMAFOROS---------------------------
//
////TODO: Semaforos
//void crear_dic_semaforos_FS(){
//	dic_semaforos_tablas = dictionary_create();
//}
//
//void agregar_mutex_a_dic(char* tabla, sem_t* mutex_tabla){
//	dictionary_put(dic_semaforos_tablas, tabla, mutex_tabla);
//	puts("Se agrego el semaforo en el diccionario.");
//}
//
//sem_t* obtener_mutex_tabla(char* tabla){
//	puts("---Buscando mutex---");
//	return (sem_t*) dictionary_get(dic_semaforos_tablas, tabla);
//}

//---------------------------+DIRECTORIO---------------------------

int eliminar_directorio(char* tabla) {
	char* ruta_tabla = obtener_ruta_tabla(tabla);

	DIR* directorio = opendir(ruta_tabla);
	if(directorio == NULL)
		return -1;

	struct dirent* dir_a_eliminar;
	size_t path_len = strlen(ruta_tabla);
	int eliminado = 0;

	while (!eliminado && (dir_a_eliminar = readdir(directorio))){
		int removed = -1;

		//Se saltean "." y ".."
		if (!strcmp(dir_a_eliminar->d_name, ".") || !strcmp(dir_a_eliminar->d_name, ".."))
			continue;

		size_t length = path_len + strlen(dir_a_eliminar->d_name) + 2;
		char* arch_a_eliminar = malloc(length);

		if(arch_a_eliminar){
			snprintf(arch_a_eliminar, length, "%s/%s", ruta_tabla, dir_a_eliminar->d_name);

           	if(!string_ends_with(arch_a_eliminar, "Metadata"))
           		liberar_bloques(arch_a_eliminar);

        	removed = unlink(arch_a_eliminar);

           	free(arch_a_eliminar);
		}
		eliminado = removed;
	}

	closedir(directorio);

	if(!eliminado)
		eliminado = rmdir(ruta_tabla);

	//frees
	return eliminado;
}

//---------------------------METADATA---------------------------
char* obtener_path_metadata(char* tabla) {
	return string_from_format("%s%s/Metadata", g_ruta.tablas, tabla);
}

t_config* obtener_metadata(char* tabla) {
	char* path = obtener_path_metadata(tabla);
	t_config* metadata = config_create(path);
	free(path);
	return metadata;
}

int obtener_part_metadata(char* tabla) {
	t_config* metadata = obtener_metadata(tabla);
	int particiones = config_get_int_value(metadata, "PARTITIONS");
	config_destroy(metadata);
	return particiones;
}

char* obtener_consistencia_metadata(char* tabla) {
	t_config* metadata = obtener_metadata(tabla);
	char* consistencia = config_get_string_value(metadata, "CONSISTENCY");
	return consistencia; //config_destroy(metadata);
}

int obtener_tiempo_compactacion_metadata(char* tabla) {
	t_config* metadata = obtener_metadata(tabla);
	int tiempo_compactacion = config_get_int_value(metadata, "COMPACTATION_TIME");
	config_destroy(metadata);
	return tiempo_compactacion;
}

//---------------------------BLOQUES---------------------------
char* formatear_registro(registro_t* registro) {
	uint16_t key = registro->key;
	char* value = registro->value;
	char* ts = mseg_a_string(registro->timestamp);
	return string_from_format("%s;%d;%s\n", ts, key, value);
}

int tam_registro(registro_t* registro) {
//	puts("----------------------Entre a tam_registro---------------------");
	char* registro_formateado = formatear_registro(registro);
	int tam_registro = strlen(registro_formateado);
	free(registro_formateado);
	return tam_registro;
}

char* obtener_ruta_bloque(int nro_bloque) {
	return string_from_format("%s%d.bin", g_ruta.bloques, nro_bloque);

}

void escribir_registro_bloque(registro_t* registro, char* ruta_bloque, char* ruta_archivo) {
//	puts("-----------Entre a escribir_registro_bloque-------------------");
	FILE* archivo_bloque = txt_open_for_append(ruta_bloque);
	char* string_registro = formatear_registro(registro);
	printf("Ruta bloque: %s\n", ruta_bloque);
//	printf("Formateo registro: %s\n", string_registro);
//	int tam = strlen(string_registro);
//	printf("Tam registro: %d\n", tam);
//	int espBloque = espacio_restante_bloque(ruta_archivo);
//	printf("Espacio restante Bloque: %d\n", espBloque);

	if(strlen(string_registro) <= espacio_restante_bloque(ruta_archivo)) {
		txt_write_in_file(archivo_bloque, string_registro);
//		puts("Escribo el registro completo");
	} else {
		if(cant_bloques_disponibles() == 0)
			return; //TODO log: no hay bloques disponibles

		int tam_restante = espacio_restante_bloque(ruta_archivo);
		char* primera_mitad_registro = string_substring_until(string_registro, tam_restante);
		txt_write_in_file(archivo_bloque, primera_mitad_registro);

		int nro_bloque = siguiente_bloque_disponible();
		agregar_bloque_archivo(ruta_archivo, nro_bloque);
		ocupar_bloque(nro_bloque);

		char* nuevo_bloque = obtener_ruta_bloque(nro_bloque);
		FILE* archivo_nuevo_bloque = txt_open_for_append(nuevo_bloque);
		char* mitad_restante_registro = string_substring_from(string_registro, tam_restante);
		txt_write_in_file(archivo_nuevo_bloque, mitad_restante_registro);
		printf("Ruta bloque: %s\n", nuevo_bloque);

		txt_close_file(archivo_nuevo_bloque);
		free(primera_mitad_registro);
		free(nuevo_bloque);
		free(mitad_restante_registro);
	}

	aumentar_tam_archivo(ruta_archivo, registro);
	puts("Aumente tam archivo");
	txt_close_file(archivo_bloque);
	free(string_registro);
}

int obtener_siguiente_bloque_archivo(char* ruta_archivo, int nro_bloque) {
//	puts("-----------Entre a obtener_siguiente_bloque_archivo-------------------");
//	printf("RUTA ARCHIVO: %s\tNRO BLOQUE: %d\n", ruta_archivo, nro_bloque);
	t_config* archivo = config_create(ruta_archivo);
	char** lista_bloques = config_get_array_value(archivo, "BLOCKS");

	if(nro_bloque == -1) {
		char* bloque = lista_bloques[0];
//		printf("Primer bloque como string: %s\n", bloque);
		int mi_bloque = atoi(bloque);
//		printf("Primer bloque como int: %d\n", mi_bloque);
		config_destroy(archivo);
		return mi_bloque;
	} else {
		int bloques_usados = cantidad_bloques_usados(ruta_archivo);
		char* mi_bloque = string_itoa(nro_bloque);
		for(int tam = 0; *(lista_bloques + tam); tam++) {
//			printf("Entre al for, busco al bloque %s\n", mi_bloque);
			char* bloque = *(lista_bloques + tam);
//			printf("Bloque como string: %s\n", bloque);
//			char* sig_bloque = *(lista_bloques + tam +1);
//			printf("Siguiente bloque como string: %s\n", sig_bloque);

			if(string_equals_ignore_case(bloque, mi_bloque) && tam+1 < bloques_usados) {
//				printf("Entre al if\t%s\t%s\n", bloque, mi_bloque);
				char* sig_bloque = *(lista_bloques + tam +1);
				int bloque_siguiente = atoi(sig_bloque);
//				printf("Siguiente Bloque como int: %d\n", bloque_siguiente);
				config_destroy(archivo);
				return bloque_siguiente;
			}
		}
    }
    config_destroy(archivo);
    return -1;
}

registro_t* obtener_registro(char* buffer) {
//	puts("---OBTENER REGISTRO---");
	char* bufferCopy = strdup(buffer);
	registro_t* registro = malloc(sizeof(registro_t));
	char* valor;

	char* actual = strtok(bufferCopy, ";");
	valor = strdup(actual);
	registro->timestamp = string_a_mseg(valor);

	actual = strtok(NULL, ";");
	valor = strdup(actual);
	registro->key = (uint16_t)atoi(valor);

	actual = strtok(NULL, "\n");
	valor = strdup(actual);
	registro->value = valor;

	free(bufferCopy);
//	puts("Pase el registro formateado a registro");
	return registro;
}

t_list* buscar_key_en_bloques(char* ruta_archivo, uint16_t key, int tipo_archivo) { //Tipo archivo: si es .bin=0, .tmp=1
	puts("Entre a buscar_key_en_bloques");
	int nro_bloque = obtener_siguiente_bloque_archivo(ruta_archivo, -1);
	char* ruta_bloque = obtener_ruta_bloque(nro_bloque);
imprimirContenidoArchivo(ruta_bloque);
	FILE* archivo_bloque = fopen(ruta_bloque, "r");
	t_list* registros = crear_lista_registros();
	int status = 1;
	char* buffer = string_new();
	char* s_caracter;
	char caracter_leido;

	while(status) {
		caracter_leido = fgetc(archivo_bloque);

		switch(caracter_leido) {
		case '\n': //tengo un registro completo
			strcat(buffer, "\n");
			registro_t* registro = obtener_registro(buffer);

			if(registro->key == key) {
				list_add(registros, registro); //lo agrego solo si tiene la key que busco
				status = tipo_archivo; //si es binario, se pone en 0 y corta el while
			}
			strcpy(buffer, "");
			break;

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

		default:
			s_caracter = string_from_format("%c", caracter_leido);
			strcat(buffer, s_caracter);
			break;
		}
	}
	return registros; //En la funcion que lo llamo, tengo que validar que no este vacio y destruir la lista
}

//---------------------------BITARRAY---------------------------

int cant_bytes() {
	return (Metadata_FS.blocks + 7) / 8;
}

void inicializar_bitmap() {
//	puts("inicializar bitmap");
	if(carpeta_esta_vacia(g_ruta.carpeta_metadata)) {
		FILE* archivo_bitmap = fopen(g_ruta.bitmap, "w+");
		//char* bitmap = string_repeat(0, cant_bytes());
		printf("Iniciando bitmap con %d bloques, bytes %d\n", Metadata_FS.blocks,cant_bytes());
		//fwrite(bitmap, sizeof(char), sizeof(char)*strlen(bitmap), archivo_bitmap);

		fclose(archivo_bitmap);
		truncate(g_ruta.bitmap, cant_bytes());
		//free(bitmap);
	}
	else
		puts("Bitmap ya creado");
}

void chequear_bitmap(t_bitarray* bitarray) {
	int i=0;
	puts("\n###CHEQUEANDO BITMAP");
	while(i < Metadata_FS.blocks){
		if(bloque_esta_ocupado(bitarray, i))
			printf("%d,", i);
		i++;
	}
	puts("\n#### FIN CHEQUEO");
}

t_bitarray* get_bitmap() {
	//puts("Get bitmap");
	FILE* archivo_bitmap = fopen(g_ruta.bitmap, "r");
	char* bitmap = malloc(cant_bytes() + 1);
	int resultado_read = fread(bitmap, sizeof(char), sizeof(char)*cant_bytes()+1, archivo_bitmap);
	//TODO: resultado_read
	bitmap[cant_bytes()] = 0;
	t_bitarray* bitarray = bitarray_create_with_mode(bitmap, cant_bytes(), LSB_FIRST);
	fclose(archivo_bitmap);
	//TODO free(bitmap);

	return bitarray;
}

void actualizar_bitmap(t_bitarray* bitarray) {
	FILE* bitmap = fopen(g_ruta.bitmap, "w+");
//	printf("Escribiendo archivo %d bytes\n", strlen(bitarray->bitarray));
	fwrite(bitarray->bitarray, sizeof(char), sizeof(char)*cant_bytes(), bitmap);
//	puts("Cerrando archivo");
	fclose(bitmap);
}

void inicializar_bloques_disp() {
	int nro_bloque = 0;
	int contador_disponibles = 0;
	t_bitarray* bitmap = get_bitmap();
	while(nro_bloque < Metadata_FS.blocks) {
		if(!bloque_esta_ocupado(bitmap, nro_bloque)) {
			contador_disponibles++;
		} else {
			//printf(" %d,",nro_bloque);
		}
		nro_bloque++;
	}
	eliminar_bitarray(bitmap);
	bloques_disponibles += contador_disponibles;
}

void eliminar_bitarray(t_bitarray* bitarray) {
	bitarray_destroy(bitarray);
}

int bloque_esta_ocupado(t_bitarray* bitmap, int nro_bloque) {
	int test = bitarray_test_bit(bitmap, nro_bloque);
	//printf("El valor del bloque %d es %d\n", nro_bloque, test);
	return test;
}

int siguiente_bloque_disponible() {
	puts("Entre a sig bloque disp");
	int nro_bloque = 0;
	t_bitarray* bitmap = get_bitmap();
	while(nro_bloque < Metadata_FS.blocks && bloque_esta_ocupado(bitmap,nro_bloque))
		nro_bloque++;
	if(nro_bloque == Metadata_FS.blocks) {
		return -1;
	}
	eliminar_bitarray(bitmap);
	return nro_bloque;
}

void ocupar_bloque(int nro_bloque) {
	t_bitarray* bitarray = get_bitmap();
//	printf("\nBitmap levantado, seteando valor para bloque %d\n",nro_bloque);
	bitarray_set_bit(bitarray, nro_bloque);
//	puts("Actualizando bitmap");
	actualizar_bitmap(bitarray);
//	puts("Liberando estructura");
	eliminar_bitarray(bitarray);
}

void liberar_bloque(int nro_bloque) {
//	printf("Liberando bloque %d\n",nro_bloque);
	t_bitarray* bitarray = get_bitmap();
	bitarray_clean_bit(bitarray, nro_bloque);
	actualizar_bitmap(bitarray);
	eliminar_bitarray(bitarray);
	//limpiar_bloque(nro_bloque); //deja vacio el archivo
}

void ejemplo_bitarray(){
	inicializar_bitmap();
	puts("Iniciado!\n");
	printf("Cantidad de bloques disponibles %d",cant_bloques_disponibles());
	ocupar_bloque(0);
//	printf("\nPost ocupar 0 , disp: %d\n",cant_bloques_disp());
	ocupar_bloque(1);
	ocupar_bloque(3);
	ocupar_bloque(4);
	ocupar_bloque(54);
	printf("\nPost ocupar cantidad de bloques disponibles %d",cant_bloques_disponibles());
	printf("\nPrimer bloque disponible: %d\n",siguiente_bloque_disponible());
	liberar_bloque(3);
	printf("\nPost liberar cantidad de bloques disponibles %d\n",cant_bloques_disponibles());
	sleep(2);
}

//--------------------------APLANAR LISTAS-----------------------
char* aplanar(char** lista) {
	int tam = 0;
	while(*(lista + tam))
		tam++;

    int sum = 0;
    int count = 0;
//    printf("Length %d\n",tam);
    for (int i = 0; i < tam; i++) {
    	sum += strlen(lista[i]);
    }
    sum++;  // Make room for terminating null character
    sum += 1; //Sumo el espacio para corchete inicial, al final reemplazo ultima coma por ]

    char* buf;
    if ((buf = calloc(sum, sizeof(char))) != NULL) {
    	*buf = '[';
    	count++;
        for (int i = 0; i < tam; i++) {
          	memcpy(buf + count, lista[i], strlen(lista[i]));
        	count += strlen(lista[i]) + 1;
           	buf[count-1] = ',';
        }
        buf[count-1]=']';
    }
    return buf;
}

void ejemplo_aplanar() {
	char* a = "Hola ";
	char* b = "como ";
	char* c = "te ";
	char* d = "va";
	char** array = malloc(sizeof(char*)*5);
	*array = a;
	*(array + 1) = b;
	*(array + 2) = c;
	*(array + 3) = d;
	*(array + 4) = NULL;
	printf("La lista aplanada es %s\n", aplanar(array));
}

//---------------------------TMP Y BIN---------------------------
char* agregar_bloque_bloques(char** lista_s_bloques, int bloque) {
	puts("-------------------Entre a agregar_bloque_bloques-------------------");
	printf("Bloques antes de hacer nada: %s\n", aplanar(lista_s_bloques));
	char* s_bloque = string_from_format(",%d]", bloque);
	printf("Bloque como String: %s\n", s_bloque);
	char* bloques_viejos = aplanar(lista_s_bloques);
	printf("Bloque viejos: %s\n", bloques_viejos);
	int longitud = strlen(bloques_viejos);
	printf("Longitud: %d\n", longitud);
	char* mis_bloques = string_substring_until(bloques_viejos, longitud-1);
	printf("Bloques sin ]: %s\n", mis_bloques);
	strcat(mis_bloques, s_bloque);
	printf("Mis bloques finales: %s\n", mis_bloques);
	return mis_bloques;
}

FILE* crear_tmp(char* tabla, char* nombre_tmp) {
	FILE* temporal = crear_archivo(nombre_tmp, tabla, ".tmp");
	return temporal;
}

int agregar_bloque_archivo(char* ruta_archivo, int nro_bloque) {
	if(cant_bloques_disponibles() == 0) {
		return 0; //TODO log: no hay bloques disponibles
	}
	t_config* archivo = config_create(ruta_archivo);
	char** bloques_ant = config_get_array_value(archivo, "BLOCKS");
	char* bloques_tot = agregar_bloque_bloques(bloques_ant, nro_bloque);
	config_set_value(archivo, "BLOCKS", bloques_tot);
	config_save(archivo);
	restar_bloques_disponibles(1);
	config_destroy(archivo);
	free(bloques_tot);
	return 1;
}

int obtener_ultimo_bloque(char* ruta_archivo){
	t_config* archivo = config_create(ruta_archivo);
	char** bloques = config_get_array_value(archivo, "BLOCKS");
	int tam = cantidad_bloques_usados(ruta_archivo);
	printf("tam bloques: %d\n", tam);
	int ultimo = tam;
	ultimo--;
	printf("ultimo: %d\n", ultimo);
	char* ultimo_bloque = bloques[ultimo];
	printf("char bloque: %s\n", ultimo_bloque);
	int rdo = atoi(ultimo_bloque);
	return rdo;
}

int obtener_tam_archivo(char* ruta_archivo) {
//	puts("-------------------Entre a obtener_tam_archivo-------------------");
//	printf("Ruta archivo: %s\n", ruta_archivo);
	t_config* archivo = config_create(ruta_archivo);
//	puts("Config create");
	int tam_archivo = config_get_int_value(archivo, "SIZE");
//	puts("config_get_int_value");
	config_destroy(archivo);
//	puts("Config destroy");
	return tam_archivo;
}

void aumentar_tam_archivo(char* ruta_archivo, registro_t* registro) {
//	puts("-------------------Entre a aumentar_tam_archivo-------------------");
	t_config* archivo = config_create(ruta_archivo);
//	puts("config create");
	int tam_viejo = config_get_int_value(archivo, "SIZE");
//	puts("config get int value");
	int tam_nuevo = tam_viejo + tam_registro(registro);
	printf("Tam viejo: %d\tTam Registro: %d\tTam nuevo: %d\n", tam_viejo, tam_registro(registro), tam_nuevo);
	char* tam = string_itoa(tam_nuevo);
//	printf("%s\n", tam);
	config_set_value(archivo, "SIZE", tam);
//	puts("config set value");
	config_save(archivo);
//	puts("config save");
	config_destroy(archivo);
	free(tam);
}

int cantidad_bloques_usados(char* ruta_archivo) {
//	puts("-------------------Entre a cantidad_bloques_usados-------------------");
	t_config* archivo = config_create(ruta_archivo);
//	puts("config create");
	char** lista_bloques = config_get_array_value(archivo, "BLOCKS");
//	puts("config get array value");
	int cant_bloques = 0;
	while(*(lista_bloques + cant_bloques))
		cant_bloques++;
//    printf("Cantidad de Bloques usados: %d\n",cant_bloques);
	return cant_bloques;
}

int espacio_restante_bloque(char* ruta_archivo) {
//	puts("-------------------Entre a espacio_restante_bloque-------------------");
	int tamBloque = Metadata_FS.block_size;
	printf("Tam maximo bloque: %d\n", tamBloque);
	int tamArchivo = obtener_tam_archivo(ruta_archivo);
	printf("Tam archivo: %d\n", tamArchivo);
	int bloquesUsados = cantidad_bloques_usados(ruta_archivo);
	int espacio_disponible = tamBloque*bloquesUsados - tamArchivo;
//	int espacio_disponible = cantidad_bloques_usados(ruta_archivo) * Metadata_FS.block_size - obtener_tam_archivo(ruta_archivo);
	printf("Espacio Disponible: %d\n", espacio_disponible);
	return espacio_disponible;
}

void liberar_bloques(char* ruta_archivo) {
	int nro_bloque = -1;
	int bloque = obtener_siguiente_bloque_archivo(ruta_archivo, nro_bloque);
//	printf("Nro bloque: %d\t Nro siguiente: %d\n", nro_bloque, bloque);
	do {
		liberar_bloque(bloque);
		incrementar_bloques_disponibles(1);
//		printf("Libere el bloque: %d\n", bloque);
		bloque = obtener_siguiente_bloque_archivo(ruta_archivo, bloque);
//		printf("Nro siguiente: %d\n", bloque);

	}
	while (bloque > 0);
//	puts("sali del while");
}

//---------------------------DIRECTORIOS---------------------------
int carpeta_esta_vacia(char* ruta_carpeta) {
//	puts("Carpeta esta vacia");
	int n = 0;
	DIR* carpeta = opendir(ruta_carpeta); //No valido que no existe, siempre va a existir
	struct dirent* contenido;

	while ((contenido = readdir(carpeta)) != NULL) {
		if(++n > 3)
			break;
	}
	closedir(carpeta);
	if (n <= 3) //Directorio vacio (siempre estan . y ..) el 3 es para que funcione con el bitmap, que tiene el archivo de metadata tambien
		return 1;
	else
		return 0; //Tiene archivos
}


//*****************************************************************
//De aca para abajo estan corregidos y funcionan sin memory leaks.
//*****************************************************************

int cant_bloques_disponibles() {
	sem_wait(&mutex_cant_bloques);
	int cantidad = bloques_disponibles;
	sem_post(&mutex_cant_bloques);

	return cantidad;
}

void restar_bloques_disponibles(int num) {
	sem_wait(&mutex_cant_bloques);
	bloques_disponibles = bloques_disponibles - num;
	sem_post(&mutex_cant_bloques);

}

void incrementar_bloques_disponibles(int num) {
	sem_wait(&mutex_cant_bloques);
	bloques_disponibles = bloques_disponibles + num;
	sem_post(&mutex_cant_bloques);

}

int puede_crear_particiones(instr_t* i) {
	int particiones = atoi(obtener_parametro(i, 2));
	int resultado;
	sem_wait(&mutex_cant_bloques);
	resultado = (bloques_disponibles >= particiones);
	if(resultado) {
		bloques_disponibles= bloques_disponibles - particiones;// Esto reserva la cantidad de bloques para que finalice bien el CREATE.
	}
	sem_post(&mutex_cant_bloques);
	return resultado;
}

void crear_particiones(instr_t* instr) {
	int cantidad = atoi(obtener_parametro(instr, 2));
	char* tabla = obtener_nombre_tabla(instr);
	char* nomb_part ;

	for(int num = 0; num < cantidad; num++) {
		nomb_part = string_from_format("Part%d", num);
		FILE* archivo_binario = crear_archivo(nomb_part, tabla, ".bin");
		archivo_inicializar(archivo_binario);
		free(nomb_part);
		fclose(archivo_binario);
	}

	char* mensaje = string_from_format("Se crearon las particiones de la tabla \"%s\" correctamente.", tabla );
	loggear_FS(mensaje);
	free(mensaje);
}

void crear_metadata(instr_t* instr) {
	char* tabla = obtener_nombre_tabla(instr);
	FILE* archivo_metadata = crear_archivo("Metadata", tabla, "");
	metadata_inicializar(archivo_metadata, instr);
	fclose(archivo_metadata);
	char* mensaje = string_from_format("Se creó el metadata en la tabla \"%s\".", tabla);
	loggear_FS(mensaje);
	free(mensaje);
}

void metadata_inicializar(FILE* f, instr_t* instr) {
	char* consist = obtener_parametro(instr, 1);
	char* part = obtener_parametro(instr, 2);
	char* time = obtener_parametro(instr, 3);
	fprintf(f, "%s%s%s%s%s%s%s", "CONSISTENCY=", consist, "\nPARTITIONS=", part, "\nCOMPACTATION_TIME=", time, "\n");
}

int archivo_inicializar(FILE* f) {
	puts("-------------------Entre a archivo_inicializar-------------------");
	int bloque_num = siguiente_bloque_disponible();
	char* contenido = string_from_format("SIZE=%d\nBLOCKS=[%d]\n", 0, bloque_num);
	txt_write_in_file(f, contenido);
	ocupar_bloque(bloque_num);
	restar_bloques_disponibles(1);

//	printf("Numero de bloque: %d\n", bloque_num);
	return bloque_num;
}

FILE* crear_archivo(char* nombre, char* tabla, char* ext) {
	char* ruta = string_from_format("%s%s/%s%s", g_ruta.tablas, tabla, nombre, ext);
	FILE* archivo = fopen(ruta, "w+"); //Modo: lo crea vacio para lectura y escritura. Si existe borra lo anterior.
	free(ruta);
	return archivo;
}

void crear_directorio(char* ruta, char* nombre) {
	char* ruta_dir = string_from_format("%s%s", ruta, nombre);

	if (!mkdir(ruta_dir, S_IRWXU)) {
		char* mensaje = string_from_format("Se creó correctamente la carpeta \"%s\" en el directorio %s", nombre, ruta);
		loggear_FS(mensaje);
		free(ruta_dir);
		free(mensaje);
	} else {
		char* mensaje = string_from_format("No se creó la carpeta \"%s\". Ya existe.", nombre);
		loggear_FS(mensaje);
		free(ruta_dir);
		free(mensaje);
	}
}

void crear_bloques() {  //Los bloques van a partir del numero 0.
	char* ruta_bloques = string_from_format("%s", g_ruta.bloques);
	if(carpeta_esta_vacia(ruta_bloques)) {
		puts("La carpeta no esta creada");
		int cantidad = Metadata_FS.blocks;
		bloques_disponibles = cantidad;
		char* num;
		for (int i = 0; i <= cantidad; i++) {
			num = string_itoa(i);
			crear_bloque(num);
			free(num);
		}
		loggear_FS("Se crearon los bloques del File System.");
	} else
		loggear_FS("No se crearon los bloques, ya existen.");
}

void crear_bloque(char* nombre) {
	char* ruta = string_from_format("%s%s.bin", g_ruta.bloques, nombre);
	FILE* f = fopen(ruta, "w+");
	free(ruta);
	fclose(f);
}

void loggear_FS(char* valor) {
	sem_wait(&mutex_log);
	log_info(g_logger, valor);
	sem_post(&mutex_log);

	printf("--------------\n");
}

void loggear_FS_error(char* valor, instr_t* i) {

	sem_wait(&mutex_log);
	log_error(g_logger, valor);
	sem_post(&mutex_log);

	list_replace(i->parametros, 0, valor);

	printf("--------------\n");

	//Esto no genera memory leak, pisa el parametro bien. testeado!
}

//char* obtener_por_clave(char* ruta, char* clave) {
//	t_config* c = config_create(ruta);
//	char* valor;
//	valor = config_get_string_value(c, clave);
//	config_destroy(c);
//	return valor;
//}

void inicializar_configuracion(void) {
	rutaConfiguracion = "Lissandra.config";
	g_config = config_create(rutaConfiguracion);
	config_FS.punto_montaje = config_get_string_value(g_config, "PUNTO_MONTAJE");
	config_FS.puerto_escucha = config_get_string_value(g_config, "PUERTO_ESCUCHA");
	config_FS.tamanio_value = config_get_int_value(g_config, "TAMAÑO_VALUE");
	config_FS.retardo = (mseg_t)config_get_int_value(g_config, "RETARDO");
	config_FS.tiempo_dump = (mseg_t)config_get_int_value(g_config, "TIEMPO_DUMP");

	loggear_FS("Se leyó el archivo de configuración");
}

void iniciar_logger(){
	g_logger = log_create("Lissandra.log", "File System", 1, LOG_LEVEL_TRACE);
}

//void actualizar_tiempo_dump_config(mseg_t value) {
//
//	char* val = string_itoa((int) value);
//
//	sem_wait(&mutex_tiempo_dump_config);
//	config_set_value(g_config, "TIEMPO_DUMP", val);
//	sem_post(&mutex_tiempo_dump_config);
//
//	config_save(g_config);
//
//	char* mensaje = string_from_format("Se actualizo el tiempo de dumpeo en el archivo de configuracion. Nuevo valor: %s", val);
//	loggear_FS(mensaje);
//	free(mensaje);
//	free(val);
//}
//
//void actualizar_tiempo_retardo_config(mseg_t value) {
//
//	char* val = string_itoa( value);
//
//	sem_wait(&mutex_tiempo_retardo_config);
//	config_set_value(g_config, "TIEMPO_RETARDO", val);
//	sem_post(&mutex_tiempo_retardo_config);
//
//	config_save(g_config);
//
//	char* mensaje = string_from_format("Se actualizo el tiempo de retardo en el archivo de configuracion. Nuevo valor: %s", val);
//	loggear_FS(mensaje);
//	free(mensaje);
//	free(val);
//
//}

void inicializar_directorios() {

	crear_directorio(config_FS.punto_montaje, "Metadata");
	crear_directorio(config_FS.punto_montaje, "Tablas");
	crear_directorio(config_FS.punto_montaje, "Bloques");

	loggear_FS("Directorios listos.");

	leer_metadata_FS();

}

void leer_metadata_FS() {

	FILE* archivo = fopen(g_ruta.metadata, "w+"); //Modo: lo crea vacio para lectura y escritura. Si existe borra lo anterior.
	Metadata_FS.block_size = config_get_int_value(g_config, "BLOCK_SIZE");
	Metadata_FS.blocks = config_get_int_value(g_config, "BLOCKS");
	Metadata_FS.magic_number = config_get_string_value(g_config, "MAGIC_NUMBER");  //Leo del archivo de configuracion.

	char* mensaje = malloc(sizeof(char) * 200);
	sprintf(mensaje, "Metadata leído. Los datos son:\nBLOCK_SIZE = %d\nBLOCKS = %d\nMAGIC_NUMBER = %s",
			Metadata_FS.block_size, Metadata_FS.blocks,	Metadata_FS.magic_number);
	fprintf(archivo, "BLOCK_SIZE = %d\nBLOCKS = %d\nMAGIC_NUMBER = %s", Metadata_FS.block_size, Metadata_FS.blocks,	Metadata_FS.magic_number);
	fclose(archivo);
	loggear_FS(mensaje);
	free(mensaje);
}

