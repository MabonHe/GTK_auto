#ifndef __FORMAT_H__
#define __FORMAT_H__

const char *format2string(unsigned int format);
unsigned int string2format(const char *str);
bool is_valid(unsigned int format);
bool is_raw(unsigned int format);
int get_bpp(unsigned int format);
void RGB2YUV(unsigned short R, unsigned short G, unsigned short B, unsigned char* Y, unsigned char* U, unsigned char* V);
void convert_bayer_block(unsigned int x, unsigned int y,
    unsigned int width, unsigned int height,
    unsigned short bayer_data[4], unsigned char *out_buf,
    unsigned int src_fmt, unsigned int dst_fmt);
bool convert_format(unsigned char *in_buf,
    unsigned int width, unsigned int height,
    unsigned char **out_buf, unsigned int *out_len,
    unsigned int src_fmt, unsigned int dst_fmt);

#endif
