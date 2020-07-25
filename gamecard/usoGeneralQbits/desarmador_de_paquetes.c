/*
 * desarmador_de_paquetes.c
 *
 *  Created on: 5 abr. 2020
 *      Author: utnso
 */
#include "desarmador_de_paquetes.h"

void* recibir_paquete(int* alocador,int socket){
	void* buffer="QbitsRacers";
	if(recv(socket,alocador,sizeof(int),0)!=0){
		buffer=malloc(*alocador);
		recv(socket,buffer,*alocador,0);
	}
	return buffer;

}

int conseguir_codigo_operacion(void* buffer){

	int codigo_operacion;

	memcpy(&codigo_operacion,buffer,sizeof(int));

	return codigo_operacion;
}

void deserializar_ejecutar_new_pokemon(void* paquete){
	uint32_t pos_y, pos_x, cantidad, id_mensaje;
	char*nombre_pokemon;
	int offset=0;
	int longitud_pokemon;

	memcpy(&pos_x,paquete+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(&pos_y,paquete+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(&cantidad,paquete+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(&id_mensaje,paquete+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(&longitud_pokemon,paquete+offset,sizeof(int));
	offset+=sizeof(int);

	nombre_pokemon = malloc(longitud_pokemon);

	memcpy(nombre_pokemon,paquete+offset,longitud_pokemon);

	NEW_POKEMON_GAME_CARD(id_mensaje,pos_y,pos_x,nombre_pokemon,cantidad);

	free(nombre_pokemon);

}

void deserializar_ejecutar_catch_pokemon(void* paquete){

	uint32_t pos_y, pos_x, id_mensaje;
	char*nombre_pokemon;
	int offset=0;
	int longitud_pokemon;

	memcpy(&pos_x,paquete+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(&pos_y,paquete+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(&id_mensaje,paquete+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(&longitud_pokemon,paquete+offset,sizeof(int));
	offset+=sizeof(int);

	nombre_pokemon = malloc(longitud_pokemon);

	memcpy(nombre_pokemon,paquete+offset,longitud_pokemon);

	CATCH_POKEMON_GAME_CARD(id_mensaje,pos_y,pos_x,nombre_pokemon);

	free(nombre_pokemon);

}

void deserializar_ejecutar_get_pokemon(void* paquete){

	char*nombre_pokemon;
	uint32_t id_mensaje;
	int offset=0;
	int longitud_pokemon;

	memcpy(&id_mensaje,paquete+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(&longitud_pokemon,paquete+offset,sizeof(int));
	offset+=sizeof(int);

	nombre_pokemon = malloc(longitud_pokemon);

	memcpy(nombre_pokemon,paquete+offset,longitud_pokemon);

	GET_POKEMON_GAME_CARD(id_mensaje,nombre_pokemon);

	free(nombre_pokemon);

}

void* ejecutar_y_desarmar(int codigo_operacion,void* paquete,int socket){

	switch(codigo_operacion){

	case NEW_POKEMON:
		deserializar_ejecutar_new_pokemon(paquete);
		break;

	case CATCH_POKEMON:
		deserializar_ejecutar_catch_pokemon(paquete);
		break;

	case GET_POKEMON:
		deserializar_ejecutar_get_pokemon(paquete);
		break;

	default:
		log_info(log_cod_invalido,"Codigo Invalido");
		break;
	}
	return NULL;
}
