/*
 * buddy_system.c
 *
 *  Created on: 15 abr. 2020
 *      Author: utnso
 */

#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include "tads_y_globales.h"
#include <commons/collections/list.h>
#include "funciones_compartidas.h"
#include <standard_libraries.h>
#include <commons/string.h>

uint32_t tamanio_particion_necesario_bs(uint32_t espacio_reservado) {
	//Te da el tamanio de particion que vas a usar del buddy, por mas que tu numero sea mas chico
	//porque tiene que ser potencia de 2.
	uint32_t tamanio_necesario = 0;
	int exponente = 0;

	while(tamanio_necesario < espacio_reservado) {
		tamanio_necesario = potencia(2, exponente);
		exponente++;
	}
	return tamanio_necesario;
}

int obtener_particion_libre_bs(uint32_t espacio_requerido) {
	//Hay que ir revisando las libres y eventualmente ir dividiendo por 2.
	//Devuelve el numero de particion, en caso de no encontrar devuelve -1.
	int particion_a_utilizar = -1;
	bool se_encontro_particion = false;
	int iterador = 0;

	sem_wait(&sem_lista_particiones);
	while(!se_encontro_particion && iterador < lista_de_particiones->elements_count) {
		metadata_t* una_md = list_get(lista_de_particiones, iterador);
		if(una_md->libre && una_md->tamanio_ocupado >= espacio_requerido) {
			particion_a_utilizar = iterador;
			se_encontro_particion = true;
		}
		iterador++;
	}
	sem_post(&sem_lista_particiones);
	return particion_a_utilizar;
}

void achicar_particion_libre_bs(int particion_a_utilizar, uint32_t espacio_requerido) {

	//Le pasas la particion que te corresponde, cuanto queres y hace el laburo de subdividir tutti lo que se pueda
	sem_wait(&sem_lista_particiones);
	metadata_t* una_md = list_get(lista_de_particiones, particion_a_utilizar);
	sem_post(&sem_lista_particiones);

	if(una_md->tamanio_ocupado / 2 >= espacio_requerido && una_md->tamanio_ocupado / 2 >= TAMANO_MINIMO_PARTICION) {
		sem_wait(&sem_lista_particiones);
		//Se setea la nueva metadata
		metadata_t* nueva_md = (metadata_t*) malloc(sizeof(metadata_t));
		nueva_md->codigo_de_cola = una_md->codigo_de_cola;
		nueva_md->id_mensaje_contenido = -1;
		nueva_md->instante_entrada = get_timestamp();
		nueva_md->libre = true;
		nueva_md->posicion_en_lista = una_md->posicion_en_lista + 1;
		nueva_md->tamanio_ocupado = una_md->tamanio_ocupado / 2;
		nueva_md->tamanio_reservado = una_md->tamanio_ocupado / 2;
		nueva_md->timestamp_LRU = get_timestamp();

		//Updateas la que ya habia
		una_md->tamanio_ocupado = una_md->tamanio_ocupado / 2;
		una_md->tamanio_reservado = una_md->tamanio_ocupado / 2;

		//Reemplazamos de manera iterativa para meter algo en el medio
		for(int i = particion_a_utilizar + 1; i < lista_de_particiones->elements_count; i++) {
			//Meto la nueva y la que saco me queda en la misma variable
			nueva_md = list_replace(lista_de_particiones, i, nueva_md);
			nueva_md->posicion_en_lista = nueva_md->posicion_en_lista + 1;
		}
		list_add(lista_de_particiones, nueva_md); //Agrego la que me quedo al final de tutti
		sem_post(&sem_lista_particiones);

		actualizar_posiciones();
	}
	//Si se puede seguir achicando, llamo otra vez recursivamente
	if(una_md->tamanio_ocupado / 2 >= espacio_requerido && una_md->tamanio_ocupado / 2 >= TAMANO_MINIMO_PARTICION)
		achicar_particion_libre_bs(particion_a_utilizar, espacio_requerido);
}

