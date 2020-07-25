/*
 * funciones_compartidas.c
 *
 *  Created on: 15 abr. 2020
 *      Author: utnso
 */

#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include "tads_y_globales.h"
#include <standard_libraries.h>

void loggear_mensaje(char* mensaje, int flag) {
	//Para loggear en nuestros dos logs. En true (1) usa los dos, en false (0) solo el nuestro
	sem_wait(&sem_logs);
	if(flag) {
		log_info(log_oficial, mensaje);
		log_info(log_interno, mensaje);
	} else {
		log_info(log_interno, mensaje);
	}
	sem_post(&sem_logs);
}

uint32_t generar_id_mensaje() {
	sem_wait(&sem_acum_id);
	acum_id_mensaje++;
	uint32_t valor_a_retornar = acum_id_mensaje;
	sem_post(&sem_acum_id);
	return valor_a_retornar;
}

uint32_t generar_id_correlativo() {
	sem_wait(&sem_acum_correlativo);
	acum_id_correlativo++;
	uint32_t valor_a_retornar = acum_id_correlativo;
	sem_post(&sem_acum_correlativo);
	return valor_a_retornar;
}

uint32_t get_timestamp() {
	char* tiempo_string = temporal_get_string_time();
	char** array_tiempo = string_split(tiempo_string, ":");
	char* minutos_y_segundos = (char*)malloc(20);
	memset(minutos_y_segundos, '\0', 20);
	string_append(&minutos_y_segundos, array_tiempo[0]);
	string_append(&minutos_y_segundos, array_tiempo[1]);
	string_append(&minutos_y_segundos, array_tiempo[2]);
	string_append(&minutos_y_segundos, array_tiempo[3]);
	uint32_t resultado = atoi(minutos_y_segundos);
	free(tiempo_string);

	int i=0;
	while(array_tiempo[i]!=NULL){
		free(array_tiempo[i]);
		i++;
	}
	free(array_tiempo);

	free(minutos_y_segundos);
	return resultado;
	//(unsigned)time(NULL);
}

cliente_t* cliente_suscripto(long id_cliente, int cola_requerida, char* ip_cliente, int puerto_cliente) {
	//Busca al cliente en la lista de clientes. Si no esta, crea uno nuevo y agrega el socket.
	//Si lo encuentra, agrega el socket

	sem_wait(&sem_lista_de_clientes);

	bool es_el_cliente(cliente_t* un_cli){
		return un_cli->id == id_cliente;
	}
	cliente_t* cliente;
	char* cod_cola = string_itoa(cola_requerida);

	t_list* lista_filtrada = list_filter(lista_de_clientes, (void*)es_el_cliente);

	if(lista_filtrada->elements_count > 0) {
		//Encontre al cliente
		cliente = list_find(lista_de_clientes, (void*)es_el_cliente);
		/*
		cliente->ip = ip_cliente;
		cliente->puerto = puerto_cliente;
		*/
		free(ip_cliente);
	} else {
		//No encontre al cliente
		cliente = (cliente_t*)malloc(sizeof(cliente_t));
		cliente->id = id_cliente;
		cliente->ip = ip_cliente;
		cliente->puerto = puerto_cliente;
		list_add(lista_de_clientes, cliente);
	}

	list_destroy(lista_filtrada);
	free(cod_cola);

	sem_post(&sem_lista_de_clientes);

	return cliente;
}

void agregar_suscriptor(int cola_requerida, long id_cliente, char* ip_cliente, int puerto_cliente) {

	char* id = string_itoa(id_cliente);
	char* msj = (char*)malloc(100);
	memset(msj, '\0', 100);
	string_append(&msj, "Cliente ");
	string_append(&msj, id);
	string_append(&msj, " suscripto a ");

	cliente_t* cliente = cliente_suscripto(id_cliente, cola_requerida, ip_cliente, puerto_cliente);

	t_list* lista_susc_a_usar;
	pthread_mutex_t* sem_lista_susc;

	switch(cola_requerida){

	case NEW_POKEMON:
		string_append(&msj, "NEW_POKEMON");
		loggear_mensaje(msj, 1);
		lista_susc_a_usar = lista_de_suscriptores_new_pokemon;
		sem_lista_susc = &sem_susc_new;
		break;

	case APPEARED_POKEMON:
		string_append(&msj, "APPEARED_POKEMON");
		loggear_mensaje(msj, 1);
		lista_susc_a_usar = lista_de_suscriptores_appeared_pokemon;
		sem_lista_susc = &sem_susc_app;
		break;

	case CATCH_POKEMON:
		string_append(&msj, "CATCH_POKEMON");
		loggear_mensaje(msj, 1);
		lista_susc_a_usar = lista_de_suscriptores_catch_pokemon;
		sem_lista_susc = &sem_susc_catch;
		break;

	case CAUGHT_POKEMON:
		string_append(&msj, "CAUGHT_POKEMON");
		loggear_mensaje(msj, 1);
		lista_susc_a_usar = lista_de_suscriptores_caught_pokemon;
		sem_lista_susc = &sem_susc_caught;
		break;

	case GET_POKEMON:
		string_append(&msj, "GET_POKEMON");
		loggear_mensaje(msj, 1);
		lista_susc_a_usar = lista_de_suscriptores_get_pokemon;
		sem_lista_susc = &sem_susc_get;
		break;

	case LOCALIZED_POKEMON:
		string_append(&msj, "LOCALIZED_POKEMON");
		loggear_mensaje(msj, 1);
		lista_susc_a_usar = lista_de_suscriptores_localized_pokemon;
		sem_lista_susc = &sem_susc_localized;
		break;

	default:
		loggear_mensaje("Se recibio un CODIGO INVALIDO", 0);
		break;
	}

	sem_wait(sem_lista_susc);
	bool es_el_cliente(cliente_t* un_cli){
		return un_cli->id == id_cliente;
	}

	t_list* lista_filtrada = list_filter(lista_susc_a_usar, (void*)es_el_cliente);

	if(lista_filtrada->elements_count == 0) {
		//El cliente NO estaba suscripto
		list_add(lista_susc_a_usar, cliente);
	}

	list_destroy(lista_filtrada);

	sem_post(sem_lista_susc);

	free(msj);
	free(id);

	//Off ack de la suscripcion...

}

