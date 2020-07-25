
#include "funciones_auxiliares.h"

void comportamiento_entrenador(entrenador_t* entrenador){

	//sem_wait(&entrenador->sem_operaciones_entrenador);
	sem_wait(&entrenador->cambio_de_estado);
	bool el_entrenador_esta_en_exit = entrenador->estado_actual == EXIT;
	sem_post(&entrenador->cambio_de_estado);

	//sem_post(&entrenador->sem_operaciones_entrenador);


	while(!el_entrenador_esta_en_exit){

		sem_wait(&entrenador->semaforo_entre_en_ready);
		entrenador->ciclos_que_ejecute+=desplazar_en_mapa_al(entrenador);
		char* pokemon_a_atrapar = obtener_pokemon(entrenador->posicion_actual);
		if(pokemon_a_atrapar!=NULL &&  !tiene_capacidad_maxima_pokemon(entrenador)){
			CATCH_POKEMON_TEAM(pokemon_a_atrapar,entrenador->posicion_actual->pos_x,entrenador->posicion_actual->pos_y);
		}else{
			//Si estoy BLOCK alguien se va a mover hasta mi posicion para
			// intercambiar pokemones
			if(entrenador->estado_actual!=BLOCK && entrenador->estado_actual!=EXIT)
				realizar_intercambio(entrenador);
		}
		//Este semaforo lo posteamos luego de hacer un CAUGHT o un intercambio
		sem_wait(&entrenador->termine_operacion);
		verificar_si_seguimos_planificando();

		sem_wait(&entrenador->cambio_de_estado);
		el_entrenador_esta_en_exit = entrenador->estado_actual == EXIT;
		sem_post(&entrenador->cambio_de_estado);
	}

}

void verificar_si_seguimos_planificando(){
	bool flag= list_size(POSICIONES_ENTRENADORES)==list_size(COLA_EXIT->elements);
	if(flag){
		sem_wait(&SEM_LOGS);
		log_info(log_obligatorio,"Todos los entrenadores finalizaron con sus objetivos :D");
		sem_post(&SEM_LOGS);
		printf("\033[0;36m@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\033[0m  \\  \033[0;36m@@@@@@@@@@@@@@@@@@@@@\033[0m  , \033[0;36m@@@@@@@@@@#/  /@@@@@@@@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#\033[0m @@& \033[0;36m/@@*        @@@@@@@&\033[0m $#  \033[0;36m@@@\033[0m    &@@  \033[0;36m,$@@@@@@@@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\033[0m @@@@  *@@@@( \033[0;36m#@@@@@@@@@\033[0m  @@@*   @@@@@&  \033[0;36m@@@@@@@@@@@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@@@@@@@@@@@@@@&\033[0m       @@@@ @@@@@  \033[0;36m@@@@@@@@@@\033[0m  @@@@@#@@@@@@  \033[0;36m@@@@@@@@@@@@@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@$\033[0m  &@@@@@@@@* \033[0;31m((((((((((\033[0m  @@@@@@@@@@@@@@&*   \033[0;36m/&@@@@@@@@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\033[0m  #@@@@@@( \033[0;31m/(((((((\033[0m  @@@@@@@@@@@@   \033[0;36m$@@@@@@@@@@@@@@@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\033[0m  @@@@@@(  \033[0;31m(((\033[0m   @@@@@@@@@@@@  \033[0;36m@@@@@@@@@@@@@@@@@@@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@/            \\@@@@@@@@@@@&\033[0m  @@@@@@@@@@@@(*&@@@@@@@@@@@@@@   \033[0;36m,&@@@@@@@@@@@@@@@@@@@@@@@@\033[0m\n\033[0;36m@@@@@/                    \\@@@@@@@@&$\033[0m   @@  @@@*@@@@@@@ @@@   @@@@@@@@@   \033[0;36m*@@@@@@@@@@@@@@@@@@@@\033[0m\n\033[0;36m@@@/       \033[1;35mTODOS  LOS\033[0;36m       \\@@@@@@@@\033[0m ,@@* @@@@ /@@@@  $@@@@@  &@@$   \033[0;36m$@@@@@@@@@@@@@@@@@@@@@@@@\033[0m\n\033[0;36m@@        \033[1;35mENTRENADORES\033[0;36m       @@@@@@@/\033[0m @@@  @@@   @@@*   $@@@@  (@@@@&(    \033[0;36m&@@@@@@@@@@@@@@@@@@@@\033[0m\n\033[0;36m@@      \033[1;35mCOMPLETARON  SUS\033[0;36m      *@@@@@\033[0m  @@@&  @@/  \033[0;33m*(/\033[0m    @@@@   @@@@@@ \033[0;36m(@@@@@@@@ @@@@@@@@@@@@@@@\033[0m\n\033[0;36m@@@\\       \033[1;35mOBJETIVOS\033[0;36m           \\@@@@&\033[0m $@@@@  \033[0;33m(########(\033[0m(@*   (@@@@@@* \033[0;36m@@@@@@@@(  /@@@@/    *@@@\033[0m\n\033[0;36m@@@@@\\                    /@@@@@@@@@@/\033[0m @@@@ \033[0;33m*///*. ..(#\033[0m &@&@@@@@@@$   \033[0;36m,@@@@@@@\033[0m .@   *@@#  \033[0;36m@@@@@\033[0m\n\033[0;36m@@@@@@@@@\\            /@@@@@@@@@@@@@#\033[0m    @@@@@(  \033[0;33m**\033[0m  &@@@@@@@@@@@@@@$    \033[0;36m$@@@\033[0m  @@@@@@@$ \033[0;36m@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@   \033[0;31m*((/\033[0m  /@@@@@@@@@@@@@@@@@@@@@@@@(   \033[0;31m,*/(*\033[0m   @@@@&/$@@@@@*   \033[0;36m@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@@@@@@@@@@@@@/  \033[0;31m*(((((((,\033[0m   @@@@@@@@@@@@@@@@@@@@@(    \033[0;31m/(((((\033[0m  @@@@@@@&   \033[0;36m#@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@@@@@@@@@@   \033[0;31m((((((((((/\033[0m  @@**@@@@@@@@@@@@@@@@@@@@@@ \033[0;31m,((*(*\033[0m  @@@@,  \033[0;36m#@@@@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@@@@@@@#  \033[0;31m(((((((((((((((((\033[0m *@@@/ @@@@@@@@@@@@@@&@@@  \033[0;31m(((/\033[0m @@$  \033[0;31m,/\033[0;36m ,@@@@@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@@@@@@  \033[0;31m(((((((((((((/  /(( \033[0m*@@ \033[0;31m,(\033[0m @@@@@@@@@@@@@*$@@ \033[0;31m.((((*  *(((((\033[0;36m  @@@@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@@@@@(  \033[0;31m.**\033[0m        #@  \033[0;31m(((*   ,(((/\033[0m @@@(@@@@@@@& \033[0;31m(.  /(((((  (((((((*\033[0;36m &@@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@@@@@@@&\033[0m   &@@@@@@@@& \033[0;31m*((/.((((((((\033[0m  @# @@@@@@@ \033[0;31m.((//(((((((  ,((((((*\033[0;36m $@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@@@@@\033[0m  &@@@@@@@@@@@@/ \033[0;31m/((((((((((((*   (/  \033[0m@@@\033[0;31m  ((((((((((((. \033[0;36m@\033[0;31m   ((((\033[0;36m  @@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@@$\033[0m ,@@@@@@@@@@@@@@@  \033[0;31m((((((((((((((*(((((/   *(((((((((((((*\033[0;36m @@@@@\\  *@@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@\033[0m  @@@@@@@@@@@@@@@@/ \033[0;31m(((((((((((((((((*\033[0m @@@@ \033[0;31m(((((((((((((((/\033[0;36m #@@@@@@@@@@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@#\033[0m &@@@@@@@@@@@@@@@@ \033[0;31m,(((((((((((((((((,\033[0m @@@@ \033[0;31m(((((((((((((((/\033[0;36m (@@@@@@@@@@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@\033[0m  &@@@@@@@@@@@@@@@@ \033[0;31m,(((((((((((((((((((/,,/((((((((((((((((*\033[0;36m $@@@@@@@@@@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@\033[0m  @@@@@@@@@@@@@@@&  \033[0;31m(((((((((((((((((((((((((((((((((((((((\033[0;36m  #&@@@@@@@@@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@@\033[0m  (@@@@@@@@@@@&,    \033[0;31m/((((((((((((.              ,((((((/  \033[0;33m/####(\033[0;36m   @@@@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@@@@@\033[0m     **    \033[0;33m/#####,   \033[0;31m***    \033[0;36m,&@@@@@@@@@@@@@@@@@  \033[0;33m*//(#########(, \033[0;36m*@@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@@@@@@@@@@@@  \033[0;33m###(((#######\033[0;36m   @@@@@@@@@@@@@@@@@@@@@@@@   \033[0;33m(######\033[0;36m  @@@@@@@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \033[0;33m*####/\033[0;36m ,@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  \033[0;33m###\033[0;36m    @@@@@@@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@   \033[0;33m#/\033[0;36m  ,@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\033[0m\n\033[0;36m@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\033[1;35m	 Q-BITS RACERS  2020\033[0m\n");
		estoy_planificando=!flag;
		sem_post(&termino_todo);
	}
}

