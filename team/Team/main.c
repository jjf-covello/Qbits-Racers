#include "main.h"
#include "funciones_auxiliares.h"
#include "algoritmos_de_planificacion.h"
#include "servidor.h"

#include <errno.h>

int main(){
	remove("log_server.log");
	remove("log_metricas.log");
	remove("log_obligatorio.log");
	log_server = log_create("log_server.log","log_server",0,LOG_LEVEL_INFO);
	log_obligatorio = log_create("log_obligatorio.log","log_obligatorio",0,LOG_LEVEL_INFO);
	log_metricas = log_create("log_metricas.log","log_metricas",0,LOG_LEVEL_INFO);
	leer_config();
	inicializar_semaforos();
	estoy_planificando=true;

	pthread_t* hilo_server;
	pthread_create(&hilo_server,NULL,(void*)levantar_server,NULL);
	pthread_detach(hilo_server);

	inicializar_team();

	pthread_t* hilo_planificador;
	pthread_create(&hilo_planificador,NULL,(void*)planificar,NULL);
	pthread_detach(hilo_planificador);

	sem_wait(&termino_todo);

	enviar_fin_de_suscripcion(APPEARED_POKEMON);
	enviar_fin_de_suscripcion(CAUGHT_POKEMON);
	enviar_fin_de_suscripcion(LOCALIZED_POKEMON);

	loggear_metricas_finales_globales();

	liberar_globales();
	return 0;
}
void levantar_server(){levantar_server_multithread();}
void liberar_globales(){

	//LIBERO SEMAFOROS

	sem_destroy(&SEM_TRANSICION);
	sem_destroy(&SEM_LOGS);
	//No puse ni wait y ni post en los diccionarios del main (George's Idea)
	sem_destroy(&SEM_DIC_FALTANTES_ENTRENADORES);
	sem_destroy(&SEM_DIC_MAPA_POKEMON);
	sem_destroy(&SEM_DIC_OBJETIVOS_ENTRENADORES); //SOLO LLAMAMOS ESTE DIC EN EL MAIN
	sem_destroy(&SEM_DIC_POKEMON_ENTRENADORES); //SOLO LLAMAMOS ESTE DIC EN EL MAIN
	sem_destroy(&SEM_DIC_VERIFICADOR_MENSAJES); //SOLO LLAMAMOS ESTE DIC EN EL MAIN
	sem_destroy(&SEM_LISTA_ATRAPANDO); //Fijarse su inicializacion
	sem_destroy(&SEM_LISTA_ID); //Fijarse su inicializacion
	sem_destroy(&SEM_OBJETIVOS_ENTRENADORES);
	sem_destroy(&SEM_POKEMON_ENTRENADORES);
	sem_destroy(&SEM_POSICIONES_ENTRENADORES); //SOLO LLAMAMOS ESTE DIC EN EL MAIN
	sem_destroy(&SEM_LISTA_DE_ENTRENADORES);
	sem_destroy(&finalizar_proceso);
	sem_destroy(&termino_todo);

	//listas globales
	void destructor_posiciones(coordenadas_t* una_coordenada){
		if(una_coordenada!=NULL)
		free(una_coordenada);
	}
	list_destroy_and_destroy_elements(POSICIONES_ENTRENADORES,destructor_posiciones); //=[[1,2],[3,7],[5,5],[8,1]]
	void destructor_pokemons(char* pokemon){
		if(pokemon != NULL && !string_is_empty(pokemon)){
			free(pokemon);//ojo no se si se comaprten los char* de ser asi hay que destruir una sola lista
		}
	}
	void destructor_de_lista_pokemon(t_list* una_lista){
		if(una_lista != NULL && !list_is_empty(una_lista)){
			list_destroy_and_destroy_elements(una_lista,destructor_pokemons);
		}else if(una_lista != NULL){
			list_destroy(una_lista);
		}

	}
	list_destroy_and_destroy_elements(POKEMON_ENTRENADORES,destructor_de_lista_pokemon); //=[[Pikachu,Squirtle,Pidgey],[Squirtle,Charmander],[Bulbasaur],[Weedle]]
	list_destroy_and_destroy_elements( OBJETIVOS_ENTRENADORES,destructor_de_lista_pokemon); //=[[Pikachu,Pikachu,Squirtle,Pidgey],[Pikachu,Charmander,Charmander],[Squirtle,Bulbasaur],[Weedle, Weedle, Kakuna]]


	/*
	void destructor_ids(){
		//no se que mierda hay aca

	}
	list_destroy_and_destroy_elements( LISTA_ID,destructor_ids);
	 */
	list_destroy(LISTA_ID);
	void destructor_atrapandos(atrapando_t* intento){
		if(intento!=NULL){
			if(intento->pokemon!=NULL&& !string_is_empty(intento->pokemon)){
			free(intento->pokemon);
			}
		free(intento);
		}
	}
	list_destroy_and_destroy_elements( LISTA_ATRAPANDO,destructor_atrapandos);

	//colas
	queue_destroy(COLA_NEW);
	queue_destroy(COLA_READY);
	queue_destroy(COLA_BLOCK);
	queue_destroy(COLA_EXEC);
	queue_destroy(COLA_EXIT);// todos los entrenadores los libere arriba

	//chars*
	free(IP_BROKER);
	free(LOG_FILE);

	//configs
	//config_destroy(config_server);

	//logs
	log_destroy(log_server);
	//log_destroy(log_metricas); //Seguro rompe aca
	//log_destroy(log_obligatorio); ROMPE ACA TODO ESTO ROMPIA POR LA CONDICION DE CARRERA LOGS VS FIN DE PROC

	//diccionarios
	dictionary_destroy(DIC_POKEMON_ENTRENADORES);
	dictionary_destroy(DIC_OBJETIVOS_ENTRENADORES);
	dictionary_destroy(DIC_FALTANTES_ENTRENADORES);
	void destructor_de_listas(t_list* una_lista){

		void destructor_de_tuplas(tupla_t* una_tupla){
			if(una_tupla!=NULL){
				free(una_tupla);
			}
		}
		list_destroy_and_destroy_elements(una_lista,destructor_de_tuplas);

	}
	dictionary_destroy_and_destroy_elements(DIC_MAPA_POKEMON,destructor_de_listas);

	void destructor_entrenadores(entrenador_t* entrenador){
		if(entrenador!=NULL){
			liberar_entrenador(entrenador);
		}
	}
	list_destroy_and_destroy_elements( LISTA_DE_ENTRENADORES,destructor_entrenadores);
}

