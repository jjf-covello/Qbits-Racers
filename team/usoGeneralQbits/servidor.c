/*
 * servidor.c
 *
 *  Created on: 5 abr. 2020
 *      Author: utnso
 */

#include "servidor.h"
#include "desarmador_de_paquetes.h"

int levantar_server_multithread(){

	remove("log_Servidor.log");
	logger= log_create("log_Servidor.log","Servidor", 0, LOG_LEVEL_DEBUG);
	sem_init(&mutex_log,0,1);
	leer_config_y_setear();



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
	sem_wait(&mutex_log);
	log_info(logger, "Levantamos el servidor\n");
	sem_post(&mutex_log);

	int activado = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	if( bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor)) != 0){
		sem_wait(&mutex_log);
		log_info(logger, "Fallo el bind \n ");
		sem_post(&mutex_log);
		server_levantado=false;
		return 1;
	}
	server_levantado=true;
	int cliente;
	printf("Estoy escuchando... \n");

	sem_wait(&mutex_log);
	log_info(logger, "Servidor listo para recibir un cliente\n");
	sem_post(&mutex_log);

	while(obtener_flag_de_planificacion()){

		listen(servidor, 100);
		pthread_t *hilo;
		struct sockaddr_in direccionCliente;
		unsigned tamanioDireccion= sizeof(struct sockaddr_in);
		cliente = accept(servidor, (void*) &direccionCliente, &tamanioDireccion);

		sem_wait(&mutex_log);
		log_info(logger, "Recibimos un cliente\n");
		sem_post(&mutex_log);
		pthread_create(&hilo, NULL,  (void*)atender_cliente, cliente);
		pthread_detach(hilo);

	}





	sem_wait(&mutex_log);
	log_info(logger, "Cerro el servidor\n");
	sem_post(&mutex_log);
	printf("Cerro el servidor \n");
	log_destroy(logger);
	sem_destroy(&mutex_log);

	return 0;

}

void enviar_ack(int socket_escucha){
	int ack=5138008;
	void* paquete= malloc(sizeof(int));
	memcpy(paquete,&ack,sizeof(int));
	send(socket_escucha,paquete,sizeof(int),0);
}

void atender_cliente(int cliente){

	void* buffer;
	int alocador;

	if(patova(cliente)){
		sem_wait(&mutex_log);
		log_info(logger, "Recibimos conexion \n");
		sem_post(&mutex_log);

		buffer=recibir_paquete(&alocador,cliente);

		if(alocador!=0){
			int codigo_operacion = conseguir_codigo_operacion(buffer);
			ejecutar_y_desarmar(codigo_operacion,buffer+sizeof(int),cliente);
			free(buffer);
		}

		enviar_ack(cliente);

		sem_wait(&mutex_log);
		log_info(logger, "Se desconecto el cliente\n");
		sem_post(&mutex_log);

		close(cliente);
	}else{

		sem_wait(&mutex_log);
		log_info(logger, "Se me conecto un don nadie y lo kickee\n");
		sem_post(&mutex_log);
		close(cliente);


	}
}
bool patova(int cliente){
	char*lectura=(char*)malloc(strlen("vivajorge")+1);
	int return_del_recv=recv(cliente,lectura,strlen("vivajorge")+1,0);
	int validacion= strcmp(lectura,"vivajorge");
	free(lectura);
	return validacion==0;
}



int levantar_server_singlethread(){


	remove("log_Servidor.log");
	logger= log_create("log_Servidor.log","Servidor", 0, LOG_LEVEL_DEBUG);
	sem_init(&mutex_log,0,1);
	leer_config_y_setear();


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


void leer_config_y_setear(){
	config= config_create("config.config");
	PUERTO_ESCUCHA= config_get_int_value(config,"PUERTO_TEAM");
	IP=config_get_string_value(config,"IP_TEAM");

}


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