bool obtener_flag_de_planificacion(){
	return estoy_planificando;
}

char* obtener_pokemon(coordenadas_t* coordenada){
	char* pokemon_a_retornar = NULL;

	void obtener_coordenada(char* pokemon, t_list* lista_coordenadas){

		bool mismas_coordenadas(tupla_t* una_tupla){
			return una_tupla->pos_x == coordenada->pos_x &&
					una_tupla->pos_y == coordenada->pos_y;
		}
		bool existe_coordenada = list_any_satisfy(lista_coordenadas,mismas_coordenadas);

		if(existe_coordenada){
			pokemon_a_retornar = malloc(strlen(pokemon)+1);
			strcpy(pokemon_a_retornar,pokemon);
		}
	}
	sem_wait(&SEM_DIC_MAPA_POKEMON);
	dictionary_iterator(DIC_MAPA_POKEMON,obtener_coordenada);
	sem_post(&SEM_DIC_MAPA_POKEMON);

	return pokemon_a_retornar;
}

entrenador_t*entrenador_mas_cercano_en_new(uint32_t pos_x,uint32_t pos_y){

	t_list* lista_a_retornar = entrenador_mas_cercano_en_a(COLA_NEW->elements,pos_x,pos_y);

	return lista_a_retornar;
}

bool esta_mas_cerca(entrenador_t*entrenador_1,entrenador_t* entrenador_2, uint32_t pos_x,uint32_t pos_y){

	float distancia_1=modulo(pos_x-entrenador_1->posicion_actual->pos_x,pos_y-entrenador_1->posicion_actual->pos_y) ;
	float distancia_2=modulo(pos_x-entrenador_2->posicion_actual->pos_x,pos_y-entrenador_2->posicion_actual->pos_y);
	return distancia_1 <= distancia_2;

}
entrenador_t* buscar_entrenador_mas_cercano(uint32_t pos_x,uint32_t pos_y){
	//SOLO ENTRENADORES NEW Y BLOCK(SOLO LOS QUE NO ESTEN ESPERANDO UN CAUGHT)
	sem_wait(&SEM_TRANSICION);
	entrenador_t* entrenador_new=entrenador_mas_cercano_en_new(pos_x,pos_y);
	entrenador_t* entrenador_block=entrenador_mas_cercano_en_block(pos_x,pos_y);
	sem_post(&SEM_TRANSICION); //A VER SI CON ESTO EVITO LA CONDICION FALOPA DEL PRINCIPIO TODO
	if(entrenador_new==NULL){
		if(entrenador_block==NULL){
			//estoy jodido
			return NULL;
		}else{
			return entrenador_block;
		}
	}else{
		if(entrenador_block==NULL){
			return entrenador_new;
		}else{
			return entrenador_mas_cercano(entrenador_new,entrenador_block,pos_x,pos_y);
		}
	}
}
entrenador_t* entrenador_mas_cercano_en_block(uint32_t pos_x,uint32_t pos_y){

	bool entrenadores_ociosos(entrenador_t* entrenador){
		return entrenador->estoy_ocioso;
	}
	t_list* lista_de_ociosos=list_filter(COLA_BLOCK->elements,entrenadores_ociosos);

	entrenador_t* entrenador_a_retornar = entrenador_mas_cercano_en_a(lista_de_ociosos,pos_x,pos_y);

	list_destroy(lista_de_ociosos);

	return entrenador_a_retornar;
}

