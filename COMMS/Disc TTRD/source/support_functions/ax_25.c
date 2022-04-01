#include "ax_25.h"


///////////////////////////////////////////// LFSR.C ////////////////////////////////////
//Init 
/**
 * Creates and initializes the LFSR
 * @param h the LFSR handle. Should point to a valid memory region
 * @param mask the polynomial of the LFSR
 * @param seed the initial seed
 * @param order the order of the LFSR
 * @return 0 on success or a negative number in case of error
 */
int32_t lfsr_init(lfsr_handle_t *h, uint32_t mask, uint32_t seed, uint32_t order)
{
	  if(!h || order > 31){
    return -1;
  }
  h->mask = mask;
  h->seed = seed;
  h->shift_reg = seed;
  h->order = order;
  return 0;
}

//Setter
/**
 * Resets the LFSR at the initial state
 * @param h the LFSR handle.
 * @return 0 on success or a negative number in case of error
 */
int32_t lfsr_reset(lfsr_handle_t *h)
{
  if(!h){
    return -1;
  }

  h->shift_reg = h->seed;
  return 0;
}

//Getter
/**
 * Get the next output bit from the LFSR
 * @param h the LFSR handle. Should be a VALID LFSR handle and it is
 * responsibility of the caller to ensure this. Otherwise the result is
 * undefined.
 * @return  the output bit of the LFSR
 */
uint8_t lfsr_next_bit(lfsr_handle_t *h)
{
  uint8_t bit;
  uint8_t out_bit;
  out_bit = h->shift_reg & 0x1;
  bit = bit_count(h->shift_reg & h->mask) % 2;
  h->shift_reg = (h->shift_reg >> 1) | (bit << h->order);
  return out_bit;
}

/**
 * Feeds the LFSR with the input bit and return the scrambled output
 * @param h the LFSR handle. Should be a VALID LFSR handle and it is
 * responsibility of the caller to ensure this. Otherwise the result is
 * undefined.
 * @param bit the input bit. Should be placed at LS bit.
 * @return the scrambled bit, placed at the LS bit of the output byte.
 */
uint8_t lfsr_next_bit_scramble(lfsr_handle_t *h, uint8_t bit)
{
  uint8_t out_bit;
  uint8_t new_bit;
  out_bit = h->shift_reg & 0x1;
  new_bit = (bit_count(h->shift_reg & h->mask) % 2) ^ (bit & 0x1);
  h->shift_reg = (h->shift_reg >> 1) | (new_bit << h->order);
  return out_bit;
}

/**
 * Feeds the LFSR with the input bit and return the descrambled output
 * @param h the LFSR handle. Should be a VALID LFSR handle and it is
 * responsibility of the caller to ensure this. Otherwise the result is
 * undefined.
 * @param bit the input bit. Should be placed at LS bit.
 * @return the descrambled bit, placed at the LS bit of the output byte.
 */
uint8_t lfsr_next_bit_descramble(lfsr_handle_t *h, uint8_t bit)
{
  uint8_t out_bit;
  out_bit = (bit_count(h->shift_reg & h->mask) % 2) ^ (bit & 0x1);
  h->shift_reg = (h->shift_reg >> 1) | ((bit & 0x1) << h->order);
  return out_bit;
}




////////////////////////////////////////// SCRAMBLER.C ///////////////////////////////////////
// Init
int32_t scrambler_init (scrambler_handle_t *h, uint32_t pol, uint32_t seed, uint32_t order)
{
  if(!h){
    return -1;
  }
  h->prev_bit = 0;
  return lfsr_init(&h->lfsr, pol, seed, order);
}
int32_t descrambler_init (scrambler_handle_t *h, uint32_t pol, uint32_t seed, uint32_t order)
{
  return scrambler_init(h, pol, seed, order);
}


// Setters
int32_t scrambler_reset (scrambler_handle_t *h)
{
	if(!h){
    return -1;
  }
  h->prev_bit = 0;
  return lfsr_reset(&h->lfsr);	
}
int32_t descrambler_reset (scrambler_handle_t *h)
{
	return scrambler_reset(h);
}
int32_t scramble_data (scrambler_handle_t *h, uint8_t *out, const uint8_t *in, size_t len)
{
  size_t i;
  if(!h){
    return -1;
  }

  for(i = 0; i < len; i++){
    out[i] = lfsr_next_bit_scramble(&h->lfsr, in[i] & 0x1);
    out[i] |= (lfsr_next_bit_scramble(&h->lfsr, (in[i] >> 1) & 0x1) << 1);
    out[i] |= (lfsr_next_bit_scramble(&h->lfsr, (in[i] >> 2) & 0x1) << 2);
    out[i] |= (lfsr_next_bit_scramble(&h->lfsr, (in[i] >> 3) & 0x1) << 3);
    out[i] |= (lfsr_next_bit_scramble(&h->lfsr, (in[i] >> 4) & 0x1) << 4);
    out[i] |= (lfsr_next_bit_scramble(&h->lfsr, (in[i] >> 5) & 0x1) << 5);
    out[i] |= (lfsr_next_bit_scramble(&h->lfsr, (in[i] >> 6) & 0x1) << 6);
    out[i] |= (lfsr_next_bit_scramble(&h->lfsr, (in[i] >> 7) & 0x1) << 7);
  }
  return 0;
}