int leer_config(){

	config_server=config_create("config.config");
	char** lista_posiciones_strings=config_get_array_value(config_server, "POSICIONES_ENTRENADORES");
	POSICIONES_ENTRENADORES = ubicar_entrenadores(lista_posiciones_strings);

	char** lista_pokemons_entrenadores=config_get_array_value(config_server, "POKEMON_ENTRENADORES");
	POKEMON_ENTRENADORES = obtener_lista_de_pokemons(lista_pokemons_entrenadores);

	char** lista_objetivo_entrenador=config_get_array_value(config_server, "OBJETIVOS_ENTRENADORES");
	OBJETIVOS_ENTRENADORES = obtener_lista_de_pokemons(lista_objetivo_entrenador);

	/*
	printear_lista_de_lista(OBJETIVOS_ENTRENADORES);
	printear_lista_de_lista(POKEMON_ENTRENADORES);
	printear_lista_de_coordenadas(POSICIONES_ENTRENADORES);
	*/

	TIEMPO_RECONEXION = config_get_int_value(config_server,"TIEMPO_RECONEXION");
	RETARDO_CICLO_CPU = config_get_int_value(config_server,"RETARDO_CICLO_CPU");
	ALPHA_SJF = config_get_double_value(config_server,"ALPHA");

	char* planificacion = config_get_string_value(config_server,"ALGORITMO_PLANIFICACION");
	ALGORITMO_PLANIFICACION = malloc(strlen(planificacion)+1);
	strcpy(ALGORITMO_PLANIFICACION,planificacion);

	QUANTUM = config_get_int_value(config_server,"QUANTUM");

	char* aux=config_get_string_value(config_server,"IP_BROKER");
	IP_BROKER=malloc(strlen(aux)+1);
	strcpy(IP_BROKER,aux);

	ESTIMACION_INICIAL = config_get_int_value(config_server,"ESTIMACION_INICIAL");
	PUERTO_BROKER = config_get_int_value(config_server, "PUERTO_BROKER");


	char* aux_team=config_get_string_value(config_server,"IP_TEAM");
	IP_TEAM=malloc(strlen(aux_team)+1);
	strcpy(IP_TEAM,aux_team);

	ESTIMACION_INICIAL = config_get_int_value(config_server,"ESTIMACION_INICIAL");
	PUERTO_TEAM = config_get_int_value(config_server, "PUERTO_TEAM");

	char* auxiliar_log=config_get_string_value(config_server, "LOG_FILE");
	LOG_FILE= malloc(strlen(auxiliar_log)+1);
	strcpy(LOG_FILE,aux);
	
	aux_team=config_get_string_value(config_server,"IP_SUSCRIPCION");
	IP_SUSCRIPCION=malloc(strlen(aux_team)+1);
	strcpy(IP_SUSCRIPCION,aux_team);
	PUERTO_SUSCRIPCION=config_get_int_value(config_server, "PUERTO_SUSCRIPCION");

	liberar_char_doble_puntero(lista_posiciones_strings);
	liberar_char_doble_puntero(lista_pokemons_entrenadores);
	liberar_char_doble_puntero(lista_objetivo_entrenador);
	config_destroy(config_server);

	if(POSICIONES_ENTRENADORES->elements_count == POKEMON_ENTRENADORES->elements_count &&
			POKEMON_ENTRENADORES->elements_count == OBJETIVOS_ENTRENADORES->elements_count){
		return 0;
	}
	//exit(1);
}

