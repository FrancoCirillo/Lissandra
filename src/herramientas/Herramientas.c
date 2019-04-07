/*
 * Herramientas.c
 *
 *  Created on: 7 abr. 2019
 *      Author: rodrigo diaz
 */


#include "Herramientas.h"
#include <stdio.h>
#include <stdlib.h>
int main(){
	//Variable que almacenara los datos parseados
	instr *x;
	//Mensaje a parsear
	char* mensaje="123456;parametro1;parametro2;parametro3;parametro4;";
	char* mensajeSerializado;
	deserializar(mensaje,x);
	//Serializar una variable
	mensajeSerializado=serializar(x);
}
int digitosDe(int x){
    int count=0;
	if(x==0){
		return 1;
	}
    while(x != 0)
    {
        // n = n/10
        x /= 10;
        ++count;
    }
	return count;
}
char* serializar(instr *x){
	char* caracterSeparador=";";
	char* codigoInstruccion=malloc(64);
	// Obtengo cantidad de digitos del codigo y lo multiplico por el tamanio de un char+ tamanio de campos+tamanio de ;
	char* mensajeSerializado=malloc(sizeof(char)*(digitosDe(x->codigoInstruccion))+sizeof(x->param1)+sizeof(x->param2)+sizeof(x->param3)+sizeof(x->param4)+sizeof(char)*6);
	printf("\n-------------------\nComienza serializacion\n");

	sprintf(codigoInstruccion,"%d",x->codigoInstruccion);
	strcat(mensajeSerializado,codigoInstruccion);
	strcat(mensajeSerializado,caracterSeparador);

	strcat(mensajeSerializado,x->param1);
	strcat(mensajeSerializado,caracterSeparador);

	strcat(mensajeSerializado,x->param2);
	strcat(mensajeSerializado,caracterSeparador);

	strcat(mensajeSerializado,x->param3);
	strcat(mensajeSerializado,caracterSeparador);

	strcat(mensajeSerializado,x->param4);
	strcat(mensajeSerializado,caracterSeparador);

	printf("\n------------\nSe serializo correctamente, el resultado es: %s\n",mensajeSerializado);
	free(codigoInstruccion);
	return mensajeSerializado;
}
int deserializar(char* mensaje,instr* outPut){
	//El mensaje debe poseer el \0 final
	//formato: codigoInstruccion;param1;param2;param3;param4;
	char* aux=mensaje;
	char* extracto;
	int contador=0;
	int posicionInicial=0;
	int posicionFinal;
	char caracterSeparador=';';
	puts("\n ------------------- \n Iniciando parseo");
	while(*aux && contador<5){
		if(*aux==caracterSeparador){


			//Obtengo donde se detecto el ;
			posicionFinal=strlen(mensaje)-strlen(aux);

			extracto=substr(mensaje,posicionInicial,posicionFinal);
			//La proxima corto desde la posicion final del anterior +1 para cortar el ;
			posicionInicial=posicionFinal+1;

			//printf("\n ; encontrado, recortando %s",extracto);

			if(contador==0){
				outPut->codigoInstruccion=atoi(extracto);
				contador++;
				//puts("Codigo encontrado");
			}else if(contador==1){
				outPut->param1=extracto;
				contador++;
				//puts("Param 1 encontrado");
			}else if(contador==2){
				outPut->param2=extracto;
				contador++;
				//puts("Param 2 encontrado");
			}else if(contador==3){
				outPut->param3=extracto;
				contador++;
				//puts("Param 3 encontrado");
			}else if(contador==4){
				outPut->param4=extracto;
				contador++;
				//puts("Param 4 encontrado");
			}
		}
		//printf("\n %c obtenido, restan %d    \n",*aux,strlen(aux));
		aux++;
	}
	printf("\n Se pudo parsear correctamente!!,\n codigo de instruccion %d,\n parametro1 %s,\n parametro2 %s,\n parametro3 %s,\n parametro4 %s  ",outPut->codigoInstruccion,outPut->param1,outPut->param2,outPut->param3,outPut->param4);
	puts("\n Parseo finalizado \n ---------------");
	return 1;
}

char* substr(const char *src, int m, int n)
{
	// get length of the destination string
	int len = n - m;

	// allocate (len + 1) chars for destination (+1 for extra null character)
	char *dest = (char*)malloc(sizeof(char) * (len + 1));

	// extracts characters between m'th and n'th index from source string
	// and copy them into the destination string
	for (int i = m; i < n && (*src != '\0'); i++)
	{
		*dest = *(src + i);
		dest++;
	}

	// null-terminate the destination string
	*dest = '\0';

	// return the destination string
	return dest - len;
}

