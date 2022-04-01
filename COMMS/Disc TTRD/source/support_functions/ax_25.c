#include "ax_25.h"

#undef __FILE_ID__
#define __FILE_ID__ 669

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
static const uint8_t AX25_SYNC_FLAG_MAP_BIN[8] = {0, 1, 1, 1, 1, 1, 1, 0};

uint8_t interm_send_buf[AX25_PREAMBLE_LEN + AX25_POSTAMBLE_LEN+ AX25_MAX_FRAME_LEN + AX25_MAX_ADDR_LEN] = {0};
			
uint8_t tmp_bit_buf[(AX25_PREAMBLE_LEN + AX25_POSTAMBLE_LEN + AX25_MAX_FRAME_LEN + AX25_MAX_ADDR_LEN) * 8] = {0};
		
uint8_t tmp_buf[AX25_MAX_FRAME_LEN * 2] = {0};

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
/////////////////////////////////////////////////////////////////////////////////////////////////////
//Setters
/**
 * This function tries to extract a valid AX.25 payload for the input data.
 * This method can be called repeatedly with input data that can be random noise
 * or subset of the actual frame. When the entire frame is retrieved this functions
 * returns the frame size.
 *
 * @param h the AX.25 decoder handle
 * @param out the output buffer that should hold the AX.25 payload. It should be
 * enough in size to hold an entire AX.25 payload.
 * @param out_len the length of the decoded frame when it is available
 * @param in the input buffer
 * @param len the size of the input buffer
 * @return AX25_DEC_NOT_READY if the frame has not yet (entirely) retrieved, or
 * AX25_DEC_OK when a frame successfully retrieved.
 * error.
 */
