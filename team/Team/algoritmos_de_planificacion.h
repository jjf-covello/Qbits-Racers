
#ifndef ALGORITMOS_DE_PLANIFICACION_H_
#define ALGORITMOS_DE_PLANIFICACION_H_

#include "globales.h"

// PROTOTIPOS

void planificar();
void FIFO();
void RR();
void SJF_con_desalojo();
void SJF_sin_desalojo();
entrenador_t* entrenador_con_menor_estimacion();

#endif /* ALGORITMOS_DE_PLANIFICACION_H_ */
