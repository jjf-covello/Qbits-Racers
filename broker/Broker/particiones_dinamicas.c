/*
 * particiones_dinamicas.c
 *
 *  Created on: 15 abr. 2020
 *      Author: utnso
 */
#include <math.h>
#include "tads_y_globales.h"

//Algoritmos para encontrar particion libre

metadata_t* encontrar_libre_por_first_fit(uint32_t tamanio_requerido){

	bool metadata_esta_libre_y_tiene_espacio(metadata_t* metadata){
		return metadata->libre && metadata->tamanio_ocupado >= tamanio_requerido;
	}

	sem_wait(&sem_lista_particiones);
	metadata_t* metadata = list_find(lista_de_particiones, (void*)metadata_esta_libre_y_tiene_espacio);
	sem_post(&sem_lista_particiones);

	return metadata;
}

metadata_t* encontrar_libre_por_best_fit(uint32_t tamanio_requerido){

	bool metadata_esta_libre_y_tiene_espacio(metadata_t* metadata){
		return metadata->libre && metadata->tamanio_ocupado >= tamanio_requerido;
	}

	sem_wait(&sem_lista_particiones);
	t_list* lista_particiones_libres_con_tamanio_suficiente = list_filter(lista_de_particiones, (void*)metadata_esta_libre_y_tiene_espacio);

	bool ordenar_menor_a_mayor_tamanio(metadata_t* metadata_mas_chica, metadata_t* metadata){
		return metadata_mas_chica->tamanio_ocupado < metadata->tamanio_ocupado;
	}

	t_list* lista_con_particiones_libres_ordenada_por_tamanio = list_sorted(lista_particiones_libres_con_tamanio_suficiente, (void*)ordenar_menor_a_mayor_tamanio);

	metadata_t* metadata = list_get(lista_con_particiones_libres_ordenada_por_tamanio, 0);
	sem_post(&sem_lista_particiones);

	return metadata;
}

//Algoritmos para elegir victima
//Tecnicamente no eliminan, sino que sobreescriben la data del que deberia ser eliminado.

void eliminar_por_fifo(){

	bool ordenar_por_entrada_mas_vieja(metadata_t* metadata_mas_vieja, metadata_t* metadata){
		return (metadata->instante_entrada > metadata_mas_vieja->instante_entrada);
	}

	sem_wait(&sem_lista_particiones);
	t_list* lista_con_particiones_ordenadas_por_entrada = list_sorted(lista_de_particiones, (void*)ordenar_por_entrada_mas_vieja);

	bool metadata_usada(metadata_t* metadata){
		if(metadata->libre)
			return false;
		else
			return true;
	}

	metadata_t* metadata = list_find(lista_con_particiones_ordenadas_por_entrada, (void*)metadata_usada);
	sem_post(&sem_lista_particiones);

	if(!metadata == NULL){
		//Log stuff
		uint32_t offset = obtener_offset_en_cache(metadata->posicion_en_lista);

		//char* num = string_itoa(cache_broker + offset);
		char* num = (char*)malloc(20);
		memset(num, '\0', 20);
		sprintf(num, "%p", cache_broker + offset);
		char* msj = (char*)malloc(strlen("Particion eliminada. Posicion de inicio: ")+strlen(num));
		memset(msj, '\0', strlen("Particion eliminada. Posicion de inicio: ")+strlen(num));
		memcpy(msj, "Particion eliminada. Posicion de inicio: ", strlen("Particion eliminada. Posicion de inicio: "));
		string_append(&msj, num);
		loggear_mensaje(msj, 1);
		free(msj);
		free(num);

		sacar_mensaje_de_cola(metadata->codigo_de_cola, metadata->id_mensaje_contenido);

		metadata->libre = true;
		metadata->codigo_de_cola = NULL;
		metadata->timestamp_LRU = NULL;
		metadata->instante_entrada = NULL;
		metadata->id_mensaje_contenido = NULL;

		//list_add_in_index(lista_de_particiones, metadata->posicion_en_lista, metadata);
	}

	list_destroy(lista_con_particiones_ordenadas_por_entrada);
}

