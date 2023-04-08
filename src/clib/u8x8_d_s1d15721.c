/*

  u8x8_d_s1d15721.c

  240x64 display
  https://github.com/olikraus/u8g2/issues/1473
  http://datasheet.datasheetarchive.com/originals/library/Datasheets-ISS16/DSAIH000309343.pdf

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2020, olikraus@gmail.com

  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification,
  are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright notice, this list
    of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice, this
    list of conditions and the following disclaimer in the documentation and/or other
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include "u8x8.h"

#define LCD_CMD_DATA_WRITE 0x1D         // Display Data Write (n-bytes)
#define LCD_CMD_PAGE_ADDR 0xB1          // Page Address Set (2 byte)
#define LCD_CMD_COL_ADDR 0x13           // Column Address Set (2 byte)
#define LCD_CMD_DISPLAY_OFFON 0xAE      // Display ON/OFF: 0=Off/1=On                             // =ST7565
#define LCD_CMD_DISPLAY_REVERSE 0xA6    // Display/Normal: 0=Normal/1=Reverse                     // =ST7565
#define LCD_CMD_DISPLAY_ALL_LIGHT 0xA4  // Display All Lighting: 0=Normal/1=All On                // =ST7565
#define LCD_CMD_DISPLAY_START_LINE 0x8A // Display Start Line (2/3 byte)                          // !=
#define LCD_CMD_DISPLAY_MODE 0x66       // Display Mode (2 byte): 0=4 gray-scale, 1=binary        // !=
#define LCD_CMD_GRAY_SCALE_PATTERN 0x39 // Gray Scale Pattern (2 byte)                            // !=
#define LCD_CMD_COM_OUT_STATUS 0xC4     // Common Output Status Select: 0=Normal/1=Reverse        // !=
#define LCD_CMD_DUTY_SET 0x6D           // Duty Set Command (3 byte)
#define LCD_CMD_BUILTIN_OSC 0xAA        // Built-in Oscillator Circuit: [0]:0=Off/1=On
#define LCD_CMD_BUILTIN_OSC_FREQ 0x5F   // Built-in Oscillator Circuit Frequency Select (2 byte)
#define LCD_CMD_PWR_CONTROL 0x25        // Power Control Set (2 byte)
#define LCD_CMD_LC_DRIVE_VOLTAGE 0x2B   // Liquid Crystal Drive Voltage Select (2 byte)
#define LCD_CMD_EL_VOLUME 0x81          // Electronic Volume Set (2 byte)
#define LCD_CMD_RESET 0xE2              // Software Reset
#define LCD_COL_ADDR_SET_DIR 0xA0       // Compatible with display RAM address SEG output 0=Normal/1=Reverse

static const uint8_t u8x8_d_s1d15721_powersave0_seq[] = {
    U8X8_START_TRANSFER(),             /* enable chip, delay is part of the transfer start */
    U8X8_C(LCD_CMD_DISPLAY_OFFON | 1), /* display on */
    U8X8_END_TRANSFER(),               /* disable chip */
    U8X8_END()                         /* end of sequence */
};

static const uint8_t u8x8_d_s1d15721_powersave1_seq[] = {
    U8X8_START_TRANSFER(),         /* enable chip, delay is part of the transfer start */
    U8X8_C(LCD_CMD_DISPLAY_OFFON), /* display off, enter sleep mode */
    U8X8_END_TRANSFER(),           /* disable chip */
    U8X8_END()                     /* end of sequence */
};

// static const uint8_t u8x8_d_s1d15721_flip0_seq[] = {
//     U8X8_START_TRANSFER(), /* enable chip, delay is part of the transfer start */
//     U8X8_C(0xA6),          /* LCD Mapping */
//     U8X8_END_TRANSFER(),   /* disable chip */
//     U8X8_END()             /* end of sequence */
// };

// static const uint8_t u8x8_d_s1d15721_flip1_seq[] = {
//     U8X8_START_TRANSFER(), /* enable chip, delay is part of the transfer start */
//     U8X8_C(0xA7),          /* LCD Mapping */
//     U8X8_END_TRANSFER(),   /* disable chip */
//     U8X8_END()             /* end of sequence */
// };

static const uint8_t u8x8_d_s1d15721_flip0_seq[] = {
    U8X8_START_TRANSFER(),              /* enable chip, delay is part of the transfer start */
    U8X8_C(LCD_CMD_COM_OUT_STATUS | 1), /* common output status select reverse */
    U8X8_C(LCD_COL_ADDR_SET_DIR | 1),   /* column address set direction reverse */
    U8X8_END_TRANSFER(),                /* disable chip */
    U8X8_END()                          /* end of sequence */
};

