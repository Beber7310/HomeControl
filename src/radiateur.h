/*
 * radiateur.h
 *
 *  Created on: 16 d�c. 2013
 *      Author: Bertrand
 */

#ifndef RADIATEUR_H_
#define RADIATEUR_H_

void radiateur_init(void);
void radiateur_init_pgm(int rad);

void * radiateur_loop(void * arg);

#endif /* RADIATEUR_H_ */