void printear_lista_de_coordenadas(t_list* lista_de_coordenadas){
	void printer(coordenadas_t* una_coordenadas){
		printf("(%d , %d )\n",una_coordenadas->pos_x,una_coordenadas->pos_y);
	}
	list_iterate(lista_de_coordenadas,printer);
}

void printear_lista_de_lista(t_list* lista_de_listas){

	void iterador(t_list* lista){
		void printear_pokemon(char* pokemon){

			printf("%s",pokemon);
		}
		list_iterate(lista, printear_pokemon);
		printf("\n");
	}

	list_iterate(lista_de_listas,iterador);
}

void setear_dic_pokemon(){
	DIC_POKEMON_ENTRENADORES = dictionary_create();

	void lista_a_dic(t_list* lista){

		void lista_a_strings(char* pokemon){
			int cantidad = 1;
			if(dictionary_has_key(DIC_POKEMON_ENTRENADORES,pokemon)){
				cantidad += dictionary_get(DIC_POKEMON_ENTRENADORES,pokemon);
			}
			dictionary_put(DIC_POKEMON_ENTRENADORES,pokemon,cantidad);
		}
		list_iterate(lista,lista_a_strings);
	}
	list_iterate(POKEMON_ENTRENADORES,lista_a_dic);
}

void setear_dic_objetivos(){
	DIC_OBJETIVOS_ENTRENADORES = dictionary_create();

	void lista_a_dic(t_list* lista){
		if(list_is_empty(lista)){

		}else{
			void lista_a_strings(char* pokemon){
				int cantidad = 1;
				if(dictionary_has_key(DIC_OBJETIVOS_ENTRENADORES,pokemon)){
					cantidad += dictionary_get(DIC_OBJETIVOS_ENTRENADORES,pokemon);
				}
				dictionary_put(DIC_OBJETIVOS_ENTRENADORES,pokemon,cantidad);
			}
			list_iterate(lista,lista_a_strings);
		}
	}
	list_iterate(OBJETIVOS_ENTRENADORES,lista_a_dic);
}

