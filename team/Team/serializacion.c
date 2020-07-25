
#include "serializacion.h"


uint32_t deserializar_id(void* paquete){

	uint32_t id_mensaje;
	memcpy(&id_mensaje, paquete, sizeof(uint32_t));

	free(paquete);

	return id_mensaje;
}

uint32_t serializar_get_pokemon(char* nombre_pokemon){

	int socket_de_envio= conectar_con_servidor(IP_BROKER,PUERTO_BROKER);
	int peso_char =  strlen(nombre_pokemon) +1 ;
	int peso_total = peso_char + sizeof(int);
	int offset = 0;

	void* paquete = malloc(peso_total+sizeof(int));

	// Hicimos el free de paquete, paquete_a_mandar y
	// de recepcion pero sigue saltando el error con Valgrind

	memcpy(paquete,&peso_total,sizeof(int));
	offset+=sizeof(int);

	memcpy(paquete+offset,&peso_char,sizeof(int));
	offset+=sizeof(int);

	memcpy(paquete+offset,nombre_pokemon,peso_char);
	offset+=sizeof(int);

	void* paquete_a_mandar = crear_paquete(GET_POKEMON,paquete);

	free(paquete);

	int peso_paquete=0;
	memcpy(&peso_paquete,paquete_a_mandar,sizeof(int));

	/*while(socket_de_envio < 3){
		sem_wait(&SEM_LOGS);
		log_info(log_obligatorio,"No pudimos conectarnos con Broker (socket= %d)",socket_de_envio);
		sem_post(&SEM_LOGS);
		sleep(TIEMPO_RECONEXION);

		socket_de_envio=conectar_con_servidor(IP_BROKER,PUERTO_BROKER);
	}*/

	uint32_t id = 0;

	if(send(socket_de_envio, paquete_a_mandar,peso_paquete+sizeof(int),0) == 0)
	{
		log_info(log_obligatorio,"No me pude comunicar con Broker, vamos a realizar la operacion %d por default",GET_POKEMON);
		free(paquete_a_mandar);
	}else{
		free(paquete_a_mandar);
		int alocador = 0;
		void* recepcion = malloc(sizeof(uint32_t));
		recv(socket_de_envio,recepcion,sizeof(uint32_t),0);
		id = deserializar_id(recepcion);
	}
	return id;
}

uint32_t serializar_catch_pokemon(char* nombre_pokemon, uint32_t pos_x, uint32_t pos_y){

	int peso_char =  strlen(nombre_pokemon) +1 ;
	int peso_total = peso_char + sizeof(int) + (sizeof(uint32_t) * 2);
	int offset = 0;

	void* paquete = malloc(peso_total+sizeof(int));


	memcpy(paquete+offset,&peso_total,sizeof(int));
	offset+=sizeof(int);

	memcpy(paquete+offset,&pos_x,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(paquete+offset,&pos_y,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(paquete+offset,&peso_char,sizeof(int));
	offset+=sizeof(int);

	memcpy(paquete+offset,nombre_pokemon,peso_char);

	void* paquete_a_mandar = crear_paquete(CATCH_POKEMON,paquete);

	free(paquete);
	int peso_paquete=0;
	memcpy(&peso_paquete,paquete_a_mandar,sizeof(int));
	int socket_de_envio= conectar_con_servidor(IP_BROKER,PUERTO_BROKER);
	/* JORGE DICE QUE ESTE WHILE NO VA
	while(socket_de_envio < 3){
		sem_wait(&SEM_LOGS);
		log_info(log_obligatorio,"No pudimos conectarnos con Broker (socket= %d)",socket_de_envio);
		sem_post(&SEM_LOGS);
		sleep(TIEMPO_RECONEXION);

		socket_de_envio=conectar_con_servidor(IP_BROKER,PUERTO_BROKER);
	}*/

	if(send(socket_de_envio, paquete_a_mandar,peso_paquete+sizeof(int),0) != (peso_paquete+sizeof(int)))
	{
		free(paquete_a_mandar);
		return 0;
	}else{

		free(paquete_a_mandar);
	}

	uint32_t retorno;

	recv(socket_de_envio,&retorno,sizeof(uint32_t),0);

	return retorno;
}