/**
 * Performs NRZI encoding and scrambling at the same time
 * @param h the scrambler handler
 * @param out the output buffer
 * @param in the input buffer
 * @param len the size of the input buffer
 * @return 0 on success or a negative number in case of error
 */
int32_t scramble_data_nrzi (scrambler_handle_t *h, uint8_t *out, const uint8_t *in, size_t len)
{
  size_t i;
  uint8_t new_bit;
  uint8_t inv;

  if(!h){
    return -1;
  }

  for(i = 0; i < len; i++){
    /* NRZI */
    inv = ~in[i];

    new_bit = ( (inv & 0x1) + h->prev_bit) % 2;
    h->prev_bit = new_bit;
    out[i] = lfsr_next_bit_scramble(&h->lfsr, new_bit);

    new_bit = (( (inv >> 1)  & 0x1) + h->prev_bit) % 2;
    h->prev_bit = new_bit;
    out[i] |= lfsr_next_bit_scramble(&h->lfsr, new_bit) << 1;

    new_bit = (( (inv >> 2)  & 0x1) + h->prev_bit) % 2;
    h->prev_bit = new_bit;
    out[i] |= lfsr_next_bit_scramble(&h->lfsr, new_bit) << 2;

    new_bit = (( (inv >> 3)  & 0x1) + h->prev_bit) % 2;
    h->prev_bit = new_bit;
    out[i] |= lfsr_next_bit_scramble(&h->lfsr, new_bit) << 3;

    new_bit = (( (inv >> 4)  & 0x1) + h->prev_bit) % 2;
    h->prev_bit = new_bit;
    out[i] |= lfsr_next_bit_scramble(&h->lfsr, new_bit) << 4;

    new_bit = (( (inv >> 5)  & 0x1) + h->prev_bit) % 2;
    h->prev_bit = new_bit;
    out[i] |= lfsr_next_bit_scramble(&h->lfsr, new_bit) << 5;

    new_bit = (( (inv >> 6)  & 0x1) + h->prev_bit) % 2;
    h->prev_bit = new_bit;
    out[i] |= lfsr_next_bit_scramble(&h->lfsr, new_bit) << 6;

    new_bit = (( (inv >> 7)  & 0x1) + h->prev_bit) % 2;
    h->prev_bit = new_bit;
    out[i] |= lfsr_next_bit_scramble(&h->lfsr, new_bit) << 7;
  }
  return 0;
}
int32_t descramble_data (scrambler_handle_t *h, uint8_t *out, const uint8_t *in, size_t len)
{
  size_t i;
  if(!h){
    return -1;
  }

  for(i = 0; i < len; i++){
    out[i] = lfsr_next_bit_descramble(&h->lfsr, in[i] & 0x1);
    out[i] |= lfsr_next_bit_descramble(&h->lfsr, (in[i] >> 1) & 0x1) << 1;
    out[i] |= lfsr_next_bit_descramble(&h->lfsr, (in[i] >> 2) & 0x1) << 2;
    out[i] |= lfsr_next_bit_descramble(&h->lfsr, (in[i] >> 3) & 0x1) << 3;
    out[i] |= lfsr_next_bit_descramble(&h->lfsr, (in[i] >> 4) & 0x1) << 4;
    out[i] |= lfsr_next_bit_descramble(&h->lfsr, (in[i] >> 5) & 0x1) << 5;
    out[i] |= lfsr_next_bit_descramble(&h->lfsr, (in[i] >> 6) & 0x1) << 6;
    out[i] |= lfsr_next_bit_descramble(&h->lfsr, (in[i] >> 7) & 0x1) << 7;
  }
  return 0;
}

/**
 * Descramble and perform NRZI decoding at once
 * @param h
 * @param out
 * @param in
 * @param len
 * @return
 */
