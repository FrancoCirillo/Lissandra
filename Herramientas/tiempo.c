//-------tiempo.c-------

#include "tiempo.h"


mseg_t dif_timestamps(mseg_t tiempo1, mseg_t tiempo2){
	return abs(tiempo1 - tiempo2);
}

mseg_t max_timestamp(mseg_t tiempo1, mseg_t tiempo2){
	return tiempo1>tiempo2? tiempo1: tiempo2;
}

mseg_t obtener_ts(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	mseg_t milisegundosDesdeEpoch = (mseg_t)(tv.tv_sec) * 1000 + (mseg_t)(tv.tv_usec) / 1000;
	return milisegundosDesdeEpoch;
}

char* mseg_a_string(mseg_t numero) {
	return string_from_format("%"PRIu64, numero);
}//Hacer el free();

mseg_t string_a_mseg(char* numero) {
    return strtoull(numero, NULL, 10);
}
//formato para imprimir: printf("%" PRIu64 ,a);


