#include "memoria.h"

int main() {

	printf("PROCESO MEMORIA\n");

	//inicializarConfiguracion();

	leerConsola();

	//config_destroy(g_config);

	return 0;
}

void inicializarConfiguracion() {
	char* rutaConfig = "memoria.config";
	configuracion.PUERTO = atoi(getByKey(rutaConfig, "PUERTO"));
	configuracion.IP_FS = atoi(getByKey(rutaConfig, "IP_FS"));
	configuracion.PUERTO_FS = atoi(getByKey(rutaConfig, "PUERTO_FS"));
	configuracion.IP_SEEDS = getByKey(rutaConfig, "IP_SEEDS");
	configuracion.PUERTO_SEEDS = getByKey(rutaConfig, "PUERTO_SEEDS");
	configuracion.RETARDO_MEMORIA = getByKey(rutaConfig, "RETARDO_MEMORIA");
	configuracion.RETARDO_FS = getByKey(rutaConfig, "RETARDO_FS");
	configuracion.TAMANIO_MEMORIA = getByKey(rutaConfig, "TAMANIO_MEMORIA");
	configuracion.RETARDO_JOURNAL = getByKey(rutaConfig, "RETARDO_JOURNAL");
	configuracion.RETARDO_GOSSIPING = getByKey(rutaConfig, "RETARDO_GOSSIPING");
	configuracion.MEMORY_NUMBER = getByKey(rutaConfig, "MEMORY_NUMBER");
	configuracion.RUTA_LOG = getByKey(rutaConfig, "RUTA_LOG");
}

void loggear(char *valor) {
	g_logger = log_create(configuracion.RUTA_LOG, "memoria", 1, LOG_LEVEL_INFO);
	log_info(g_logger, valor);
	log_destroy(g_logger);
}

char* getByKey(char* ruta, char* key) {
	char* valor;
	g_config = config_create(ruta);
	valor = config_get_string_value(g_config, key);

	printf("-----------\nGenerando config, valor obtenido para %s, es:   %s \n ---------", key, valor);

	return valor;
}

void leerConsola() {
	char *actual, *comando, *param1, *param2, *param3, *param4;
	comando = param1 = param2 = param3 = param4 = NULL;

	printf("\n----------Esperando requests----------\n");
	printf("Cuando desee finalizar, presione ENTER\n\n");
	char* request = readline("> ");

	while(strncmp(request, "", 1) != 0) {
		actual = strtok (request, " ");

		for(int i=0; actual != NULL; i++)
		{
			switch(i){
			case 0: comando = strdup(actual); break;
			case 1: param1 = actual != NULL? strdup(actual): actual; break;
			case 2: param2 = actual != NULL? strdup(actual): actual; break;
			case 3: param3 = actual != NULL? strdup(actual): actual; break;
			case 4: param4 = actual != NULL? strdup(actual): actual; break;
			}

			if(i==2 && strcmp(comando, "INSERT")==0){
				actual = strtok (NULL, "");
				param3 = strdup(actual);
				break;
			}

			actual = strtok (NULL, " ");
		}

		free(request);

		reconocerRequest(comando, param1, param2, param3, param4);
		comando = param1 = param2 = param3 = param4 = NULL;

		request = readline("> ");
	}

	free(request);
}

void reconocerRequest(char* comando, char* param1, char* param2, char* param3, char* param4) {

	if (strcmp(comando, "SELECT")==0) {
		printf ("Se detecto comando 'SELECT'\n");
		crearInstruccion(CODIGO_SELECT, param1, param2, param3, param4);
	}
	else if (strcmp(comando, "INSERT")==0) {
		printf ("Se detecto comando 'INSERT'\n");
		crearInstruccion(CODIGO_INSERT, param1, param2, param3, param4);
	}
	else if (strcmp(comando, "CREATE")==0) {
		printf ("Se detecto comando 'CREATE'\n");
		crearInstruccion(CODIGO_CREATE, param1, param2, param3, param4);
	}
	else if (strcmp(comando, "DESCRIBE")==0) {
		printf ("Se detecto comando 'DESCRIBE'\n");
		crearInstruccion(CODIGO_DESCRIBE, param1, param2, param3, param4);
	}
	else if (strcmp(comando, "DROP")==0) {
		printf ("Se detecto comando 'DROP'\n");
		crearInstruccion(CODIGO_DROP, param1, param2, param3, param4);
	}
	else if (strcmp(comando, "JOURNAL")==0) {
		printf ("Se detecto comando 'JOURNAL'\n");
		crearInstruccion(CODIGO_JOURNAL, param1, param2, param3, param4);
	}
	else
		printf ("Comando invalido\n\n");
}

instr_t *crearInstruccion(int codigoRequest, char* p1, char* p2, char* p3, char* p4) {
	time_t nuevoTSmp = obtener_tiempo();

	instr_t instruccionCreada ={
		.timestamp = nuevoTSmp,
		.codigoInstruccion = codigoRequest,
		.param1 = p1 != NULL? strdup(p1): p1,
		.param2 = p2 != NULL? strdup(p2): p2,
		.param3 = p3 != NULL? strdup(p3): p3,
		.param4 = p4 != NULL? strdup(p4): p4
	};

	instr_t *miInstr = malloc(sizeof(instruccionCreada));

	memcpy(miInstr, &instruccionCreada, sizeof(instruccionCreada));

	print_instruccion(miInstr);

	return miInstr;
}

void print_instruccion(instr_t* instruccion){
	printf("timestamp: %lld\n", (long long)instruccion->timestamp);
	printf("codigoInstruccion: %d\n", instruccion->codigoInstruccion);
	printf("param1: %s\n", instruccion->param1);
	printf("param2: %s\n", instruccion->param2);
	printf("param3: %s\n", instruccion->param3);
	printf("param4: %s\n\n", instruccion->param4);
}