int32_t descramble_data_nrzi (scrambler_handle_t *h, uint8_t *out, const uint8_t *in,
		      size_t len)
{
  size_t i;
  uint8_t new_bit;
  uint8_t curr_bit;

  if(!h){
    return -1;
  }

  for(i = 0; i < len; i++){
    curr_bit = lfsr_next_bit_descramble(&h->lfsr, in[i] & 0x1);
    new_bit = (~( (curr_bit - h->prev_bit) % 2)) & 0x1;
    h->prev_bit = curr_bit;
    out[i] = new_bit;

    curr_bit = lfsr_next_bit_descramble (&h->lfsr, (in[i] >> 1) & 0x1);
    new_bit= (~((curr_bit - h->prev_bit) % 2)) & 0x1;
    h->prev_bit = curr_bit;
    out[i] |= new_bit << 1;

    curr_bit = lfsr_next_bit_descramble (&h->lfsr, (in[i] >> 2) & 0x1);
    new_bit= (~((curr_bit - h->prev_bit) % 2)) & 0x1;
    h->prev_bit = curr_bit;
    out[i] |= new_bit << 2;

    curr_bit = lfsr_next_bit_descramble (&h->lfsr, (in[i] >> 3) & 0x1);
    new_bit= (~((curr_bit - h->prev_bit) % 2)) & 0x1;
    h->prev_bit = curr_bit;
    out[i] |= new_bit << 3;

    curr_bit = lfsr_next_bit_descramble (&h->lfsr, (in[i] >> 4) & 0x1);
    new_bit= (~((curr_bit - h->prev_bit) % 2)) & 0x1;
    h->prev_bit = curr_bit;
    out[i] |= new_bit << 4;

    curr_bit = lfsr_next_bit_descramble (&h->lfsr, (in[i] >> 5) & 0x1);
    new_bit= (~((curr_bit - h->prev_bit) % 2)) & 0x1;
    h->prev_bit = curr_bit;
    out[i] |= new_bit << 5;

    curr_bit = lfsr_next_bit_descramble (&h->lfsr, (in[i] >> 6) & 0x1);
    new_bit= (~((curr_bit - h->prev_bit) % 2)) & 0x1;
    h->prev_bit = curr_bit;
    out[i] |= new_bit << 6;

    curr_bit = lfsr_next_bit_descramble (&h->lfsr, (in[i] >> 7) & 0x1);
    new_bit= (~((curr_bit - h->prev_bit) % 2)) & 0x1;
    h->prev_bit = curr_bit;
    out[i] |= new_bit << 7;
  }
  return 0;
}

int32_t scramble_one_bit_per_byte (scrambler_handle_t *h, uint8_t *out,
			   const uint8_t *in, size_t bit_cnt)
{
  size_t i;
  if(!h){
    return -1;
  }
  for(i = 0; i < bit_cnt; i++){
    out[i] = in[i] ^ lfsr_next_bit(&h->lfsr);
  }
  return 0;
}

int32_t descramble_one_bit_per_byte (scrambler_handle_t *h, uint8_t *out, const uint8_t *in, size_t bit_cnt)
{
  size_t i;
  if(!h){
    return -1;
  }
  for(i = 0; i < bit_cnt; i++){
    out[i] = lfsr_next_bit_descramble(&h->lfsr, in[i]);
  }
  return 0;
}



////////////////////////////////////////// AX_25.C ///////////////////////////////////////
uint8_t interm_send_buf[AX25_PREAMBLE_LEN + AX25_POSTAMBLE_LEN
			+ AX25_MAX_FRAME_LEN + AX25_MAX_ADDR_LEN] = {0};
uint8_t tmp_bit_buf[(AX25_PREAMBLE_LEN + AX25_POSTAMBLE_LEN
    + AX25_MAX_FRAME_LEN + AX25_MAX_ADDR_LEN) * 8] = {0};

scrambler_handle_t h_scrabler;		
	
// Init 
/**
 * Initializes the AX.25 decoder handler
 * @param h pointer to an AX.25 decoder handler
 * @return 0 on success or a negative number in case of error
 */
int32_t ax25_rx_init(ax25_handle_t *h)
{
  if(!h){
    return -1;
  }
  descrambler_init (&h->descrambler, __SCRAMBLER_POLY, __SCRAMBLER_SEED,  __SCRAMBLER_ORDER);
  return ax25_rx_reset(h);
}

/**
 * Resets to the initial defaults the AX.25 receiver
 * @param h the AX.25 decoder handler
 * @return 0 on success or a negative number in case of error
 */
int32_t ax25_rx_reset(ax25_handle_t *h)
{
  if(!h){
    return -1;
  }
  h->decoded_num = 0;
  h->state = AX25_NO_SYNC;
  h->shift_reg = 0x0;
  h->bit_cnt = 0;
  return descrambler_reset(&h->descrambler);
}

		
/**
 * Prepared the AX.25 bit-stream that should be sent over the air.
 * The data are scrambled using the G3RUH self-synchronizing scrambler and
 * then are NRZI encoded. Also as AX.25 dictates that the LS bits should be
 * sent first, this function properly swap the bits in every bit. So the
 * transmitting routing should sent the bits MS bit first. This is performed
 * for user convenient due to the fact that most teleccomunication systems
 * send the MS first.
 *
 * @param out the output buffer that will hold the encoded data
 * @param in the input data containing the payload
 * @param len the length of the input data
 * @param is_wod set to true if this frame is a WOD
 * @return the length of the encoded data or -1 in case of error
 */