int32_t ax25_recv_nrzi (ax25_handle_t *h, uint8_t *out, size_t *out_len, const uint8_t *in, size_t len)
{
  size_t decode_len;
  ax25_decode_status_t status;

  if (len == 0) 
		{
    return AX25_DEC_NOT_READY;
		}

  /* Now descramble the AX.25 frame and at the same time do the NRZI decoding */
  descramble_data_nrzi (&h->descrambler, tmp_buf, in, len);

  status  = ax25_recv(h, out, &decode_len, tmp_buf, len);
  *out_len = decode_len;
  return status;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * This function tries to extract a valid AX.25 payload for the input data.
 * This method can be called repeatedly with input data that can be random noise
 * or subset of the actual frame. When the entire frame is retrieved this functions
 * returns the frame size.
 *
 * @param h the AX.25 decoder handle
 * @param out the output buffer that should hold the AX.25 payload. It should be
 * enough in size to hold an entire AX.25 payload.
 * @param out_len the length of the decoded frame when it is available
 * @param in the input buffer
 * @param len the size of the input buffer
 * @return AX25_DEC_NOT_READY if the frame has not yet (entirely) retrieved, or
 * AX25_DEC_OK when a frame successfully retrieved.
 * error.
 */
int32_t ax25_recv(ax25_handle_t *h, uint8_t *out, size_t *out_len, const uint8_t *in, size_t len)
{
  size_t i;
  size_t decode_len;
  ax25_decode_status_t status;

  if(len == 0) {
    return AX25_DEC_NOT_READY;
  }

  for(i = 0; i < len; i++){
    tmp_buf[i] = reverse_byte(in[i]);
  }

  /* Perform the actual decoding */
  status = ax25_decode(h, out, &decode_len, tmp_buf, len);
  if( status != AX25_DEC_OK){
    return status;
  }
  *out_len = decode_len;
  return AX25_DEC_OK;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * This function extracts the AX.25 payload from an AX.25 frame
 * @param out the output buffer
 * @param in the buffer with the AX.25 frame
 * @param frame_len the AX.25 frame size in bytes
 * @param addr_len the AX.25 address length in bytes
 * @return the size of the payload in bytes or appropriate error code
 */
int32_t ax25_extract_payload(uint8_t *out, const uint8_t *in, size_t frame_len, size_t addr_len, size_t ctrl_len)
{
//  if (!C_ASSERT (out != NULL && in != NULL)) { // SYSVIEW checks
//    return AX25_DEC_FAIL;
//  }

  if(addr_len != AX25_MIN_ADDR_LEN && addr_len != AX25_MAX_ADDR_LEN) {
    return AX25_DEC_SIZE_ERROR;
  }

  if(addr_len + ctrl_len >= frame_len || ctrl_len > 2) {
    return AX25_DEC_SIZE_ERROR;
  }

  /* Skip also the control field and the frame type field */
  memcpy(out, in + addr_len + ctrl_len + 1, frame_len - addr_len - ctrl_len -1);
  return frame_len - addr_len - ctrl_len - 1;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Creates the header field of the AX.25 frame
 * @param out the output buffer with enough memory to hold the address field
 * @param dest_addr the destination callsign address
 * @param dest_ssid the destination SSID
 * @param src_addr the callsign of the source
 * @param src_ssid the source SSID
 */
size_t ax25_create_addr_field (uint8_t *out, const uint8_t  *dest_addr,uint8_t dest_ssid, const uint8_t *src_addr, uint8_t src_ssid)
{
	uint16_t i = 0;
	//strnlen (dest_addr, AX25_CALLSIGN_MAX_LEN) is used to return len with max number of bytes
  for (i = 0; i < strnlen ((char*)dest_addr, AX25_CALLSIGN_MAX_LEN); i++)
	{
    *out++ = dest_addr[i] << 1;
  }
  /*
   * Perhaps the destination callsign was smaller that the maximum allowed.
   * In this case the leftover bytes should be filled with space
   */
	for (; i < AX25_CALLSIGN_MAX_LEN; i++) 
	{
    *out++ = ' ' << 1;
  }
	/* Apply SSID, reserved and C bit */
  /* FIXME: C bit is set to 0 implicitly */
  *out++ = ((0x0F & dest_ssid) << 1) | 0x60;
  //*out++ = ((0b1111 & dest_ssid) << 1) | 0b01100000;

  for (i = 0; i < strnlen ((char*)src_addr, AX25_CALLSIGN_MAX_LEN); i++) 
	{
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
/////////////////////////////////////////////////////////////////////////////////////////////////////
size_t ax25_prepare_frame (uint8_t *out, const uint8_t *info, size_t info_len, ax25_frame_type_t type, uint8_t *addr, size_t addr_len, uint16_t ctrl, size_t ctrl_len)
{
  uint16_t fcs;
  size_t i;
  if (info_len > AX25_MAX_FRAME_LEN) {
    return 0;
  }


  /* Repeat the AX.25 sync flag a pre-defined number of times */
  memset(out, AX25_SYNC_FLAG, AX25_PREAMBLE_LEN);
  i = AX25_PREAMBLE_LEN;

  /* Insert address and control fields */
  if (addr_len == AX25_MIN_ADDR_LEN || addr_len == AX25_MAX_ADDR_LEN) {
    memcpy (out + i, addr, addr_len);
    i += addr_len;
  }
  else {
    return 0;
  }

  if (ctrl_len == AX25_MIN_CTRL_LEN || ctrl_len == AX25_MAX_CTRL_LEN) {
    memcpy (out + i, &ctrl, ctrl_len);
    i += ctrl_len;
  }
  else {
    return 0;
  }

  /*
   * Set the PID depending the frame type.
   * FIXME: For now, only the "No layer 3 is implemented" information is
   * inserted
   */
  if (type == AX25_I_FRAME || type == AX25_UI_FRAME) {
    out[i++] = 0xF0;
  }
  memcpy (out + i, info, info_len);
  i += info_len;

  /* Compute the FCS. Ignore the AX.25 preamble */
  fcs = ax25_fcs (out + AX25_PREAMBLE_LEN, i - AX25_PREAMBLE_LEN);

  /* The MS bits are sent first ONLY at the FCS field */
  out[i++] = fcs & 0xFF;
  out[i++] = (fcs >> 8) & 0xFF;

  /* Append the AX.25 postample*/
  memset(out+i, AX25_SYNC_FLAG, AX25_POSTAMBLE_LEN);
  return i + AX25_POSTAMBLE_LEN;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
//Getters
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
/////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Calculates the FCS of the AX25 frame
 * @param buffer data buffer
 * @param len size of the buffer
 * @return the FCS of the buffer
 */
uint16_t ax25_fcs (uint8_t *buffer, size_t len)
{
  uint16_t fcs = 0xFFFF;
  while (len--) {
    fcs = (fcs >> 8) ^ crc16_ccitt_table_reverse[(fcs ^ *buffer++) & 0xFF];
  }
  return fcs ^ 0xFFFF;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Checks if the destination field of an AX.25 frame matched a specific address
 * @param ax25_frame an ax.25 frame, decoded using the \p ax25_recv() function.
 * @param frame_len the size of the decoded AX.25 frame
 * @param dest string with the desired address
 * @return 1 if the \p addr matched the destination address of the AX.25 frame,
 * 0 otherwise.
 */
uint8_t ax25_check_dest_callsign (const uint8_t *ax25_frame, size_t frame_len, const char *dest)
{
  size_t callsign_len;
  size_t i;

  callsign_len = strnlen(dest, AX25_CALLSIGN_MAX_LEN );

  /* Perform some size sanity checks */
  if(callsign_len < AX25_CALLSIGN_MIN_LEN || callsign_len > frame_len) 
		{
    return 0;
		}

  for(i = 0; i < callsign_len; i++)
		{
    if((ax25_frame[i] >> 1) != dest[i]){
      return 0;
    }
		}

  /* All good, this frame was for us */
  return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructs an AX.25 by performing bit stuffing.
 * @param out the output buffer to hold the frame. To keep it simple,
 * each byte of the buffer holds only one bit. Also the size of the
 * buffer should be enough, such that the extra stuffed bits are fitting
 * on the allocated space.
 *
 * @param out_len due to bit stuffing the output size can vary. This
 * pointer will hold the resulting frame size after bit stuffing.
 *
 * @param buffer buffer holding the data that should be encoded.
 * Note that this buffer SHOULD contain the leading and trailing
 * synchronization flag, all necessary headers and the CRC.
 *
 * @param buffer_len the length of the input buffer.
 *
 * @return the resulting status of the encoding
 */
ax25_encode_status_t ax25_bit_stuffing (uint8_t *out, size_t *out_len, const uint8_t *buffer, const size_t buffer_len)
{
  uint8_t bit;
  uint8_t shift_reg = 0x0;
  size_t out_idx = 0;
  size_t i;

  /* Leading FLAG field does not need bit stuffing */
  for(i = 0; i < AX25_PREAMBLE_LEN; i++){
    memcpy (out + out_idx, AX25_SYNC_FLAG_MAP_BIN, 8);
    out_idx += 8;
  }

  /* Skip the AX.25 preamble and postable */
  buffer += AX25_PREAMBLE_LEN;
  for (i = 0; i < 8 * (buffer_len - AX25_PREAMBLE_LEN - AX25_POSTAMBLE_LEN); i++) {
    bit = (buffer[i / 8] >> (i % 8)) & 0x1;
    shift_reg = (shift_reg << 1) | bit;
    out[out_idx++] = bit;
    /* Check if bit stuffing should be applied */
    if((shift_reg & 0x1F) == 0x1F){
      out[out_idx++] = 0;
      shift_reg = 0x0;
    }
  }

  /*Postamble does not need bit stuffing */
  for(i = 0; i < AX25_POSTAMBLE_LEN; i++){
    memcpy (out + out_idx, AX25_SYNC_FLAG_MAP_BIN, 8);
    out_idx += 8;
  }

  *out_len = out_idx;
  return AX25_ENC_OK;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void
ax25_decoder_reset(ax25_handle_t *h)
{
  h->state = AX25_NO_SYNC;
  h->decoded_num = 0;
  h->bit_cnt = 0;
  h->shift_reg = 0x0;
  h->dec_byte = 0x0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void
ax25_decoder_enter_sync(ax25_handle_t *h)
{
  h->state = AX25_IN_SYNC;
  h->decoded_num = 0;
  h->bit_cnt = 0;
  h->shift_reg = 0x0;
  h->dec_byte = 0x0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void
ax25_decoder_enter_frame_end(ax25_handle_t *h)
{
  h->state = AX25_FRAME_END;
  h->decoded_num = 0;
  h->bit_cnt = 0;
  h->shift_reg = 0x0;
  h->dec_byte = 0x0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * This function tries to find a valid AX.25 frame. Consecutive calls of this
 * function will continue the decoding of a frame.
 * @param h the AX.25 handle
 * @param out the output buffer
 * @param out_len the length of the decoded frame, if any
 * @param ax25_frame buffer containing the received bits
 * @param len the length of the \p ax25_frame buffer
 * @return AX25_DEC_NOT_READY if yet no AX.25 frame received or
 * AX25_DEC_OK if an AX.25 frame successfully retrieved.
 */
ax25_decode_status_t
ax25_decode (ax25_handle_t *h, uint8_t *out, size_t *out_len,
	     const uint8_t *ax25_frame, size_t len)
{
  size_t i;
  uint16_t fcs;
  uint16_t recv_fcs;
  uint16_t new_bit;

  for(i = 0; i < len * 8; i++){
    new_bit = (((ax25_frame[i/8] >> (i%8) ) & 0x1) << 7);
    h->shift_reg = (h->shift_reg >> 1) | new_bit;
    h->dec_byte = (h->dec_byte >> 1) | new_bit;

    switch(h->state){
      case AX25_NO_SYNC:
	if(h->shift_reg == AX25_SYNC_FLAG){
	  ax25_decoder_enter_sync(h);
	}
	break;
      case AX25_IN_SYNC:
	/*
	 * If the received byte was an AX.25 sync flag, there are two
	 * possibilities. Either it was the end of frame or just a repeat of the
	 * preamble.
	 *
	 * Also in case in error at the preamble, the G3RUH polynomial should
	 * re-sync after 3 repetitions of the SYNC flag. For this reason we demand
	 * that the distance between the last SYNC flag is greater than 3 bytes
	 */
	if(h->shift_reg == AX25_SYNC_FLAG){
	  if(h->decoded_num < 3){
	    ax25_decoder_enter_sync(h);
	  }
	  else{
	    /* This was the end of frame. Check the CRC*/
	    if(h->decoded_num > AX25_MIN_ADDR_LEN){
	      fcs = ax25_fcs(out, h->decoded_num - sizeof(uint16_t));
	      recv_fcs = ( ((uint16_t)out[h->decoded_num - 1]) << 8) |
		  out[h->decoded_num - 2];
	      if(recv_fcs == fcs){
		*out_len = h->decoded_num - sizeof(uint16_t);
		return AX25_DEC_OK;
	      }
	      else{
		return AX25_DEC_CRC_FAIL;
	      }
	    }
	    ax25_decoder_enter_frame_end(h);
	  }
	}
	else if ((h->shift_reg & 0xfc) == 0x7c) {
	  /*This was a stuffed bit */
	  h->dec_byte <<= 1;
	}
	else if((h->shift_reg & 0xfe) == 0xfe){
	  /* This is definitely an error */
	  ax25_decoder_reset(h);
	}
	else{
	  h->bit_cnt++;
	  if(h->bit_cnt == 8){
	    h->bit_cnt = 0;
	    out[h->decoded_num++] = h->dec_byte;

	    /* if the maximum allowed frame reached, restart */
	    if(h->decoded_num > AX25_MAX_FRAME_LEN){
	      ax25_decoder_reset(h);
	    }
	  }
	}
	break;
      case AX25_FRAME_END:
	/* Skip the trailing SYNC flags that may exist */
	if(h->shift_reg == AX25_SYNC_FLAG){
	  h->decoded_num = 0;
	  h->bit_cnt = 0;
	  h->shift_reg = 0x0;
	  h->dec_byte = 0x0;
	}
	else{
	  h->bit_cnt++;
	  if (h->bit_cnt/8 > 4) {
	    ax25_decoder_reset(h);
	  }
	}
	break;
      default:
	ax25_decoder_reset(h);
    }

  }
  return AX25_DEC_NOT_READY;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////







