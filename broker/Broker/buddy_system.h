/*
 * buddy_system.h
 *
 *  Created on: 18 abr. 2020
 *      Author: utnso
 */

#ifndef BUDDY_SYSTEM_H_
#define BUDDY_SYSTEM_H_

uint32_t tamanio_particion_necesario_bs();
int obtener_particion_libre_bs(uint32_t espacio_requerido);
void achicar_particion_libre_bs(int particion_a_utilizar, uint32_t espacio_requerido);
void mergear_metadatas_bs();
void guardar_en_cache_bs(void* nuevo_paquete, int peso_total, uint32_t id_correlativo, uint32_t cod_cola, uint32_t id_mensaje);
void liberar_particion_bs();

#endif /* BUDDY_SYSTEM_H_ */
