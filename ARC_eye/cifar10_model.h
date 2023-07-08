/*
* Copyright 2019, Synopsys, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-3-Clause license found in
* the LICENSE file in the root directory of this source tree.
*
*/

//
// Convolutional Neural network model for small pictures classification.
// Based on standart Caffe [1] example for CIFAR-10 [2] dataset
//
// Correspondence of classes:
//  0: airplane
//  1: automobile
//  2: bird
//  3: cat
//  4: deer
//  5: dog
//  6: frog
//  7: horse
//  8: ship
//  9: truck
//


#ifndef _CIFAR10_MODEL_H_
#define _CIFAR10_MODEL_H_

#include <stdint.h>

#include "mli_types.h"

//=============================================
//
// Model interface
//
//=============================================
// Input tensor. To be filled with input image by user befor calling inference function (cifar10_cf_net).
#define IN_POINTS (32 * 32 * 3)
extern mli_tensor * const cifar10_cf_net_input;

// Output tensor for model. Will be filled with probabilities vector by model
#define OUT_POINTS (10)
extern mli_tensor * const cifar10_cf_net_output;

void cifar10_cf_net(void);


//=============================================
//
// Model bit depth configuration
//
//=============================================
#define MODEL_FX_8       (8)
#define MODEL_FX_16      (16)
#define MODEL_FX_8W16D   (816)

//#define MODEL_BIT_DEPTH (MODEL_FX_16)

typedef int16_t d_type;
#endif

