#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "format.h"

#include <linux/videodev2.h>

const char *format2string(unsigned format)
{
    switch (format) {
    case V4L2_PIX_FMT_SRGGB8: return "RGGB8";
    case V4L2_PIX_FMT_SGRBG8: return "GRBG8";
    case V4L2_PIX_FMT_SGBRG8: return "GBRG8";
    case V4L2_PIX_FMT_SBGGR8: return "BGGR8";
    case V4L2_PIX_FMT_SRGGB10: return "RGGB10";
    case V4L2_PIX_FMT_SGRBG10: return "GRBG10";
    case V4L2_PIX_FMT_SGBRG10: return "GBRG10";
    case V4L2_PIX_FMT_SBGGR10: return "BGGR10";
    case V4L2_PIX_FMT_NV12: return "NV12";
    case V4L2_PIX_FMT_YUYV: return "YUYV";
    case V4L2_PIX_FMT_UYVY: return "UYVY";
    default: return "Unknown";
    }
    return "Unknown";
}

unsigned int string2format(const char *str)
{
    if (strcmp(str, "RGGB8") == 0)
        return V4L2_PIX_FMT_SRGGB8;
    if (strcmp(str, "GRBG8") == 0)
        return V4L2_PIX_FMT_SGRBG8;
    if (strcmp(str, "GBRG8") == 0)
        return V4L2_PIX_FMT_SGBRG8;
    if (strcmp(str, "BGGR8") == 0)
        return V4L2_PIX_FMT_SBGGR8;
    if (strcmp(str, "RGGB10") == 0)
        return V4L2_PIX_FMT_SRGGB10;
    if (strcmp(str, "GRBG10") == 0)
        return V4L2_PIX_FMT_SGRBG10;
    if (strcmp(str, "GBRG10") == 0)
        return V4L2_PIX_FMT_SGBRG10;
    if (strcmp(str, "BGGR10") == 0)
        return V4L2_PIX_FMT_SBGGR10;
    if (strcmp(str, "NV12") == 0)
        return V4L2_PIX_FMT_NV12;
    if (strcmp(str, "YUYV") == 0)
        return V4L2_PIX_FMT_YUYV;
    if (strcmp(str, "UYVY") == 0)
        return V4L2_PIX_FMT_UYVY;
    return 0;
}

bool is_valid(unsigned int format)
{
    switch (format) {
    case V4L2_PIX_FMT_SRGGB8:
    case V4L2_PIX_FMT_SGRBG8:
    case V4L2_PIX_FMT_SGBRG8:
    case V4L2_PIX_FMT_SBGGR8:
    case V4L2_PIX_FMT_SRGGB10:
    case V4L2_PIX_FMT_SGRBG10:
    case V4L2_PIX_FMT_SGBRG10:
    case V4L2_PIX_FMT_SBGGR10:
    case V4L2_PIX_FMT_NV12:
    case V4L2_PIX_FMT_YUYV:
    case V4L2_PIX_FMT_UYVY:
        return true;
    default:
        return false;
    }
}

bool is_raw(unsigned int format)
{
    switch (format) {
    case V4L2_PIX_FMT_SRGGB8:
    case V4L2_PIX_FMT_SGRBG8:
    case V4L2_PIX_FMT_SGBRG8:
    case V4L2_PIX_FMT_SBGGR8:
    case V4L2_PIX_FMT_SRGGB10:
    case V4L2_PIX_FMT_SGRBG10:
    case V4L2_PIX_FMT_SGBRG10:
    case V4L2_PIX_FMT_SBGGR10:
        return true;
    default:
        return false;
    }
}

int get_bpp(unsigned int format)
{
    switch (format) {
    case V4L2_PIX_FMT_SRGGB8: return 8;
    case V4L2_PIX_FMT_SGRBG8: return 8;
    case V4L2_PIX_FMT_SGBRG8: return 8;
    case V4L2_PIX_FMT_SBGGR8: return 8;
    case V4L2_PIX_FMT_SRGGB10: return 16;
    case V4L2_PIX_FMT_SGRBG10: return 16;
    case V4L2_PIX_FMT_SGBRG10: return 16;
    case V4L2_PIX_FMT_SBGGR10: return 16;
    case V4L2_PIX_FMT_NV12: return 12;
    case V4L2_PIX_FMT_YUYV: return 16;
    case V4L2_PIX_FMT_UYVY: return 16;
    default: return 16;
    }
}

