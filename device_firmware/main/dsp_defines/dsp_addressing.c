/*
 * sigma_adressing.c
 *
 * Created: 3-12-2024 
 * Author: Perry Petiet
 * 
 * This module is used to convert the header file created by the
 * DSP_parameter_generator.ps1 to a structure that contains the addresses
 * in a way directly used by the settings module to get the correct
 * sigma dsp addresses when sending data to the dsp_control module.
 * 
 */
/******************************* INCLUDES ********************************/

#include "dsp_addressing.h"

/******************************* GLOBAL VARIABLES ************************/

const sigma_dsp_addresses_t dsp_addresses = 
{
  .input_addresses[0] =
  {
    .eq_addresses = 
    {
      MOD_INPUT1_EQ_ALG0_STAGE0_B0_ADDR,
      MOD_INPUT1_EQ_ALG0_STAGE1_B0_ADDR,
      MOD_INPUT1_EQ_ALG0_STAGE2_B0_ADDR,
      MOD_INPUT1_EQ_ALG0_STAGE3_B0_ADDR,
      MOD_INPUT1_EQ_ALG0_STAGE4_B0_ADDR
    }
  },
  .input_addresses[1] = 
  {
    .eq_addresses = 
    {
      MOD_INPUT2_EQ_ALG0_STAGE0_B0_ADDR,
      MOD_INPUT2_EQ_ALG0_STAGE1_B0_ADDR,
      MOD_INPUT2_EQ_ALG0_STAGE2_B0_ADDR,
      MOD_INPUT2_EQ_ALG0_STAGE3_B0_ADDR,
      MOD_INPUT2_EQ_ALG0_STAGE4_B0_ADDR
    }
  },

  .output_addresses[0] = 
  {
    .mux = MOD_OUTPUT1_SELECT_MONOSWSLEW_ADDR,
    .eq_addresses = 
    {
      MOD_OUTPUT1_EQ_ALG0_STAGE0_B0_ADDR,
      MOD_OUTPUT1_EQ_ALG0_STAGE1_B0_ADDR,
      MOD_OUTPUT1_EQ_ALG0_STAGE2_B0_ADDR,
      MOD_OUTPUT1_EQ_ALG0_STAGE3_B0_ADDR,
      MOD_OUTPUT1_EQ_ALG0_STAGE4_B0_ADDR
    }
  },
  .output_addresses[1] = 
  {
    .mux = MOD_OUTPUT2_SELECT_MONOSWSLEW_ADDR,
    .eq_addresses = 
    {
      MOD_OUTPUT2_EQ_ALG0_STAGE0_B0_ADDR,
      MOD_OUTPUT2_EQ_ALG0_STAGE1_B0_ADDR,
      MOD_OUTPUT2_EQ_ALG0_STAGE2_B0_ADDR,
      MOD_OUTPUT2_EQ_ALG0_STAGE3_B0_ADDR,
      MOD_OUTPUT2_EQ_ALG0_STAGE4_B0_ADDR
    }
  },
  .output_addresses[2] = 
  {
    .mux = MOD_OUTPUT3_SELECT_MONOSWSLEW_ADDR,
    .eq_addresses = 
    {
      MOD_OUTPUT3_EQ_ALG0_STAGE0_B0_ADDR,
      MOD_OUTPUT3_EQ_ALG0_STAGE1_B0_ADDR,
      MOD_OUTPUT3_EQ_ALG0_STAGE2_B0_ADDR,
      MOD_OUTPUT3_EQ_ALG0_STAGE3_B0_ADDR,
      MOD_OUTPUT3_EQ_ALG0_STAGE4_B0_ADDR
    }
  },
  .output_addresses[3] = 
  {
    .mux = MOD_OUTPUT4_SELECT_MONOSWSLEW_ADDR,
    .eq_addresses = 
    {
      MOD_OUTPUT4_EQ_ALG0_STAGE0_B0_ADDR,
      MOD_OUTPUT4_EQ_ALG0_STAGE1_B0_ADDR,
      MOD_OUTPUT4_EQ_ALG0_STAGE2_B0_ADDR,
      MOD_OUTPUT4_EQ_ALG0_STAGE3_B0_ADDR,
      MOD_OUTPUT4_EQ_ALG0_STAGE4_B0_ADDR
    }
  }
};
/******************************* LOCAL FUNCTIONS *************************/

/******************************* GLOBAL FUNCTIONS ************************/