#ifndef _BASE64_CODEC_H_
#define _BASE64_CODEC_H_

/**
 * Calculate the output size needed to base64-encode in_size bytes.
 */
#define OUT_ENBASE64_SIZE(in_size)      (((in_size) + 2) / 3 * 4 + 1)

/**
 * Calculate the output size needed to base64-decode in_size bytes.
 */
#define OUT_DEBASE64_SIZE(in_size)      ((in_size) * 3 / 4)

/**
 * Encode data to base64 and null-terminate.
 *
 * @param out       buffer for encoded data
 * @param out_size  size in bytes of the output buffer, must be at
 *                  least OUT_ENBASE64_SIZE(in_size)
 * @param in        input data buffer
 * @param in_size   size in bytes of the 'in' buffer
 * @return          number of bytes written, zero for null data or error happen
 */
extern int base64_encode(char *out, int out_size, const unsigned char *in, int in_size);

/**
 * Decode a base64-encoded string.
 *
 * @param out       buffer for decoded data
 * @param out_size  size in bytes of the out buffer, must be at
 *                  least 3/4 of the length of in
 * @param in        null-terminated input string
 * @return          number of bytes written, zero for null data or error happen
 */
extern int base64_decode(unsigned char *out, int out_size, const char *in);

#endif /* _BASE64_CODEC_H_ */

