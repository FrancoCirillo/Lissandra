//--------registros.h--------

#ifndef REGISTROS_H_
#define REGISTROS_H_

typedef struct registro_t {
	mseg_t timestamp;
	uint16_t key;
	char* value;
} registro_t;


#endif /* REGISTROS_H_ */