void eliminar_por_LRU(){

	bool ordenar_mayor_a_menor_timestamp(metadata_t* metadata_mas_vieja, metadata_t* metadata){
		return metadata->timestamp_LRU > metadata_mas_vieja->timestamp_LRU;
	}

	sem_wait(&sem_lista_particiones);
	t_list* lista_con_particiones_ordenadas_por_timestamp = list_sorted(lista_de_particiones, (void*)ordenar_mayor_a_menor_timestamp);

	bool metadata_usada(metadata_t* metadata){
		if(metadata->libre)
			return false;
		else
			return true;
	}

	metadata_t* metadata = list_find(lista_con_particiones_ordenadas_por_timestamp, (void*)metadata_usada);
	sem_post(&sem_lista_particiones);

	if(!metadata == NULL){
		//Log stuff
		uint32_t offset = obtener_offset_en_cache(metadata->posicion_en_lista);
		//char* offset_string = string_itoa(cache_broker + offset);
		char* offset_string = (char*)malloc(20);
		memset(offset_string, '\0', 20);
		sprintf(offset_string, "%p", cache_broker + offset);
		char* msj = (char*)malloc(strlen("Particion eliminada. Posicion de inicio: ")+strlen(offset_string));
		memset(msj, '\0', strlen("Particion eliminada. Posicion de inicio: ")+strlen(offset_string));
		string_append(&msj, "Particion eliminada. Posicion de inicio: ");
		string_append(&msj, offset_string);
		loggear_mensaje(msj, 1);
		free(offset_string);
		free(msj);

		sacar_mensaje_de_cola(metadata->codigo_de_cola, metadata->id_mensaje_contenido);

		metadata->libre = true;
		metadata->codigo_de_cola = NULL;
		metadata->timestamp_LRU = NULL;
		metadata->instante_entrada = NULL;
		metadata->id_mensaje_contenido = NULL;

		//list_add_in_index(lista_de_particiones, metadata->posicion_en_lista, metadata);
	}

	list_destroy(lista_con_particiones_ordenadas_por_timestamp);
}

//Compactacion

void compactar(){
	int i = 0;
	int j = 0;
	int offset = 0;
	void* buffer;

	t_list* lista_particiones_aux;

	bool metadata_esta_libre(metadata_t* metadata){
		return metadata->libre;
	}

	sem_wait(&sem_lista_particiones);

	t_list* lista_particiones_libres = list_filter(lista_de_particiones, (void*)metadata_esta_libre);

	uint32_t obtener_tamanios(metadata_t* metadata){
		return metadata->tamanio_ocupado;
	}

	t_list* lista_de_tamanios = list_map(lista_particiones_libres, (void*)obtener_tamanios);

	uint32_t sumar_tamanios(uint32_t tamanio_inicial, uint32_t tamanio_siguiente){
		return tamanio_inicial + tamanio_siguiente;
	}

	uint32_t tamanio_bloque_libre = list_fold(lista_de_tamanios, 0, (void*)sumar_tamanios);

	int cantidad_usados = abs(list_count_satisfying(lista_de_particiones, (void*)metadata_esta_libre) - list_size(lista_de_particiones));


	//Comienzo burocracia para mover las particiones en memoria posta posta
	bool metadata_esta_ocupada(metadata_t* metadata){
		return !metadata->libre;
	}

	lista_particiones_aux = list_filter(lista_de_particiones, (void*)metadata_esta_ocupada);

	sem_post(&sem_lista_particiones);

	for(j = 0; j < lista_particiones_aux->elements_count; j++){
		metadata_t* metadata = list_get(lista_particiones_aux, j);

		buffer = malloc(metadata->tamanio_reservado);

		uint32_t offset_viejo = obtener_offset_en_cache(metadata->posicion_en_lista);

		memcpy(buffer, cache_broker + offset_viejo, metadata->tamanio_reservado);

		memcpy(cache_broker+offset, buffer, metadata->tamanio_reservado);

		offset += metadata->tamanio_reservado;

		free(buffer);
	}
	//Fin burocracia

	sem_wait(&sem_lista_particiones);

	while(cantidad_usados < list_size(lista_de_particiones)){

		/*bool metadata_esta_libre(metadata_t* metadata){
			return metadata->libre;
		}*/

		metadata_t* metadata_libre = list_remove_by_condition(lista_de_particiones, (void*)metadata_esta_libre);

		free(metadata_libre);
	}

	metadata_t* bloque_libre = (metadata_t*)malloc(sizeof(metadata_t));

	bloque_libre->tamanio_ocupado = tamanio_bloque_libre;
	bloque_libre->tamanio_reservado = tamanio_bloque_libre;
	bloque_libre->libre = true;
	bloque_libre->codigo_de_cola = NULL;
	bloque_libre->timestamp_LRU = NULL;
	bloque_libre->instante_entrada = NULL;
	bloque_libre->posicion_en_lista = NULL;
	bloque_libre->id_mensaje_contenido = NULL;

	list_add(lista_de_particiones, bloque_libre);

	void modificar_posicion_en_lista(metadata_t* metadata){
		metadata->posicion_en_lista = i;
		i++;
	}

	list_iterate(lista_de_particiones, (void*)modificar_posicion_en_lista);

	loggear_mensaje("Compactacion realizada", 1);
	list_destroy(lista_particiones_libres);
	list_destroy(lista_de_tamanios);
	list_destroy(lista_particiones_aux);
	sem_post(&sem_lista_particiones);
}

