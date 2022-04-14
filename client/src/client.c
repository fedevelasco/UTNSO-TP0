#include "client.h"

int main(void)
{

	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;


	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();
	log_info(logger, "Soy un logg");


	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();

	// Loggeamos el valor de config
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");
	valor = config_get_string_value(config, "CLAVE");

	log_info(logger, puerto);

	/* ---------------- LEER DE CONSOLA ---------------- */

	leer_consola(logger);

	/*---------------------------------------------------CONEXION AL SERVIDOR---------------------------------------------------*/

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(logger, "Server", ip, puerto);

	// Enviamos al servidor el valor de CLAVE como mensaje
	enviar_mensaje(valor, conexion);

	// Armamos y enviamos el paquete
	paquete(conexion);

	terminar_programa(conexion, logger, config);

}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;

	nuevo_logger = log_create("tp0.log", "Client", 1, LOG_LEVEL_INFO);

	if(nuevo_logger == NULL){
		printf("No fue posible crear el Logger\n");
		exit(1);
	}

	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config;

	nuevo_config = config_create("../client/cliente.config");

	if (nuevo_config == NULL){
		printf("No fue posible cargar la config\n");
		exit(2);
	}

	return nuevo_config;
}

void leer_consola(t_log* logger)
{
	char* linea;

	while(1){

		linea = readline("> ");

		if(linea[0] == '\0'){
			break;
		}

		log_info(logger, linea);
	}

	free(linea);

}

void paquete(int conexion)
{
	char* leido = readline("> ");
	t_paquete* paquete = crear_paquete();

	while(leido[0] != '\0'){
		agregar_a_paquete(paquete, leido, strlen(leido)+1);
		free(leido);
		leido = readline("> ");
	}

	free(leido);
	enviar_paquete(paquete, conexion);
	eliminar_paquete(paquete);
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	if(logger != NULL){
		log_destroy(logger);
	}

	if (config != NULL){
		config_destroy(config);
	}

}