void RGB2YUV(unsigned short R, unsigned short G, unsigned short B, unsigned char *Y, unsigned char *U, unsigned char *V)
{
    int Rp, Gp, Bp;
    int oY, oU, oV;
    Rp = R; Gp = G; Bp = B;
    oY = (257 * Rp + 504 * Gp + 98 * Bp) / 4000 + 16;
    oU = (-148 * Rp - 291 * Gp + 439 * Bp) / 4000 + 128;
    oV = (439 * Rp - 368 * Gp - 71 * Bp) / 4000 + 128;
    if (oY > 255) oY = 255;
    if (oY < 0) oY = 0;
    if (oU > 255) oU = 255;
    if (oU < 0) oU = 0;
    if (oV > 255) oV = 255;
    if (oV < 0) oV = 0;
    *Y = (unsigned char)oY;
    *U = (unsigned char)oU;
    *V = (unsigned char)oV;
}

void YUV2RGB(unsigned char Y, unsigned char U, unsigned char V, unsigned short *R, unsigned short *G, unsigned short *B)
{
    int Yp, Up, Vp, Ypp;
    int oR, oG, oB;
    Yp = Y - 16;
    Up = (U - 128);
    Vp = (V - 128);
    Ypp = 9535 * Yp;

    oB = (Ypp + 16531 * Up) >> 11;
    oG = (Ypp - 6660 * Vp - 3203 * Up) >> 11;
    oR = (Ypp + 13074 * Vp) >> 11;
    if (oR > 1023) oR = 1023;
    if (oR < 0) oR = 0;
    if (oG > 1023) oG = 1023;
    if (oG < 0) oG = 0;
    if (oB > 1023) oB = 1023;
    if (oB < 0) oB = 0;
    *R = (unsigned short)oR;
    *G = (unsigned short)oG;
    *B = (unsigned short)oB;
}

