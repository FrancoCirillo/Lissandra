#include "fileSystem.h"

int main() {
	printf("PROCESO FILESYSTEM \n");

	//crearArchivoConfig();
	inicializarConfig();


	loggear("File system is comming..");

	printf("Creado el log");


    //accept();  recibir un buffer,
     //Guardar datos a quién le contesto.




/*    VISTO CON RODRI: pseudocódigo
      * intruccion
      * la memoria
      * -Servidor debe recibir mensaje ya como instruccion-memoria; crea hilo con funcion procesar
      * -Hacer struct que contenga instruccion y memoria
      * procesar(struct instruccion-memoria){
      *     Interpreto intruccion y utilizo response
      *             if(codigo==CODIGO_CREATE)
      *                     hacer_create(instruccion)// piso codigo de instruccion por codigo de error/Respuesta y parametro 1 por respuesta
      *             responder(instruccion-memoria);
      *
      *             RECORDAR EL JOIN;
      *             Y MALLOC.
      *
      *
      *             Cola de colas/ Castear.

	*/

	return 0;
}

//TODO Terminarlo bien. Dejarlo listo.
void inicializarConfig(void){
	configuracion.PUNTO_MONTAJE = strcat(RUTA_PUNTO_MONTAJE,"Configuracion_LFS.config");
	configuracion.PUERTO_ESCUCHA = 5555;
	configuracion.TAMANIO_VALUE = 5;
	configuracion.RETARDO = 20000;
	configuracion.TIEMPO_DUMP = 40000;
}

void loggear(char *valor) {
		g_logger = log_create(configuracion.PUNTO_MONTAJE,"FS", 1, LOG_LEVEL_INFO);
		log_info(g_logger, valor);
		log_destroy(g_logger);
	}

char* leer_config(char* clave) {
	char* valor;
	g_config = config_create(RUTA_PUNTO_MONTAJE);
	valor = config_get_string_value(g_config, clave);
	config_destroy(g_config);
	return valor;
}
