#include "server.h"

int main(void) {

	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	//Config
	char* ip;
	char* puerto;
	t_config* config;

	config = iniciar_config();
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");


	int server_fd = iniciar_servidor(logger, "Server", ip, puerto);
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(logger, "Server", server_fd);

	t_list* lista;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	close(server_fd);
	close(cliente_fd);
	return EXIT_SUCCESS;
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config;

	nuevo_config = config_create("../server/server.config");

	if (nuevo_config == NULL){
		printf("No fue posible cargar la config\n");
		exit(2);
	}

	return nuevo_config;
}