void convert_bayer_block(unsigned int x, unsigned int y,
    unsigned int width, unsigned int height,
    unsigned short bayer_data[4], unsigned char *out_buf,
    unsigned int src_fmt, unsigned int dst_fmt)
{
    unsigned char *Ybase;
    unsigned char *UVbase;
    unsigned char Y, U, V;
    unsigned short R, Gr, Gb, B;

    switch (src_fmt) {
    case V4L2_PIX_FMT_SRGGB8: R = bayer_data[0] << 2; Gr = bayer_data[1] << 2; Gb = bayer_data[2] << 2; B = bayer_data[3] << 2; break;
    case V4L2_PIX_FMT_SRGGB10: R = bayer_data[0]; Gr = bayer_data[1]; Gb = bayer_data[2]; B = bayer_data[3]; break;
    case V4L2_PIX_FMT_SGRBG8: Gr = bayer_data[0] << 2; R = bayer_data[1] << 2; B = bayer_data[2] << 2; Gb = bayer_data[3] << 2; break;
    case V4L2_PIX_FMT_SGRBG10: Gr = bayer_data[0]; R = bayer_data[1]; B = bayer_data[2]; Gb = bayer_data[3]; break;
    case V4L2_PIX_FMT_SGBRG8: Gb = bayer_data[0] << 2; B = bayer_data[1] << 2; R = bayer_data[2] << 2; Gr = bayer_data[3] << 2; break;
    case V4L2_PIX_FMT_SGBRG10: Gb = bayer_data[0]; B = bayer_data[1]; R = bayer_data[2]; Gr = bayer_data[3]; break;
    case V4L2_PIX_FMT_SBGGR8: B = bayer_data[0] << 2; Gb = bayer_data[1] << 2; Gr = bayer_data[2] << 2; R = bayer_data[3] << 2; break;
    case V4L2_PIX_FMT_SBGGR10: B = bayer_data[0]; Gb = bayer_data[1]; Gr = bayer_data[2]; R = bayer_data[3]; break;
    default: return;
    }

    switch (dst_fmt) {
    case V4L2_PIX_FMT_SRGGB8:
        out_buf[y * width + x] = (R >> 2);
        out_buf[y * width + x + 1] = (Gr >> 2);
        out_buf[(y + 1) * width + x] = (Gb >> 2);
        out_buf[(y + 1) * width + x + 1] = (B >> 2);
        break;
    case V4L2_PIX_FMT_SGRBG8:
        out_buf[y * width + x] = (Gr >> 2);
        out_buf[y * width + x + 1] = (R >> 2);
        out_buf[(y + 1) * width + x] = (B >> 2);
        out_buf[(y + 1) * width + x + 1] = (Gb >> 2);
        break;
    case V4L2_PIX_FMT_SGBRG8:
        out_buf[y * width + x] = (Gb >> 2);
        out_buf[y * width + x + 1] = (B >> 2);
        out_buf[(y + 1) * width + x] = (R >> 2);
        out_buf[(y + 1) * width + x + 1] = (Gr >> 2);
        break;
    case V4L2_PIX_FMT_SBGGR8:
        out_buf[y * width + x] = (B >> 2);
        out_buf[y * width + x + 1] = (Gb >> 2);
        out_buf[(y + 1) * width + x] = (Gr >> 2);
        out_buf[(y + 1) * width + x + 1] = (R >> 2);
        break;
    case V4L2_PIX_FMT_SRGGB10:
        *((unsigned short *)out_buf + y * width + x) = R;
        *((unsigned short *)out_buf + y * width + x + 1) = Gr;
        *((unsigned short *)out_buf + (y + 1) * width + x) = Gb;
        *((unsigned short *)out_buf + (y + 1) * width + x + 1) = B;
        break;
    case V4L2_PIX_FMT_SGRBG10:
        *((unsigned short *)out_buf + y * width + x) = Gr;
        *((unsigned short *)out_buf + y * width + x + 1) = R;
        *((unsigned short *)out_buf + (y + 1) * width + x) = B;
        *((unsigned short *)out_buf + (y + 1) * width + x + 1) = Gb;
        break;
    case V4L2_PIX_FMT_SGBRG10:
        *((unsigned short *)out_buf + y * width + x) = Gb;
        *((unsigned short *)out_buf + y * width + x + 1) = B;
        *((unsigned short *)out_buf + (y + 1) * width + x) = R;
        *((unsigned short *)out_buf + (y + 1) * width + x + 1) = Gr;
        break;
    case V4L2_PIX_FMT_SBGGR10:
        *((unsigned short *)out_buf + y * width + x) = B;
        *((unsigned short *)out_buf + y * width + x + 1) = Gb;
        *((unsigned short *)out_buf + (y + 1) * width + x) = Gr;
        *((unsigned short *)out_buf + (y + 1) * width + x + 1) = R;
        break;
    case V4L2_PIX_FMT_NV12:
        Ybase = out_buf;
        UVbase = Ybase + width * height;
        RGB2YUV(R, (Gr + Gb) / 2, B, &Y, &U, &V);
        Ybase[y * width + x] = Ybase[y * width + x + 1] = Ybase[(y + 1) * width + x] = Ybase[(y + 1) * width + x + 1] = Y;
        UVbase[y / 2 * width + x / 2 * 2] = U;
        UVbase[y / 2 * width + x / 2 * 2 + 1] = V;
        break;
    case V4L2_PIX_FMT_UYVY:
        RGB2YUV(R, (Gr + Gb) / 2, B, &Y, &U, &V);
        out_buf[y * width * 2 + x * 2] = U;
        out_buf[y * width * 2 + x * 2 + 1] = Y;
        out_buf[y * width * 2 + x * 2 + 2] = V;
        out_buf[y * width * 2 + x * 2 + 3] = Y;
        out_buf[(y + 1) * width * 2 + x * 2] = U;
        out_buf[(y + 1) * width * 2 + x * 2 + 1] = Y;
        out_buf[(y + 1) * width * 2 + x * 2 + 2] = V;
        out_buf[(y + 1) * width * 2 + x * 2 + 3] = Y;
        break;
    case V4L2_PIX_FMT_YUYV:
        RGB2YUV(R, (Gr + Gb) / 2, B, &Y, &U, &V);
        out_buf[y * width * 2 + x * 2] = Y;
        out_buf[y * width * 2 + x * 2 + 1] = U;
        out_buf[y * width * 2 + x * 2 + 2] = Y;
        out_buf[y * width * 2 + x * 2 + 3] = V;
        out_buf[(y + 1) * width * 2 + x * 2] = Y;
        out_buf[(y + 1) * width * 2 + x * 2 + 1] = U;
        out_buf[(y + 1) * width * 2 + x * 2 + 2] = Y;
        out_buf[(y + 1) * width * 2 + x * 2 + 3] = V;
        break;
    default:
        break;
    }
}