void setear_dic_faltantes(){
	DIC_FALTANTES_ENTRENADORES = dictionary_create();
	DIC_MAPA_POKEMON = dictionary_create();

	void pokemon_faltantes(char* pokemon, int objetivo){
		t_list* lista_posiciones = list_create();
		int tengo = dictionary_get(DIC_POKEMON_ENTRENADORES,pokemon);
		if(objetivo - tengo < 0)
			exit(-57);
		if(objetivo-tengo>0){
			dictionary_put(DIC_FALTANTES_ENTRENADORES,pokemon,objetivo-tengo);
		}
		dictionary_put(DIC_MAPA_POKEMON,pokemon,lista_posiciones); //un asco pero funca
	}
	dictionary_iterator(DIC_OBJETIVOS_ENTRENADORES,pokemon_faltantes);
	limpiar_dic_faltantes();
}

void limpiar_dic_faltantes(){
	void sacar_cumplidos(char* pokemon, int cantidad){
		if(cantidad == 0){
			t_list* lista_faltantes = dictionary_remove(DIC_FALTANTES_ENTRENADORES,pokemon);
			void destructor(char* un_pokemon){
				if(un_pokemon!=NULL&&!string_is_empty(un_pokemon))
				free(un_pokemon);
			}
			list_destroy_and_destroy_elements(lista_faltantes,destructor);
		}
		dictionary_iterator(DIC_FALTANTES_ENTRENADORES,sacar_cumplidos);
	}
}

void inicializar_suscripciones(){

	pthread_t* hilo_suscripcion_appeared;
	pthread_create(&hilo_suscripcion_appeared,NULL,suscribirse,APPEARED_POKEMON);
	pthread_detach(hilo_suscripcion_appeared);

	pthread_t* hilo_suscripcion_localized;
	pthread_create(&hilo_suscripcion_localized,NULL,suscribirse,LOCALIZED_POKEMON);
	pthread_detach(hilo_suscripcion_localized);

	pthread_t* hilo_suscripcion_caught;
	pthread_create(&hilo_suscripcion_caught,NULL,suscribirse,CAUGHT_POKEMON);
	pthread_detach(hilo_suscripcion_caught);

}

void suscribirse(int cola){
	int socket_escucha=0;
	//while(estoy_planificando){
		socket_escucha=conectar_con_servidor(IP_BROKER,PUERTO_BROKER);
		while(socket_escucha<3 && estoy_planificando){
			sem_wait(&SEM_LOGS);
			log_info(log_obligatorio,"No pudimos conectarnos con Broker (socket= %d)",socket_escucha);
			sem_post(&SEM_LOGS);
			sleep(TIEMPO_RECONEXION);

			sem_wait(&SEM_LOGS);
			log_info(log_obligatorio,"Vamos a volver a conectar a la cola %d, despues de %d (s) ",cola,TIEMPO_RECONEXION);
			sem_post(&SEM_LOGS);
			socket_escucha=conectar_con_servidor(IP_BROKER,PUERTO_BROKER);
		}

		//esto deberia volar con la nueva version
		switch(cola){

		case APPEARED_POKEMON:
			suscripcion[0]=socket_escucha;
			break;
		case CAUGHT_POKEMON:
			suscripcion[1]=socket_escucha;
			break;
		case LOCALIZED_POKEMON:
			suscripcion[2]=socket_escucha;
			break;

		}
		sem_wait(&SEM_LOGS);
		log_info(log_obligatorio,"Pudimos conectarnos con Broker (socket= %d)!",socket_escucha);
		sem_post(&SEM_LOGS);

		enviar_suscripcion(socket_escucha,cola);
		int retorno=0;
		/*while(estoy_planificando && retorno!=57){
			retorno=atender_suscripcion(socket_escucha);
			if(retorno!=57){
				enviar_ack(socket_escucha);
			}
			//5138008
		}*/
		close(socket_escucha);

	//}
}