void mergear_libres_contiguas_v2() {
	//Inspirado en el merge de buddy
	bool hubo_merge = true;

	sem_wait(&sem_lista_particiones);
	//1.
	while(hubo_merge) {
		hubo_merge = false;
		for(int i = 0; i + 1 < lista_de_particiones->elements_count; i++) {
			metadata_t* md_primera = list_get(lista_de_particiones, i);
			metadata_t* md_siguiente = list_get(lista_de_particiones, i+1);

			if(md_primera->libre && md_siguiente->libre) {
				md_primera->tamanio_ocupado = md_primera->tamanio_ocupado + md_siguiente->tamanio_ocupado;

				metadata_t* md_a_borrar = list_remove(lista_de_particiones, i + 1);
				free(md_a_borrar);
				hubo_merge = true;

				//Hago lo mismo que en actualizar_posiciones() pero sin los semaforos
				for(int j = 0; j < lista_de_particiones->elements_count; j++) {
					metadata_t* md_primera = list_get(lista_de_particiones, j);
					md_primera->posicion_en_lista = j;
				}
			}
		}
	}
	sem_post(&sem_lista_particiones);

	actualizar_posiciones();
}

void mergear_libres_contiguas(){
	metadata_t* metadata;
	metadata_t* metadata_sig;
	bool termine_de_revisar_lista = false;
	int i = 0;

	sem_wait(&sem_lista_particiones);
	metadata = list_get(lista_de_particiones, 0);

	while(!termine_de_revisar_lista){

		if(metadata->libre){

			if(metadata->posicion_en_lista + 1 >= list_size(lista_de_particiones)){
				termine_de_revisar_lista = true;
				break;
			}

			metadata_sig = list_get(lista_de_particiones, metadata->posicion_en_lista + 1);

			if(metadata_sig->libre){

				metadata->tamanio_ocupado += metadata_sig->tamanio_ocupado;

				metadata_t* md_a_remover = list_remove(lista_de_particiones, metadata_sig->posicion_en_lista);

				void modificar_posicion_en_lista(metadata_t* md){
					md->posicion_en_lista = i;
					i++;
				}

				list_iterate(lista_de_particiones, (void*)modificar_posicion_en_lista);

				free(md_a_remover);

			}
			else{
				//me paro 2 metadatas adelante (validar que no se termine la lista)
				//valido que pueda
				if(metadata->posicion_en_lista + 2 >= list_size(lista_de_particiones)){
					termine_de_revisar_lista = true;
					break;
				}

				metadata = list_get(lista_de_particiones, metadata->posicion_en_lista + 2);
				//vuelvo a preguntar...
			}
		}
		else{
			//me paro en la metadata sig (validar que no se termine la lista)
			//valido que pueda
			if(metadata->posicion_en_lista + 1 >= list_size(lista_de_particiones)){
				termine_de_revisar_lista = true;
				break;
			}

			metadata = list_get(lista_de_particiones, metadata->posicion_en_lista + 1);
			//vuelvo a preguntar...
		}
	}
	sem_post(&sem_lista_particiones);
}

