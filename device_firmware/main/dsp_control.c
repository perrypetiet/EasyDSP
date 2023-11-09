/*
 * dsp_control.c
 *
 * Created: 8-11-2023 
 * Author: Perry Petiet
 * 
 * This module is used to control the DSP on a high level.
 * It initializes the sigma_dsp module and contains functions to
 * calculate the correct parameter data for eq's and muxes. the
 * functions take own defined datatypes and send the data to program
 * the sigma dsp to the sigma_dsp module which then uses the safeload
 * write functionality of the dsp chip.
 * 
 */ 
 /******************************* INCLUDES ********************************/

 #include "dsp_control.h"

 /******************************* GLOBAL VARIABLES ************************/

 static const char *TAG = "DSP_control";

 /******************************* LOCAL FUNCTIONS *************************/

 /******************************* GLOBAL FUNCTIONS ************************/

 bool init_dsp_control()
 {
    if(init_sigma_dsp(ADA_I2C_SCL_PIN, 
                      ADA_I2C_SDA_PIN, 
                      ADA_I2C_PORT_NUM, 
                      ADA_I2C_ADDRESS, 
                      ADA_GPIO_RESET) == SIGMA_DSP_INIT_SUCCESS)
    {
        return true;
    }
    return false;
 }

bool deinit_dsp_control()
{
    if(deinit_sigma_dsp() == SIGMA_DSP_DEINIT_SUCCESS)
    {
        return true;
    }
    return false;
}

bool dsp_control_mux(uint16_t sigma_dsp_address, mux_t *mux)
{
    printf("Writing index: %d\n", mux->index);

    uint8_t data[ADA_PARAM_REG_SIZE] = {0, 0, 0, 0};
    data[ADA_PARAM_REG_SIZE - 1] = mux->index;

    if(sigma_dsp_write_burst(sigma_dsp_address,
                             sizeof(data),
                             data) == SIGMA_DSP_WRITE_SUCCESS)
    {
        return true;
    }
    return false;
}