void liberar_entrenador(entrenador_t* un_entrenador){

	free(un_entrenador->posicion_actual);
	free(un_entrenador->posicion_deseada);
	sem_destroy(&un_entrenador->semaforo_entrenador);
	sem_destroy(&un_entrenador->semaforo_entre_en_ready);
	sem_destroy(&un_entrenador->semaforo_puedo_hacer_otra_rafaga);
	sem_destroy(&un_entrenador->termine_operacion);
	sem_destroy(&un_entrenador->sem_operaciones_entrenador);

	/*
	uint32_t estado_actual;
	uint32_t id_pokemon;
	uint32_t id_entrenador;
	bool estoy_ocioso;
	bool estoy_esperando_un_intercambio;
	long estimacion_anterior;
	long estimacion_actual;
	int ciclos_que_ejecute;
	 */

	void destructor(char* un_pokemon){
		if(un_pokemon!=NULL && !string_is_empty(un_pokemon))
		free(un_pokemon);
	}
	list_destroy_and_destroy_elements(un_entrenador->pokemones_que_tengo_para_intercambiar,destructor);
	list_destroy_and_destroy_elements(un_entrenador->pokemones_que_me_faltan,destructor);
	free(un_entrenador);
}

entrenador_t*entrenador_mas_cercano_en_a(t_list* lista_entrenadores,uint32_t pos_x,uint32_t pos_y){

	entrenador_t* mas_cercano;
	bool buscar_el_mas_cerca(entrenador_t* entrenador_1,entrenador_t* entrenador_2){

		return esta_mas_cerca(entrenador_1,entrenador_2, pos_x,pos_y);

	}
	if(list_is_empty(lista_entrenadores)){
		return NULL;
	}else{
		t_list* lista_ordenada=list_sorted(lista_entrenadores,buscar_el_mas_cerca);
		mas_cercano = list_remove(lista_ordenada,0);
		list_destroy(lista_ordenada);
		return mas_cercano;
	}
}

entrenador_t* entrenador_mas_cercano(entrenador_t* entrenador_new,entrenador_t* entrenador_block,uint32_t pos_x,uint32_t pos_y){

	if(esta_mas_cerca(entrenador_new,entrenador_block,pos_x,pos_y)){
		return entrenador_new;
	}else{
		return entrenador_block;
	}
}

double modulo(int x,int y){
	return sqrt(pow((double)x,2)+pow((double)y,2));
}
void pasar_de_cola_a(entrenador_t* entrenador, int cola_deseada){

	sem_wait(&entrenador->cambio_de_estado);
	bool condicion=entrenador->estado_actual == READY;
	sem_post(&entrenador->cambio_de_estado);

	if(condicion){
		sem_wait(&sem_entrenadores_ready);
	}

	sem_wait(&SEM_TRANSICION);

	t_queue* cola_a_pasar=obtener_cola(cola_deseada);
	int cola_del_entrenador=entrenador->estado_actual;
	t_queue* cola_a_salir=obtener_cola(cola_del_entrenador);



	bool mismo_id(entrenador_t* un_entrenador){
		return un_entrenador->id_entrenador == entrenador->id_entrenador;
	}
	list_remove_by_condition(cola_a_salir->elements,mismo_id);
	queue_push(cola_a_pasar,entrenador);

	sem_post(&SEM_TRANSICION);

	if(cola_deseada == READY){
		sem_post(&sem_entrenadores_ready);
	}

	//sem_wait(&entrenador->sem_operaciones_entrenador);
	sem_wait(&entrenador->cambio_de_estado);
	entrenador->estado_actual=cola_deseada;
	sem_post(&entrenador->cambio_de_estado);
	//sem_post(&entrenador->sem_operaciones_entrenador);
}

t_queue*obtener_cola(int cola_deseada){

	switch(cola_deseada){

	case NEW:
		return COLA_NEW;
		break;

	case READY:
		return COLA_READY;
		break;

	case EXEC:
		return COLA_EXEC;
		break;

	case BLOCK:
		return COLA_BLOCK;
		break;

	case EXIT:
		return COLA_EXIT;
		break;
	}

	return NULL;
}

bool es_mi_id(uint32_t id){

	bool buscar_id(uint32_t otro_id){
		return id == otro_id;
	}
	sem_wait(&SEM_LISTA_ID);
	bool retorno = list_any_satisfy(LISTA_ID, buscar_id);
	sem_post(&SEM_LISTA_ID);

	return retorno;
}

bool son_iguales(t_list* una_lista, t_list* otra_lista){

	int longitud_lista = list_size(una_lista);
	bool valor_retorno = 1;

	for(int i=0;i<longitud_lista;i++){
		valor_retorno = strcmp(list_get(una_lista,i),list_get(una_lista,i)) && valor_retorno;
	}

	return valor_retorno;
}

bool cumpli_todos_mis_objetivos(entrenador_t* un_entrenador){

	sem_wait(&SEM_POKEMON_ENTRENADORES);
	t_list* lista_pokemon_actuales = list_get(POKEMON_ENTRENADORES,un_entrenador->id_entrenador);
	sem_post(&SEM_POKEMON_ENTRENADORES);

	sem_wait(&SEM_OBJETIVOS_ENTRENADORES);
	t_list* lista_objetivos = list_get(OBJETIVOS_ENTRENADORES,un_entrenador->id_entrenador);
	sem_post(&SEM_OBJETIVOS_ENTRENADORES);

	return list_size(lista_pokemon_actuales) == list_size(lista_objetivos)
			&& list_is_empty(un_entrenador->pokemones_que_tengo_para_intercambiar);
}