uint32_t potencia(uint32_t base, uint32_t exponente) {
	uint32_t acumulador = 1;
	for(int i = 0; i < exponente; i++){
		acumulador = acumulador * base;
	}
	return acumulador;
}

void desconectar_cliente(cliente_t* cliente, char* cod_cola, int flag) {
	//For the sake of reutilizar: con el flag en 0 solo saco un socket
	//con el flag en 1 hago la desconexion a full

	bool es_el_cliente(cliente_t* un_cli){
		return un_cli->id == cliente->id;
	}

	if(flag) {
		int cod_iterativo = 100;
		while(cod_iterativo < 106) {
			char* cod_iterativo_string = string_itoa(cod_iterativo);


			//Saco al suscriptor de la lista
			t_list* lista_susc_a_usar;
			pthread_mutex_t* sem_lista_susc;
			switch(cod_iterativo){

			case NEW_POKEMON:
				lista_susc_a_usar = lista_de_suscriptores_new_pokemon;
				sem_lista_susc = &sem_susc_new;
				break;

			case APPEARED_POKEMON:
				lista_susc_a_usar = lista_de_suscriptores_appeared_pokemon;
				sem_lista_susc = &sem_susc_app;
				break;

			case CATCH_POKEMON:
				lista_susc_a_usar = lista_de_suscriptores_catch_pokemon;
				sem_lista_susc = &sem_susc_catch;
				break;

			case CAUGHT_POKEMON:
				lista_susc_a_usar = lista_de_suscriptores_caught_pokemon;
				sem_lista_susc = &sem_susc_caught;
				break;

			case GET_POKEMON:
				lista_susc_a_usar = lista_de_suscriptores_get_pokemon;
				sem_lista_susc = &sem_susc_get;
				break;

			case LOCALIZED_POKEMON:
				lista_susc_a_usar = lista_de_suscriptores_localized_pokemon;
				sem_lista_susc = &sem_susc_localized;
				break;

			default:
				loggear_mensaje("Se recibio un CODIGO INVALIDO", 0);
				break;
			}

			sem_wait(sem_lista_susc);
			cliente_t* cliente_removido = list_remove_by_condition(lista_susc_a_usar, (void*)es_el_cliente);
			sem_post(sem_lista_susc);

			if(cliente_removido != NULL){
				//Log stuff
				char* idcli = string_itoa(cliente->id);
				char* log_this = (char*)malloc(strlen("Cliente ")+strlen(idcli)+strlen(" se bajo de la cola ")+strlen(cod_cola));
				memset(log_this, '\0', strlen("Cliente ")+strlen(idcli)+strlen(" se bajo de la cola ")+strlen(cod_cola));
				string_append(&log_this, "Cliente ");
				string_append(&log_this, idcli);
				string_append(&log_this, " se bajo de la cola ");
				string_append(&log_this, cod_iterativo_string);
				loggear_mensaje(log_this, 0);
				free(idcli);
				free(log_this);
			}

			free(cod_iterativo_string);
			cod_iterativo++;
		}
	} else {
		//Saco al suscriptor de la lista
		t_list* lista_susc_a_usar;
		pthread_mutex_t* sem_lista_susc;
		switch(atoi(cod_cola)){

		case NEW_POKEMON:
			lista_susc_a_usar = lista_de_suscriptores_new_pokemon;
			sem_lista_susc = &sem_susc_new;
			break;

		case APPEARED_POKEMON:
			lista_susc_a_usar = lista_de_suscriptores_appeared_pokemon;
			sem_lista_susc = &sem_susc_app;
			break;

		case CATCH_POKEMON:
			lista_susc_a_usar = lista_de_suscriptores_catch_pokemon;
			sem_lista_susc = &sem_susc_catch;
			break;

		case CAUGHT_POKEMON:
			lista_susc_a_usar = lista_de_suscriptores_caught_pokemon;
			sem_lista_susc = &sem_susc_caught;
			break;

		case GET_POKEMON:
			lista_susc_a_usar = lista_de_suscriptores_get_pokemon;
			sem_lista_susc = &sem_susc_get;
			break;

		case LOCALIZED_POKEMON:
			lista_susc_a_usar = lista_de_suscriptores_localized_pokemon;
			sem_lista_susc = &sem_susc_localized;
			break;

		default:
			loggear_mensaje("Se recibio un CODIGO INVALIDO", 0);
			break;
		}

		sem_wait(sem_lista_susc);
		cliente_t* cliente_removido = list_remove_by_condition(lista_susc_a_usar, (void*)es_el_cliente);
		sem_post(sem_lista_susc);

		if(cliente_removido != NULL){
			//Log stuff
			char* idcli = string_itoa(cliente->id);
			char* log_this = (char*)malloc(strlen("Cliente ")+strlen(idcli)+strlen(" se bajo de la cola ")+strlen(cod_cola));
			memset(log_this, '\0', strlen("Cliente ")+strlen(idcli)+strlen(" se bajo de la cola ")+strlen(cod_cola));
			string_append(&log_this, "Cliente ");
			string_append(&log_this, idcli);
			string_append(&log_this, " se bajo de la cola ");
			string_append(&log_this, cod_cola);
			loggear_mensaje(log_this, 0);
			free(idcli);
			free(log_this);

			//TODO
			//Un free de cliente por aca?
		}
	}
	/*
	//Log stuff
	char* idcli = string_itoa(cliente->id);
	char* log_this = (char*)malloc(strlen("Cliente ")+strlen(idcli)+strlen(" salio del sistema"));
	memset(log_this, '\0', strlen("Cliente ")+strlen(idcli)+strlen(" salio del sistema"));
	string_append(&log_this, "Cliente ");
	string_append(&log_this, idcli);
	string_append(&log_this, " salio del sistema");
	loggear_mensaje(log_this, 0);
	free(idcli);
	free(log_this);

	sem_wait(&sem_lista_de_clientes);
	list_remove_by_condition(lista_de_clientes, (void*)es_el_cliente);
	sem_post(&sem_lista_de_clientes);
	free(cliente);
	 */
}

