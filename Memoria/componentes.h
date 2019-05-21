#ifndef COMPONENTES_H_
#define COMPONENTES_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include "../Herramientas/tiempo.h"

typedef struct RegistroMem
{
	 mseg_t timestamp;
	 uint16_t key;
	 char value[];
} RegistroMem;

int gran_malloc_inicial(int tamanioMemoria);

#endif //COMPONENTES_H