void convert_yuv_block(unsigned int x, unsigned int y,
    unsigned int width, unsigned int height,
    unsigned char *in_buf, unsigned char *out_buf,
    unsigned int src_fmt, unsigned int dst_fmt)
{
    unsigned char *YBase;
    unsigned char *UVBase;
    unsigned char Y[4];
    unsigned char U[4];
    unsigned char V[4];
    unsigned short R, G, B;

    switch (src_fmt) {
    case V4L2_PIX_FMT_NV12:
        YBase = in_buf;
        UVBase = in_buf + width * height;
        Y[0] = YBase[y * width + x];
        Y[1] = YBase[y * width + x + 1];
        Y[2] = YBase[(y + 1) * width + x];
        Y[3] = YBase[(y + 1) * width + x + 1];
        U[0] = U[1] = U[2] = U[3] = UVBase[y / 2 * width + x / 2 * 2];
        V[0] = V[1] = V[2] = V[3] = UVBase[y / 2 * width + x / 2 * 2 + 1];
        break;
    case V4L2_PIX_FMT_UYVY:
        Y[0] = in_buf[y * width * 2 + x * 2 + 1];
        Y[1] = in_buf[y * width * 2 + x * 2 + 3];
        Y[2] = in_buf[(y + 1) * width * 2 + x * 2 + 1];
        Y[3] = in_buf[(y + 1) * width * 2 + x * 2 + 3];
        U[0] = U[1] = in_buf[y * width * 2 + x * 2];
        U[2] = U[3] = in_buf[(y + 1) * width * 2 + x * 2];
        V[0] = V[1] = in_buf[y * width * 2 + x * 2 + 2];
        V[2] = V[3] = in_buf[(y + 1) * width * 2 + x * 2 + 2];
        break;
    case V4L2_PIX_FMT_YUYV:
        Y[0] = in_buf[y * width * 2 + x * 2];
        Y[1] = in_buf[y * width * 2 + x * 2 + 2];
        Y[2] = in_buf[(y + 1) * width * 2 + x * 2];
        Y[3] = in_buf[(y + 1) * width * 2 + x * 2 + 2];
        U[0] = U[1] = in_buf[y * width * 2 + x * 2 + 1];
        U[2] = U[3] = in_buf[(y + 1) * width * 2 + x * 2 + 1];
        V[0] = V[1] = in_buf[y * width * 2 + x * 2 + 3];
        V[2] = V[3] = in_buf[(y + 1) * width * 2 + x * 2 + 3];
        break;
    default:
        return;
    }

    switch (dst_fmt) {
    case V4L2_PIX_FMT_NV12:
        YBase = out_buf;
        UVBase = out_buf + width * height;
        YBase[y * width + x] = Y[0];
        YBase[y * width + x + 1] = Y[1];
        YBase[(y + 1) * width + x] = Y[2];
        YBase[(y + 1) * width + x + 1] = Y[3];
        UVBase[y / 2 * width + x / 2 * 2] = U[0];
        UVBase[y / 2 * width + x / 2 * 2 + 1] = V[0];
        break;
    case V4L2_PIX_FMT_UYVY:
        out_buf[y * width * 2 + x * 2 + 1] = Y[0];
        out_buf[y * width * 2 + x * 2 + 3] = Y[1];
        out_buf[(y + 1) * width * 2 + x * 2 + 1] = Y[2];
        out_buf[(y + 1) * width * 2 + x * 2 + 3] = Y[3];
        out_buf[y * width * 2 + x * 2] = U[0];
        out_buf[(y + 1) * width * 2 + x * 2] = U[2];
        out_buf[y * width * 2 + x * 2 + 2] = V[0];
        out_buf[(y + 1) * width * 2 + x * 2 + 2] = V[2];
        break;
    case V4L2_PIX_FMT_YUYV:
        out_buf[y * width * 2 + x * 2] = Y[0];
        out_buf[y * width * 2 + x * 2 + 2] = Y[1];
        out_buf[(y + 1) * width * 2 + x * 2] = Y[2];
        out_buf[(y + 1) * width * 2 + x * 2 + 2] = Y[3];
        out_buf[y * width * 2 + x * 2 + 1] = U[0];
        out_buf[(y + 1) * width * 2 + x * 2 + 1] = U[2];
        out_buf[y * width * 2 + x * 2 + 3] = V[0];
        out_buf[(y + 1) * width * 2 + x * 2 + 3] = V[2];
        break;
    case V4L2_PIX_FMT_SRGGB8:
        YUV2RGB(Y[0], U[0], V[0], &R, &G, &B);
        out_buf[y * width + x] = (R >> 2);
        out_buf[y * width + x + 1] = (G >> 2);
        out_buf[(y + 1) * width + x] = (G >> 2);
        out_buf[(y + 1) * width + x + 1] = (B >> 2);
        break;
    case V4L2_PIX_FMT_SGRBG8:
        YUV2RGB(Y[0], U[0], V[0], &R, &G, &B);
        out_buf[y * width + x] = (G >> 2);
        out_buf[y * width + x + 1] = (R >> 2);
        out_buf[(y + 1) * width + x] = (B >> 2);
        out_buf[(y + 1) * width + x + 1] = (G >> 2);
        break;
    case V4L2_PIX_FMT_SGBRG8:
        YUV2RGB(Y[0], U[0], V[0], &R, &G, &B);
        out_buf[y * width + x] = (G >> 2);
        out_buf[y * width + x + 1] = (B >> 2);
        out_buf[(y + 1) * width + x] = (R >> 2);
        out_buf[(y + 1) * width + x + 1] = (G >> 2);
        break;
    case V4L2_PIX_FMT_SBGGR8:
        YUV2RGB(Y[0], U[0], V[0], &R, &G, &B);
        out_buf[y * width + x] = (B >> 2);
        out_buf[y * width + x + 1] = (G >> 2);
        out_buf[(y + 1) * width + x] = (G >> 2);
        out_buf[(y + 1) * width + x + 1] = (R >> 2);
        break;
    case V4L2_PIX_FMT_SRGGB10:
        YUV2RGB(Y[0], U[0], V[0], &R, &G, &B);
        *((unsigned short *)out_buf + y * width + x) = R;
        *((unsigned short *)out_buf + y * width + x + 1) = G;
        *((unsigned short *)out_buf + (y + 1) * width + x) = G;
        *((unsigned short *)out_buf + (y + 1) * width + x + 1) = B;
        break;
    case V4L2_PIX_FMT_SGRBG10:
        YUV2RGB(Y[0], U[0], V[0], &R, &G, &B);
        *((unsigned short *)out_buf + y * width + x) = G;
        *((unsigned short *)out_buf + y * width + x + 1) = R;
        *((unsigned short *)out_buf + (y + 1) * width + x) = B;
        *((unsigned short *)out_buf + (y + 1) * width + x + 1) = G;
        break;
    case V4L2_PIX_FMT_SGBRG10:
        YUV2RGB(Y[0], U[0], V[0], &R, &G, &B);
        *((unsigned short *)out_buf + y * width + x) = G;
        *((unsigned short *)out_buf + y * width + x + 1) = B;
        *((unsigned short *)out_buf + (y + 1) * width + x) = R;
        *((unsigned short *)out_buf + (y + 1) * width + x + 1) = G;
        break;
    case V4L2_PIX_FMT_SBGGR10:
        YUV2RGB(Y[0], U[0], V[0], &R, &G, &B);
        *((unsigned short *)out_buf + y * width + x) = B;
        *((unsigned short *)out_buf + y * width + x + 1) = G;
        *((unsigned short *)out_buf + (y + 1) * width + x) = G;
        *((unsigned short *)out_buf + (y + 1) * width + x + 1) = R;
        break;
    default:
        return;
    }
}