void revisar_send_y_esperar_ack(void* parametros) {
	struct parametros_esperar_ack* parametros_leidos = (struct parametros_esperar_ack*) parametros;

	t_list* lista_a_usar;
	pthread_mutex_t* sem_lista;

	switch(parametros_leidos->cola_a_usar){

	case NEW_POKEMON:
		lista_a_usar = lista_mensajes_new;
		sem_lista = &sem_mensajes_new;
		break;

	case APPEARED_POKEMON:
		lista_a_usar = lista_mensajes_app;
		sem_lista = &sem_mensajes_app;
		break;

	case CATCH_POKEMON:
		lista_a_usar = lista_mensajes_catch;
		sem_lista = &sem_mensajes_catch;
		break;

	case CAUGHT_POKEMON:
		lista_a_usar = lista_mensajes_caught;
		sem_lista = &sem_mensajes_caught;
		break;

	case GET_POKEMON:
		lista_a_usar = lista_mensajes_get;
		sem_lista = &sem_mensajes_get;
		break;

	case LOCALIZED_POKEMON:
		lista_a_usar = lista_mensajes_localized;
		sem_lista = &sem_mensajes_localized;
		break;

	default:
		loggear_mensaje("Se recibio un CODIGO INVALIDO", 0);
		break;
	}

	sem_wait(sem_lista);
	bool es_el_mensaje(mensaje_enviado_t* un_mensaje){
		return un_mensaje->mensaje->id_mensaje == parametros_leidos->id_mensaje_enviado;
	}
	mensaje_enviado_t* mensaje_enviado = list_find(lista_a_usar, (void*)es_el_mensaje);

	if(mensaje_enviado != NULL) {
		sem_wait(&mensaje_enviado->sem_mensaje);
		list_add(mensaje_enviado->cli_enviado, parametros_leidos->cliente);
		sem_post(&mensaje_enviado->sem_mensaje);
	}
	sem_post(sem_lista);

	int ack = -1;
	int return_del_recv;
	char* cola_string = string_itoa(parametros_leidos->cola_a_usar);
	int socket_de_la_cola = parametros_leidos->socket;

	if(parametros_leidos->return_del_send == parametros_leidos->peso_esperado) {
		//Se pudo hacer el send
		return_del_recv = recv(socket_de_la_cola, &ack, 4, 0);

		if(return_del_recv > 0 && ack == 5138008) {
			//Hay algo!
			sem_wait(sem_lista);
			mensaje_enviado_t* mensaje_enviado = list_find(lista_a_usar, (void*)es_el_mensaje);
			if(mensaje_enviado != NULL) {
				sem_wait(&mensaje_enviado->sem_mensaje);
				list_add(mensaje_enviado->cli_ack, parametros_leidos->cliente->id);
				sem_post(&mensaje_enviado->sem_mensaje);
			}

			//Log stuff
			char* idmsj = string_itoa(parametros_leidos->id_mensaje_enviado);
			char* idcli = string_itoa(parametros_leidos->cliente->id);
			char* log_this = (char*)malloc(strlen("Mensaje ")+strlen(idmsj)+strlen(" recibido por ")+strlen(idcli));
			memset(log_this, '\0', strlen("Mensaje ")+strlen(idmsj)+strlen(" recibido por ")+strlen(idcli));
			string_append(&log_this, "Mensaje ");
			string_append(&log_this, idmsj);
			string_append(&log_this, " recibido por ");
			string_append(&log_this, idcli);
			loggear_mensaje(log_this, 1);
			free(idmsj);
			free(idcli);
			free(log_this);
			sem_post(sem_lista);
		} else {
			//Se desconectaron...
			desconectar_cliente(parametros_leidos->cliente, cola_string, 0);
		}
	} else {
		//El send no funco
		desconectar_cliente(parametros_leidos->cliente, cola_string, 0);
	}

	close(socket_de_la_cola);

	free(parametros_leidos);
	free(cola_string);
}

