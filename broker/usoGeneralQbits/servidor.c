/*
 * servidor.c
 *
 *  Created on: 5 abr. 2020
 *      Author: utnso
 */

#include <signal.h>

#include "servidor.h"
#include "desarmador_de_paquetes.h"

int levantar_server_multithread(char* IP_RECIBIDA, int PUERTO_RECIBIDO){

	IP = IP_RECIBIDA;
	PUERTO_ESCUCHA = PUERTO_RECIBIDO;

	/*
	remove("log_Servidor.log");
	logger= log_create("log_Servidor.log","Servidor", 0, LOG_LEVEL_DEBUG);
	sem_init(&mutex_log,0,1);
	leer_config_y_setear();
	 */
	/*pthread_t *hiloConsola;

	pthread_create(&hiloConsola, NULL, (void*)iniciarConsola,NULL);


	pthread_detach(hiloConsola);
	sem_wait(&mutexLog);
	log_info(logger,"El hilo de Consola esta funcionando\n");
	sem_post(&mutexLog);
	 */


	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family= AF_INET;
	direccionServidor.sin_addr.s_addr= inet_addr(IP); //INADDR_ANY;
	direccionServidor.sin_port=htons(PUERTO_ESCUCHA);

	//El PUERTO_ESCUCHA deberia ser int (?)

	int servidor = socket(AF_INET, SOCK_STREAM , 0);
	loggear_mensaje("Server Broker arriba", 0);
	printf("Server Broker arriba\n");

	int activado = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	if( bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor)) != 0){
		loggear_mensaje("Fallo del bind", 0);
		server_levantado=false;
		return 1;
	}

	server_levantado=true;


	sem_init(&sem_conexion, 0, 1);
	sem_init(&sem_conectar_con_servidor, 0, 1);

	while(server_levantado){
		int cliente;
		listen(servidor, 100);
		pthread_t *hilo;
		struct sockaddr_in direccionCliente;
		unsigned tamanioDireccion= sizeof(struct sockaddr_in);
		cliente = accept(servidor, (void*) &direccionCliente, &tamanioDireccion);

		//sem_wait(&sem_conexion);
		pthread_create(&hilo, NULL,  (void*)atender_cliente, cliente);
		pthread_detach(hilo);
	}

	loggear_mensaje("Server cerrado", 0);
	printf("Server cerrado\n");
	//log_destroy(logger);
	//sem_destroy(&mutex_log);

	return 0;

}

bool patova(int sock_cliente) {
	char* lectura = (char*)malloc(strlen("vivajorge")+1);
	int return_del_recv = recv(sock_cliente, lectura, strlen("vivajorge")+1, 0); //Si pasan cosas, revisar el flag
	int validacion =  strcmp(lectura, "vivajorge");
	free(lectura);
	return validacion == 0;
}

void atender_cliente(int sock_cliente){

	void* buffer;
	int alocador;
	
	sem_wait(&sem_conexion);

	if(patova(sock_cliente)) {
		loggear_mensaje("Se conecto un cliente. Hilo de atencion disparado", 1);

		buffer=recibir_paquete(&alocador,sock_cliente);
		ejecutar_operacion(buffer, sock_cliente);

		//TODO: Deberiamos revisar el buffer antes de hacer nada a ver si todavia esta vacio.

		free(buffer);

		loggear_mensaje("Se atendio el pedido correctamente", 0);
	} else {
		close(sock_cliente);
		loggear_mensaje("Me mandaron basura, lo ignoro.", 0);
	}

	sem_post(&sem_conexion);

}

/*
int levantar_server_singlethread(){


	remove("log_Servidor.log");
	logger= log_create("log_Servidor.log","Servidor", 0, LOG_LEVEL_DEBUG);
	sem_init(&mutex_log,0,1);
	leer_config_y_setear();


	pthread_t *hiloConsola;

	pthread_create(&hiloConsola, NULL, (void*)iniciarConsola,NULL);


	pthread_detach(hiloConsola);
	sem_wait(&mutexLog);
	log_info(logger,"El hilo de Consola esta funcionando\n");
	sem_post(&mutexLog);

	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family= AF_INET;
	direccionServidor.sin_addr.s_addr= inet_addr(IP); //INADDR_ANY;
	direccionServidor.sin_port=htons(PUERTO_ESCUCHA);

	//El PUERTO_ESCUCHA deberia ser int (?)

	int servidor = socket(AF_INET, SOCK_STREAM , 0);
	sem_wait(&mutex_log);
	log_info(logger, "Levantamos el servidor\n");
	sem_post(&mutex_log);

	int activado = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	if( bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor)) != 0){
		sem_wait(&mutex_log);
		log_info(logger, "Fallo el bind \n ");
		sem_post(&mutex_log);
		return 1;
	}

	int cliente;
	printf("Estoy escuchando... \n");

	sem_wait(&mutex_log);
	log_info(logger, "Servidor listo para recibir un cliente\n");
	sem_post(&mutex_log);
	server_levantado=true;

	while(server_levantado){

		listen(servidor, 100);
		struct sockaddr_in direccionCliente;
		unsigned tamanioDireccion= sizeof(struct sockaddr_in);
		cliente = accept(servidor, (void*) &direccionCliente, &tamanioDireccion);
		atender_single_request(cliente);
		sem_wait(&mutex_log);
		log_info(logger, "Recibimos un cliente\n");
		sem_post(&mutex_log);


	}





	sem_wait(&mutex_log);
	log_info(logger, "Cerro el servidor\n");
	sem_post(&mutex_log);
	printf("Cerro el servidor \n");
	log_destroy(logger);
	sem_destroy(&mutex_log);

	return 0;

}
 */

/*
void leer_config_y_setear(){
	config= config_create("config.config");
	PUERTO_ESCUCHA= config_get_int_value(config,"PUERTO");
	IP=config_get_string_value(config,"IP");

}
 */

/*
void atender_single_request(int cliente){
	void* buffer;
	int alocador;

	sem_wait(&mutex_log);
	log_info(logger, "Recibimos conexion \n");
	sem_post(&mutex_log);

	buffer=recibir_paquete(&alocador,cliente);

	if(0<alocador){
		ejecutar_operacion(buffer, cliente);
		free(buffer);
	}

	sem_wait(&mutex_log);
	log_info(logger, "Se desconecto el cliente\n");
	sem_post(&mutex_log);
	close(cliente);
}
 */