void guardar_en_cache_pdinamicas(metadata_t* metadata, int tamanio_requerido, int tamanio_original,void* nuevo_paquete, uint32_t id_correlativo, uint32_t cod_cola, uint32_t id_mensaje){
	int i = 0;
	int j = 0;

	//Divido la metadata en 2 metadatas: una con el size ocupado y la nueva libre con el size que le quedo libre
	//Tengo 2 courses of action: que quede un size libre o que no

	if(tamanio_requerido == metadata->tamanio_ocupado){
		metadata->libre = false;
		metadata->tamanio_ocupado = tamanio_requerido;
		metadata->tamanio_reservado = tamanio_original;
		metadata->timestamp_LRU = get_timestamp();
		metadata->instante_entrada = get_timestamp();
		metadata->id_correlativo = id_correlativo;
		metadata->codigo_de_cola = cod_cola;
		metadata->id_mensaje_contenido = id_mensaje;

		//Escribo posta posta
		uint32_t offset = obtener_offset_en_cache(metadata->posicion_en_lista);
		memcpy(cache_broker + offset, nuevo_paquete, tamanio_original);

		//Log stuff
		char* num1 = string_itoa(id_mensaje);
		char* msj = (char*)malloc(strlen("Mensaje ")+strlen(num1)+strlen(" guardado a partir de la posicion "));
		memset(msj, '\0', strlen("Mensaje ")+strlen(num1)+strlen(" guardado a partir de la posicion "));
		memcpy(msj, "Mensaje ", strlen("Mensaje "));
		string_append(&msj, num1);
		string_append(&msj, " guardado a partir de la posicion ");

		char* num2 = (char*)malloc(20);
		memset(num2, '\0', 20);
		sprintf(num2, "%p", cache_broker + offset);
		string_append(&msj, num2);
		free(num2);
		free(num1);

		loggear_mensaje(msj, 1);
		free(msj);

	}else{
		metadata_t* metadata_a_agregar = (metadata_t*)malloc(sizeof(metadata_t));
		int nuevo_tamanio_libre = metadata->tamanio_ocupado - tamanio_requerido;

		metadata->tamanio_ocupado = tamanio_requerido;
		metadata->tamanio_reservado = tamanio_original;
		metadata->libre = false;
		metadata->timestamp_LRU = get_timestamp();
		metadata->instante_entrada = get_timestamp();
		metadata->id_correlativo = id_correlativo;
		metadata->codigo_de_cola = cod_cola;
		metadata->id_mensaje_contenido = id_mensaje;

		//Escribo posta posta
		uint32_t offset = obtener_offset_en_cache(metadata->posicion_en_lista);
		memcpy(cache_broker + offset, nuevo_paquete, tamanio_original);

		//Log stuff
		char* num1 = string_itoa(id_mensaje);
		char* num2 = (char*)malloc(20);
		memset(num2, '\0', 20);
		sprintf(num2, "%p", cache_broker + offset);
		char* msj = (char*)malloc(strlen("Mensaje ")+strlen(num1)+strlen(" guardado a partir de la posicion ")+strlen(num2));
		memset(msj, '\0', strlen("Mensaje ")+strlen(num1)+strlen(" guardado a partir de la posicion ")+strlen(num2));
		memcpy(msj, "Mensaje ", strlen("Mensaje "));
		string_append(&msj, num1);
		string_append(&msj, " guardado a partir de la posicion ");
		string_append(&msj, num2);
		loggear_mensaje(msj, 1);
		free(num1);
		free(num2);
		free(msj);
		//

		metadata_a_agregar->tamanio_ocupado = nuevo_tamanio_libre;
		metadata_a_agregar->libre = true;
		metadata_a_agregar->codigo_de_cola = NULL;
		metadata_a_agregar->timestamp_LRU = NULL;
		metadata_a_agregar->instante_entrada = NULL;
		metadata_a_agregar->posicion_en_lista = NULL;
		metadata_a_agregar->id_mensaje_contenido = NULL;

		sem_wait(&sem_lista_particiones);
		/* ESTO NO ESTARIA FUNCIONANDO
		list_add(lista_de_particiones, metadata_a_agregar); ESTO NO ESTARIA FUNCIONANDO

		for(i = 0; i < list_size(lista_de_particiones)+1; i++){
			metadata = list_replace(lista_de_particiones, metadata->posicion_en_lista + 1, metadata_a_agregar);

			if(metadata == NULL)
				break;

			metadata_a_agregar = metadata;
		}
		 */
		int tamanio_final_lista = list_size(lista_de_particiones)+1;
		for(i = metadata->posicion_en_lista; i < tamanio_final_lista; i++) {
			metadata = list_replace(lista_de_particiones, i + 1, metadata_a_agregar);

			if(metadata == NULL) {
				list_add(lista_de_particiones, metadata_a_agregar);
				break;
			}

			metadata_a_agregar = metadata;
		}

		void modificar_posicion_en_lista(metadata_t* metadata){
			metadata->posicion_en_lista = j;
			j++;
		}

		list_iterate(lista_de_particiones, (void*)modificar_posicion_en_lista);
		sem_post(&sem_lista_particiones);
	}
}