void comportamiento_default(int codigo_de_operacion, uint32_t id_mensaje){
	switch(codigo_de_operacion){
	case GET_POKEMON:
		sem_wait(&SEM_LOGS);
		log_info(log_obligatorio,"No me pude comunicar con Broker, vamos a realizar la operacion GET_POKEMON (%d) por default",GET_POKEMON);
		sem_post(&SEM_LOGS);
		break;
	case CATCH_POKEMON:
		sem_wait(&SEM_LOGS);
		log_info(log_obligatorio,"No me pude comunicar con Broker, vamos a realizar la operacion CAUGHT_POKEMON (%d) por default",CAUGHT_POKEMON);
		sem_post(&SEM_LOGS);
		CAUGHT_POKEMON_TEAM(id_mensaje,1);
		break;
	default:
		printf("Oh no! Algo ha salido mal!");
	}
}

t_list* obtener_lista_de_pokemons(char** lista){
	t_list* lista_de_pokemons=list_create();
	int i=0;
	bool llegue_al_null = false;
	while(list_get(POSICIONES_ENTRENADORES,i) != NULL){
		if(!llegue_al_null){
			t_list* una_lista=leer_lista_string(lista[i]);
			list_add(lista_de_pokemons,una_lista);
			llegue_al_null = lista[i] == NULL;
		}else{
			list_add(lista_de_pokemons,list_create());
		}
		i++;
	}
	return lista_de_pokemons;

}

t_list* leer_lista_string(char*lista){

	if(lista == NULL){
		return list_create();
	}
	t_list* lista_retorno=list_create();
	int i=0;
	int longitud=strlen(lista)+1;
	char* nombre_string= malloc(longitud);
	memset(nombre_string,'\0',longitud);
	int offset=0;
	while(lista[i]!=NULL){
		if(lista[i]=='|'){
			char* copia=malloc(strlen(nombre_string)+1);
			strcpy(copia,nombre_string);
			list_add(lista_retorno,copia);
			memset(nombre_string,'\0',longitud);
			offset=0;
		}else{
			memcpy(nombre_string+offset,&(lista[i]),1);
			offset++;
		}
		i++;
	}
	char* ultima_copia=malloc(strlen(nombre_string)+1);
	strcpy(ultima_copia,nombre_string);
	list_add(lista_retorno,ultima_copia);
	free(nombre_string);
	return lista_retorno;
}

t_list* ubicar_entrenadores(char** lista){
	t_list* lista_entrenadores_posicion=list_create();
	int i=0;
	while(lista[i]!=NULL){
		coordenadas_t* una_coordenada=leer_coordenada_string(lista[i]);
		list_add(lista_entrenadores_posicion,una_coordenada);
		i++;
	}
	return lista_entrenadores_posicion;
}


coordenadas_t* leer_coordenada_string(char*coordenada_string){

	coordenadas_t* una_coordenada=(coordenadas_t*)malloc(sizeof(coordenadas_t));
	int i=0;
	int longitud_total=strlen(coordenada_string)+1;
	char*numero_auxiliar_string=malloc(longitud_total);
	memset(numero_auxiliar_string,'\0',longitud_total);
	int offset=0;
	while(coordenada_string[i]!=NULL){
		if(coordenada_string[i]=='|'){
			una_coordenada->pos_x=atoi(numero_auxiliar_string);
			offset=0;
		}else{
			memcpy(numero_auxiliar_string+offset,&(coordenada_string[i]),1);
			offset++;
		}
		i++;
	}
	una_coordenada->pos_y=atoi(numero_auxiliar_string);

	free(numero_auxiliar_string);

	return una_coordenada;
}

void liberar_char_doble_puntero(char**char_doble_puntero){
	int i=0;
	while(char_doble_puntero[i]!=NULL){
		free(char_doble_puntero[i]);
		i++;
	}
	free(char_doble_puntero);

}

bool tiene_capacidad_maxima_pokemon(entrenador_t* entrenador){

	sem_wait(&SEM_POKEMON_ENTRENADORES);
	t_list* lista_pokemon_actuales = list_get(POKEMON_ENTRENADORES,entrenador->id_entrenador);
	sem_post(&SEM_POKEMON_ENTRENADORES);

	sem_wait(&SEM_OBJETIVOS_ENTRENADORES);
	t_list* lista_pokemon_requeridos = list_get(OBJETIVOS_ENTRENADORES,entrenador->id_entrenador);
	sem_post(&SEM_OBJETIVOS_ENTRENADORES);

	return list_size(lista_pokemon_actuales) == list_size(lista_pokemon_requeridos);
}

void realizar_intercambio(entrenador_t* entrenador){

	//Realizo 5 ciclos de CPU ya que se va a realizar un intercambio
	for(int i=0; i<5;i++){
		//sem_post(&entrenador->semaforo_puedo_hacer_otra_rafaga);
		sem_wait(&entrenador->semaforo_entrenador);
	}
	entrenador->ciclos_que_ejecute+=5;

	sem_wait(&SEM_LOG_METRICAS);
	log_info(log_metricas,"El entrenador %d acaba de ejecutar %d ciclos de ejecucion para un intercambio",entrenador->id_entrenador,5);
	sem_post(&SEM_LOG_METRICAS);

	bool tiene_mi_posicion(entrenador_t* un_entrenador){
		return entrenador->posicion_actual->pos_x == un_entrenador->posicion_actual->pos_x
				&& entrenador->posicion_actual->pos_y == un_entrenador->posicion_actual->pos_y
				&& !(un_entrenador->estoy_esperando_un_caught);
	}

	entrenador_t* otro_entrenador = list_find(COLA_BLOCK->elements,tiene_mi_posicion);

	if(otro_entrenador != NULL){ // SI NO ENTRA, ME GANO OTRO DE MANO
		intercambiar_pokemones(entrenador,otro_entrenador);
		mover_de_cola(otro_entrenador);
	}else{
		printf("Me ganaron de mano picaron\n");
	}
	mover_de_cola(entrenador);
}

