/*
 * WEARABLE INDOOR LOCALIZATION DEVICE
 * ECE 453, FALL 2014
 *
 * Calvin Hareng
 * Pedro Melgarejo
 * Robert Wagner
 *
 * GEN_LIB.H
 */

#ifndef GEN_LIB_H_
#define GEN_LIB_H_

#include <msp430f2619.h>
#include <stdio.h>
#include "IQmathlib.h"	//optimized math libraries
#include "Qmathlib.h"	//

//data sampling
#define COLLECTION_DURATION 1
#define SAMPLES_PER_DURATION 10

//boolean type
typedef int bool;
#define true 1
#define false 0

#endif /* GEN_LIB_H_ */
