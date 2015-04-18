/* 
 * File:   include.h
 * Author: Tae
 *
 * Created on April 17, 2015, 2:41 PM
 */

#ifndef INCLUDE_H
#define	INCLUDE_H

/* Device header file */
#if defined(__XC16__)
    #include <xc.h>
#endif


#include <stddef.h>	// definition of NULL
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include <libpic30.h>   // for delays

#endif	/* INCLUDE_H */