void intercambiar_pokemones(entrenador_t*  un_entrenador, entrenador_t* otro_entrenador){

	//sem_wait(&un_entrenador->sem_operaciones_entrenador);
	char* pokemon_que_voy_a_intercambiar = list_remove(un_entrenador->pokemones_que_tengo_para_intercambiar, 0);
	//sem_post(&un_entrenador->sem_operaciones_entrenador);

	//sem_wait(&otro_entrenador->sem_operaciones_entrenador);
	char* pokemon_que_voy_a_recibir = list_remove(otro_entrenador->pokemones_que_tengo_para_intercambiar, 0);
	//sem_post(&otro_entrenador->sem_operaciones_entrenador);

	sem_wait(&SEM_POKEMON_ENTRENADORES); //SI ROMPE ES ESTE SEMAFORO TODO
	t_list* pokemones_un_entrenador = list_get(POKEMON_ENTRENADORES,un_entrenador->id_entrenador);
	t_list* pokemones_otro_entrenador = list_get(POKEMON_ENTRENADORES,otro_entrenador->id_entrenador);

	list_add(pokemones_un_entrenador,pokemon_que_voy_a_recibir);
	list_add(pokemones_otro_entrenador,pokemon_que_voy_a_intercambiar);

	bool voy_a_recibir_ese_pokemon(char* un_pokemon){
		return !strcmp(un_pokemon,pokemon_que_voy_a_recibir);
	}
	list_remove_by_condition(pokemones_otro_entrenador,voy_a_recibir_ese_pokemon);

	bool voy_a_dar_ese_pokemon(char* un_pokemon){
		return !strcmp(un_pokemon,pokemon_que_voy_a_intercambiar);
	}
	list_remove_by_condition(pokemones_un_entrenador,voy_a_dar_ese_pokemon);

	//actualizar_lista_de_pokemones_faltantes(un_entrenador,pokemon_que_voy_a_recibir);
	//actualizar_lista_de_pokemones_faltantes(otro_entrenador,pokemon_que_voy_a_intercambiar);
	agregar_pokemon_que_no_me_sirve(un_entrenador,pokemon_que_voy_a_recibir);
	agregar_pokemon_que_no_me_sirve(otro_entrenador,pokemon_que_voy_a_intercambiar);

	sem_post(&SEM_POKEMON_ENTRENADORES);

	sem_wait(&SEM_LOGS);
	log_info(log_obligatorio, "El entrenador %d intercambio un %s por un %s del entrenador %d",un_entrenador->id_entrenador,pokemon_que_voy_a_intercambiar,pokemon_que_voy_a_recibir,otro_entrenador->id_entrenador);
	sem_post(&SEM_LOGS);

	sem_wait(&SEM_LOG_METRICAS);
	log_info(log_metricas,"Solucionamos deadlock intercambiando un %s del entrenador %d  por un %s del entrenador %d",pokemon_que_voy_a_intercambiar,un_entrenador->id_entrenador,pokemon_que_voy_a_recibir,otro_entrenador->id_entrenador);
	sem_post(&SEM_LOG_METRICAS);

	cantidad_de_deadlocks_resueltos++;

}

void mover_de_cola(entrenador_t* entrenador){

	if(cumpli_todos_mis_objetivos(entrenador)){

		entrenador->estoy_ocioso=false;
		entrenador->estoy_esperando_un_intercambio = false;
		sem_wait(&SEM_LOGS);
		log_info(log_obligatorio,"El entrenador %d va a pasar a la cola EXIT ya que cumplio su objetivo personal",entrenador->id_entrenador);
		sem_post(&SEM_LOGS);
		pasar_de_cola_a(entrenador,EXIT);

	}else{
		if(!tiene_capacidad_maxima_pokemon(entrenador)){

			if(puedo_atrapar_algun_pokemon_del_mapa(entrenador)){

				//Voy a READY para poder atrapar a ese pokemon
				coordenadas_t* new_coordinates=asignar_coordenadas_del_pokemon(entrenador);
				entrenador->posicion_deseada->pos_x = new_coordinates->pos_x;
				entrenador->posicion_deseada->pos_y = new_coordinates->pos_y;
				free(new_coordinates);
				sem_wait(&SEM_LOGS);
				log_info(log_obligatorio,"El entrenador %d va a pasar a la cola READY ya que puede atrapar otro pokemon",entrenador->id_entrenador);
				sem_post(&SEM_LOGS);
				pasar_de_cola_a(entrenador,READY);
				sem_post(&entrenador->semaforo_entre_en_ready);

			}else{

				//Me quedo en BLOCK hasta que aparezca en el mapa un pokemon que necesite
				entrenador->estoy_ocioso=true;
				pasar_de_cola_a(entrenador,BLOCK);
				sem_wait(&SEM_LOGS);
				log_info(log_obligatorio,"El entrenador %d va a pasar a la cola BLOCK porque no hay pokemons que pueda atrapar",entrenador->id_entrenador);
				sem_post(&SEM_LOGS);
			}

		}else{

			//Hay que realizar un intercambio
			entrenador->estoy_ocioso=false;
			sem_wait(&SEM_LOGS);
			log_info(log_obligatorio,"El entrenador %d va a pasar a la cola BLOCK porque necesita hacer un intercambio",entrenador->id_entrenador);
			sem_post(&SEM_LOGS);
			pasar_de_cola_a(entrenador,BLOCK);
			avisar_mis_coordenadas(entrenador);
		}
	}
	sem_post(&entrenador->termine_operacion);
}

