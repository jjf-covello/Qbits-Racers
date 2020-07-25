
#include "algoritmos_de_planificacion.h"

void planificar(){

	if(!strcmp(ALGORITMO_PLANIFICACION,"FIFO")){
		FIFO();
	}else if (!strcmp(ALGORITMO_PLANIFICACION,"RR")){
		RR();
	}else if (!strcmp(ALGORITMO_PLANIFICACION,"SJF-CD")){
		SJF_con_desalojo();
	}else if (!strcmp(ALGORITMO_PLANIFICACION,"SJF-SD")){
		SJF_sin_desalojo();
	}
}

void FIFO(){

	while(estoy_planificando){

		/*sem_wait(&SEM_TRANSICION);
		bool hay_alguien_en_ready = queue_is_empty(COLA_READY);
		sem_post(&SEM_TRANSICION);*/

		sem_wait(&SEM_TRANSICION);
		entrenador_t* entrenador = queue_peek(COLA_READY);
		sem_post(&SEM_TRANSICION);


		if(entrenador != NULL){

			/*sem_wait(&SEM_TRANSICION);
			entrenador_t* entrenador = queue_peek(COLA_READY);
			sem_post(&SEM_TRANSICION);*/

			sem_wait(&SEM_LOGS);
			log_info(log_obligatorio,"El entrenador %d le toca entrar a la cola EXEC por el algoritmo %s",entrenador->id_entrenador,ALGORITMO_PLANIFICACION);
			sem_post(&SEM_LOGS);


			pasar_de_cola_a(entrenador,EXEC);

			sem_wait(&SEM_LOG_METRICAS);
			log_info(log_metricas,"Cambio de contexto para que el entrenador %d use la cpu",entrenador->id_entrenador);
			sem_post(&SEM_LOG_METRICAS);

			cantidad_de_cambios_de_contexto++;
				/*
				sem_wait(&entrenador->cambio_de_estado);
				bool sigo_en_exec=(entrenador->estado_actual==EXEC);
				sem_post(&entrenador->cambio_de_estado);
				*/

			while((entrenador->estado_actual==EXEC)/*sigo_en_exec*/){
				sleep(RETARDO_CICLO_CPU);
				//sem_wait(&entrenador->semaforo_puedo_hacer_otra_rafaga);
				sem_post(&entrenador->semaforo_entrenador);
				/*
				sem_wait(&entrenador->cambio_de_estado);
				sigo_en_exec=(entrenador->estado_actual==EXEC);
				sem_post(&entrenador->cambio_de_estado);
				*/
			}

			sem_wait(&SEM_LOG_METRICAS);
			log_info(log_metricas,"Cambio de contexto para que el entrenador %d deje la cpu",entrenador->id_entrenador);
			sem_post(&SEM_LOG_METRICAS);

			cantidad_de_cambios_de_contexto++;
		}else{
		sleep(RETARDO_CICLO_CPU);
		}
	}
}

