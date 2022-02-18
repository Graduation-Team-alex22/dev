/*
 * adcs_error_handler.h
 *
 *  Created on: Jul 16, 2016
 *      Author: azisi
 */

#ifndef ADCS_ERROR_HANDLER_H_
#define ADCS_ERROR_HANDLER_H_

typedef enum {
    ERROR_OK = 0,
    ERROR_SGP4,
    ERROR_TLE,
    ERROR_TIME,
    ERROR_SENSOR,
    ERROR_ACTUATOR,
    ERROR_FLASH,
    ERROR_GPS,
    ERROR_HAL_INIT,
    ERROR_UNRESOLVED

} adcs_error_status;

extern adcs_error_status error_status;
extern uint8_t trasmit_error_status;

void error_handler(adcs_error_status error);
adcs_error_status error_propagation(adcs_error_status current_error);

#endif /* ADCS_ERROR_HANDLER_H_ */
