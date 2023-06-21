#include <stdlib.h>
#include <math.h>
#include "embARC.h"
#include <stdio.h>
#include "mli_api.h"

#include "cifar10_ref_inout.h"
#include "cifar10_model.h"

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


//================================================================================
// Transform MLI FX tensor to float array
//=================================================================================
static mli_status mli_hlp_fx_tensor_to_float (const mli_tensor * src, float *dst, uint32_t dst_size) {
	uint32_t elem_num = mli_hlp_count_elem_num(src, 0);
	if (elem_num > dst_size)
		return MLI_STATUS_LENGTH_ERROR;
	if (elem_num == 0)
		return MLI_STATUS_BAD_TENSOR;

	const float scale_val = 1.0f / (float) (1u << (src->el_params.fx.frac_bits));
	if (src->el_type == MLI_EL_FX_16) {
		int16_t *src_arr = src->data;
		for (int idx = 0; idx < elem_num; idx++)
			dst[idx] = (float) (scale_val * src_arr[idx]);
	} else {
		int8_t *src_arr = src->data;
		for (int idx = 0; idx < elem_num; idx++)
			dst[idx] = (float) (scale_val * src_arr[idx]);
	}
	return MLI_STATUS_OK;
}

//========================================================================================
// Image pre-processing for CIFAR-10 net
//========================================================================================
static void cifar10_preprocessing(const void * image_, mli_tensor * net_input_) {
	const unsigned char * in = image_;
	d_type * const dst = (d_type * const)net_input_->data;

	for (int idx = 0; idx < IN_POINTS; idx++){
		dst[idx] = (d_type)((int)in[idx] - 128);
	}
}

uint8_t net_process(void * kSingleIn) {

	size_t output_elements = mli_hlp_count_elem_num(cifar10_cf_net_output, 0);
	float * pred_data = malloc(output_elements * sizeof(float));
	cifar10_preprocessing(kSingleIn, cifar10_cf_net_input);
	uint8_t maxid = 0;
	cifar10_cf_net();

	if (MLI_STATUS_OK == mli_hlp_fx_tensor_to_float(cifar10_cf_net_output, pred_data, output_elements)) 
	{
		uint8_t temp[10];
		for(int i=0;i<output_elements;i++){
			temp[i] = (uint8_t)(pred_data[i] * 256);
			printf("pred_data[%d]: %d\n", i, temp[i]);
		}
			
		for(int i=1;i<output_elements;i++)   /* compare */
		{
			if(temp[maxid] < temp[i])
				maxid = i;
			
		}
	}
	return maxid;
}