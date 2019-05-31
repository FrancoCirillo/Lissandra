//---------memoria.c---------

#include "memoria.h"

int main(int argc, char *argv[])
{

	check_inicial(argc, argv);
	iniciar_log();
	iniciar_config();
	inicializar_configuracion();
	conexionesActuales = dictionary_create();
	idsNuevasConexiones = malloc(sizeof(identificador));

	callback = ejecutar_instruccion;

	enviar_datos_a_FS(argv);
	int listenner = iniciar_servidor(miIPMemoria, configuracion.PUERTO);

	//recibir el tamanio del Value

	gran_malloc_inicial();
	inicializar_tabla_segmentos();

	tamanioValue = 32;
	tamanioRegistro = sizeof(mseg_t) + sizeof(uint16_t) + tamanioValue;

	cantidadDeSectores = configuracion.TAMANIO_MEMORIA / tamanioRegistro; //Se trunca automaticamente al entero (por ser todos int)

	printf("cantidadDeSectores = TAMANIO_MEMORIA / tamanioRegistro\n%d = %d / %d", cantidadDeSectores, configuracion.TAMANIO_MEMORIA, tamanioRegistro);

	sectorOcupado = malloc(cantidadDeSectores * sizeof(bool));
	memset(sectorOcupado, false, cantidadDeSectores * sizeof(bool));

	vigilar_conexiones_entrantes(listenner, callback, conexionesActuales, CONSOLA_MEMORIA);
	//config_destroy(g_config);

	return 0;
}

void inicializar_configuracion()
{
	puts("Configuracion:");
	char *rutaConfig = "memoria.config";
	configuracion.PUERTO = obtener_por_clave(rutaConfig, "PUERTO");
	configuracion.IP_FS = obtener_por_clave(rutaConfig, "IP_FS");
	configuracion.PUERTO_FS = obtener_por_clave(rutaConfig, "PUERTO_FS");
	configuracion.IP_SEEDS = obtener_por_clave(rutaConfig, "IP_SEEDS");
	configuracion.PUERTO_SEEDS = obtener_por_clave(rutaConfig, "PUERTO_SEEDS");
	configuracion.RETARDO_MEMORIA = atoi(obtener_por_clave(rutaConfig, "RETARDO_MEMORIA"));
	configuracion.RETARDO_FS = atoi(obtener_por_clave(rutaConfig, "RETARDO_FS"));
	configuracion.TAMANIO_MEMORIA = atoi(obtener_por_clave(rutaConfig, "TAMANIO_MEMORIA"));
	configuracion.RETARDO_JOURNAL = atoi(obtener_por_clave(rutaConfig, "RETARDO_JOURNAL"));
	configuracion.RETARDO_GOSSIPING = atoi(obtener_por_clave(rutaConfig, "RETARDO_GOSSIPING"));
	configuracion.MEMORY_NUMBER = atoi(obtener_por_clave(rutaConfig, "MEMORY_NUMBER"));
	configuracion.RUTA_LOG = obtener_por_clave(rutaConfig, "RUTA_LOG");
}


void iniciar_log()
{
	g_logger = log_create(configuracion.RUTA_LOG, nombreDeMemoria, 1, LOG_LEVEL_INFO);

}

void iniciar_config()
{
	g_config = config_create("memoria.config");
}

char *obtener_por_clave(char *ruta, char *clave)
{
	char *valor;
	valor = config_get_string_value(g_config, clave);
	printf(" %s: %s \n", clave, valor);

	return valor;
}

void ejecutar_instruccion(instr_t *instruccion, char *remitente)
{
	int codigoNeto = instruccion->codigo_operacion %100; //Los primeros dos digitos son los posibles codigos de operacion
	if(instruccion->codigo_operacion > BASE_COD_ERROR){
		puts("Me llego un codigo de error");
		ejecutar_instruccion_error(instruccion);
	}
	else{
		switch (codigoNeto)
		{
		case CODIGO_SELECT:
			sleep(3);
			ejecutar_instruccion_select(instruccion);
			break;
		case DEVOLUCION_SELECT:
			ejecutar_instruccion_devolucion_select(instruccion);
			break;
		case CODIGO_INSERT:
			sleep(3);
			ejecutar_instruccion_insert(instruccion, true);
			break;
		case CODIGO_CREATE:
			sleep(3);
			ejecutar_instruccion_create(instruccion);
			break;
		case CODIGO_DESCRIBE:
			ejecutar_instruccion_describe(instruccion);
			break;
		case CODIGO_DROP:
			sleep(3);
			ejecutar_instruccion_drop(instruccion);
			break;
		case CODIGO_JOURNAL:
			ejecutar_instruccion_journal(instruccion);
			break;
		case CODIGO_EXITO:
			ejecutar_instruccion_exito(instruccion);
			break;

		default:
			puts("El comando no pertenece a la Memoria");
			break;
		}
	}
}



void check_inicial(int argc, char* argv[])
{
	if (argc < 2 || strcmp(argv[1], "4") == 0 || strcmp(argv[1], "2") == 0)
	{
		puts("Uso: MEMORIA <NUMERERO-DE-MEMORIA>");
		puts("<NUMERERO-DE-MEMORIA> Funcionales por el momento: 3, 8, 9");
		puts("Tampoco se puede elegir el numero 4 porque es el IP que (Por el momento, testing) usa el Kernel");
		puts("Tampoco se puede elegir el numero 2 porque es el IP que (Por el momento, testing) usa el FS");

		exit(0);
	}

	sprintf(nombreDeMemoria, "Memoria_%s", argv[1]);
}