int atender_suscripcion(int socket_escucha){

	void* buffer;
	int alocador;
	sem_post(&SEM_LOGS);
	log_info(log_obligatorio, "Recibimos un pedido de suscripcion \n");
	sem_post(&SEM_LOGS);

	buffer=recibir_paquete(&alocador,socket_escucha);

	if(alocador!=0 && buffer != NULL){
		int codigo_operacion = conseguir_codigo_operacion(buffer);
		log_info(log_obligatorio, "el pedido era del codigo %d \n",codigo_operacion);
		ejecutar_y_desarmar(codigo_operacion,buffer+sizeof(int),socket_escucha);
		free(buffer);
	}else{
		return 57;
	}

	sem_wait(&SEM_LOGS);
	log_info(log_obligatorio, "Termine el pedido\n");
	sem_wait(&SEM_LOGS);
	return 0;
}

void enviar_ack(int socket_escucha){
	int ack=5138008;
	void* paquete= malloc(sizeof(int));
	memcpy(paquete,&ack,sizeof(int));
	send(socket_escucha,paquete,sizeof(int),0);
	free(paquete);
}


void enviar_fin_de_suscripcion(int cola){

	int socket_escucha=conectar_con_servidor(IP_BROKER,PUERTO_BROKER);
	//Aca habia un while cualquier queja hablen con Jorgito, yo no estaba taaan convencido
	//Posible Solucion: utilizar el vector de sockets y pregunto si todos son mayores de 3
	if(socket_escucha<3){
		sem_wait(&SEM_LOGS);
		log_info(log_obligatorio,"No pudimos conectarnos con Broker (socket= %d)",socket_escucha);
		sem_post(&SEM_LOGS);
		/*sleep(TIEMPO_RECONEXION);

		sem_wait(&SEM_LOGS);
		log_info(log_obligatorio,"Vamos a volver a conectar a la cola %d, despues de %d (s) ",cola,TIEMPO_RECONEXION);
		sem_post(&SEM_LOGS);*/

		//socket_escucha=conectar_con_servidor(IP_BROKER,PUERTO_BROKER);
	}else{
		int peso=sizeof(int)+sizeof(long);
		int offset=0;
		long id = calcular_id();

		void* paquete= malloc(peso+sizeof(int));
		memcpy(paquete,&peso,sizeof(int));
		offset+=sizeof(int);
		memcpy(paquete+offset,&cola,sizeof(int));
		offset+=sizeof(int);
		memcpy(paquete+offset,&id,sizeof(long));

		void* para_envio=crear_paquete(TERMINAR_SUSCRIPCION,paquete);
		enviar_paquete(socket_escucha,para_envio);
		sem_wait(&SEM_LOGS);
		log_info(log_obligatorio,"Ya no estoy suscripto a la cola %d",cola);
		sem_post(&SEM_LOGS);
		free(para_envio);
		free(paquete);
	}
}

void enviar_suscripcion(int socket_escucha, int cola){


	int peso_ip= strlen(IP_SUSCRIPCION)+1;
	int peso=sizeof(int)+sizeof(long)+peso_ip+sizeof(int)*2;
	int offset=0;
	long id = calcular_id();

	void* paquete= malloc(peso+sizeof(int));
	memcpy(paquete,&peso,sizeof(int));
	offset+=sizeof(int);
	memcpy(paquete+offset,&cola,sizeof(int));
	offset+=sizeof(int);
	memcpy(paquete+offset,&id,sizeof(long));
	offset+=sizeof(long);
	memcpy(paquete+offset,&peso_ip,sizeof(int));
	offset+=sizeof(int);
	memcpy(paquete+offset,IP_SUSCRIPCION,peso_ip);
	offset+=peso_ip;
	memcpy(paquete+offset,&PUERTO_SUSCRIPCION,sizeof(int));




	void* para_envio=crear_paquete(SUSCRIPTOR,paquete);
	enviar_paquete(socket_escucha,para_envio);
	sem_wait(&SEM_LOGS);
	log_info(log_obligatorio,"Me suscribi a la cola %d",cola);
	sem_post(&SEM_LOGS);
	free(para_envio);
	free(paquete);


}
long calcular_id(){

	t_config* un_config = config_create("config.config");

	long retorno = 0;
	char* IP_TEAM = config_get_string_value(un_config,"IP_TEAM");
	char** aux = string_split(IP_TEAM,".");
	//127.0.0.1
	//127001
	long PUERTO_TEAM = config_get_long_value(un_config, "PUERTO_TEAM");

	for(int i=3;i>0;i--){
		retorno+=(atoi(aux[i])*pow(10,3-i));
	}

	retorno += PUERTO_TEAM;

	liberar_char_doble_puntero(aux);

	config_destroy(un_config);

	return retorno;
}