void encolar_y_enviar_mensajes(void* nuevo_paquete, int nuevo_peso, mensaje_t* mensaje, bool flag,uint32_t id_correlativo) {

	t_queue* cola_a_usar;
	pthread_mutex_t* sem_cola;

	t_list* lista_susc_a_usar;
	pthread_mutex_t* sem_lista_susc;

	t_list* lista_mensajes_a_usar;
	pthread_mutex_t* sem_lista_msj;

	switch(mensaje->cod_cola){

	case NEW_POKEMON:
		loggear_mensaje("Nuevo mensaje agregado a NEW_POKEMON", 1);
		cola_a_usar = cola_new_pokemon;
		lista_susc_a_usar = lista_de_suscriptores_new_pokemon;
		lista_mensajes_a_usar= lista_mensajes_new;
		sem_cola = &sem_cola_new;
		sem_lista_susc = &sem_susc_new;
		sem_lista_msj = &sem_mensajes_new;
		break;

	case APPEARED_POKEMON:
		loggear_mensaje("Nuevo mensaje agregado a APPEARED_POKEMON", 1);
		cola_a_usar = cola_appeared_pokemon;
		lista_susc_a_usar = lista_de_suscriptores_appeared_pokemon;
		lista_mensajes_a_usar= lista_mensajes_app;
		sem_cola = &sem_cola_app;
		sem_lista_susc = &sem_susc_app;
		sem_lista_msj = &sem_mensajes_app;
		break;

	case CATCH_POKEMON:
		loggear_mensaje("Nuevo mensaje agregado a CATCH_POKEMON", 1);
		cola_a_usar = cola_catch_pokemon;
		lista_susc_a_usar = lista_de_suscriptores_catch_pokemon;
		lista_mensajes_a_usar= lista_mensajes_catch;
		sem_cola = &sem_cola_catch;
		sem_lista_susc = &sem_susc_catch;
		sem_lista_msj = &sem_mensajes_catch;
		break;

	case CAUGHT_POKEMON:
		loggear_mensaje("Nuevo mensaje agregado a CAUGHT_POKEMON", 1);
		cola_a_usar = cola_caught_pokemon;
		lista_susc_a_usar = lista_de_suscriptores_caught_pokemon;
		lista_mensajes_a_usar= lista_mensajes_caught;
		sem_cola = &sem_cola_caught;
		sem_lista_susc = &sem_susc_caught;
		sem_lista_msj = &sem_mensajes_caught;
		break;

	case GET_POKEMON:
		loggear_mensaje("Nuevo mensaje agregado a GET_POKEMON", 1);
		cola_a_usar = cola_get_pokemon;
		lista_susc_a_usar = lista_de_suscriptores_get_pokemon;
		lista_mensajes_a_usar= lista_mensajes_get;
		sem_cola = &sem_cola_get;
		sem_lista_susc = &sem_susc_get;
		sem_lista_msj = &sem_mensajes_get;
		break;

	case LOCALIZED_POKEMON:
		cola_a_usar = cola_localized_pokemon;
		lista_susc_a_usar = lista_de_suscriptores_localized_pokemon;
		lista_mensajes_a_usar= lista_mensajes_localized;
		sem_cola = &sem_cola_localized;
		sem_lista_susc = &sem_susc_localized;
		sem_lista_msj = &sem_mensajes_localized;
		break;

	default:
		loggear_mensaje("Se recibio un CODIGO INVALIDO", 0);
		break;
	}

	uint32_t id_mensaje = mensaje->id_mensaje;
	uint32_t cod_cola_int = mensaje->cod_cola;
	char* cod_cola = string_itoa(mensaje->cod_cola);

	sem_wait(sem_cola);
	queue_push(cola_a_usar, mensaje);
	sem_post(sem_cola);

	//Creo el mensaje a enviar y lo agrego a la lista
	mensaje_enviado_t* nuevo_mensaje = (mensaje_enviado_t*)malloc(sizeof(mensaje_enviado_t));
	nuevo_mensaje->mensaje = mensaje;
	nuevo_mensaje->cli_enviado = list_create();
	nuevo_mensaje->cli_ack = list_create();
	sem_init(&nuevo_mensaje->sem_mensaje, 0, 1);
	sem_wait(sem_lista_msj);
	list_add(lista_mensajes_a_usar, nuevo_mensaje);
	sem_post(sem_lista_msj);

	//Itero la lista y le mando el paquete a cada suscriptor
	sem_wait(sem_lista_susc);
	for(int i = 0; i < lista_susc_a_usar->elements_count; i++) {
		sem_wait(&sem_hacer_el_send);
		cliente_t* un_cliente = list_get(lista_susc_a_usar, i);
		pthread_t* hilo;

		int return_del_send;
		int socket = conectar_con_servidor(un_cliente->ip,un_cliente->puerto);

		if(socket != -1){
			return_del_send = send(socket, nuevo_paquete, nuevo_peso, 0);

			//Log stuff
			char* idmsj = string_itoa(id_mensaje);
			char* idcli = string_itoa(un_cliente->id);
			char* log_this = (char*)malloc(strlen("Mensaje ")+strlen(idmsj)+strlen(" enviado a ")+strlen(idcli));
			memset(log_this, '\0', strlen("Mensaje ")+strlen(idmsj)+strlen(" enviado a ")+strlen(idcli));
			string_append(&log_this, "Mensaje ");
			string_append(&log_this, idmsj);
			string_append(&log_this, " enviado a ");
			string_append(&log_this, idcli);
			loggear_mensaje(log_this, 1);
			free(idmsj);
			free(idcli);
			free(log_this);

			struct parametros_esperar_ack *parametros = (struct parametros_epserar_ack*)malloc(sizeof(struct parametros_esperar_ack));
			parametros->cliente = un_cliente;
			parametros->id_mensaje_enviado = id_mensaje;
			parametros->cola_a_usar = cod_cola_int;
			parametros->return_del_send = return_del_send;
			parametros->peso_esperado = nuevo_peso;
			parametros->socket = socket;

			pthread_create(&hilo, NULL,  (void*)revisar_send_y_esperar_ack, parametros);
			pthread_detach(hilo);
		}
		sem_post(&sem_hacer_el_send);
	}
	sem_post(sem_lista_susc);

	free(cod_cola);

}