void RR(){

	while(estoy_planificando){

		/*sem_wait(&SEM_TRANSICION);
		bool hay_alguien_en_ready = queue_is_empty(COLA_READY);
		sem_post(&SEM_TRANSICION);*/

		sem_wait(&SEM_TRANSICION);
		entrenador_t* entrenador = queue_peek(COLA_READY);
		sem_post(&SEM_TRANSICION);

		if(entrenador != NULL){
			pasar_de_cola_a(entrenador,EXEC);


			sem_wait(&SEM_LOG_METRICAS);
			log_info(log_metricas,"Cambio de contexto para que el entrenador %d use la cpu",entrenador->id_entrenador);
			sem_post(&SEM_LOG_METRICAS);
			cantidad_de_cambios_de_contexto++;

			sem_wait(&SEM_LOGS);
			log_info(log_obligatorio,"El entrenador %d le toca entrar a la cola EXEC por el algoritmo %s",entrenador->id_entrenador,ALGORITMO_PLANIFICACION);
			sem_post(&SEM_LOGS);

			int i = 0;
			while((i<QUANTUM && entrenador->estado_actual==EXEC)){
				sleep(RETARDO_CICLO_CPU);
				//sem_wait(&entrenador->semaforo_puedo_hacer_otra_rafaga);
				sem_post(&entrenador->semaforo_entrenador);
				i++;
			}

			bool el_entrenador_esta_en_exec = entrenador->estado_actual==EXEC;

			if(el_entrenador_esta_en_exec){
				sem_wait(&SEM_LOGS);
				log_info(log_obligatorio,"El entrenador %d le toca entrar a la cola READY por el algoritmo %s ya que lo desalojo",entrenador->id_entrenador,ALGORITMO_PLANIFICACION);
				sem_post(&SEM_LOGS);

				pasar_de_cola_a(entrenador,READY);

				//sem_post(&entrenador->semaforo_entre_en_ready);
			}

			sem_wait(&SEM_LOG_METRICAS);
			log_info(log_metricas,"Cambio de contexto para que el entrenador %d deje la cpu",entrenador->id_entrenador);
			sem_post(&SEM_LOG_METRICAS);
			cantidad_de_cambios_de_contexto++;
		}else{
		sleep(RETARDO_CICLO_CPU);
		}
	}
}

entrenador_t* entrenador_con_menor_estimacion(){

	bool aplicar_sjf(entrenador_t* un_entrenador, entrenador_t* otro_entrenador){
		long una_estimacion, otra_estimacion;
		una_estimacion = (1-ALPHA_SJF)*un_entrenador->estimacion_actual + ALPHA_SJF * un_entrenador->estimacion_anterior;
		otra_estimacion = (1-ALPHA_SJF)*otro_entrenador->estimacion_actual + ALPHA_SJF * otro_entrenador->estimacion_anterior;
		return una_estimacion<otra_estimacion;
	}
	list_sort(COLA_READY->elements,aplicar_sjf);

	entrenador_t* entrenador_a_retornar = queue_peek(COLA_READY);

	return entrenador_a_retornar;
}

void SJF_con_desalojo(){

	while(estoy_planificando){

		sem_wait(&SEM_TRANSICION);
		bool hay_alguien_en_ready = queue_is_empty(COLA_READY);
		sem_post(&SEM_TRANSICION);

		if(!hay_alguien_en_ready){

			sem_wait(&SEM_TRANSICION);
			int cant_de_procesos_en_ready = queue_size(COLA_READY);
			entrenador_t* entrenador = entrenador_con_menor_estimacion();
			sem_post(&SEM_TRANSICION);

			sem_wait(&SEM_LOGS);
			log_info(log_obligatorio,"El entrenador %d le toca entrar a la cola EXEC por el algoritmo %s",entrenador->id_entrenador,ALGORITMO_PLANIFICACION);
			sem_post(&SEM_LOGS);

			pasar_de_cola_a(entrenador,EXEC);


			sem_wait(&SEM_LOG_METRICAS);
			log_info(log_metricas,"Cambio de contexto para que el entrenador %d use la cpu",entrenador->id_entrenador);
			sem_post(&SEM_LOG_METRICAS);
			cantidad_de_cambios_de_contexto++;

			int cant_rafagas = 0;
			int cant_actual_procesos_en_ready = queue_size(COLA_READY);

			sem_wait(&SEM_TRANSICION);
			bool no_entro_alguien_mas_en_ready=cant_actual_procesos_en_ready == queue_size(COLA_READY);
			sem_post(&SEM_TRANSICION);
			sem_wait(&entrenador->cambio_de_estado);
			bool sigo_en_exec=(entrenador->estado_actual==EXEC);
			sem_post(&entrenador->cambio_de_estado);

			while(sigo_en_exec && no_entro_alguien_mas_en_ready){
				sleep(RETARDO_CICLO_CPU);
				//sem_wait(&entrenador->semaforo_puedo_hacer_otra_rafaga);
				sem_post(&entrenador->semaforo_entrenador);
				cant_rafagas++;

				sem_wait(&SEM_TRANSICION);
				no_entro_alguien_mas_en_ready = cant_actual_procesos_en_ready == queue_size(COLA_READY);/*this unmutexear*/
				sem_post(&SEM_TRANSICION);
				
				sem_wait(&entrenador->cambio_de_estado);
				sigo_en_exec = (entrenador->estado_actual==EXEC);
				sem_post(&entrenador->cambio_de_estado);
			}
			//Actualizamos las estimaciones para el SJF
			entrenador->estimacion_anterior = entrenador->estimacion_actual;
			entrenador->estimacion_actual = cant_rafagas;

			//cantidad_de_ciclos_cpu_total+=cant_rafagas;

			//bool el_entrenador_esta_en_exec = entrenador->estado_actual==EXEC;

			if(sigo_en_exec){
				sem_wait(&SEM_LOGS);
				log_info(log_obligatorio,"El entrenador %d le toca entrar a la cola READY por el algoritmo %s ya que lo desalojo",entrenador->id_entrenador,ALGORITMO_PLANIFICACION);
				sem_post(&SEM_LOGS);
				pasar_de_cola_a(entrenador,READY);
			}							
			sem_wait(&SEM_LOG_METRICAS);
			log_info(log_metricas,"Cambio de contexto para que el entrenador %d deje la cpu",entrenador->id_entrenador);
			sem_post(&SEM_LOG_METRICAS);
			cantidad_de_cambios_de_contexto++;
		}else{
		sleep(RETARDO_CICLO_CPU);
		}
	}
}

