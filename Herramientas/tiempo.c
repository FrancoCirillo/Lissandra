//-------tiempo.c-------

#include "tiempo.h"

////Ejemplo Pthread
//
//#include <pthread.h>
//
//void* funcion(void* numero){
//	int* numero_pasado=(int*) numero;
//	int suma=0;
//	printf("Parametro recibido %d",*numero_pasado);
//	for(int i=0; i<*numero_pasado;i++){
//		suma+=i;
//	}
//	printf("\n La suma para %d es %d \n",*numero_pasado,suma);
//	free(numero);
//
//}
//// int main(){
//	// //Variable que almacenara los datos parseados
//	// instr *x;
//	// //Mensaje a parsear
//	// char* mensaje="123456;parametro1;parametro2;parametro3;parametro4;";
//	// char* mensajeSerializado;
//	// deserializar(mensaje,x);
//	// //Serializar una variable
//	// mensajeSerializado=serializar(x);
//	int lista[5]={4000000,2000000,1000000,2000000,2000000};
//	pthread_t threads_ids[5];
//	for(int i=0;i<5;i++){
//		int *parametro=malloc(sizeof(int));
//		*parametro=lista[i];
//		pthread_attr_t attr;
//		printf("\n ###### \n Iniciando hilo %d con parametro %d",i,*parametro);
//		pthread_attr_init(&attr);
//		pthread_create(&threads_ids[i],&attr,funcion,parametro);
//	}
//	printf("\n ########### \n Llamando a fin de hilos \n");
//	for(int i=0;i<5;i++){
//		pthread_join(threads_ids[i],NULL);
//
//	}
//	printf("FIN");
//}


mseg_t obtener_ts(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;   // retorna en mili segundos.
			//El pasaje es porque timeval tiene 2 atributos que son en segundos y en microsegundos.
}


mseg_t dif_timestamps(mseg_t tiempo1, mseg_t tiempo2){
	return abs(tiempo1 - tiempo2);
}

mseg_t max_timestamp(mseg_t tiempo1, mseg_t tiempo2){
	return tiempo1>tiempo2? tiempo1: tiempo2;
}

mseg_t get_ts(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	mseg_t millisecondsSinceEpoch = (mseg_t)(tv.tv_sec) * 1000 + (mseg_t)(tv.tv_usec) / 1000;
	return millisecondsSinceEpoch;
}

char* mseg_to_string(mseg_t number) {
    return string_from_format("%llu", number);
}//Hacer el free();

mseg_t string_to_mseg(char* number) {
    return strtoull(number, NULL, 10);
}