void guardar_en_cache(void* nuevo_paquete, int peso_total, uint32_t id_correlativo, uint32_t cod_cola, uint32_t id_mensaje) {
	sem_wait(&sem_cache);
	int posicion_lista;
	switch(COD_ALGORITMO_MEMORIA){

	case BS:
		guardar_en_cache_bs(nuevo_paquete, peso_total, id_correlativo, cod_cola, id_mensaje);
		break;

	case PARTICIONES:
		algoritmo_chevere(peso_total, nuevo_paquete, id_correlativo, cod_cola, id_mensaje);
		break;


	default:
		loggear_mensaje("Se recibio un CODIGO INVALIDO", 0);
		break;
	}
	sem_post(&sem_cache);
}

void actualizar_posiciones() {
	//Va iterando la lista haciendo que la posicion que dice ocupar la metadata
	//sea efectivamente la que ocupa
	sem_wait(&sem_lista_particiones);
	for(int i = 0; i < lista_de_particiones->elements_count; i++) {
		metadata_t* md_primera = list_get(lista_de_particiones, i);
		md_primera->posicion_en_lista = i;
	}
	sem_post(&sem_lista_particiones);
}

uint32_t obtener_offset_en_cache(int particion_a_usar) {
	//Te devuelve cuando te tenes que correr desde el inicio de la cache
	//para escribir donde te corresponde
	uint32_t acumulador = 0;
	sem_wait(&sem_lista_particiones);
	for(int i = 0; i < particion_a_usar; i++) {
		metadata_t* md = list_get(lista_de_particiones, i);
		acumulador += md->tamanio_ocupado;
	}
	sem_post(&sem_lista_particiones);
	return acumulador;
}

