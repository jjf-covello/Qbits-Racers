/*
 * desarmador_de_paquetes.c
 *
 *  Created on: 5 abr. 2020
 *      Author: utnso
 */
#include "desarmador_de_paquetes.h"

void* recibir_paquete(int* alocador,int socket){
	void* buffer=NULL;
	if(recv(socket,alocador,sizeof(int),0)!=0){
		buffer=malloc(*alocador);
		recv(socket,buffer,*alocador,0);
	}
	return buffer;


}

void ejecutar_operacion(void* paquete,int socket){

	int codigo_operacion=conseguir_codigo_operacion(paquete);
	ejecutar_y_desarmar(codigo_operacion,paquete+sizeof(int),socket);

}


int conseguir_codigo_operacion(void* paquete){
	int cod_operacion;
	memcpy(&cod_operacion,paquete,sizeof(int));

	return cod_operacion;
}


void* ejecutar_y_desarmar(int codigo_operacion,void* paquete,int socket){

	switch(codigo_operacion){

	case NEW_POKEMON:
		log_info(log_operaciones,"Recibi un NEW_POKEMON");
		break;

	case APPEARED_POKEMON:
		log_info(log_operaciones,"Recibi un APPEARED_POKEMON");
		break;

	case CATCH_POKEMON:
		log_info(log_operaciones,"Recibi un CATCH_POKEMON");
		break;

	case CAUGHT_POKEMON:
		log_info(log_operaciones,"Recibi un CAUGHT_POKEMON");
		break;

	case GET_POKEMON:
		log_info(log_operaciones,"Recibi un GET_POKEMON");
		break;
	}

	//Mando el ack
	void* ack = malloc(sizeof(int));
	int num = 5138008;
	memcpy(ack, &num, sizeof(int));
	send(socket, ack, sizeof(int), 0);
	free(ack);

	return NULL;
}