static const uint8_t u8x8_d_s1d15721_flip1_seq[] = {
    U8X8_START_TRANSFER(),            /* enable chip, delay is part of the transfer start */
    U8X8_C(LCD_CMD_COM_OUT_STATUS),   /* common output status select normal */
    U8X8_C(LCD_COL_ADDR_SET_DIR | 1), /* column address set direction reverse */
    U8X8_END_TRANSFER(),              /* disable chip */
    U8X8_END()                        /* end of sequence */
};

static const uint8_t u8x8_d_s1d15721_flip2_seq[] = {
    U8X8_START_TRANSFER(),          /* enable chip, delay is part of the transfer start */
    U8X8_C(LCD_CMD_COM_OUT_STATUS), /* common output status select normal */
    U8X8_C(LCD_COL_ADDR_SET_DIR),   /* column address set direction normal */
    U8X8_END_TRANSFER(),            /* disable chip */
    U8X8_END()                      /* end of sequence */
};

static const uint8_t u8x8_d_s1d15721_flip3_seq[] = {
    U8X8_START_TRANSFER(),              /* enable chip, delay is part of the transfer start */
    U8X8_C(LCD_CMD_COM_OUT_STATUS | 1), /* common output status select reverse */
    U8X8_C(LCD_COL_ADDR_SET_DIR),       /* column address set direction normal */
    U8X8_END_TRANSFER(),                /* disable chip */
    U8X8_END()                          /* end of sequence */

};
uint8_t u8x8_d_s1d15721_common(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  uint8_t x, y, c;
  uint8_t *ptr;
  switch (msg)
  {
  case U8X8_MSG_DISPLAY_DRAW_TILE:
    u8x8_cad_StartTransfer(u8x8);

    x = ((u8x8_tile_t *)arg_ptr)->x_pos;
    x += u8x8->x_offset;
    x *= 8;

    u8x8_cad_SendCmd(u8x8, LCD_CMD_PAGE_ADDR); // Page Address - Row
    u8x8_cad_SendArg(u8x8, (((u8x8_tile_t *)arg_ptr)->y_pos));

    y = ((u8x8_tile_t *)arg_ptr)->y_pos;

    u8x8_cad_SendCmd(u8x8, LCD_CMD_COL_ADDR); /* col */
    u8x8_cad_SendArg(u8x8, x);

    /* 4 Mar 2022: added the missing page address, issue 1802 */
    u8x8_cad_SendCmd(u8x8, LCD_CMD_PAGE_ADDR); /* page address */
    u8x8_cad_SendArg(u8x8, y);

    u8x8_cad_SendCmd(u8x8, LCD_CMD_DATA_WRITE); // Data Write

    c = ((u8x8_tile_t *)arg_ptr)->cnt;
    c *= 8;
    ptr = ((u8x8_tile_t *)arg_ptr)->tile_ptr;
    do
    {
      u8x8_cad_SendData(u8x8, c, ptr); /* note: SendData can not handle more than 255 bytes */
      arg_int--;
    } while (arg_int > 0);

    u8x8_cad_EndTransfer(u8x8);
    break;
  case U8X8_MSG_DISPLAY_SET_POWER_SAVE:
    if (arg_int == 0)
      u8x8_cad_SendSequence(u8x8, u8x8_d_s1d15721_powersave0_seq);
    else
      u8x8_cad_SendSequence(u8x8, u8x8_d_s1d15721_powersave1_seq);
    break;
  case U8X8_MSG_DISPLAY_SET_FLIP_MODE:
    if (arg_int == 0)
    {
      u8x8_cad_SendSequence(u8x8, u8x8_d_s1d15721_flip0_seq);
      u8x8->x_offset = u8x8->display_info->default_x_offset;
    }
    else
    {
      u8x8_cad_SendSequence(u8x8, u8x8_d_s1d15721_flip1_seq);
      u8x8->x_offset = u8x8->display_info->flipmode_x_offset;
    }
    break;
#ifdef U8X8_WITH_SET_CONTRAST
  case U8X8_MSG_DISPLAY_SET_CONTRAST:
    break;
#endif
  default:
    return 0;
  }
  return 1;
}

/*================================================*/
/* s1d15721 240x64 */