bool dsp_control_eq_secondorder(uint16_t sigma_dsp_address,
                                equalizer_t *eq)
{
    float A;
    float w0;
    float alpha;
    float gainLinear;
    float b0;
    float b1;
    float b2;
    float a0;
    float a1;
    float a2;
    float coefficients[ADA_COEFFICIENT_AMOUNT];

    A=pow(10,(eq->boost/40));           // 10^(boost/40)
    w0=2*M_PI*eq->freq/ADA_SAMPLE_FREQ; // 2*PI*freq/FS
    gainLinear = pow(10,(eq->gain/20)); // 10^(gain/20)

    switch(eq->filter_type)
    {
        case FILTER_TYPE_PEAK:
            alpha = sin(w0)/(2*eq->q);
            a0    =  1 + alpha/A;
            a1    = -2 * cos(w0);
            a2    =  1 - alpha/A;
            b0    = (1 + alpha*A) * gainLinear;
            b1    = -(2 * cos(w0)) * gainLinear;
            b2    = (1 - alpha*A) * gainLinear;
            break;
        case FILTER_TYPE_LOWSHELF:
            alpha = sin(w0)/2*sqrt((A+1/A)*(1/eq->s-1)+2);
            a0    = (A+1) + (A-1)*cos(w0) + 2*sqrt(A)*alpha;
            a1    = -2*((A-1) + (A+1)*cos(w0));
            a2    = (A+1) + (A-1)*cos(w0) - 2*sqrt(A)*alpha;
            b0    = A*((A+1) - (A-1)*cos(w0) + 2*sqrt(A)*alpha) * gainLinear;
            b1    = 2*A*((A-1) - (A+1)*cos(w0)) * gainLinear;
            b2    = A*((A+1) - (A-1)*cos(w0) - 2*sqrt(A)*alpha) * gainLinear;
            break;
        case FILTER_TYPE_HIGHSHELF:
            alpha = sin(w0)/2*sqrt((A+1/A)*(1/eq->s-1)+2);
            a0    = (A+1) - (A-1)*cos(w0) + 2*sqrt(A)*alpha;
            a1    = 2*((A-1) - (A+1)*cos(w0));
            a2    = (A+1) - (A-1)*cos(w0) - 2*sqrt(A)*alpha;
            b0    = A*((A+1) + (A-1)*cos(w0) + 2*sqrt(A)*alpha) * gainLinear;
            b1    = -2*A*((A-1) + (A+1)*cos(w0)) * gainLinear;
            b2    = A*((A+1) + (A-1)*cos(w0) - 2*sqrt(A)*alpha) * gainLinear;
            break;
        case FILTER_TYPE_LOWPASS:
            alpha = sin(w0)/(2*eq->q);
            a0    = 1 + alpha;
            a1    = -2*cos(w0);
            a2    = 1 - alpha;
            b0    = (1 - cos(w0)) * (gainLinear/2);
            b1    = (1 - cos(w0))  * gainLinear;
            b2    = (1 - cos(w0)) * (gainLinear/2);
            break;
        case FILTER_TYPE_HIGHPASS:
            alpha = sin(w0)/(2*eq->q);
            a0    = 1 + alpha;
            a1    = -2*cos(w0);
            a2    = 1 - alpha;
            b0    = (1 + cos(w0)) * (gainLinear/2);
            b1    = -(1 + cos(w0)) * gainLinear;
            b2    = (1 + cos(w0)) * (gainLinear/2);
            break;
        case FILTER_TYPE_BANDPASS:
            alpha = sin(w0) * sinh(log(2)/(2 * eq->bandwidth * w0/sin(w0)));
            a0    = 1 + alpha;
            a1    = -2*cos(w0);
            a2    = 1 - alpha;
            b0    = alpha * gainLinear;
            b1    = 0;
            b2    = -alpha * gainLinear;
            break;
        case FILTER_TYPE_BANDSTOP:
            alpha = sin(w0) * sinh(log(2)/(2 * eq->bandwidth * w0/sin(w0)));
            a0    = 1 + alpha;
            a1    = -2*cos(w0);
            a2    = 1 - alpha;
            b0    = 1 * gainLinear;
            b1    = -2*cos(w0) * gainLinear;
            b2    = 1 * gainLinear;
            break;
        default:
            // Peak:
            alpha = sin(w0)/(2*eq->q);
            a0    =  1 + alpha/A;
            a1    = -2 * cos(w0);
            a2    =  1 - alpha/A;
            b0    = (1 + alpha*A) * gainLinear;
            b1    = -(2 * cos(w0)) * gainLinear;
            b2    = (1 - alpha*A) * gainLinear;
            break;
    }
    // For Sigma DSP implementation we need to normalize all the coefficients respect to a0
    // and inverting by sign a1 and a2

    if(a0 != 0.00 && eq->state == STATE_ON)
    {
        if(eq->phase == PHASE_NON_INVERTED)
        {
            coefficients[0] = b0/a0;
            coefficients[1] = b1/a0;
            coefficients[2] = b2/a0;
            coefficients[3] = -1*a1/a0;
            coefficients[4] = -1*a2/a0;
        }
        else
        {
            coefficients[0] = -1*b0/a0;
            coefficients[1] = -1*b1/a0;
            coefficients[2] = -1*b2/a0;
            coefficients[3] = -1*a1/a0; 
            coefficients[4] = -1*a2/a0;
        }
    }
    else // (eq->state == STATE_OFF)
    {
        coefficients[0] = 1.00;
        coefficients[1] = 0;
        coefficients[2] = 0;
        coefficients[3] = 0;
        coefficients[4] = 0;
    }
    // I need to conver the floats to uint8, each float is 32bit(4 ints)
    uint8_t data[ADA_COEFFICIENT_AMOUNT * 4];

    for(int i = 0; i < ADA_COEFFICIENT_AMOUNT; i++)
    {
        int32_t fixedval = (coefficients[i] * ((int32_t)1 << 23));

        data[i * 4 + 0] = (fixedval >> 24) & 0xFF;
        data[i * 4 + 1] = (fixedval >> 16) & 0xFF;
        data[i * 4 + 2] = (fixedval >>  8) & 0xFF;
        data[i * 4 + 3] = fixedval & 0xFF;
    }

    if(sigma_dsp_write_burst(sigma_dsp_address,
                             sizeof(data),
                             data) == SIGMA_DSP_WRITE_SUCCESS)
    {
        ESP_LOGI(TAG, "EQ write success.");
        return true;
    }
    ESP_LOGW(TAG, "EQ write failed.");
    return false;
}

 /******************************* THE END *********************************/