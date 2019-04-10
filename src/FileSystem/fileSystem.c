#include "fileSystem.h"

int main() {
	printf("PROCESO FILESYSTEM \n");
	server_fd = iniciar_servidor(IP_FILESYSTEM, PORT); //socket(), bind(), listen()
	puts("Listo para escuchar");
	puts("Escuchando...");
	escuchar_en(server_fd);
	close(server_fd);
	return 0;
}