coordenadas_t* asignar_coordenadas_del_pokemon(entrenador_t*entrenador){

	t_list* coordenadas_posibles=list_create();

	void filtrar_coordenadas(char*pokemon,t_list* lista_de_coordenadas){

		bool no_las_buscan(tupla_t* tupla){
			return tupla->flag==NADIE_LO_BUSCA;
		}
		t_list* lista_filtrada = list_filter(lista_de_coordenadas,no_las_buscan);
		list_add_all(coordenadas_posibles,lista_filtrada);
		list_destroy(lista_filtrada);
	}
	dictionary_iterator(DIC_MAPA_POKEMON,filtrar_coordenadas);

	tupla_t* tupla_mas_cercana=list_get(coordenadas_posibles,0);

	void mas_cerca(tupla_t* una_tupla){
		int distancia_mas_cercana=modulo(tupla_mas_cercana->pos_x-entrenador->posicion_actual->pos_x,tupla_mas_cercana->pos_y-entrenador->posicion_actual->pos_y);
		int distancia_de_una_tupla=modulo(una_tupla->pos_x-entrenador->posicion_actual->pos_x,una_tupla->pos_y-entrenador->posicion_actual->pos_y);
		if(distancia_mas_cercana>distancia_de_una_tupla){
			tupla_mas_cercana=una_tupla;
		}
	}
	list_iterate(coordenadas_posibles,mas_cerca);
	list_destroy(coordenadas_posibles);

	coordenadas_t* la_coordenada_mas_cerca=malloc(sizeof(coordenadas_t));
	la_coordenada_mas_cerca->pos_x=tupla_mas_cercana->pos_x;
	la_coordenada_mas_cerca->pos_y=tupla_mas_cercana->pos_y;
	tupla_mas_cercana->flag = LO_ESTAN_BUSCANDO;

	return la_coordenada_mas_cerca;
}


int desplazar_en_mapa_al(entrenador_t* entrenador){

	int diferencia_x = abs(entrenador->posicion_actual->pos_x - entrenador->posicion_deseada->pos_x);
	int diferencia_y = abs(entrenador->posicion_actual->pos_y - entrenador->posicion_deseada->pos_y);

	int diferencia_total = diferencia_x + diferencia_y;

	for(int i=0; i<diferencia_total; i++){
		//sem_post(&entrenador->semaforo_puedo_hacer_otra_rafaga);
		sem_wait(&entrenador->semaforo_entrenador);
	}

	sem_wait(&SEM_LOGS);
	log_info(log_obligatorio,"El entrenador %d se movio desde (%d;%d) hasta (%d;%d)",entrenador->id_entrenador,entrenador->posicion_actual->pos_x,entrenador->posicion_actual->pos_y,entrenador->posicion_deseada->pos_x,entrenador->posicion_deseada->pos_y);
	sem_post(&SEM_LOGS);

	//Hacer un issue si hay que loggear cada vez que avanzo o cuando ya llegue a la posicion
	entrenador->posicion_actual->pos_x = entrenador->posicion_deseada->pos_x;
	entrenador->posicion_actual->pos_y = entrenador->posicion_deseada->pos_y;

	sem_wait(&SEM_LOG_METRICAS);
	log_info(log_metricas,"El entrenador %d acaba de ejecutar %d ciclos de ejecucion",entrenador->id_entrenador,diferencia_total);
	sem_post(&SEM_LOG_METRICAS);


	return diferencia_total;
}

void ordenar_lista_de_tradeo(entrenador_t* un_entrenador,entrenador_t* otro_entrenador){

	bool le_sirve_a_un_entrenador(char* un_pokemon, char* otro_pokemon){

		t_list* objetivos_un_entrenador = list_get(OBJETIVOS_ENTRENADORES,un_entrenador->id_entrenador);

		bool es_objetivo(char* objetivo){
			return !strcmp(objetivo,un_pokemon);
		}
		return list_any_satisfy(objetivos_un_entrenador,es_objetivo);
	}
	list_sort(otro_entrenador->pokemones_que_tengo_para_intercambiar,le_sirve_a_un_entrenador);
}

void avisar_mis_coordenadas(entrenador_t* entrenador){

	sem_wait(&SEM_LOGS);
	log_info(log_obligatorio,"Entramos en deadlock, por lo tanto buscamos alguien para intercambiar con el entrenador %d",entrenador->id_entrenador);
	sem_post(&SEM_LOGS);
	t_list* pokemones_que_quiero_intercambiar = entrenador->pokemones_que_tengo_para_intercambiar;
	char* pokemon = list_get(pokemones_que_quiero_intercambiar,0);

	t_list* potenciales_entrenadores = entrenadores_para_intercambiar(entrenador,pokemon);

	entrenador_t* otro_entrenador = entrenador_mas_cercano_en_a(potenciales_entrenadores,
			entrenador->posicion_actual->pos_x, entrenador->posicion_actual->pos_y);

	list_destroy(potenciales_entrenadores);

	if(otro_entrenador!=NULL){
		sem_wait(&SEM_LOGS);
		log_info(log_obligatorio,"Encontramos al entrenador %d para intercambiar con el entrenador %d",otro_entrenador->id_entrenador,entrenador->id_entrenador);
		sem_post(&SEM_LOGS);
		otro_entrenador->posicion_deseada->pos_x = entrenador->posicion_actual->pos_x;
		otro_entrenador->posicion_deseada->pos_y = entrenador->posicion_actual->pos_y;

		//Me pone adelante de la lista el pokemon a tradear
		ordenar_lista_de_tradeo(entrenador, otro_entrenador);

		pasar_de_cola_a(otro_entrenador,READY);
		sem_post(&otro_entrenador->semaforo_entre_en_ready);

		sem_wait(&SEM_LOGS);
		log_info(log_obligatorio,"El entrenador %d va a pasar a la cola READY porque necesita hacer un intercambio con el entrenador %d",otro_entrenador->id_entrenador,entrenador->id_entrenador);
		sem_post(&SEM_LOGS);

		entrenador->estoy_esperando_un_intercambio = false;
	}else{
		sem_wait(&SEM_LOGS);
		log_info(log_obligatorio,"No encontramos un entrenador para intercambiar con el entrenador %d",entrenador->id_entrenador);
		sem_post(&SEM_LOGS);

		entrenador->estoy_esperando_un_intercambio = true;
	}
}

sem_t* obetener_semaforo_log(){
	return &SEM_LOGS;
}