int32_t ax25_send (uint8_t *out, const uint8_t *in, size_t len, uint8_t is_wod)
{
 ax25_encode_status_t status;
  uint8_t addr_buf[AX25_MAX_ADDR_LEN] = {0};
  size_t addr_len = 0;
  size_t interm_len;
  size_t ret_len;
  size_t i;
  size_t pad_bits = 0;
  uint8_t dest_ssid = is_wod ? __UPSAT_DEST_SSID_WOD :__UPSAT_DEST_SSID;
	/* Create the address field */
  addr_len = ax25_create_addr_field (addr_buf,
				     (const uint8_t *)__UPSAT_DEST_CALLSIGN,
				     dest_ssid,
				     (const uint8_t *) __UPSAT_CALLSIGN,
				     __UPSAT_SSID);
	/*
	* Prepare address and payload into one frame placing the result in
	* an intermediate buffer
	*/
	interm_len = ax25_prepare_frame (interm_send_buf, in, len, AX25_UI_FRAME,
				   addr_buf, addr_len, __UPSAT_AX25_CTRL, 1);
  if(interm_len == 0){
    return -1;
  }				 
	status = ax25_bit_stuffing(tmp_bit_buf, &ret_len, interm_send_buf, interm_len);
  if( status != AX25_ENC_OK){
    return -1;
  }			
	
	  /* Pack now the bits into full bytes */
  memset(interm_send_buf, 0, sizeof(interm_send_buf));
  for (i = 0; i < ret_len; i++) {
    interm_send_buf[i/8] |= tmp_bit_buf[i] << (i % 8);
  }

  /*Perhaps some padding is needed due to bit stuffing */
  if(ret_len % 8){
    pad_bits = 8 - (ret_len % 8);
  }
  ret_len += pad_bits;

  /* Perform NRZI and scrambling based on the G3RUH polynomial */
  scrambler_init (&h_scrabler, __SCRAMBLER_POLY, __SCRAMBLER_SEED,
		  __SCRAMBLER_ORDER);
  scrambler_reset(&h_scrabler);
  scramble_data_nrzi(&h_scrabler, out, interm_send_buf,
		     ret_len/8);

  /* AX.25 sends LS bit first*/
  for(i = 0; i < ret_len/8; i++){
    out[i] = reverse_byte(out[i]);
  }

  return ret_len/8;					 
						 
}

size_t
ax25_create_addr_field (uint8_t *out, const uint8_t  *dest_addr,uint8_t dest_ssid, const uint8_t *src_addr, uint8_t src_ssid)
{
	uint16_t i = 0;
	//strnlen (dest_addr, AX25_CALLSIGN_MAX_LEN) is used to return len with max number of bytes
  for (i = 0; i < strlen ((char*)dest_addr); i++) {
    *out++ = dest_addr[i] << 1;
  }
  /*
   * Perhaps the destination callsign was smaller that the maximum allowed.
   * In this case the leftover bytes should be filled with space
   */
	for (; i < AX25_CALLSIGN_MAX_LEN; i++) {
    *out++ = ' ' << 1;
  }
	/* Apply SSID, reserved and C bit */
  /* FIXME: C bit is set to 0 implicitly */
  *out++ = ((0x0F & dest_ssid) << 1) | 0x60;
  //*out++ = ((0b1111 & dest_ssid) << 1) | 0b01100000;

  for (i = 0; i < strlen ((char*)src_addr); i++) {
    *out++ = dest_addr[i] << 1;
  }
  for (; i < AX25_CALLSIGN_MAX_LEN; i++) {
    *out++ = ' ' << 1;
  }
	/* Apply SSID, reserved and C bit. As this is the last address field
   * the trailing bit is set to 1.
   */
  /* FIXME: C bit is set to 0 implicitly */
  *out++ = ((0x0F & dest_ssid) << 1) | 0x61;
  //*out++ = ((0b1111 & dest_ssid) << 1) | 0b01100001;
  return (size_t) AX25_MIN_ADDR_LEN;
}

size_t ax25_prepare_frame (uint8_t *out, const uint8_t *info, size_t info_len,
		    ax25_frame_type_t type, uint8_t *addr, size_t addr_len,
		    uint16_t ctrl, size_t ctrl_len)
{
	
	
	
}

ax25_encode_status_t ax25_bit_stuffing (uint8_t *out, size_t *out_len, const uint8_t *buffer,const size_t buffer_len)
{
 
 
 
 
}