void mergear_metadatas_bs() {
	//Chiflas a esta funcion para que junte las metadatas segun el criterio de buddy system
	//1. Chequea una a una las longitudes adyacentes para ver si hay algo para juntar
	//2. Numera de nuevo los items por si se elimino algo, para que sea consistente
	bool hubo_merge = true;

	sem_wait(&sem_lista_particiones);
	//1.
	while(hubo_merge) {
		hubo_merge = false;
		for(int i = 0; i + 1 < lista_de_particiones->elements_count; i++) {
			metadata_t* md_primera = list_get(lista_de_particiones, i);
			metadata_t* md_siguiente = list_get(lista_de_particiones, i+1);

			if(md_primera->tamanio_ocupado == md_siguiente->tamanio_ocupado && md_primera->libre && md_siguiente->libre) {
				md_primera->tamanio_ocupado = md_primera->tamanio_ocupado * 2;

				sem_post(&sem_lista_particiones);
				uint32_t offset1 = obtener_offset_en_cache(md_primera->posicion_en_lista);
				uint32_t offset2 = obtener_offset_en_cache(md_siguiente->posicion_en_lista);
				sem_wait(&sem_lista_particiones);
				//char* num1 = string_itoa(cache_broker + offset1);
				//char* num2 = string_itoa(cache_broker + offset2);
				char* num1 = (char*)malloc(20);
				memset(num1, '\0', 20);
				sprintf(num1, "%p", cache_broker + offset1);
				char* num2 = (char*)malloc(20);
				memset(num2, '\0', 20);
				sprintf(num2, "%p", cache_broker + offset2);
				char* msj = (char*)malloc(strlen("Bloques asociados. Inicio 1: ")+strlen(num1)+strlen(" - Inicio 2: ")+strlen(num2));
				memset(msj, '\0', strlen("Bloques asociados. Inicio 1: ")+strlen(num1)+strlen(" - Inicio 2: ")+strlen(num2));
				memcpy(msj, "Bloques asociados. Inicio 1: ", strlen("Bloques asociados. Inicio 1: "));
				string_append(&msj, num1);
				string_append(&msj, " - Inicio 2: ");
				string_append(&msj, num2);
				loggear_mensaje(msj, 1);
				free(num1);
				free(num2);
				free(msj);

				metadata_t* md_a_borrar = list_remove(lista_de_particiones, i + 1);
				free(md_a_borrar);
				hubo_merge = true;

				//Hago lo mismo que en actualizar_posiciones() pero sin los semaforos
				for(int i = 0; i < lista_de_particiones->elements_count; i++) {
					metadata_t* md_primera = list_get(lista_de_particiones, i);
					md_primera->posicion_en_lista = i;
				}
			}
		}
	}
	sem_post(&sem_lista_particiones);

	//2.
	actualizar_posiciones();
}

void liberar_particion_bs() {

	bool sort_FIFO(metadata_t* metadata_mas_vieja, metadata_t* metadata){
		return metadata_mas_vieja->instante_entrada < metadata->instante_entrada;
	}

	bool sort_LRU(metadata_t* metadata_mas_vieja, metadata_t* metadata){
		return metadata_mas_vieja->timestamp_LRU < metadata->timestamp_LRU;
	}

	t_list* particiones_ordenadas_por_alg;
	//La lista se duplica, los elementos no

	sem_wait(&sem_lista_particiones);
	if(COD_ALGORITMO_REEMPLAZO == FIFO) {
		particiones_ordenadas_por_alg = list_sorted(lista_de_particiones,(void*)sort_FIFO);
	} else {
		particiones_ordenadas_por_alg = list_sorted(lista_de_particiones,(void*)sort_LRU);
	}

	int iterador = 0;
	metadata_t* md_a_liberar = list_get(particiones_ordenadas_por_alg, iterador);
	while(md_a_liberar->libre) {
		iterador++;
		md_a_liberar = list_get(particiones_ordenadas_por_alg, iterador);
	}
	list_destroy(particiones_ordenadas_por_alg);
	md_a_liberar->libre = true;
	sem_post(&sem_lista_particiones);

	uint32_t offset = obtener_offset_en_cache(md_a_liberar->posicion_en_lista);
	//char* num = string_itoa(cache_broker + offset);
	char* num = (char*)malloc(20);
	memset(num, '\0', 20);
	sprintf(num, "%p", cache_broker + offset);
	char* msj = (char*)malloc(strlen("Particion liberada. Posicion de inicio: ")+strlen(num));
	memset(msj, '\0', strlen("Particion liberada. Posicion de inicio: ")+strlen(num));
	memcpy(msj, "Particion liberada. Posicion de inicio:  ", strlen("Particion liberada. Posicion de inicio: "));
	string_append(&msj, num);
	loggear_mensaje(msj, 1);
	free(msj);
	free(num);

	//Saco el mensaje de la cola
	sacar_mensaje_de_cola(md_a_liberar->codigo_de_cola, md_a_liberar->id_mensaje_contenido);

	mergear_metadatas_bs();

}

void guardar_en_cache_bs(void* nuevo_paquete, int peso_total, uint32_t id_correlativo, uint32_t cod_cola, uint32_t id_mensaje) {
	int particion_a_utilizar = obtener_particion_libre_bs(peso_total);

	while(particion_a_utilizar < 0) { //Itero aca hasta que consiga una particion libre
		liberar_particion_bs();
		particion_a_utilizar = obtener_particion_libre_bs(peso_total);
	}

	//Encontre una particion libre
	achicar_particion_libre_bs(particion_a_utilizar, peso_total);//La hago lo mas chiquita posible

	sem_wait(&sem_lista_particiones);
	//La agarro y la actualizo
	metadata_t* md_a_usar = list_get(lista_de_particiones, particion_a_utilizar);
	md_a_usar->id_mensaje_contenido = id_mensaje;
	md_a_usar->instante_entrada = get_timestamp();
	md_a_usar->libre = false;
	md_a_usar->tamanio_reservado = peso_total;
	md_a_usar->timestamp_LRU = get_timestamp();
	md_a_usar->codigo_de_cola = cod_cola;
	md_a_usar->id_correlativo = id_correlativo;
	sem_post(&sem_lista_particiones);

	//Escribo posta posta en memoria
	uint32_t offset = obtener_offset_en_cache(particion_a_utilizar);
	memcpy(cache_broker + offset, nuevo_paquete, peso_total);

	char* num1 = string_itoa(id_mensaje);
	//char* num2 = string_itoa(cache_broker + offset);
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
}











