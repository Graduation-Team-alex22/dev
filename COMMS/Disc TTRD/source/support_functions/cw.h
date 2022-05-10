#ifndef INC_CW_H_
#define INC_CW_H_

// Processor Header
#include "../main/main.h"

#define CW_DOT_DURATION_MS 60
#define CW_DASH_DURATION_MS (3*CW_DOT_DURATION_MS)
#define CW_SYMBOL_SPACE_MS CW_DOT_DURATION_MS
#define CW_CHAR_SPACE_MS (3*CW_DOT_DURATION_MS)

/*
 * NOTE: Normally the ITU word space is 7 dots long. However our encoder
 * applies a char character after each character. So at the end of a word
 * a space with duration of 4 more dots should be applied.
 */
#define CW_WORD_SPACE_MS (4*CW_DOT_DURATION_MS)
//comms_rf_stat_t comms_stats;
//extern comms_rf_stat_t comms_stats;



/**
 * The different CW symbols
 */
typedef enum {
  CW_DOT = 0,          //!< CW_DOT a dot (pulse of short duration)
  CW_DASH = 1,         //!< CW_DASH a dash (pulse of long duration)
  CW_CHAR_DELIM = 2,   //!< CW_CHAR_DELIM character pause delimiter
  CW_WORD_DELIM = 3,   //!< CW_WORD_DELIM word pause delimiter
  CW_SYMBOL_DELIM = 4, //!< CW_SYMBOL_DELIM symbol pause delimiter
  CW_INVALID = 5       //!< CW_INVALID invalid symbol
} cw_symbol_t;

typedef struct {
  uint8_t cw_on;
  uint32_t duration_ms;
} cw_pulse_t;
/**
 * The status of the CW encoding/transmission
 */
typedef enum {
  CW_OK = 0,   //!< CW_OK all ok
  CW_ERROR = -61//!< CW_ERROR an error occured
} cw_status_t;

/**
 * A CW character and its mapping to CW symbols
 */
typedef struct {
  cw_symbol_t s[10];
  uint8_t s_num;
  uint8_t is_valid;
} cw_char_t;

//Init
void cw_init (void);

//Update

//Setters


//Getters
char cw_get_uptime_hours_char(comms_rf_stat_t *h);

char cw_get_uptime_mins_char(comms_rf_stat_t *h);

char cw_get_temp_char(comms_rf_stat_t *h);

char cw_get_cont_errors_char(comms_rf_stat_t *h);

char cw_get_last_error_char(comms_rf_stat_t *h);

char cw_get_bat_voltage_char(comms_rf_stat_t *h);

char cw_get_bat_current_char(comms_rf_stat_t *h);


//Setter / Getter
int32_t cw_encode(cw_pulse_t *out, size_t *out_len, const uint8_t *in, size_t len);



#endif /* INC_CW_H_ */
