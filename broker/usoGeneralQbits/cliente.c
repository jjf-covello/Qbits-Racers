/*
 * cliente.c
 *
 *  Created on: 5 abr. 2020
 *      Author: utnso
 */

#include "cliente.h"

void mandar_credencial_al_patova(int socket){
	char* credencial = "vivajorge";
	send(socket,credencial,strlen("vivajorge")+1,0);
}

int conectar_con_servidor(char* ip_cliente, int puerto_cliente) {
		sem_wait(&sem_conectar_con_servidor);
	    int socket_fd;
	    struct hostent *server_host;
	    struct sockaddr_in server_address;

	    /* Get server host from server name. */
	    server_host = gethostbyname(ip_cliente);

	    /* Initialise IPv4 server address with server host. */
	    memset(&server_address, 0, sizeof server_address);
	    server_address.sin_family = AF_INET;
	    server_address.sin_port = htons(puerto_cliente);
	    memcpy(&server_address.sin_addr.s_addr, server_host->h_addr, server_host->h_length);

	    /* Create TCP socket. */
	    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	    if (socket_fd == -1) {
	        perror("Error en la creacion del socket");
	        sem_post(&sem_conectar_con_servidor);
	        return -1;
	    }


	    /* Connect to socket with server address. */
	    if (connect(socket_fd, (struct sockaddr *)&server_address, sizeof server_address) == -1) {
			perror("Error en la conexion del socket");
			sem_post(&sem_conectar_con_servidor);
	        return -1;
		}

	    mandar_credencial_al_patova(socket_fd);
	    sem_post(&sem_conectar_con_servidor);
	    return socket_fd;
}
/// TENE EN CUENTA QUE ESTO TE DA UNICAMENTE EL SOCKET PARA HACER REQUEST AL SERVER
//    / SI EL SERVER ES MULTITHREAD PODES MANDAR INFINIDAD DE REQUEST MIENTRAS QUE SI ES UN
//    / SINGLE THREAD VAS A TENER QUE CERRAR Y VOLVER A LLAMAR A ESTA FUNCION
