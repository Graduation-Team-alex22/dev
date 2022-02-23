/*
 * upsat-comms-software: Communication Subsystem Software for UPSat satellite
 *
 *  Copyright (C) 2016, Libre Space Foundation <http://librespacefoundation.org/>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INC_LOG_H_
#define INC_LOG_H_

#include "config.h"
#include "services.h"
#include <string.h>

static uint8_t _log_uart_buffer[COMMS_UART_BUF_LEN];
static uint8_t _ecss_dbg_buffer[200];
static size_t _ecss_dbg_buffer_len;

extern uint8_t dbg_msg;

#if COMMS_UART_DBG_EN

#if COMMS_UART_DEST_OBC
#define LOG_UART_DBG(huart, M, ...)					\
	if(dbg_msg == 1 || dbg_msg == 2) { \
	snprintf(_log_uart_buffer, COMMS_UART_BUF_LEN,			\
			"[DEBUG] %s:%d: " M "\n",			\
			 __FILE__, __LINE__, ##__VA_ARGS__);		\
      event_dbg_api (_ecss_dbg_buffer, _log_uart_buffer,		\
                         &_ecss_dbg_buffer_len);	\
      HAL_uart_tx (DBG_APP_ID, _ecss_dbg_buffer, _ecss_dbg_buffer_len);	} \

#define LOG_UART_ERROR(huart, M, ...) 					\
	if(dbg_msg == 1 || dbg_msg == 2) {\
	snprintf(_log_uart_buffer, COMMS_UART_BUF_LEN, 			\
			"[ERROR] %s:%d: " M "\n",			\
			 __FILE__, __LINE__, ##__VA_ARGS__);		\
      event_dbg_api (_ecss_dbg_buffer, _log_uart_buffer, \
      	&_ecss_dbg_buffer_len);		\
      HAL_uart_tx (DBG_APP_ID, _ecss_dbg_buffer, _ecss_dbg_buffer_len);	}\

#else

#define LOG_UART_DBG(huart, M, ...) 									\
	snprintf(_log_uart_buffer, COMMS_UART_BUF_LEN, 							\
			"[DEBUG] %s:%d: " M "\n",							\
			 __FILE__, __LINE__, ##__VA_ARGS__);						\
	HAL_UART_Transmit (huart, _log_uart_buffer,							\
			   strlen (_log_uart_buffer), COMMS_DEFAULT_TIMEOUT_MS);	\

#define LOG_UART_ERROR(huart, M, ...) 									\
	snprintf(_log_uart_buffer, COMMS_UART_BUF_LEN, 							\
			"[ERROR] %s:%d: " M "\n",							\
			 __FILE__, __LINE__, ##__VA_ARGS__);						\
	HAL_UART_Transmit (huart, _log_uart_buffer,							\
			   strlen (_log_uart_buffer), COMMS_DEFAULT_TIMEOUT_MS);	\

#endif /* COMMS_UART_DEST_OBC */

#else
#define LOG_UART_DBG(huart, M, ...)
#define LOG_UART_ERROR(huart, M, ...)
#endif

#endif /* INC_LOG_H_ */