void* armar_paquete_leido(void* paquete_leido_incompleto, int cola, int id_correlativo, int peso_en_cache) {

	void* paquete_leido_completo;
	int offset = 0;
	char* string_con_barra;
	int long_pokemon;
	int peso_total;
	uint32_t cant_coordenadas;

	switch(cola){

	case NEW_POKEMON:
		peso_total = peso_en_cache + 1 + 2 * sizeof(uint32_t);

		memcpy(&long_pokemon, paquete_leido_incompleto + 3 * sizeof(uint32_t), sizeof(uint32_t));

		string_con_barra = (char*)malloc(long_pokemon);

		paquete_leido_completo = malloc(peso_total + sizeof(peso_total));

		memcpy(paquete_leido_completo, &peso_total, sizeof(uint32_t));
		memcpy(paquete_leido_completo + sizeof(uint32_t), &cola, sizeof(uint32_t));
		memcpy(paquete_leido_completo + 2 * sizeof(uint32_t), paquete_leido_incompleto, 3 * sizeof(uint32_t));
		offset += 3 * sizeof(uint32_t);
		memcpy(paquete_leido_completo + 5 * sizeof(uint32_t), &id_correlativo, sizeof(uint32_t));
		memcpy(paquete_leido_completo + 6 * sizeof(uint32_t), paquete_leido_incompleto + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(string_con_barra, paquete_leido_incompleto + offset, long_pokemon-1);
		memcpy(string_con_barra + long_pokemon-1, "\0", 1);
		memcpy(paquete_leido_completo + 7 * sizeof(uint32_t), string_con_barra, long_pokemon);

		free(string_con_barra);
		break;

	case APPEARED_POKEMON:
		peso_total = peso_en_cache + 1 + 2 * sizeof(uint32_t);

		memcpy(&long_pokemon, paquete_leido_incompleto + 2 * sizeof(uint32_t), sizeof(uint32_t));

		string_con_barra = (char*)malloc(long_pokemon);

		paquete_leido_completo = malloc(peso_total + sizeof(peso_total));

		memcpy(paquete_leido_completo, &peso_total, sizeof(uint32_t));
		memcpy(paquete_leido_completo + sizeof(uint32_t), &cola, sizeof(uint32_t));
		memcpy(paquete_leido_completo + 2 * sizeof(uint32_t), paquete_leido_incompleto, 2 * sizeof(uint32_t));
		offset += 2 * sizeof(uint32_t);
		memcpy(paquete_leido_completo + 4 * sizeof(uint32_t), &id_correlativo, sizeof(uint32_t));
		memcpy(paquete_leido_completo + 5 * sizeof(uint32_t), paquete_leido_incompleto + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(string_con_barra, paquete_leido_incompleto + offset, long_pokemon-1);
		memcpy(string_con_barra + long_pokemon-1, "\0", 1);
		memcpy(paquete_leido_completo + 6 * sizeof(uint32_t), string_con_barra, long_pokemon);

		free(string_con_barra);
		break;

	case CATCH_POKEMON:
		peso_total = peso_en_cache + 1 + 2 * sizeof(uint32_t);

		memcpy(&long_pokemon, paquete_leido_incompleto + 2 * sizeof(uint32_t), sizeof(uint32_t));

		string_con_barra = (char*)malloc(long_pokemon);

		paquete_leido_completo = malloc(peso_total + sizeof(peso_total));

		memcpy(paquete_leido_completo, &peso_total, sizeof(uint32_t));
		memcpy(paquete_leido_completo + sizeof(uint32_t), &cola, sizeof(uint32_t));
		memcpy(paquete_leido_completo + 2 * sizeof(uint32_t), paquete_leido_incompleto, 2 * sizeof(uint32_t));
		offset += 2 * sizeof(uint32_t);
		memcpy(paquete_leido_completo + 4 * sizeof(uint32_t), &id_correlativo, sizeof(uint32_t));
		memcpy(paquete_leido_completo + 5 * sizeof(uint32_t), paquete_leido_incompleto + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(string_con_barra, paquete_leido_incompleto + offset, long_pokemon-1);
		memcpy(string_con_barra + long_pokemon-1, "\0", 1);
		memcpy(paquete_leido_completo + 6 * sizeof(uint32_t), string_con_barra, long_pokemon);

		free(string_con_barra);
		break;

	case CAUGHT_POKEMON: //ID antes del 1/0 o despues?
		peso_total = peso_en_cache + 2 * sizeof(uint32_t);

		paquete_leido_completo = malloc(peso_total + sizeof(peso_total));

		memcpy(paquete_leido_completo, &peso_total, sizeof(uint32_t));
		memcpy(paquete_leido_completo + sizeof(uint32_t), &cola, sizeof(uint32_t));
		memcpy(paquete_leido_completo + 2 * sizeof(uint32_t), paquete_leido_incompleto, sizeof(uint32_t));
		memcpy(paquete_leido_completo + 3 * sizeof(uint32_t), &id_correlativo, sizeof(uint32_t));

		break;

	case GET_POKEMON:
		peso_total = peso_en_cache + 1 + 2 * sizeof(uint32_t);

		memcpy(&long_pokemon, paquete_leido_incompleto, sizeof(uint32_t));

		string_con_barra = (char*)malloc(long_pokemon);

		paquete_leido_completo = malloc(peso_total + sizeof(peso_total));

		memcpy(paquete_leido_completo, &peso_total, sizeof(uint32_t));
		memcpy(paquete_leido_completo + sizeof(uint32_t), &cola, sizeof(uint32_t));
		memcpy(paquete_leido_completo + 2 * sizeof(uint32_t), &id_correlativo, sizeof(uint32_t));
		memcpy(paquete_leido_completo + 3 * sizeof(uint32_t), paquete_leido_incompleto, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(string_con_barra, paquete_leido_incompleto + offset, long_pokemon-1);
		memcpy(string_con_barra + long_pokemon-1, "\0", 1);
		memcpy(paquete_leido_completo + 4 * sizeof(uint32_t), string_con_barra, long_pokemon);

		free(string_con_barra);
		break;

	case LOCALIZED_POKEMON:
		peso_total = peso_en_cache + 1 + 2 * sizeof(uint32_t);

		memcpy(&long_pokemon, paquete_leido_incompleto, sizeof(uint32_t));

		string_con_barra = (char*)malloc(long_pokemon);
		memset(string_con_barra, '\0', long_pokemon);

		paquete_leido_completo = malloc(peso_total + sizeof(peso_total));

		memcpy(paquete_leido_completo, &peso_total, sizeof(uint32_t));
		memcpy(paquete_leido_completo + sizeof(uint32_t), &cola, sizeof(uint32_t));
		memcpy(paquete_leido_completo + 2 * sizeof(uint32_t), &id_correlativo, sizeof(uint32_t));
		memcpy(paquete_leido_completo + 3 * sizeof(uint32_t), paquete_leido_incompleto, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(string_con_barra, paquete_leido_incompleto + offset, long_pokemon-1);
		offset += long_pokemon-1;
		//memcpy(string_con_barra + long_pokemon-1, "\0", 1);
		memcpy(paquete_leido_completo + 4 * sizeof(uint32_t), string_con_barra, long_pokemon);
		memcpy(&cant_coordenadas, paquete_leido_incompleto + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(paquete_leido_completo + 4 * sizeof(uint32_t) + long_pokemon, &cant_coordenadas, sizeof(uint32_t));
		memcpy(paquete_leido_completo + 5 * sizeof(uint32_t) + long_pokemon, paquete_leido_incompleto + offset, 2 * cant_coordenadas * sizeof(uint32_t));

		free(string_con_barra);
		break;

	default:
		loggear_mensaje("Se recibio un CODIGO INVALIDO", 0);
		break;
	}

	return paquete_leido_completo;
}

void* leer_de_cache(int posicion) {

	uint32_t offset = obtener_offset_en_cache(posicion);

	sem_wait(&sem_lista_particiones);
	metadata_t* md = list_get(lista_de_particiones, posicion);
	md->timestamp_LRU = get_timestamp();
	void* paquete_leido_incompleto = malloc(md->tamanio_reservado);//TODO
	memcpy(paquete_leido_incompleto, cache_broker + offset, md->tamanio_reservado);

	void* paquete_leido_completo = armar_paquete_leido(paquete_leido_incompleto, md->codigo_de_cola, md->id_correlativo, md->tamanio_reservado);

	free(paquete_leido_incompleto);
	sem_post(&sem_lista_particiones);
	return paquete_leido_completo;
}

uint32_t posicion_segun_id(uint32_t id_mensaje) {
	sem_wait(&sem_lista_particiones);
	bool es_la_metadata_del_id(metadata_t* metadata){
		return metadata->id_mensaje_contenido == id_mensaje;
	}

	metadata_t* metadata = list_find(lista_de_particiones, (void*)es_la_metadata_del_id);
	uint32_t retorno = metadata->posicion_en_lista;
	sem_post(&sem_lista_particiones);
	return retorno;
}

bool ya_conoce_el_mensaje(mensaje_enviado_t* mensaje, long id_cliente) {

	bool es_el_id_del_cliente(long cierto_id){
		return  cierto_id == id_cliente;
	}

	return list_any_satisfy(mensaje->cli_ack, (void*)es_el_id_del_cliente);
}

void forwardear_mensajes(uint32_t cola_requerida, long id_cliente) {

	sem_wait(&sem_cache);

	t_queue* cola_a_usar;
	pthread_mutex_t* sem_cola;

	t_list* lista_mensajes_a_usar;
	pthread_mutex_t* sem_lista_msj;

	switch(cola_requerida){

	case NEW_POKEMON:
		cola_a_usar = cola_new_pokemon;
		lista_mensajes_a_usar = lista_mensajes_new;
		sem_cola = &sem_cola_new;
		sem_lista_msj = &sem_mensajes_new;
		break;

	case APPEARED_POKEMON:
		cola_a_usar = cola_appeared_pokemon;
		lista_mensajes_a_usar = lista_mensajes_app;
		sem_cola = &sem_cola_app;
		sem_lista_msj = &sem_mensajes_app;
		break;

	case CATCH_POKEMON:
		cola_a_usar = cola_catch_pokemon;
		lista_mensajes_a_usar = lista_mensajes_catch;
		sem_cola = &sem_cola_catch;
		sem_lista_msj = &sem_mensajes_catch;
		break;

	case CAUGHT_POKEMON:
		cola_a_usar = cola_caught_pokemon;
		lista_mensajes_a_usar = lista_mensajes_caught;
		sem_cola = &sem_cola_caught;
		sem_lista_msj = &sem_mensajes_caught;
		break;

	case GET_POKEMON:
		cola_a_usar = cola_get_pokemon;
		lista_mensajes_a_usar = lista_mensajes_get;
		sem_cola = &sem_cola_get;
		sem_lista_msj = &sem_mensajes_get;
		break;

	case LOCALIZED_POKEMON:
		cola_a_usar = cola_localized_pokemon;
		lista_mensajes_a_usar = lista_mensajes_localized;
		sem_cola = &sem_cola_localized;
		sem_lista_msj = &sem_mensajes_localized;
		break;

	default:
		loggear_mensaje("Se recibio un CODIGO INVALIDO", 0);
		break;
	}

	sem_wait(sem_cola);
	sem_wait(sem_lista_msj);
	sem_wait(&sem_lista_de_clientes);
	int tamanio = queue_size(cola_a_usar);

	//Tengo que chequear en los "registros" lo que ya mande para que, si este tipo
	//se esta volviendo a conectar, no le genere redundancia y le pueda mandar algo que le quedo colgado

	for(int i = 0; i < tamanio; i++) {
		mensaje_t* mensaje = queue_pop(cola_a_usar);
		uint32_t id_mensaje_requerido = mensaje->id_mensaje;

		bool es_el_mensaje_del_id(mensaje_enviado_t* mensaje_env){
			return  mensaje_env->mensaje->id_mensaje == id_mensaje_requerido;
		}
		mensaje_enviado_t* msj = list_find(lista_mensajes_a_usar, (void*)es_el_mensaje_del_id);

		if(msj != NULL && !ya_conoce_el_mensaje(msj, id_cliente)) {
			//TODO: consider poner en un if lo de NULL y luego la otra condicion tomando el semaforo del mensaje para mutexear
			//Si no conoce el mensaje...

			sem_wait(&msj->sem_mensaje);
			//sem_wait(&sem_cache); ACA GENERABA DIDLO
			uint32_t posicion_particion = posicion_segun_id(mensaje->id_mensaje);
			void* paquete_a_mandar = leer_de_cache(posicion_particion);
			//sem_post(&sem_cache); IDEM
			sem_post(&msj->sem_mensaje);

			int size_paquete;
			memcpy(&size_paquete, paquete_a_mandar, 4);

			bool es_el_cliente(cliente_t* un_cli){
				return un_cli->id == id_cliente;
			}
			cliente_t* un_cliente = list_find(lista_de_clientes, (void*)es_el_cliente);

			int return_del_send;
			int socket_wachin = conectar_con_servidor(un_cliente->ip,un_cliente->puerto);
			if(socket_wachin != -1){
				return_del_send = send(socket_wachin, paquete_a_mandar, size_paquete + 4, 0);

				struct parametros_esperar_ack *parametros = (struct parametros_esperar_ack *)malloc(sizeof(struct parametros_esperar_ack));
				parametros->cliente = un_cliente;
				parametros->id_mensaje_enviado = msj->mensaje->id_mensaje;
				parametros->cola_a_usar = msj->mensaje->cod_cola;
				parametros->return_del_send = return_del_send;
				parametros->peso_esperado = size_paquete + 4;
				parametros->socket = socket_wachin;

				pthread_t* hilo;
				pthread_create(&hilo, NULL,  (void*)revisar_send_y_esperar_ack, parametros);
				pthread_detach(hilo);
			}

			free(paquete_a_mandar);
		}
		queue_push(cola_a_usar, mensaje);
	}
	sem_post(&sem_lista_de_clientes);
	sem_post(sem_cola);
	sem_post(sem_lista_msj);

	sem_post(&sem_cache);
}

void sacar_mensaje_de_cola(int id_cola, uint32_t id_mensaje) {

	t_queue* cola_a_usar;
	pthread_mutex_t* sem_cola;

	t_list* lista_a_usar;
	pthread_mutex_t* sem_lista;

	switch(id_cola){

	case NEW_POKEMON:
		cola_a_usar = cola_new_pokemon;
		sem_cola = &sem_cola_new;
		lista_a_usar = lista_mensajes_new;
		sem_lista = &sem_mensajes_new;
		break;

	case APPEARED_POKEMON:
		cola_a_usar = cola_appeared_pokemon;
		sem_cola = &sem_cola_app;
		lista_a_usar = lista_mensajes_app;
		sem_lista = &sem_mensajes_app;
		break;

	case CATCH_POKEMON:
		cola_a_usar = cola_catch_pokemon;
		sem_cola = &sem_cola_catch;
		lista_a_usar = lista_mensajes_catch;
		sem_lista = &sem_mensajes_catch;
		break;

	case CAUGHT_POKEMON:
		cola_a_usar = cola_caught_pokemon;
		sem_cola = &sem_cola_caught;
		lista_a_usar = lista_mensajes_caught;
		sem_lista = &sem_mensajes_caught;
		break;

	case GET_POKEMON:
		cola_a_usar = cola_get_pokemon;
		sem_cola = &sem_cola_get;
		lista_a_usar = lista_mensajes_get;
		sem_lista = &sem_mensajes_get;
		break;

	case LOCALIZED_POKEMON:
		cola_a_usar = cola_localized_pokemon;
		sem_cola = &sem_cola_localized;
		lista_a_usar = lista_mensajes_localized;
		sem_lista = &sem_mensajes_localized;
		break;

	default:
		loggear_mensaje("Se recibio un CODIGO INVALIDO", 0);
		break;
	}

	sem_wait(sem_cola);
	sem_wait(sem_lista);
	int tamanio = queue_size(cola_a_usar);

	for(int i = 0; i < tamanio; i++) {
		mensaje_t* mensaje_actual = queue_pop(cola_a_usar);
		if(mensaje_actual->id_mensaje != id_mensaje) {
			queue_push(cola_a_usar, mensaje_actual);
		} else {
			bool es_el_mensaje(mensaje_enviado_t* un_mensaje){
				return un_mensaje->mensaje->id_mensaje == id_mensaje;
			}
			mensaje_enviado_t* mensaje_enviado = list_remove_by_condition(lista_a_usar, (void*)es_el_mensaje);
			
			free(mensaje_actual);
			list_destroy(mensaje_enviado->cli_ack);
			//mensaje_enviado->cli_ack = NULL;
			list_destroy(mensaje_enviado->cli_enviado);
			//mensaje_enviado->cli_enviado = NULL;
			sem_destroy(&mensaje_enviado->sem_mensaje);
			free(mensaje_enviado);
		}
	}
	sem_post(sem_cola);
	sem_post(sem_lista);
}

cliente_t* obtener_cliente_por_id(long id_cliente) {

	sem_wait(&sem_lista_particiones);
	bool es_el_cliente(cliente_t* un_cli){
		return un_cli->id == id_cliente;
	}
	cliente_t* cliente_encontrado = list_find(lista_de_clientes, (void*)es_el_cliente);
	sem_post(&sem_lista_particiones);

	return cliente_encontrado;
}

void disconnect(char* cod_cola, long id) {
	cliente_t* el_cliente = obtener_cliente_por_id(id);

	desconectar_cliente(el_cliente, cod_cola, 0);
}

void* armar_paquete_cache(void* paquete_completo, int peso_total, int long_sin_endchar, int cola) {

	void* paquete_cache;
	uint32_t cantidad_de_coord = 0;
	int offset = 0;

	switch(cola){

	case NEW_POKEMON:
		paquete_cache = malloc(peso_total - 1 - 3 * sizeof(uint32_t));
		offset += 2 * sizeof(uint32_t);
		memcpy(paquete_cache, paquete_completo + offset, 3 * sizeof(uint32_t));
		offset += 4 * sizeof(uint32_t);
		memcpy(paquete_cache + 3 * sizeof(uint32_t), paquete_completo + offset, sizeof(uint32_t)+long_sin_endchar);
		break;

	case APPEARED_POKEMON:
		paquete_cache = malloc(peso_total - 1 - 3 * sizeof(uint32_t));
		offset += 2 * sizeof(uint32_t);
		memcpy(paquete_cache, paquete_completo + offset, 2 * sizeof(uint32_t));
		offset += 3 * sizeof(uint32_t);
		memcpy(paquete_cache + 2 * sizeof(uint32_t), paquete_completo + offset, sizeof(uint32_t)+long_sin_endchar);
		break;

	case CATCH_POKEMON:
		paquete_cache = malloc(peso_total - 1 - 3 * sizeof(uint32_t));
		offset += 2 * sizeof(uint32_t);
		memcpy(paquete_cache, paquete_completo + offset, 2 * sizeof(uint32_t));
		offset += 3 * sizeof(uint32_t);
		memcpy(paquete_cache + 2 * sizeof(uint32_t), paquete_completo + offset, sizeof(uint32_t)+long_sin_endchar);
		break;

	case CAUGHT_POKEMON://OK/FAIL - ID
		paquete_cache = malloc(peso_total - 2 * sizeof(uint32_t));
		offset += 2 * sizeof(uint32_t);
		memcpy(paquete_cache, paquete_completo + offset, sizeof(uint32_t));
		break;

	case GET_POKEMON:
		paquete_cache = malloc(peso_total - 1 - 3 * sizeof(uint32_t));
		offset += 3 * sizeof(uint32_t);
		memcpy(paquete_cache, paquete_completo + offset, sizeof(uint32_t)+long_sin_endchar);
		break;

	case LOCALIZED_POKEMON:
		paquete_cache = malloc(peso_total - 1 - 3 * sizeof(uint32_t));
		offset += 3 * sizeof(uint32_t);
		memcpy(paquete_cache, paquete_completo + offset, sizeof(uint32_t)+long_sin_endchar);
		offset += sizeof(uint32_t)+long_sin_endchar+1;
		memcpy(&cantidad_de_coord, paquete_completo + offset, sizeof(uint32_t));
		memcpy(paquete_cache + sizeof(uint32_t)+long_sin_endchar, paquete_completo + offset, (1 + 2*cantidad_de_coord)*sizeof(uint32_t));
		break;

	default:
		loggear_mensaje("Se recibio un CODIGO INVALIDO", 0);
		break;
	}

	return paquete_cache;
}