t_log* obtener_log_obligatorio(){

	return log_obligatorio;
}
t_list* pokemones_que_no_me_sirven(entrenador_t* entrenador){

	sem_wait(&SEM_POKEMON_ENTRENADORES);
	t_list* pokemones_para_tradear = list_duplicate(list_get(POKEMON_ENTRENADORES, entrenador->id_entrenador));
	sem_post(&SEM_POKEMON_ENTRENADORES);

	sem_wait(&SEM_OBJETIVOS_ENTRENADORES);
	t_list* necesito = list_get(OBJETIVOS_ENTRENADORES, entrenador->id_entrenador);
	sem_post(&SEM_OBJETIVOS_ENTRENADORES);

	void obtener_pokemones_sobrantes(char* pokemon){
		bool es_mi_pokemon(char* otro_pokemon){
			return !strcmp(pokemon,otro_pokemon);
		}
		if(!list_any_satisfy(necesito,es_mi_pokemon))
		{
			list_remove_by_condition(pokemones_para_tradear,es_mi_pokemon);
		}
	}
	list_iterate(pokemones_para_tradear,obtener_pokemones_sobrantes);
	return pokemones_para_tradear;
}

t_list* entrenadores_para_intercambiar(entrenador_t* entrenador_que_necesita_intercambiar, char* pokemon){

	bool esta_en_sus_objetivos(entrenador_t* entrenador){

		bool esta_esperando_un_intercambio = entrenador->estoy_esperando_un_intercambio;
		bool esta_ocioso = entrenador->estoy_ocioso ;

		return (esta_ocioso || esta_esperando_un_intercambio) && quiero_ese_pokemon(entrenador,pokemon);
	}

	sem_wait(&SEM_TRANSICION);
	t_list* entrenadores = list_filter(COLA_NEW->elements, esta_en_sus_objetivos);
	t_list* lista_filtrada = list_filter(COLA_BLOCK->elements, esta_en_sus_objetivos);
	t_list* lista_sin_filtrar = list_create();
	list_add_all(lista_sin_filtrar,lista_filtrada);
	list_add_all(lista_sin_filtrar,entrenadores);
	sem_post(&SEM_TRANSICION);

	//Compara los intereses de cada entrenador

	bool tiene_algo_para_tradear(entrenador_t* entrenador){

		bool me_interesan_eso_pokemones(char* un_pokemon){
			t_list* objetivos = entrenador_que_necesita_intercambiar->pokemones_que_me_faltan;

			bool pertenece_a_mis_objetivos(char* objetivo){
				return !strcmp(objetivo,un_pokemon);
			}
			return list_any_satisfy(objetivos, pertenece_a_mis_objetivos);
		}

		return list_any_satisfy(entrenador->pokemones_que_tengo_para_intercambiar,me_interesan_eso_pokemones);
	}
	t_list* lista_a_retornar = list_filter(lista_sin_filtrar, tiene_algo_para_tradear);
	//Quieren y NO tienen el pokemon que estoy cambiando

	if(!list_is_empty(lista_a_retornar)){
		//El intercambio es beneficioso para ambos entrenadores
		list_destroy(entrenadores);
		list_destroy(lista_filtrada);
		list_destroy(lista_sin_filtrar);
		return lista_a_retornar;
	}else{
		//El intercambio es beneficioso solo para un entrenador
		list_destroy(lista_a_retornar);
		list_destroy(lista_filtrada);
		list_destroy(entrenadores);

		bool tiene_para_intercambiar(entrenador_t* un_entrenador){
			return !list_is_empty(un_entrenador->pokemones_que_tengo_para_intercambiar);
		}
		t_list* lista_a_retornar_dos = list_filter(lista_sin_filtrar,tiene_para_intercambiar);
		list_destroy(lista_sin_filtrar);
		return lista_a_retornar_dos;
	}
}

bool quiero_ese_pokemon(entrenador_t* entrenador, char* pokemon){

	t_list* pokemones_que_me_faltan = entrenador->pokemones_que_me_faltan;

	bool una_funcion(char* el_pokemon){
		return !strcmp(el_pokemon,pokemon);
	}

	return list_any_satisfy(pokemones_que_me_faltan,una_funcion);
}

bool cumple_con_algun_objetivo_personal(entrenador_t* entrenador,char* pokemon){

	/*t_list* objetivos = list_get(OBJETIVOS_ENTRENADORES,entrenador->id_entrenador);
	t_list* pokemones_que_tengo = list_get(POKEMON_ENTRENADORES,entrenador->id_entrenador);

	bool esta_en_la_lista(char* un_pokemon){
		return !strcmp(pokemon,un_pokemon);
	}

	int cantidad_que_necesito = list_count_satisfying(objetivos,esta_en_la_lista);
	int cantidad_que_tengo = list_count_satisfying(pokemones_que_tengo,esta_en_la_lista);

	return cantidad_que_necesito>cantidad_que_tengo;*/

	t_list* lista_faltantes = entrenador->pokemones_que_me_faltan;

	bool remover_pokemon(char* un_pokemon){
		return !strcmp(pokemon,un_pokemon);
	}
	char* pokemon_removido = list_remove_by_condition(lista_faltantes,remover_pokemon);

	bool retorno = pokemon_removido != NULL;

	if(retorno){
		free(pokemon_removido);
	}

	return retorno;
}

void agregar_pokemon_que_no_me_sirve(entrenador_t* entrenador,char* pokemon){

	bool flag = cumple_con_algun_objetivo_personal(entrenador,pokemon);

	if(!flag){
		list_add(entrenador->pokemones_que_tengo_para_intercambiar,pokemon);
	}
}
/*
void actualizar_lista_de_pokemones_faltantes(entrenador_t* entrenador,char* pokemon){

	t_list* lista_faltantes = entrenador->pokemones_que_me_faltan;

	bool remover_pokemon(char* un_pokemon){
		return !strcmp(pokemon,un_pokemon);
	}
	char* pokemon_removido = list_remove_by_condition(lista_faltantes,remover_pokemon);

	if(pokemon_removido == NULL){
		list_add(entrenador->pokemones_que_tengo_para_intercambiar,pokemon);
	}
}*/

entrenador_t* buscar_entrenador_por_coordenada(uint32_t pos_x,uint32_t pos_y){

	bool buscar_por_coordenada(entrenador_t* un_entrenador){
		return un_entrenador->posicion_actual->pos_x == pos_x &&
				un_entrenador->posicion_actual->pos_y == pos_y;
	}
	sem_wait(&SEM_LISTA_DE_ENTRENADORES);
	entrenador_t* entrenador_a_retornar = list_find(LISTA_DE_ENTRENADORES,buscar_por_coordenada);
	sem_post(&SEM_LISTA_DE_ENTRENADORES);

	return entrenador_a_retornar;
}

