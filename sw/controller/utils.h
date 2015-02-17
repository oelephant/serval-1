/* 
 * File:   util.h
 * Author: Coleby
 *
 * Created on December 4, 2014, 12:48 AM
 */

#ifndef UTIL_H
#define	UTIL_H

unsigned int Utils_charsToInt(unsigned char c0, unsigned char c1);
void Utils_intToChars(unsigned int i, unsigned char *c0, unsigned char *c1);
void Utils_longToChars(unsigned int i, unsigned char *c0, unsigned char *c1,\
        unsigned char *c2, unsigned char *c3);

#endif	/* UTIL_H */

