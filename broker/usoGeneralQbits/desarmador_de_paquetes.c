/*
 * desarmador_de_paquetes.c
 *
 *  Created on: 5 abr. 2020
 *      Author: utnso
 */
#include "desarmador_de_paquetes.h"
#include <stdint.h>
#include <pthread.h>

//send(SOCKET, LO_QUE_MANDO, CUANTO_MANDO, 0(PORQUE_SI));

void* recibir_paquete(int* alocador,int socket){
	void* buffer=NULL;
	if(recv(socket,alocador,sizeof(int),0)!=0){
		buffer=malloc(*alocador);
		recv(socket,buffer,*alocador,0);
	}
	return buffer;
}

void ejecutar_operacion(void* paquete, int socket){

	int codigo_operacion=conseguir_codigo_operacion(paquete);
	ejecutar_y_desarmar(codigo_operacion, paquete, socket);
	//Estaba asi: ejecutar_y_desarmar(codigo_operacion,paquete+sizeof(int),socket);
	//O sea, con el paquete SIN el codigo de operacion

}


int conseguir_codigo_operacion(void* paquete){
	int cod_operacion;
	memcpy(&cod_operacion,paquete,sizeof(int));

	return cod_operacion;
}

void ejecutar_new_pokemon(void* paquete, int socket) {
	//Agarro el id, calculo el peso de nuevo, pongo en un nuevo_paquete y se lo pasamos a la cola
	uint32_t longitud_pokemon;
	int offset=0;

	offset = 4 * sizeof(uint32_t);
	memcpy(&longitud_pokemon, paquete+offset, sizeof(uint32_t));

	uint32_t id_correlativo = generar_id_correlativo();

	uint32_t nuevo_peso = 6 * sizeof(uint32_t) + longitud_pokemon;

	void* nuevo_paquete = malloc(nuevo_peso + sizeof(uint32_t));

	memcpy(nuevo_paquete, &nuevo_peso, sizeof(uint32_t));
	memcpy(nuevo_paquete + sizeof(uint32_t), paquete, 4 * sizeof(uint32_t)); //Pego todos los numeros ya conocidos
	memcpy(nuevo_paquete + 5 * sizeof(uint32_t), &id_correlativo, sizeof(uint32_t)); //Pego el id
	memcpy(nuevo_paquete + 6 * sizeof(uint32_t), paquete + 4 * sizeof(uint32_t), sizeof(uint32_t) + longitud_pokemon); //Pego el peso y el string

	int peso_total = nuevo_peso + sizeof(uint32_t);

	uint32_t id_mensaje = generar_id_mensaje();
	mensaje_t* mensaje = (mensaje_t*)malloc(sizeof(mensaje_t));
	mensaje->cod_cola = 100;
	mensaje->id_correlativo = id_correlativo;
	mensaje->id_mensaje = id_mensaje;

	//Armo el paquete que guardo en cache
	void* paquete_cache = armar_paquete_cache(nuevo_paquete, peso_total, longitud_pokemon-1, 100);

	//Guardar en memoria y actualizar todas las estructuras que correspondan
	//Resto el \0, cod_op, peso y id
	guardar_en_cache(paquete_cache, peso_total - 1 - 3 * sizeof(uint32_t), mensaje->id_correlativo, mensaje->cod_cola, mensaje->id_mensaje);

	encolar_y_enviar_mensajes(nuevo_paquete, peso_total, mensaje, false, mensaje->id_correlativo);

	free(nuevo_paquete);
	free(paquete_cache);

}

void ejecutar_appeared_pokemon(void* paquete, int socket) {
	//Agarro el id, calculo el peso de nuevo, pongo en un nuevo_paquete y se lo pasamos a la cola
	uint32_t longitud_pokemon;
	int offset=0;
	uint32_t peso;
	void* nuevo_paquete;

	offset = 3 * sizeof(uint32_t); //por 3 o por 4 ???

	uint32_t id_correlativo;
	memcpy(&id_correlativo, paquete+offset, sizeof(uint32_t));
	offset = offset + sizeof(uint32_t);

	memcpy(&longitud_pokemon, paquete+offset, sizeof(uint32_t));

	peso = 5 * sizeof(uint32_t) + longitud_pokemon;

	nuevo_paquete = malloc(peso + sizeof(uint32_t));
	memcpy(nuevo_paquete, &peso, sizeof(uint32_t));
	memcpy(nuevo_paquete + sizeof(uint32_t), paquete, peso);

	int peso_total = peso + sizeof(uint32_t);

	uint32_t id_mensaje = generar_id_mensaje();
	mensaje_t* mensaje = (mensaje_t*)malloc(sizeof(mensaje_t));
	mensaje->cod_cola = 101;
	mensaje->id_correlativo = id_correlativo;
	mensaje->id_mensaje = id_mensaje;

	//Armo el paquete que guardo en cache
	void* paquete_cache = armar_paquete_cache(nuevo_paquete, peso_total, longitud_pokemon-1, 101);

	//Guardar en memoria y actualizar todas las estructuras que correspondan
	//Resto el \0, cod_op, peso y id
	guardar_en_cache(paquete_cache, peso_total - 1 - 3 * sizeof(uint32_t), mensaje->id_correlativo, mensaje->cod_cola, mensaje->id_mensaje);

	encolar_y_enviar_mensajes(nuevo_paquete, peso + sizeof(uint32_t), mensaje, false, mensaje->id_correlativo);

	free(nuevo_paquete);
	free(paquete_cache);

}

