#include "memoria.h"

int main() {
//	pruebaHilos();

	printf("PROCESO MEMORIA\n");
	printf("Como cliente: \n");
//
	int conexion_FileSystem = conectar_con_proceso(FILESYSTEM); //connect()
	puts("Listo para enviar a FILESYSTEM");
//	int conexion_Kernel = conectar_con_proceso(KERNEL);
//	puts("Listo para enviar a KERNEL");
	enviar_mensaje("123456;parametro1;parametro2;parametro3;parametro4;", conexion_FileSystem);
//	enviar_mensaje("123456;HOLA;SI;QUE;TUL;", conexion_Kernel);
	sleep(2);
	close(conexion_FileSystem);

	return 0;
}