void SJF_sin_desalojo(){

	while(estoy_planificando){

		sem_wait(&SEM_TRANSICION);
		bool hay_alguien_en_ready = queue_is_empty(COLA_READY);
		sem_post(&SEM_TRANSICION);

		if(!hay_alguien_en_ready){

			sem_wait(&SEM_TRANSICION);
			entrenador_t* entrenador = entrenador_con_menor_estimacion();
			sem_post(&SEM_TRANSICION);

			sem_wait(&SEM_LOGS);
			log_info(log_obligatorio,"El entrenador %d le toca entrar a la cola EXEC por el algoritmo %s",entrenador->id_entrenador,ALGORITMO_PLANIFICACION);
			sem_post(&SEM_LOGS);

			pasar_de_cola_a(entrenador,EXEC);

			sem_wait(&SEM_LOG_METRICAS);
			log_info(log_metricas,"Cambio de contexto para que el entrenador %d use la cpu",entrenador->id_entrenador);
			sem_post(&SEM_LOG_METRICAS);
			cantidad_de_cambios_de_contexto++;

			int cant_rafagas = 0;
			sem_wait(&entrenador->cambio_de_estado);
			bool sigo_en_exec=(entrenador->estado_actual==EXEC);
			sem_post(&entrenador->cambio_de_estado);
			while(sigo_en_exec){
				sleep(RETARDO_CICLO_CPU);
				//sem_wait(&entrenador->semaforo_puedo_hacer_otra_rafaga);
				sem_post(&entrenador->semaforo_entrenador);
				cant_rafagas++;
				sem_wait(&entrenador->cambio_de_estado);
				sigo_en_exec=(entrenador->estado_actual==EXEC);//TODO esto lo cambie desde git jejox
				sem_post(&entrenador->cambio_de_estado);
			}
			//Actualizamos las estimaciones para el SJF
			entrenador->estimacion_anterior = entrenador->estimacion_actual;
			entrenador->estimacion_actual = cant_rafagas;

			//cantidad_de_ciclos_cpu_total+=cant_rafagas;

			sem_wait(&SEM_LOG_METRICAS);
			log_info(log_metricas,"Cambio de contexto para que el entrenador %d deje la cpu",entrenador->id_entrenador);
			sem_post(&SEM_LOG_METRICAS);
			cantidad_de_cambios_de_contexto++;
		}else{
		sleep(RETARDO_CICLO_CPU);
		}
	}
}