void ejecutar_catch_pokemon(void* paquete, int socket) {
	//Generamos id, lo ponemos en el nuevo_paquete y se lo pasamos a la cola
	uint32_t longitud_pokemon;
	char*nombre_pokemon;
	int offset= 3 * sizeof(uint32_t);

	uint32_t id_correlativo = generar_id_correlativo();
	void* id_para_mandar = (void*)malloc(sizeof(uint32_t));
	memcpy(id_para_mandar, &id_correlativo, sizeof(uint32_t));
	send(socket, id_para_mandar, sizeof(uint32_t), 0);
	free(id_para_mandar);

	memcpy(&longitud_pokemon, paquete+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	uint32_t nuevo_peso = 5 * sizeof(uint32_t) + longitud_pokemon;

	void* nuevo_paquete = malloc(nuevo_peso + sizeof(uint32_t));

	memcpy(nuevo_paquete, &nuevo_peso, sizeof(uint32_t));
	memcpy(nuevo_paquete + sizeof(uint32_t), paquete, 3 * sizeof(uint32_t)); //Pego todos los numeros ya conocidos
	memcpy(nuevo_paquete + 4 * sizeof(uint32_t), &id_correlativo, sizeof(uint32_t)); //Pego el id
	memcpy(nuevo_paquete + 5 * sizeof(uint32_t), paquete + 3 * sizeof(uint32_t), sizeof(uint32_t) + longitud_pokemon); //Pego el peso y el string

	int peso_total = nuevo_peso + sizeof(uint32_t);

	uint32_t id_mensaje = generar_id_mensaje();
	mensaje_t* mensaje = (mensaje_t*)malloc(sizeof(mensaje_t));
	mensaje->cod_cola = 102;
	mensaje->id_correlativo = id_correlativo;
	mensaje->id_mensaje = id_mensaje;

	//Armo el paquete que guardo en cache
	void* paquete_cache = armar_paquete_cache(nuevo_paquete, peso_total, longitud_pokemon-1, 102);

	//Guardar en memoria y actualizar todas las estructuras que correspondan
	//Resto el \0, cod_op, peso y id
	guardar_en_cache(paquete_cache, peso_total - 1 - 3 * sizeof(uint32_t), mensaje->id_correlativo, mensaje->cod_cola, mensaje->id_mensaje);

	encolar_y_enviar_mensajes(nuevo_paquete, nuevo_peso + sizeof(uint32_t), mensaje, true, id_correlativo);

	free(nuevo_paquete);
	free(paquete_cache);
}

void ejecutar_caught_pokemon(void* paquete, int socket) {
	//Agarro el id, calculo el peso de nuevo, pongo en un nuevo_paquete y se lo pasamos a la cola
	int offset=0;
	uint32_t peso;
	void* nuevo_paquete;

	offset = 2 * sizeof(uint32_t);

	uint32_t id_mensaje_correlativo;
	memcpy(&id_mensaje_correlativo, paquete+offset, sizeof(uint32_t));
	offset = offset + sizeof(uint32_t);

	peso = 3 * sizeof(uint32_t);

	nuevo_paquete = malloc(peso + sizeof(uint32_t));
	memcpy(nuevo_paquete, &peso, sizeof(uint32_t));
	memcpy(nuevo_paquete + sizeof(uint32_t), paquete, peso);

	int peso_total = peso + sizeof(uint32_t);

	uint32_t id_mensaje = generar_id_mensaje();
	mensaje_t* mensaje = (mensaje_t*)malloc(sizeof(mensaje_t));
	mensaje->cod_cola = 103;
	mensaje->id_correlativo = id_mensaje_correlativo;
	mensaje->id_mensaje = id_mensaje;

	//Armo el paquete que guardo en cache
	void* paquete_cache = armar_paquete_cache(nuevo_paquete, peso_total, 0, 103);

	//Guardar en memoria y actualizar todas las estructuras que correspondan
	//Resto cod_op, peso y id
	guardar_en_cache(paquete_cache, peso_total - 3 * sizeof(uint32_t), mensaje->id_correlativo, mensaje->cod_cola, mensaje->id_mensaje);

	encolar_y_enviar_mensajes(nuevo_paquete, peso + sizeof(uint32_t), mensaje, false, mensaje->id_correlativo);

	free(nuevo_paquete);
	free(paquete_cache);
}

void ejecutar_get_pokemon(void* paquete, int socket) {
	//Generamos id, lo ponemos en el nuevo_paquete y se lo pasamos a la cola
	uint32_t longitud_pokemon;
	char*nombre_pokemon;
	int offset= sizeof(uint32_t);

	uint32_t id_mensaje_correlativo = generar_id_correlativo();
	void* id_para_mandar = (void*)malloc(sizeof(uint32_t));
	memcpy(id_para_mandar, &id_mensaje_correlativo, sizeof(uint32_t));
	send(socket, id_para_mandar, sizeof(uint32_t), 0);
	free(id_para_mandar);

	memcpy(&longitud_pokemon, paquete+offset, sizeof(uint32_t));

	uint32_t nuevo_peso = 3 * sizeof(uint32_t) + longitud_pokemon;

	void* nuevo_paquete = malloc(nuevo_peso + sizeof(uint32_t));

	memcpy(nuevo_paquete, &nuevo_peso, sizeof(uint32_t));
	memcpy(nuevo_paquete + sizeof(uint32_t), paquete, sizeof(uint32_t)); //Pego todos los numeros ya conocidos
	memcpy(nuevo_paquete + 2 * sizeof(uint32_t), &id_mensaje_correlativo, sizeof(uint32_t)); //Pego el id
	memcpy(nuevo_paquete +  3 * sizeof(uint32_t), paquete + sizeof(uint32_t), sizeof(uint32_t) + longitud_pokemon); //Pego el peso y el string

	int peso_total = nuevo_peso + sizeof(uint32_t);

	uint32_t id_mensaje = generar_id_mensaje();
	mensaje_t* mensaje = (mensaje_t*)malloc(sizeof(mensaje_t));
	mensaje->cod_cola = 104;
	mensaje->id_correlativo = id_mensaje_correlativo;
	mensaje->id_mensaje = id_mensaje;

	//Armo el paquete que guardo en cache
	void* paquete_cache = armar_paquete_cache(nuevo_paquete, peso_total, longitud_pokemon-1, 104);

	//Guardar en memoria y actualizar todas las estructuras que correspondan
	//Resto el \0, cod_op, peso y id
	guardar_en_cache(paquete_cache, peso_total - 1 - 3 * sizeof(uint32_t), mensaje->id_correlativo, mensaje->cod_cola, mensaje->id_mensaje);

	encolar_y_enviar_mensajes(nuevo_paquete, nuevo_peso + sizeof(uint32_t), mensaje, true, id_mensaje_correlativo);

	free(nuevo_paquete);
	free(paquete_cache);
}

void ejecutar_localized_pokemon(void* paquete, int socket) {
	//Agarro el id, calculo el peso de nuevo, pongo en un nuevo_paquete y se lo pasamos a la cola
	int offset=0;
	uint32_t peso;
	uint32_t longitud_pokemon;
	uint32_t cant_coordenadas;
	uint32_t peso_coordenadas;
	void* nuevo_paquete;

	offset = sizeof(uint32_t);

	uint32_t id_mensaje_correlativo;
	memcpy(&id_mensaje_correlativo, paquete+offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(&longitud_pokemon, paquete + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	offset += longitud_pokemon;

	memcpy(&cant_coordenadas, paquete + offset, sizeof(uint32_t));
	peso_coordenadas = cant_coordenadas * 2 * sizeof(uint32_t);

	peso = 4 * sizeof(uint32_t) + longitud_pokemon + peso_coordenadas;

	nuevo_paquete = malloc(peso + sizeof(uint32_t));
	memcpy(nuevo_paquete, &peso, sizeof(uint32_t));
	memcpy(nuevo_paquete + sizeof(uint32_t), paquete, peso);

	int peso_total = peso + sizeof(uint32_t);

	uint32_t id_mensaje = generar_id_mensaje();
	mensaje_t* mensaje = (mensaje_t*)malloc(sizeof(mensaje_t));
	mensaje->cod_cola = 105;
	mensaje->id_correlativo = id_mensaje_correlativo;
	mensaje->id_mensaje = id_mensaje;

	//Armo el paquete que guardo en cache
	void* paquete_cache = armar_paquete_cache(nuevo_paquete, peso_total, longitud_pokemon-1, 105);

	//Guardar en memoria y actualizar todas las estructuras que correspondan
	//Resto el \0, cod_op, peso y id
	guardar_en_cache(paquete_cache, peso_total - 1 - 3 * sizeof(uint32_t), mensaje->id_correlativo, mensaje->cod_cola, mensaje->id_mensaje);

	encolar_y_enviar_mensajes(nuevo_paquete, peso + sizeof(uint32_t), mensaje, false, mensaje->id_correlativo);

	free(nuevo_paquete);
	free(paquete_cache);
}

void ejecutar_nuevo_suscriptor(void* paquete, int socket) {
	uint32_t cola_requerida;
	uint32_t offset = sizeof(uint32_t);
	long id_cliente;
	char* ip_cliente;
	int puerto_cliente;
	int peso_ip;

	memcpy(&cola_requerida, paquete + offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(&id_cliente, paquete + offset, sizeof(long));
	offset+=sizeof(long);
	memcpy(&peso_ip,paquete + offset,sizeof(int));
	offset+=sizeof(int);
	ip_cliente = malloc(peso_ip);
	memcpy(ip_cliente,paquete+offset,peso_ip);
	offset+=peso_ip;
	memcpy(&puerto_cliente,paquete+offset,sizeof(int));

	agregar_suscriptor(cola_requerida, id_cliente, ip_cliente, puerto_cliente);

	forwardear_mensajes(cola_requerida, id_cliente);
}

void ejecutar_disconnect(void* paquete) {
	long id_cliente;
	uint32_t cod_cola_num;
	uint32_t offset = sizeof(uint32_t);

	memcpy(&cod_cola_num, paquete + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&id_cliente, paquete + offset, sizeof(long));

	char* cod_cola = string_itoa(cod_cola_num);

	disconnect(cod_cola, id_cliente);
	free(cod_cola);
}

void* ejecutar_y_desarmar(int codigo_operacion, void* paquete, int socket){
	//Nostros como broker no nos tomamos el laburo de abrir el paquete, total lo vamos a tener que pasar nada mas.
	//Con conocer el codigo de operacion nos alcanza.

	switch(codigo_operacion){

	case NEW_POKEMON://Hay que generar un id
		loggear_mensaje("Llego un NEW_POKEMON", 0);
		ejecutar_new_pokemon(paquete, socket);
		close(socket);
		break;

	case APPEARED_POKEMON://NO hay que generarle id
		loggear_mensaje("Llego un APPEARED_POKEMON", 0);
		ejecutar_appeared_pokemon(paquete, socket);
		close(socket);
		break;

	case CATCH_POKEMON://Hay que generar un id
		loggear_mensaje("Llego un CATCH_POKEMON", 0);
		ejecutar_catch_pokemon(paquete, socket);
		close(socket);
		break;

	case CAUGHT_POKEMON://NO hay que generar un id
		loggear_mensaje("Llego un CAUGHT_POKEMON", 0);
		ejecutar_caught_pokemon(paquete, socket);
		close(socket);
		break;

	case GET_POKEMON://Hay que generar un id
		loggear_mensaje("Llego un GET_POKEMON", 0);
		ejecutar_get_pokemon(paquete, socket);
		close(socket);
		break;

	case LOCALIZED_POKEMON://NO hay que generar un id
		loggear_mensaje("Llego un LOCALIZED_POKEMON", 0);
		ejecutar_localized_pokemon(paquete, socket);
		close(socket);
		break;

	case SUSCRIPTOR:
		loggear_mensaje("Llego una SUSCRIPCION", 0);
		ejecutar_nuevo_suscriptor(paquete, socket);
		close(socket);
		break;

	case DISCONNECT:
		loggear_mensaje("Llego un DISCONNECT", 0);
		ejecutar_disconnect(paquete);
		break;

	case QBIT:
		loggear_mensaje("Llego un QBIT", 0);
		break;

	default:
		loggear_mensaje("Se recibio un CODIGO INVALIDO", 0);
		break;
	}
	return NULL;
}
