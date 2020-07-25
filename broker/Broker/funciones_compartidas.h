/*
 * funciones_compartidas.h
 *
 *  Created on: 15 abr. 2020
 *      Author: utnso
 */

#ifndef FUNCIONES_COMPARTIDAS_H_
#define FUNCIONES_COMPARTIDAS_H_

#include <standard_libraries.h>
#include "tads_y_globales.h"

void loggear_mensaje(char* mensaje, int flag);
uint32_t generar_id_mensaje();
uint32_t generar_id_correlativo();
uint32_t get_timestamp();
void agregar_suscriptor(int cola_requerida, char* ip_cliente, int puerto_cliente);
uint32_t potencia(uint32_t base, uint32_t exponente);
void encolar_y_enviar_mensajes(void* nuevo_paquete, int nuevo_peso, int posicion_en_lista, uint32_t id_mensaje, int cod_lista);
void revisar_send_y_esperar_ack(void*);
void guardar_en_cache(void* nuevo_paquete, int peso_total, mensaje_t* mensaje);
void actualizar_posiciones();
uint32_t obtener_offset_en_cache(int particion_a_usar);
void forwardear_mensajes(uint32_t cola_requerida, long id_cliente);
void sacar_mensaje_de_cola(int id_cola, uint32_t id_mensaje);
void desconectar_cliente(cliente_t* cliente, char* cod_cola, int flag);
cliente_t* obtener_cliente_por_id(long id_cliente);
void disconnect(char* cod_cola, long id);
void* armar_paquete_cache(void* paquete_completo, int peso_total, int long_sin_endchar, int cola);

#endif /* FUNCIONES_COMPARTIDAS_H_ */
