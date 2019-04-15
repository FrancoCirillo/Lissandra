#include "fileSystem.h"

int main() {
	printf("PROCESO FILESYSTEM \n");
	/*server_fd = iniciar_servidor(IP_FILESYSTEM, PORT); //socket(), bind(), listen()
	puts("Listo para escuchar");
	puts("Escuchando...");
	escuchar_en(server_fd);
	close(server_fd);
	*/


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

void inicializarConfig(void){
	configuracion.PUNTO_MONTAJE = "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/FileSystem/archivo.log";

			//"mnj/LISANDRA_FS/archivo.log";
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