void inicializar_team(){
	//UMA DELÍCIA 5 LINEAS DE CODIGO PARA HACER DE NADA PEPELOIDES
	cantidad_de_ciclos_cpu_total=0;
	inicializar_colas();
	setear_dic_pokemon();
	setear_dic_objetivos();
	setear_dic_faltantes();
	inicializar_entrenadores();
	inicializar_suscripciones();
	//ESTE GET ESTA MEDIO CAGADO SI NO ESTAMOS CONECTADOS CON BROKER TODO
	GET_POKEMON_TEAM();
}

void inicializar_colas(){
	COLA_NEW=queue_create();
	COLA_READY=queue_create();
	COLA_BLOCK=queue_create();
	COLA_EXEC=queue_create();
	COLA_EXIT=queue_create();
	LISTA_ATRAPANDO= list_create();
	LISTA_ID= list_create();
}

void inicializar_entrenadores(){
	int longitud_lista=list_size(POSICIONES_ENTRENADORES);

	LISTA_DE_ENTRENADORES = list_create();

	for(int i=0; i<longitud_lista;i++){
		coordenadas_t* una_coordenada=list_get(POSICIONES_ENTRENADORES,i);
		entrenador_t* un_entrenador=(entrenador_t*)malloc(sizeof(entrenador_t));

		un_entrenador->posicion_actual=(coordenadas_t*)malloc(sizeof(coordenadas_t));
		//un_entrenador->posicion_actual=una_coordenada;//ojo, si borro la coordenada de la lista, falla
		un_entrenador->posicion_actual->pos_x=una_coordenada->pos_x;
		un_entrenador->posicion_actual->pos_y=una_coordenada->pos_y;

		un_entrenador->posicion_deseada=(coordenadas_t*)malloc(sizeof(coordenadas_t));
		//HACEMOS QUE DE DEFAULT QUIERA ESTAR EN DONDE EMPIEZA(QUE NO SE MUEVA)
		//SI ENTRA A EJECUTAR SI UN APEARRED O LOCALIZED VA A ENTRAR EN BLOCK DE UNA
		un_entrenador->posicion_deseada->pos_x=una_coordenada->pos_x;
		un_entrenador->posicion_deseada->pos_y=una_coordenada->pos_y;

		un_entrenador->estimacion_actual = ESTIMACION_INICIAL;
		un_entrenador->estimacion_anterior = ESTIMACION_INICIAL;

		sem_init(&un_entrenador->semaforo_entrenador,0,0);
		sem_init(&un_entrenador->semaforo_entre_en_ready,0,0);
		sem_init(&un_entrenador->sem_operaciones_entrenador,0,1);
		sem_init(&un_entrenador->semaforo_puedo_hacer_otra_rafaga,0,1);
		sem_init(&un_entrenador->termine_operacion,0,0);
		sem_init(&un_entrenador->cambio_de_estado,0,1);

		un_entrenador->estoy_ocioso=true;
		un_entrenador->estoy_esperando_un_intercambio=false;
		un_entrenador->estoy_esperando_un_caught=false;
		un_entrenador->id_entrenador=i;
		//sem_wait(&SEM_COLA_NEW);
		sem_wait(&SEM_LOGS);
		log_info(log_obligatorio,"El entrenador %d se creo y se envio a la cola NEW",i);
		sem_post(&SEM_LOGS);
		queue_push(COLA_NEW,un_entrenador);
		//sem_post(&SEM_COLA_NEW);
		un_entrenador->estado_actual=NEW;

		t_list* lista_pokemones_actuales = list_get(POKEMON_ENTRENADORES,i);

		bool no_cumple_mis_objetivos(char* el_pokemon){

			t_list* objetivos = list_get(OBJETIVOS_ENTRENADORES,un_entrenador->id_entrenador);

			bool esta_en_la_lista(char* un_pokemon){
				return !strcmp(el_pokemon,un_pokemon);
			}
			return !list_any_satisfy(objetivos,esta_en_la_lista);
		}

		un_entrenador->pokemones_que_tengo_para_intercambiar = list_filter(lista_pokemones_actuales,no_cumple_mis_objetivos);

		un_entrenador->pokemones_que_me_faltan = pokemones_que_me_faltan(un_entrenador); //Esta testeadisimo

		un_entrenador->ciclos_que_ejecute=0;
		sem_wait(&SEM_LISTA_DE_ENTRENADORES);
		list_add_in_index(LISTA_DE_ENTRENADORES,un_entrenador->id_entrenador,un_entrenador);
		sem_post(&SEM_LISTA_DE_ENTRENADORES);

		pthread_t* hilo_entrenador;
		pthread_create(&hilo_entrenador,NULL,(void*)comportamiento_entrenador,un_entrenador);
		pthread_detach(hilo_entrenador);
	}
}

