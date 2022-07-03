#ifndef __AX25_H
#define __AX25_H

// Processor Header
#include "../main/main.h"


///////////////////////////////////////////// LFSR.h ////////////////////////////////////
/* 
LFSR --> Linear feedback shift register 
				 is a shift register whose input bit is a linear function of its previous state.
*/
typedef struct {
  uint32_t mask;
  uint32_t seed;
  uint32_t order;
  uint32_t shift_reg;
}lfsr_handle_t;
//Init 
int32_t lfsr_init(lfsr_handle_t *h, uint32_t mask, uint32_t seed, uint32_t order);//implemented

//Update

//Setter
int32_t lfsr_reset(lfsr_handle_t *h);//implemented

//Getter
uint8_t lfsr_next_bit(lfsr_handle_t *h);//implemented

uint8_t lfsr_next_bit_scramble(lfsr_handle_t *h, uint8_t bit);//implemented

uint8_t lfsr_next_bit_descramble(lfsr_handle_t *h, uint8_t bit);//implemented

////////////////////////////////////////// SCRAMBLER.H ///////////////////////////////////////
typedef struct
{
  lfsr_handle_t lfsr;
  uint8_t prev_bit;
} scrambler_handle_t;

// Init Functions
int32_t scrambler_init (scrambler_handle_t *h, uint32_t pol, uint32_t seed,	uint32_t order);    //implemented

int32_t descrambler_init (scrambler_handle_t *h, uint32_t pol, uint32_t seed, uint32_t order);	//implemented


//Update functions



// Setters
int32_t scrambler_reset (scrambler_handle_t *h);//implemented

int32_t descrambler_reset (scrambler_handle_t *h);//implemented

int32_t scramble_data (scrambler_handle_t *h, uint8_t *out, const uint8_t *in, size_t len);//implemented

int32_t scramble_data_nrzi (scrambler_handle_t *h, uint8_t *out, const uint8_t *in, size_t len);//implemented

int32_t descramble_data (scrambler_handle_t *h, uint8_t *out, const uint8_t *in, size_t len);//implemented

int32_t descramble_data_nrzi (scrambler_handle_t *h, uint8_t *out, const uint8_t *in, size_t len);//implemented

int32_t scramble_one_bit_per_byte (scrambler_handle_t *h, uint8_t *out, const uint8_t *in, size_t bit_cnt);//implemented

int32_t descramble_one_bit_per_byte (scrambler_handle_t *h, uint8_t *out, const uint8_t *in, size_t bit_cnt);//implemented
//Getters

// no getter functions
////////////////////////////////////////// AX_25.H ///////////////////////////////////////

#define AX25_MAX_ADDR_LEN 28
#define AX25_MAX_FRAME_LEN 256
#define AX25_MIN_ADDR_LEN 14
#define AX25_SYNC_FLAG 0x7E
#define AX25_MIN_CTRL_LEN 1
#define AX25_MAX_CTRL_LEN 2
#define AX25_CALLSIGN_MAX_LEN 6  //is not used for now
#define AX25_CALLSIGN_MIN_LEN 2
#define AX25_PREAMBLE_LEN 16
#define AX25_POSTAMBLE_LEN 16


/**
 * AX.25 Frame types
 */
typedef enum
{
  AX25_I_FRAME, //!< AX25_I_FRAME Information frame
  AX25_S_FRAME, //!< AX25_S_FRAME Supervisory frame
  AX25_U_FRAME, //!< AX25_U_FRAME Unnumbered frame
  AX25_UI_FRAME /**!< AX25_UI_FRAME Unnumbered information frame */
} ax25_frame_type_t;
/**
 * The different states of the AX.25 decoder
 */
typedef enum
{
  AX25_NO_SYNC, //!< AX25_NO_SYNC when not frame has been seen yet
  AX25_IN_SYNC, //!< AX25_IN_SYNC the stating SYNC flag has been received
  AX25_FRAME_END//!< AX25_FRAME_END the trailing SYNC flag has been received
}ax25_decoding_state_t;
typedef enum
{
  AX25_ENC_FAIL, AX25_ENC_OK
} ax25_encode_status_t;

typedef enum
{
  AX25_DEC_NOT_READY = -56,
  AX25_DEC_CRC_FAIL = -55,
  AX25_DEC_SIZE_ERROR = -54,
  AX25_DEC_STOP_SYNC_NOT_FOUND = -53,
  AX25_DEC_START_SYNC_NOT_FOUND = -52,
  AX25_DEC_FAIL = -51,
  AX25_DEC_OK = 0
} ax25_decode_status_t;

typedef struct
{
  uint8_t address[AX25_MAX_ADDR_LEN];
  size_t address_len;
  uint16_t ctrl;
  size_t ctrl_len;
  uint8_t pid;
  uint8_t *info;
  size_t info_len;
  ax25_frame_type_t type;
} ax25_frame_t;
 
typedef struct
{
  ax25_decoding_state_t state;
  size_t decoded_num;
  uint8_t shift_reg;
  uint8_t dec_byte;
  uint8_t bit_cnt;
  scrambler_handle_t descrambler;
} ax25_handle_t;


// Init 
int32_t ax25_rx_init(ax25_handle_t *h); 

// Update
/* no update for this file */

//Setters
int32_t ax25_recv_nrzi (ax25_handle_t *h, uint8_t *out, size_t *out_len, const uint8_t *in, size_t len);

int32_t ax25_recv (ax25_handle_t *h, uint8_t *out, size_t *out_len, const uint8_t *in, size_t len);

int32_t ax25_extract_payload(uint8_t *out, const uint8_t *in, size_t frame_len, size_t addr_len, size_t ctrl_len);

int32_t ax25_rx_reset(ax25_handle_t *h);

size_t ax25_create_addr_field (uint8_t *out, const uint8_t  *dest_addr,uint8_t dest_ssid, const uint8_t *src_addr, uint8_t src_ssid); 

size_t ax25_prepare_frame (uint8_t *out, const uint8_t *info, size_t info_len, ax25_frame_type_t type, uint8_t *dest, size_t addr_len, uint16_t ctrl, size_t ctrl_len); 

//Getters
int32_t ax25_send (uint8_t *out, const uint8_t *in, size_t len, uint8_t is_wod); 

uint16_t ax25_fcs (uint8_t *buffer, size_t len);

uint8_t ax25_check_dest_callsign (const uint8_t *ax25_frame, size_t frame_len, const char *dest);


// Setter / Getter
ax25_encode_status_t ax25_bit_stuffing (uint8_t *out, size_t *out_len, const uint8_t *buffer, const size_t buffer_len);

ax25_decode_status_t ax25_decode (ax25_handle_t *h, uint8_t *out, size_t *out_len, const uint8_t *ax25_frame, size_t len);



#endif