void algoritmo_chevere(int tamanio_requerido, void* nuevo_paquete, uint32_t id_correlativo, uint32_t cod_cola, uint32_t id_mensaje){
	bool particion_libre_encontrada = false;
	bool todas_eliminadas = false;
	int tamanio_original = tamanio_requerido;
	metadata_t* metadata;
	int i;

	if(tamanio_requerido < TAMANO_MINIMO_PARTICION){
		tamanio_requerido = TAMANO_MINIMO_PARTICION;
	}

	while(!particion_libre_encontrada){

		switch(COD_ALGORITMO_BUSQUEDA){
		case FF:
			metadata = encontrar_libre_por_first_fit(tamanio_requerido);
			break;

		case BF:
			metadata = encontrar_libre_por_best_fit(tamanio_requerido);
			break;
		}


		if(metadata == NULL){
			if(FRECUENCIA_COMPACTACION != -1){
				for(i = 0; i < FRECUENCIA_COMPACTACION; i++){
					switch(COD_ALGORITMO_REEMPLAZO){
					case FIFO:
						eliminar_por_fifo();
						//mergear_libres_contiguas();
						mergear_libres_contiguas_v2();
						break;

					case LRU:
						eliminar_por_LRU();
						//mergear_libres_contiguas();
						mergear_libres_contiguas_v2();
						break;
					}

					//Voy a cheqear si tengo espacio luego de pasar por esa iteracion del for
					//Si tengo, hago un break del loop

					switch(COD_ALGORITMO_BUSQUEDA){
					case FF:
						metadata = encontrar_libre_por_first_fit(tamanio_requerido);
						break;

					case BF:
						metadata = encontrar_libre_por_best_fit(tamanio_requerido);
						break;
					}

					if(metadata != NULL)
						break;
				}
			}else{
				while(!todas_eliminadas){
					switch(COD_ALGORITMO_REEMPLAZO){
					case FIFO:
						eliminar_por_fifo();
						//mergear_libres_contiguas();
						mergear_libres_contiguas_v2();
						break;

					case LRU:
						eliminar_por_LRU();
						//mergear_libres_contiguas();
						mergear_libres_contiguas_v2();
						break;
					}

					bool hay_usadas(metadata_t* metadata){
						if(!metadata->libre) return true;
						else return false;
					}

					if(!list_any_satisfy(lista_de_particiones, (void*)hay_usadas)){
						todas_eliminadas = true;
					}
				}
			}

			if(i == FRECUENCIA_COMPACTACION)
				compactar();

		}else{
			particion_libre_encontrada = true;
		}
	}

	guardar_en_cache_pdinamicas(metadata, tamanio_requerido, tamanio_original,nuevo_paquete, id_correlativo, cod_cola, id_mensaje);
}