bool puedo_atrapar_algun_pokemon_del_mapa(entrenador_t* entrenador){

	int contador = 0;
	void contar_total_coordenadas(char* pokemon, t_list* lista_posiciones){
		bool nadie_lo_quiere(tupla_t* una_tupla){
			return una_tupla->flag == NADIE_LO_BUSCA;
		}
		contador+=list_count_satisfying(lista_posiciones,nadie_lo_quiere);
	}
	sem_wait(&SEM_DIC_MAPA_POKEMON);
	dictionary_iterator(DIC_MAPA_POKEMON,contar_total_coordenadas);
	sem_post(&SEM_DIC_MAPA_POKEMON);

	return contador > 0;
}

void actualizar_mapa(char* pokemon, entrenador_t* entrenador_que_realizo_el_catch){

	sem_wait(&SEM_DIC_MAPA_POKEMON);
	t_list* lista_de_posiciones_de_esa_especie = dictionary_get(DIC_MAPA_POKEMON, pokemon);

	bool eliminar_pokemon(tupla_t* una_coordenada){
		return una_coordenada->pos_x == entrenador_que_realizo_el_catch->posicion_actual->pos_x
				&& una_coordenada->pos_y == entrenador_que_realizo_el_catch->posicion_actual->pos_y;
	}
	tupla_t* una_tupla = list_remove_by_condition(lista_de_posiciones_de_esa_especie, eliminar_pokemon);

	dictionary_put(DIC_MAPA_POKEMON,pokemon,lista_de_posiciones_de_esa_especie);

	free(una_tupla); //Guarda con este free() quizas usan la tupla más adelante

	sem_post(&SEM_DIC_MAPA_POKEMON);
}


t_list* pokemones_que_me_faltan(entrenador_t* un_entrenador){

	t_list* objetivos = list_create();
	t_list* objetivos_real = list_get(OBJETIVOS_ENTRENADORES,un_entrenador->id_entrenador);

	t_list* pokemones_que_tengo = list_create();
	t_list* pokemones_que_tengo_real =list_get(POKEMON_ENTRENADORES,un_entrenador->id_entrenador);

	void duplicar_objetivos(char* pokemon){
		char* pokemon_duplicado = malloc(strlen(pokemon)+1);
		strcpy(pokemon_duplicado,pokemon);
		list_add(objetivos,pokemon_duplicado);
	}
	list_iterate(objetivos_real,duplicar_objetivos);

	void duplicar(char* pokemon){
		char* pokemon_duplicado = malloc(strlen(pokemon)+1);
		strcpy(pokemon_duplicado,pokemon);
		list_add(pokemones_que_tengo,pokemon_duplicado);
	}
	list_iterate(pokemones_que_tengo_real,duplicar);

	int longitud_objetivos = list_size(objetivos);
	int longitud_tengo = list_size(pokemones_que_tengo);

	for(int i=0; i<longitud_objetivos; i++){
		char* un_pokemon = list_get(objetivos,i);

		for(int j=0; j< longitud_tengo; j++){
			char* otro_pokemon = list_get(pokemones_que_tengo,j);

			if(!strcmp(un_pokemon,otro_pokemon)){
				free(list_remove(objetivos,i));
				free(list_remove(pokemones_que_tengo,j));
				i--;
				j--;
				longitud_objetivos--;
				longitud_tengo--;
			}
		}
	}

	void destructor(char* el_pokemon){
		free(el_pokemon);
	}
	list_destroy_and_destroy_elements(pokemones_que_tengo,destructor);

	return objetivos;
}

/*void decrementar_pokemones_faltantes(char* nombre_pokemon){
	int cantidad_pokemon = dictionary_get(DIC_FALTANTES_ENTRENADORES,nombre_pokemon);
	if(cantidad_pokemon > 1){
		dictionary_put(DIC_FALTANTES_ENTRENADORES,nombre_pokemon,cantidad_pokemon--);
	}else{
		dictionary_remove(DIC_FALTANTES_ENTRENADORES,nombre_pokemon);
	}
}*/

void decrementar_pokemones_faltantes(char* nombre_pokemon){
	int cantidad_pokemon = dictionary_remove(DIC_FALTANTES_ENTRENADORES,nombre_pokemon);
	if(cantidad_pokemon > 1){
		dictionary_put(DIC_FALTANTES_ENTRENADORES,nombre_pokemon,cantidad_pokemon-1);
	}
}

void incrementar_pokemones_faltantes(char* nombre_pokemon){
	sem_wait(&SEM_DIC_FALTANTES_ENTRENADORES);
	bool esta_en_el_dic = dictionary_has_key(DIC_FALTANTES_ENTRENADORES,nombre_pokemon);
	if(esta_en_el_dic){
		int cantidad_pokemon = dictionary_get(DIC_FALTANTES_ENTRENADORES,nombre_pokemon);
		dictionary_put(DIC_FALTANTES_ENTRENADORES,nombre_pokemon,cantidad_pokemon++);
	}else{
		dictionary_put(DIC_FALTANTES_ENTRENADORES,nombre_pokemon,1);
	}
	sem_post(&SEM_DIC_FALTANTES_ENTRENADORES);
}

bool le_sirve_al_team(char* nombre_pokemon){
	sem_wait(&SEM_DIC_FALTANTES_ENTRENADORES);
	bool me_sirve = dictionary_has_key(DIC_FALTANTES_ENTRENADORES,nombre_pokemon);
	sem_post(&SEM_DIC_FALTANTES_ENTRENADORES);
	return me_sirve;
}

void calcular_ciclos_globales(){
	for(int i = 0; list_size(LISTA_DE_ENTRENADORES) > i; i++){
		entrenador_t* un_entrenador = list_get(LISTA_DE_ENTRENADORES,i);
		cantidad_de_ciclos_cpu_total += un_entrenador->ciclos_que_ejecute;
	}
}