static const u8x8_display_info_t u8x8_s1d15721_240x64_display_info =
    {
        /* chip_enable_level = */ 0, /* low active CS */
        /* chip_disable_level = */ 1,

        /* post_chip_enable_wait_ns = */ 10, /* */
        /* pre_chip_disable_wait_ns = */ 20, /* */
        /* reset_pulse_width_ms = */ 1,      /* */
        /* post_reset_wait_ms = */ 10,
        /* sda_setup_time_ns = */ 30,    /*  */
        /* sck_pulse_width_ns = */ 65,   /* half of cycle time  */
        /* sck_clock_hz = */ 90000000UL, /* since Arduino 1.6.0, the SPI bus speed in Hz. Should be  1000000000/sck_pulse_width_ns */
        /* spi_mode = */ 0,              /* active high, rising edge */
        /* i2c_bus_clock_100kHz = */ 4,
        /* data_setup_time_ns = */ 30,   /*  */
        /* write_pulse_width_ns = */ 65, /* */
        /* tile_width = */ 30,           /* width of 20*8=160 pixel (30*8 = 240) */
        /* tile_height = */ 8,           /* height 8*8 = 64*/
        /* default_x_offset = */ 1,
        /* flipmode_x_offset = */ 1,
        /* pixel_width = */ 240,
        /* pixel_height = */ 64};

static const uint8_t u8x8_d_s1d15721_240x64_init_seq[] = {
    U8X8_START_TRANSFER(),                     /* enable chip, delay is part of the transfer start */
    U8X8_C(LCD_CMD_COM_OUT_STATUS | 1),        /* (5) Common Output Status (Reverse) */
    U8X8_C(LCD_CMD_DISPLAY_REVERSE | 0),       /* (3) Display Normal Reverse (Normal) */
    U8X8_C(LCD_CMD_DISPLAY_ALL_LIGHT | 0),     /* (4) Display All Light (Normal) */
    U8X8_CAA(LCD_CMD_DUTY_SET, 0x11, 0x02),    /* (18) Duty Set Command */
    U8X8_CA(LCD_CMD_DISPLAY_MODE, 0x01),       /* (15) Display Mode, Parameter 0 (0 = Gray Scale 1 = Binary */
    U8X8_CA(LCD_CMD_GRAY_SCALE_PATTERN, 0x36), /* (16) Gray Scale Pattern Set, Pattern */
    U8X8_CA(LCD_CMD_LC_DRIVE_VOLTAGE, 0x07),   /* (27) LCD Drive Mode Voltage Select, Parameter */
    U8X8_CA(LCD_CMD_EL_VOLUME, 0x10),          /* (28) Electronic Volume, Parameter */
    U8X8_CA(LCD_CMD_BUILTIN_OSC_FREQ, 0x00),   /* (24) Built-in Oscillator Frequency, Parameter  */
    U8X8_C(LCD_CMD_BUILTIN_OSC | 1),           /* (23) Built-in OSC On */
    U8X8_CA(LCD_CMD_PWR_CONTROL, 0x1F),        /* (25) Power Control Set, Parameter  */
    U8X8_CA(LCD_CMD_DISPLAY_START_LINE, 0x00), /* (6) Start Line Setup, Parameter  */
    U8X8_CA(LCD_CMD_PAGE_ADDR, 0x00),          /* (7) Page Address Set, Parameter  */
    U8X8_CA(LCD_CMD_COL_ADDR, 0x00),           /* (8) Column Address Set  */
    U8X8_C(LCD_CMD_DISPLAY_OFFON | 1),         /* (1) Display ON/OFF */
    U8X8_END_TRANSFER(),                       /* disable chip */
    U8X8_END()                                 /* end of sequence */
};

uint8_t u8x8_d_s1d15721_240x64(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{

  /* checking for the flip mode cmd first */
  if (msg == U8X8_MSG_DISPLAY_SET_FLIP_MODE)
  {
    if (arg_int == 0)
    {
      u8x8_cad_SendSequence(u8x8, u8x8_d_s1d15721_flip0_seq);
      u8x8->x_offset = u8x8->display_info->default_x_offset;
    }
    else
    {
      u8x8_cad_SendSequence(u8x8, u8x8_d_s1d15721_flip1_seq);
      u8x8->x_offset = u8x8->display_info->flipmode_x_offset;
    }
    return 1;
  }
  /* call the common procedure, this now leads to the effect, that the flip code is executed again */
  /* maybe we should paste the common code here to avoid this */

  if (u8x8_d_s1d15721_common(u8x8, msg, arg_int, arg_ptr) == 0)
  {
    /* msg not handled, then try here */
    switch (msg)
    {
    case U8X8_MSG_DISPLAY_SETUP_MEMORY:
      u8x8_d_helper_display_setup_memory(u8x8, &u8x8_s1d15721_240x64_display_info);
      break;
    case U8X8_MSG_DISPLAY_INIT:
      u8x8_d_helper_display_init(u8x8);
      u8x8_cad_SendSequence(u8x8, u8x8_d_s1d15721_240x64_init_seq);
      break;
    default:
      return 0; /* msg unknown */
    }
  }
  return 1;
}
