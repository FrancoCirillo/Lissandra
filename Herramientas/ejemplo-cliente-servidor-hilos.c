/*
 * cliente.c
 *
 *  Created on: 14 apr. 2019
 *      Author: rodrigo diaz
 */

#include <ejemplo-cliente-servidor-hilos.h>
void* iniciarServidor(void *p);
void* iniciarCliente(void *p);
void ejemplo(){
	puts("\nDECLARO VARS\n");
	pthread_t hilo_servidor;
	pthread_t hilo_cliente;
	pthread_attr_t attr_servidor;
	pthread_attr_t attr_cliente;

	puts("\nINICIO PARAMS\n");
	pthread_attr_init(&attr_cliente);
	pthread_attr_init(&attr_servidor);

	puts("\nINICIO HILOS\n");
	pthread_create(&hilo_servidor,&attr_servidor,iniciarServidor,NULL);
	pthread_create(&hilo_cliente,&attr_cliente,iniciarCliente,NULL);

	puts("\nJOINEO HILOS\n");
	pthread_join(hilo_cliente,NULL);
	pthread_join(hilo_servidor,NULL);

	puts("\n ");
	puts("\n\nFIN!");

}

void* iniciarCliente(void* p){
	sleep(5);
	int conexion_kernel = conectar_con_proceso(KERNEL); //connect()
	puts("Listo para enviar a SI MISMO");
	enviar_mensaje("123456;parametro1;parametro2;parametro3;parametro4;", conexion_kernel);
	sleep(2);
	close(conexion_kernel);

}
void* iniciarServidor(void *p){
	server_fd = iniciar_servidor(IP_KERNEL, PORT); //socket(), bind(), listen()
	puts("Listo para escuchar");
	puts("Escuchando...");
	escuchar_en(server_fd);
	close(server_fd);

}