void inicializar_semaforos(){

	sem_init(&sem_entrenadores_ready,0,0);
	sem_init(&SEM_TRANSICION,0,1);
	sem_init(&SEM_LOGS,0,1);
	//No puse ni wait y ni post en los diccionarios del main (George's Idea)
	sem_init(&SEM_DIC_FALTANTES_ENTRENADORES,0,1);
	sem_init(&SEM_DIC_MAPA_POKEMON,0,1);
	sem_init(&SEM_DIC_OBJETIVOS_ENTRENADORES,0,1); //SOLO LLAMAMOS ESTE DIC EN EL MAIN
	sem_init(&SEM_DIC_POKEMON_ENTRENADORES,0,1); //SOLO LLAMAMOS ESTE DIC EN EL MAIN
	sem_init(&SEM_DIC_VERIFICADOR_MENSAJES,0,1); //SOLO LLAMAMOS ESTE DIC EN EL MAIN
	sem_init(&SEM_LISTA_ATRAPANDO,0,1); //Fijarse su inicializacion
	sem_init(&SEM_LISTA_ID,0,1); //Fijarse su inicializacion
	sem_init(&SEM_OBJETIVOS_ENTRENADORES,0,1);
	sem_init(&SEM_POKEMON_ENTRENADORES,0,1);
	sem_init(&SEM_POSICIONES_ENTRENADORES,0,1); //SOLO LLAMAMOS ESTE DIC EN EL MAIN
	sem_init(&SEM_LISTA_DE_ENTRENADORES,0,1);
	sem_init(&SEM_LOG_METRICAS,0,1);
	sem_init(&termino_todo,0,0);
	//sem_init(&semaforo_mandar_get,0,0);
}

void loggear_metricas_finales_por(entrenador_t* entrenador){

	log_info(log_metricas,"Concluimos del entrenador %d utilizando un total de %d ciclos de ejecucion",entrenador->id_entrenador,entrenador->ciclos_que_ejecute);

	int porcentaje = (entrenador->ciclos_que_ejecute)*100/cantidad_de_ciclos_cpu_total;
	log_info(log_metricas,"El uso de cpu total del entrenador entrenador %d es %d %%",entrenador->id_entrenador,porcentaje);
}

void loggear_metricas_finales_globales(){

	calcular_ciclos_globales();

	list_iterate(LISTA_DE_ENTRENADORES,loggear_metricas_finales_por);

	log_info(log_metricas,"Concluimos la ejecucion utilizando un total de %d ciclos de ejecucion, resolviendo un total de %d deadlock(s) y cambiando de contexto %d veces.",cantidad_de_ciclos_cpu_total,cantidad_de_deadlocks_resueltos,cantidad_de_cambios_de_contexto);

}
