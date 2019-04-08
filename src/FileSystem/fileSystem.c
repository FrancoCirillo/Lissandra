#include "fileSystem.h"

int main() {
	printf("PROCESO FILESYSTEM");
	int conexionMemoria = conectar_con_proceso(MEMORIA);
	int conexionKernel = conectar_con_proceso(KERNEL);
	enviar_mensaje("123456;parametro1;parametro2;parametro3;parametro4;", conexionMemoria);
	enviar_mensaje("7890;parametro1;parametro2;parametro3;parametro4;", conexionKernel);

	close(conexionMemoria);
	close(conexionKernel);
	return 0;
}
