#include "serverUtils.h"

int iniciar_servidor(t_log* logger, const char* name, char* ip, char* puerto)
{
	int socket_servidor; //Declaramos el descriptor

	struct addrinfo infoSocket, *server_info; //Declaramos las estructuras

	memset(&infoSocket, 0, sizeof(infoSocket)); //Seteamos el valor de la informacion de la direccion a 0
	//Asignamos los valores de info de la conexion
	infoSocket.ai_family = AF_UNSPEC; //Indica a getaddrinfo(3) que la direccion va a ser de tipo IPv4 o IPv6
	infoSocket.ai_socktype = SOCK_STREAM; //Socket de tipo TCP/IP
	infoSocket.ai_flags = AI_PASSIVE; //Solo para cuando se quiere utilizar el socket para un servidor

	//Obtenemos la direccion y los datos del socket y los mete en server_info
	if (getaddrinfo(ip, puerto, &infoSocket, &server_info) != 0){
		perror("No se pudo obtener la direccion correctamente.");
		return -1;
	}

	bool conecto = false;

	// Itera por cada addrinfo devuelto
	for (struct addrinfo *p = server_info; p != NULL; p = p->ai_next) {
		// Creamos el socket de escucha del servidor
		socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

		if (socket_servidor == -1) // fallo de crear socket
			continue;

		// Asociamos el socket a un puerto
		if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
			// Si entra aca fallo el bind
			close(socket_servidor);
			continue;
		}
		// Ni bien conecta uno nos vamos del for
		conecto = true;
		break;
	}

	if(!conecto) {
		free(server_info);
		perror("No se pudo crear el socket");
		return -1;
	}

	// Escuchamos las conexiones entrantes

	listen(socket_servidor, SOMAXCONN);

	log_trace(logger, "Listo para escuchar a mi cliente");
	log_info(logger, "Escuchando en %s:%s (%s)\n", IP, PUERTO, name);

	freeaddrinfo(server_info);

	return socket_servidor;
}

int esperar_cliente(t_log* logger, const char* name, int socket_servidor)
{
	// Aceptamos un nuevo cliente
	struct sockaddr_in dir_cliente; // Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc
	socklen_t addrlenght  = sizeof(dir_cliente);

	int socket_cliente = accept(socket_servidor, (struct sockaddr *) &dir_cliente, &addrlenght );

	log_info(logger, "Cliente conectado (a %s)\n", name);

	return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	uint32_t cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(uint32_t), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}
