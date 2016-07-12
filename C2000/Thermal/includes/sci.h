/*
 * sci.h
 *
 *  Created on: Jul 1, 2016
 *      Author: Twi
 */

#ifndef INCLUDES_SCI_H_
#define INCLUDES_SCI_H_

void sci_init();
void sci_set_callback(void (*callback)(char c));

void SCI_putStringBlocking(char *msg);
void SCI_putByteBlocking(uint16_t data);

#endif /* INCLUDES_SCI_H_ */
