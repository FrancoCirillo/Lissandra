/*
 * Estructuras.c
 *
 *  Created on: 16 may. 2019
 *      Author: utnso
 */

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
 *NO FUNCIONA: string_to_upper(nomb);   //ver como hacer en crear_directorio().
 * ver actualizacion RETARDO del Archivo de config. Se actualiza en cada fin de instruccion?
 * \*/

//Pendientes Dai:
//1) Ver si el g_logger global no hace problemas..
//	 Si se crea una vez sola o ahi en el loggear_FS esta bien.

int obtener_tiempo_dump_config() {
	return (int) config_FS.tiempo_dump * 1000;
}

void  eliminar_directorio(char* tabla) { //TODO hacer despues

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
	puts("----------------------Entre a tam_registro---------------------");
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
//	printf("Formateo registro: %s\n", string_registro);
//	int tam = strlen(string_registro);
//	printf("Tam registro: %d\n", tam);
//	int espBloque = espacio_restante_bloque(ruta_archivo);
//	printf("Espacio restante Bloque: %d\n", espBloque);

	if(strlen(string_registro) <= espacio_restante_bloque(ruta_archivo)) {
		txt_write_in_file(archivo_bloque, string_registro);
//		puts("Escribo el registro completo");
	}
	else {
		if(cant_bloques_disp() == 0)
			return; //TODO log: no hay bloques disponibles

		int tam_restante = espacio_restante_bloque(ruta_archivo);
		char* primera_mitad_registro = string_substring_until(string_registro, tam_restante);
		txt_write_in_file(archivo_bloque, primera_mitad_registro);
		int nro_bloque = siguiente_bloque_disponible();
		agregar_bloque_archivo(ruta_archivo, nro_bloque);

		char* nuevo_bloque = obtener_ruta_bloque(nro_bloque);
		FILE* archivo_nuevo_bloque = txt_open_for_append(nuevo_bloque);
		char* mitad_restante_registro = string_substring_from(string_registro, tam_restante);
		txt_write_in_file(archivo_nuevo_bloque, mitad_restante_registro);

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
	t_config* archivo = config_create(ruta_archivo);
	char** lista_bloques = config_get_array_value(archivo, "BLOCKS");
	char* bloques = aplanar(lista_bloques);
	config_destroy(archivo);
	char c_bloque = nro_bloque + '0';
    for(int i = 0; i < (strlen(bloques)-1); i++) {
        if(c_bloque == bloques[i])
            return bloques[i + 1] - '0';
    }
    return -1;
}

registro_t* obtener_reg(char* buffer) {
	puts("---OBTENER REGISTRO---");
	char* bufferCopy = strdup(buffer);
	registro_t* registro = malloc(sizeof(registro_t));
	char* token = malloc(sizeof(int));

	char* actual = strtok(bufferCopy, ";");
	token = strdup(actual);
	registro->timestamp = string_a_mseg(token);

	actual = strtok(NULL, ";");
	token = strdup(actual);
	registro->key = (uint16_t)atoi(token);

	actual = strtok(NULL, "\n");
	token = strdup(actual);
	strcpy(registro->value, token);

	free(bufferCopy);
	puts("Pase el registro formateado a registro");
	return registro;
}

void imprimir_reg_fs(registro_t *registro)
{
	printf("Timestamp: %"PRIu64"\n",registro->timestamp);
	printf("Key: %d\n", registro->key);
	printf("Value: %s\n", registro->value);
	puts("");
}

t_list* buscar_key_en_bloques(char* ruta_archivo, uint16_t key, int tipo_archivo) { //Tipo archivo: si es .bin=1, .tmp=0
	int nro_bloque = obtener_siguiente_bloque_archivo(ruta_archivo, -1);
	char* ruta_bloque = obtener_ruta_bloque(nro_bloque);
	FILE* archivo_bloque = fopen(ruta_bloque, "r");
	registro_t* registro = malloc(sizeof(registro));
	t_list* registros = crear_lista_registros();
	int status = 1;
	char* buffer = string_new();
	while(status) {
		char caracter_leido = fgetc(archivo_bloque);
		char* s_caracter;
		switch(caracter_leido) {
		case '\n': //tengo un registro completo
			strcat(buffer, "\n");
			registro = obtener_reg(buffer);
			if(registro->key == key) {
				list_add(registros, registro); //lo agrego solo si tiene la key que busco
				status = tipo_archivo; //si es binario, se pone en 0 y corta el while
			}
			break;
		case EOF: //se me acabo el archivo
			fclose(archivo_bloque);
			int bloque_anterior = nro_bloque;
			nro_bloque = obtener_siguiente_bloque_archivo(ruta_archivo, bloque_anterior);
			if(nro_bloque >= 0) { //si es menor a cero, no hay mas bloques por leer
				ruta_bloque = obtener_ruta_bloque(nro_bloque);
				archivo_bloque = fopen(ruta_bloque, "r");
			} else
				status = 0; //corta el while
			break;
		default:
			s_caracter = string_from_format("%c", caracter_leido);
			strcat(buffer, s_caracter);
			free(s_caracter);
			break;
		}
	}
	return registros; //En la funcion que lo llamo, tengo que validar que no este vacio y destruir la lista
}

//---------------------------BITARRAY---------------------------
char* ruta_bitmap = "archivo.bitmap";
int cantidad_bloques = 8000;

int cant_bytes() {
	return (cantidad_bloques+7)/8;
}

void inicializar_bitmap() {
	FILE* archivo_bitmap = fopen(ruta_bitmap, "w+");
	//char* bitmap = string_repeat(0, cant_bytes());
	printf("Iniciando bitmap con %d bloques, bytes %d\n", cantidad_bloques,cant_bytes());
	//fwrite(bitmap, sizeof(char), sizeof(char)*strlen(bitmap), archivo_bitmap);

	fclose(archivo_bitmap);
	truncate(ruta_bitmap,cant_bytes());
	//free(bitmap);
}

void chequear_bitmap(t_bitarray* bitarray) {
	int i=0;
	puts("\n###CHEQUEANDO BITMAP");
	while(i < cantidad_bloques){
		if(bloque_esta_ocupado(bitarray, i))
			printf("%d,", i);
		i++;
	}
	puts("\n#### FIN CHEQUEO");
}

t_bitarray* get_bitmap() {
	FILE* archivo_bitmap = fopen(ruta_bitmap, "r");
	char*bitmap=malloc(cant_bytes()+1);
	int resultado_read = fread(bitmap, sizeof(char), sizeof(char)*cant_bytes()+1, archivo_bitmap);
	bitmap[cant_bytes()] = 0;
	t_bitarray* bitarray = bitarray_create_with_mode(bitmap, cant_bytes(), LSB_FIRST);
	fclose(archivo_bitmap);
	//TODO free(bitmap);

	return bitarray;
}

void actualizar_bitmap(t_bitarray* bitarray) {
	FILE* bitmap = fopen(ruta_bitmap, "w+");
	printf("Escrbiendo archivo %d bytes\n",strlen(bitarray->bitarray));
	fwrite(bitarray->bitarray, sizeof(char), sizeof(char)*cant_bytes(), bitmap);
	puts("Cerrando archivo");
	fclose(bitmap);
}

int cant_bloques_disp() {
	int nro_bloque = 0;
	int cantidad_disponible = 0;
	t_bitarray* bitmap = get_bitmap();
	while(nro_bloque < cantidad_bloques) {
		if(!bloque_esta_ocupado(bitmap, nro_bloque)) {
			cantidad_disponible++;
		} else {
			//printf(" %d,",nro_bloque);
		}
		nro_bloque++;
	}
	eliminar_bitarray(bitmap);

	return cantidad_disponible;
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
	int nro_bloque = 0;
	t_bitarray* bitmap = get_bitmap();
	while(nro_bloque < cantidad_bloques && bloque_esta_ocupado(bitmap,nro_bloque))
		nro_bloque++;
	if(nro_bloque == cantidad_bloques) {
		return -1;
	}
	eliminar_bitarray(bitmap);
	return nro_bloque;

}

void ocupar_bloque(int nro_bloque) {
	t_bitarray* bitarray = get_bitmap();
	printf("\nBitmap levantado, seteando valor para bloque %d\n",nro_bloque);
	bitarray_set_bit(bitarray, nro_bloque);
	puts("Actualizando bitmap");
	actualizar_bitmap(bitarray);
	puts("Liberando estructura");
	eliminar_bitarray(bitarray);
}

void liberar_bloque(int nro_bloque) {
	printf("Liberando bloque %d\n",nro_bloque);
	t_bitarray* bitarray = get_bitmap();
	bitarray_clean_bit(bitarray, nro_bloque);
	actualizar_bitmap(bitarray);
	eliminar_bitarray(bitarray);
	//limpiar_bloque(nro_bloque); //deja vacio el archivo
}

void ejemplo_bitarray(){
	inicializar_bitmap();
	puts("Iniciado!\n");
	printf("Cantidad de bloques disponibles %d",cant_bloques_disp());
	ocupar_bloque(0);
//	printf("\nPost ocupar 0 , disp: %d\n",cant_bloques_disp());
	ocupar_bloque(1);
	ocupar_bloque(3);
	ocupar_bloque(4);
	ocupar_bloque(54);
	printf("\nPost ocupar cantidad de bloques disponibles %d",cant_bloques_disp());
	printf("\nPrimer bloque disponible: %d\n",siguiente_bloque_disponible());
	liberar_bloque(3);
	printf("\nPost liberar cantidad de bloques disponibles %d\n",cant_bloques_disp());
	sleep(2);
}

//--------------------------APLANAR LISTAS-----------------------
char* aplanar(char** lista) {
	int tam = 0;
	while(*(lista + tam))
		tam++;

    int sum = 0;
    int count = 0;
    printf("Length %d\n",tam);
    for (int i = 0; i < tam; i++) {
    	sum += strlen(lista[i]);
    }
    sum++;  // Make room for terminating null character
    sum += 1; //Sumo el espacio para corchete inicial, al final reemplazo ultima coma por ]

    char* buf;
    if ((buf = calloc(sum, sizeof(char))) != NULL) {
    	*buf='[';
    	count++;
            for (int i = 0; i < tam; i++) {
            	memcpy(buf+count, lista[i], strlen(lista[i]));
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
	if(cant_bloques_disp() == 0) {
		return 0; //TODO log: no hay bloques disponibles
	}
	t_config* archivo = config_create(ruta_archivo);
	char** bloques_ant = config_get_array_value(archivo, "BLOCKS");
	char* bloques_tot = agregar_bloque_bloques(bloques_ant, nro_bloque);
	config_set_value(archivo, "BLOCKS", bloques_tot);
	config_save(archivo);
	config_destroy(archivo);
	free(bloques_tot);
	return 1;
}

int obtener_tam_archivo(char* ruta_archivo) {
	puts("-------------------Entre a obtener_tam_archivo-------------------");
	printf("Ruta archivo: %s\n", ruta_archivo);
	t_config* archivo = config_create(ruta_archivo);
//	puts("Config create");
	int tam_archivo = config_get_int_value(archivo, "SIZE");
//	puts("config_get_int_value");
	config_destroy(archivo);
//	puts("Config destroy");
	return tam_archivo;
}

void aumentar_tam_archivo(char* ruta_archivo, registro_t* registro) {
	puts("-------------------Entre a aumentar_tam_archivo-------------------");
	t_config* archivo = config_create(ruta_archivo);
//	puts("config create");
	int tam_viejo = config_get_int_value(archivo, "SIZE");
//	puts("config get int value");
	int tam_nuevo = tam_viejo + tam_registro(registro);
//	printf("Tam viejo: %d\nTam Registro: %d\nTam nuevo: %d\n", tam_viejo, tam_registro(registro), tam_nuevo);
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
	puts("-------------------Entre a cantidad_bloques_usados-------------------");
	t_config* archivo = config_create(ruta_archivo);
//	puts("config create");
	char** lista_bloques = config_get_array_value(archivo, "BLOCKS");
//	puts("config get array value");
	int cant_bloques = 0;
	while(*(lista_bloques + cant_bloques))
		cant_bloques++;
    printf("Cantidad de Bloques: %d\n",cant_bloques);
	return cant_bloques;
}

int espacio_restante_bloque(char* ruta_archivo) {
	puts("-------------------Entre a espacio_restante_bloque-------------------");
	int tamBloque = Metadata_FS.block_size;
	printf("Tam bloque: %d\n", tamBloque);
	int tamArchivo = obtener_tam_archivo(ruta_archivo);
	printf("Tam archivo: %d\n", tamArchivo);
	int bloquesUsados = cantidad_bloques_usados(ruta_archivo);
	printf("Bloques usados: %d\n", bloquesUsados);
	int espacio_disponible = tamBloque*bloquesUsados - tamArchivo;
//	int espacio_disponible = cantidad_bloques_usados(ruta_archivo) * Metadata_FS.block_size - obtener_tam_archivo(ruta_archivo);
	printf("Espacio Disponible: %d\n", espacio_disponible);
	return espacio_disponible;
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
void incremetar_bloques_disponibles(int num) {
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
	char * mje= string_from_format("Se crearon las particiones de la tabla \"%s\" correctamente.", tabla );
	loggear_FS(mje);
	free(mje);
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
//	fprintf(f, "%s%d%s%d%s", "SIZE=", 0, "\nBLOCKS=[", bloque_num, "]\n");
	// No hace el fclose(f);
	printf("Numero de bloque: %d\n", bloque_num);
	return bloque_num;
}

FILE* crear_archivo(char* nombre, char* tabla, char* ext) {
	char* ruta = string_from_format("%s%s/%s%s", g_ruta.tablas, tabla, nombre, ext);
	FILE* archivo = fopen(ruta, "w+"); //Modo: lo crea vacio para lectura y escritura. Si existe borra lo anterior.
	free(ruta);
	return archivo;
}

void crear_directorio(char* ruta, char * nomb) {

	//string_to_upper(nomb);   //No me funciona (??) TODO ver
	char* ruta_dir = concat(ruta, nomb);
	char* mje = malloc(sizeof(char) * (60 + strlen(ruta_dir)));
	if (!mkdir(ruta_dir, S_IRWXU)) {
		sprintf(mje, "Se creó correctamente la carpeta \"%s\" en el directorio %s", nomb, ruta);
		loggear_FS(mje);
		free(ruta_dir);
		free(mje);
	} else {
		sprintf(mje, "No se creó la carpeta \"%s\". Ya existe.", nomb);
		loggear_FS(mje);
		free(ruta_dir);
		free(mje);
	}
	//Concat hace un malloc. Aca tiene que haber un free
}

void crear_bloques() {  //Los bloques van a partir del numero 0.

	int cantidad = Metadata_FS.blocks;
	bloques_disponibles = cantidad;
	char* num;
	for (int i = 0; i <= cantidad; i++) {
		num = string_itoa(i);
		crear_bloque(num);
		free(num);
	}
	loggear_FS("Se crearon los bloques del File System.");
}

void crear_bloque(char * nombre) {
	char* ruta = malloc(sizeof(char) * (strlen(g_ruta.bloques) + strlen(nombre) + strlen(".bin")) + 1);
	sprintf(ruta, "%s%s%s", g_ruta.bloques, nombre, ".bin");
	FILE* f = fopen(ruta, "w+");
	free(ruta);
	fclose(f);
}

void loggear_FS(char *valor) {
	sem_wait(&mutex_log);
	log_info(g_logger, valor);
	sem_post(&mutex_log);

	printf("--------------\n");
}

void loggear_FS_error(char *valor, instr_t* i) {

	sem_wait(&mutex_log);
	log_error(g_logger, valor);
	sem_post(&mutex_log);

	list_replace(i->parametros, 0, valor);

	printf("--------------\n");

	//Esto no genera memory leak, pisa el parametro bien. testeado!
}

char* obtener_por_clave(char* ruta, char* clave) {
	t_config* c = config_create(ruta);
	char* valor;
	valor = config_get_string_value(c, clave);
	config_destroy(c);
	return valor;
}

void inicializar_configuracion(void) {
	g_config = config_create("Lissandra.config");
	config_FS.punto_montaje = config_get_string_value(g_config, "PUNTO_MONTAJE");
	config_FS.puerto_escucha = config_get_string_value(g_config, "PUERTO_ESCUCHA");
	config_FS.tamanio_value = config_get_int_value(g_config, "TAMAÑO_VALUE");
	config_FS.retardo = (mseg_t)config_get_int_value(g_config, "RETARDO");
	config_FS.tiempo_dump = (mseg_t)config_get_int_value(g_config, "TIEMPO_DUMP");

	loggear_FS("Se leyó el archivo de configuración");
}

void iniciar_logger(){
	g_logger = log_create("Lissandra.log", "File System", 1, LOG_LEVEL_INFO);
}

void actualizar_tiempo_dump_config(mseg_t value) {

	char* val = string_itoa((int) value);

	sem_wait(&mutex_tiempo_dump_config);
	config_set_value(g_config, "TIEMPO_DUMP", val);
	sem_post(&mutex_tiempo_dump_config);

	config_save(g_config);

	char* mje = concat("Se actualizo el tiempo de dumpeo en el archivo de configuracion. Nuevo valor: ", val);
	loggear_FS(mje);
	free(mje);
	free(val);
}

void actualizar_tiempo_retardo_config(mseg_t value) {

	char* val = string_itoa( value);

	sem_wait(&mutex_tiempo_retardo_config);
	config_set_value(g_config, "TIEMPO_RETARDO", val);
	sem_post(&mutex_tiempo_retardo_config);

	config_save(g_config);

	char* mje = concat("Se actualizo el tiempo de retardo en el archivo de configuracion. Nuevo valor: ", val);
	loggear_FS(mje);
	free(mje);
	free(val);

}

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