bool convert_format(unsigned char *in_buf,
    unsigned int width, unsigned int height,
    unsigned char **out_buf, unsigned int *out_len,
    unsigned int src_fmt, unsigned int dst_fmt)
{
    unsigned int x, y;
    unsigned short bayer_data[4];

    *out_len = width * height * get_bpp(dst_fmt) / 8;
    *out_buf = (unsigned char *) malloc(*out_len);

    if (!*out_buf) {
        printf("[%s] Out of memory.\n", __FUNCTION__);
        return false;
    }

    for (y = 0; y < height; y += 2) {
        for (x = 0; x < width; x += 2) {
            if (is_raw(src_fmt)) {
                if (get_bpp(src_fmt) == 8) {
                    bayer_data[0] = in_buf[y * width + x];
                    bayer_data[1] = in_buf[y * width + x + 1];
                    bayer_data[2] = in_buf[(y + 1) * width + x];
                    bayer_data[3] = in_buf[(y + 1) * width + x + 1];
                } else {
                    bayer_data[0] = *((unsigned short *)in_buf + y * width + x);
                    bayer_data[1] = *((unsigned short *)in_buf + y * width + x + 1);
                    bayer_data[2] = *((unsigned short *)in_buf + (y + 1) * width + x);
                    bayer_data[3] = *((unsigned short *)in_buf + (y + 1) * width + x + 1);
                }
                convert_bayer_block(x, y, width, height, bayer_data, *out_buf, src_fmt, dst_fmt);
            } else {
                convert_yuv_block(x, y, width, height, in_buf, *out_buf, src_fmt, dst_fmt);
            }
        }
    }

    return true;
}


