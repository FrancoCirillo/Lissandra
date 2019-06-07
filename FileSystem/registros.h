/*
 * registros.h
 *
 *  Created on: 6 jun. 2019
 *      Author: utnso
 */

#ifndef REGISTROS_H_
#define REGISTROS_H_

typedef struct registro_t {
	uint16_t key;
	char* value;
	mseg_t timestamp;
} registro_t;


#endif /* REGISTROS_H_ */
