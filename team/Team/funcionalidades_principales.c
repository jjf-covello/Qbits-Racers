
#include "funcionalidades_principales.h"

void CATCH_POKEMON_TEAM(char* pokemon,uint32_t pos_x,uint32_t pos_y){
	entrenador_t* entrenador = buscar_entrenador_por_coordenada(pos_x,pos_y);
	int id_entrenador = entrenador->id_entrenador;

	// Ya que me comunico con el broker para realizar este catch,
	// voy a gastar un ciclo de CPU

	//sem_post(&entrenador->semaforo_puedo_hacer_otra_rafaga);
	sem_wait(&entrenador->semaforo_entrenador);
	entrenador->ciclos_que_ejecute++;

	sem_wait(&SEM_LOG_METRICAS);
	log_info(log_metricas,"El entrenador %d acaba de ejecutar %d ciclos de ejecucion para el CATCH_POKEMON",entrenador->id_entrenador,1);
	sem_post(&SEM_LOG_METRICAS);

	uint32_t id = serializar_catch_pokemon(pokemon, pos_x, pos_y);
	sem_wait(&SEM_LOGS);
	log_info(log_obligatorio,"El entrenador %d quiere atrapar a un %s (id de mensaje= %d)",entrenador->id_entrenador,pokemon,id);
	sem_post(&SEM_LOGS);

	atrapando_t* intento = (atrapando_t*)malloc(sizeof(atrapando_t));
	intento->pokemon = pokemon;
	intento->id = id;
	intento->entrenador = id_entrenador;

	sem_wait(&SEM_LISTA_ATRAPANDO);
	list_add(LISTA_ATRAPANDO,intento);
	sem_post(&SEM_LISTA_ATRAPANDO);

	sem_wait(&SEM_LISTA_ID);
	list_add(LISTA_ID,id);
	sem_post(&SEM_LISTA_ID);

	entrenador->estoy_ocioso=false;
	sem_wait(&SEM_LOGS);
	log_info(log_obligatorio,"El entrenador %d pasa a la cola BLOCK porque esta esperando un CAUGHT(id= %d; pokemon= %s)",entrenador->id_entrenador,intento->id,intento->pokemon);
	sem_post(&SEM_LOGS);

	entrenador->estoy_esperando_un_caught = true;
	pasar_de_cola_a(entrenador,BLOCK);

	if(id<=0){
		comportamiento_default(CATCH_POKEMON,id);
	}
}

void GET_POKEMON_TEAM(){
	void mandar_get(char* pokemon, int cantidad){
		if(cantidad > 0){
			uint32_t id = serializar_get_pokemon(pokemon);
			if(!id){
				comportamiento_default(GET_POKEMON,id);
			}
		}
	}
	sem_wait(&SEM_DIC_FALTANTES_ENTRENADORES);
	dictionary_iterator(DIC_FALTANTES_ENTRENADORES,mandar_get);
	sem_post(&SEM_DIC_FALTANTES_ENTRENADORES);
}

void APPEARED_POKEMON_TEAM(char* nombre_pokemon,uint32_t pos_x,uint32_t pos_y,uint32_t id){

	char* nombre_pokemon_duplicado= malloc(strlen(nombre_pokemon)+1);
	strcpy(nombre_pokemon_duplicado,nombre_pokemon);

	// Descartamos las especies que no necesitamos
	// Verificar lo del ID TODO -> Issue #27
	sem_wait(&SEM_DIC_FALTANTES_ENTRENADORES);
	bool necesito_ese_pokemon = dictionary_has_key(DIC_FALTANTES_ENTRENADORES,nombre_pokemon_duplicado); //EVITO CONDICION DE CARRERA TODO
	

	if(necesito_ese_pokemon){

		decrementar_pokemones_faltantes(nombre_pokemon_duplicado);

		entrenador_t* un_entrenador=buscar_entrenador_mas_cercano(pos_x,pos_y);
		int flag=NADIE_LO_BUSCA;
		if(un_entrenador!=NULL){
			un_entrenador->posicion_deseada->pos_x=pos_x;
			un_entrenador->posicion_deseada->pos_y=pos_y;
			flag=LO_ESTAN_BUSCANDO;
			un_entrenador->id_pokemon=id;
			sem_wait(&SEM_LOGS);
			log_info(log_obligatorio, "El entrenador %d pasa a la cola READY para buscar un %s",un_entrenador->id_entrenador,nombre_pokemon);
			sem_post(&SEM_LOGS);
		}

		sem_wait(&SEM_DIC_MAPA_POKEMON);
		t_list* lista_posicion = dictionary_get(DIC_MAPA_POKEMON,nombre_pokemon);

		tupla_t* una_tupla = (tupla_t*)malloc(sizeof(tupla_t));
		una_tupla->pos_x = pos_x;
		una_tupla->pos_y = pos_y;
		una_tupla->flag = flag; //Por cada APPEARED_POKEMON nos pasan solo un pokemon
		list_add(lista_posicion,una_tupla);

		dictionary_put(DIC_MAPA_POKEMON,nombre_pokemon,lista_posicion);
		sem_post(&SEM_DIC_MAPA_POKEMON);

		if(un_entrenador!=NULL){
			pasar_de_cola_a(un_entrenador,READY);
			sem_post(&un_entrenador->semaforo_entre_en_ready);
		}
	}
	sem_post(&SEM_DIC_FALTANTES_ENTRENADORES);
	free(nombre_pokemon_duplicado);
	// TODO este free esta raro. Considerar purgarlo, dic key? es o no es? esa es la cuestión
}

void CAUGHT_POKEMON_TEAM(uint32_t id_mensaje,uint32_t resultado){

	bool encontrar_entrenador(atrapando_t* un_intento){
		return un_intento->id == id_mensaje;
	}
	sem_wait(&SEM_LISTA_ATRAPANDO);
	atrapando_t* intento = list_remove_by_condition(LISTA_ATRAPANDO,encontrar_entrenador);
	sem_post(&SEM_LISTA_ATRAPANDO);

	int id_entrenador = intento->entrenador;
	char* pokemon = intento->pokemon;

	bool buscar_por_id(entrenador_t* otro_entrenador){
		return otro_entrenador->id_entrenador == intento->entrenador;
	}

	sem_wait(&SEM_TRANSICION);
	entrenador_t* entrenador_que_realizo_el_catch = list_find(COLA_BLOCK->elements, buscar_por_id);
	sem_post(&SEM_TRANSICION);

	if(resultado == 1011 || resultado == 1){ //Pudimos atrapar al pokemon!

		sem_wait(&SEM_POKEMON_ENTRENADORES);
		t_list* lista_de_pokemon_actuales = list_get(POKEMON_ENTRENADORES,id_entrenador);
		list_add(lista_de_pokemon_actuales,pokemon);
		sem_post(&SEM_POKEMON_ENTRENADORES);

		agregar_pokemon_que_no_me_sirve(entrenador_que_realizo_el_catch,pokemon);

		actualizar_mapa(pokemon,entrenador_que_realizo_el_catch);
	}else{
		incrementar_pokemones_faltantes(pokemon);
	}

	entrenador_que_realizo_el_catch->estoy_esperando_un_caught = false;
	mover_de_cola(entrenador_que_realizo_el_catch);

	free(intento);
}
