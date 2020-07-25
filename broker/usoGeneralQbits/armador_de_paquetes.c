/*
 * armador_de_paquetes.c
 *
 *  Created on: 5 abr. 2020
 *      Author: utnso
 */
#include "armador_de_paquetes.h"

void enviar_paquete(int socket, void* paquete_serializado){

	int peso_paquete=0;
	memcpy(&peso_paquete,paquete_serializado,sizeof(int));
	send(socket,paquete_serializado,peso_paquete+sizeof(int),0);

}


void* crear_paquete(int codigo_operacion, void* buffer){
	/*
	 * el codigo de operacion que es un enum y el buffer es una estructura en bytes
	 * que esta compuesta por lo siguiente:
	 *
	 * 1) el peso de lo que sigue
	 * 2) data en bytes que desconoce
	 *
	 * */


	int peso_paquete=0;

	void* paquete;
	memcpy(&peso_paquete,buffer,sizeof(int));
	int peso_nuevo=peso_paquete+sizeof(int);

	paquete= malloc(peso_nuevo+sizeof(int));
	memcpy(paquete,&peso_nuevo,sizeof(int));
	memcpy(paquete+sizeof(int),&codigo_operacion,sizeof(int));
	memcpy(paquete+sizeof(int)+sizeof(int),buffer+sizeof(int),peso_paquete);

	return paquete;
}
