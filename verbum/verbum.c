/*
$Id: verbum.c 2 2005-05-12 02:31:14Z eric $
Copyright 2005 Eric L. Smith <eric@brouhaha.com>

Verbum is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.  Note that I am not
granting permission to redistribute or modify Verbum under the terms
of any later version of the General Public License.

Verbum is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program (in the file "COPYING"); if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111, USA.
*/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#define DEFAULT_SERIAL_PORT "/dev/ttyS5"
#define DEFAULT_BIT_RATE 2400

#define LEADER_LENGTH 20 //80
#define GAP_LENGTH 20 //40
#define E9_LENGTH 20
#define TRAILER_LENGTH 80
#define INTERCHARACTER_SPACING 2


#define _____ 0x00
#define ____X 0x01
#define ___X_ 0x02
#define ___XX 0x03
#define __X__ 0x04
#define __X_X 0x05
#define __XX_ 0x06
#define __XXX 0x07
#define _X___ 0x08
#define _X__X 0x09
#define _X_X_ 0x0a
#define _X_XX 0x0b
#define _XX__ 0x0c
#define _XX_X 0x0d
#define _XXX_ 0x0e
#define _XXXX 0x0f
#define X____ 0x10
#define X___X 0x11
#define X__X_ 0x12
#define X__XX 0x13
#define X_X__ 0x14
#define X_X_X 0x15
#define X_XX_ 0x16
#define X_XXX 0x17
#define XX___ 0x18
#define XX__X 0x19
#define XX_X_ 0x1a
#define XX_XX 0x1b
#define XXX__ 0x1c
#define XXX_X 0x1d
#define XXXX_ 0x1e
#define XXXXX 0x1f


uint8_t chargen [64][7] =
  {
    { // 20 - space
      _____,
      _____,
      _____,
      _____,
      _____,
      _____,
      _____
    },
    { // 21 - bang
      __X__,
      __X__,
      __X__,
      __X__,
      __X__,
      _____,
      __X__,
    },
    { // 22 - double quote
      _X_X_,
      _X_X_,
      _____,
      _____,
      _____,
      _____,
      _____
    },
    { // 23 - octothorpe
      _X_X_,
      _X_X_,
      XXXXX,
      _X_X_,
      XXXXX,
      _X_X_,
      _X_X_
    },
    { // 24 - dollar sign
      __X__,
      _XXXX,
      X_X__,
      _XXX_,
      __X_X,
      XXXX_,
      __X__,
    },
    { // 25 - percent sign
      XX___,
      XX__X,
      ___X_,
      __X__,
      _X___,
      X__XX,
      ___XX,
    },
    { // 26 - ampersand
      _X___,
      X_X__,
      X_X__,
      _X___,
      X_X_X,
      X__X_,
      _XX_X,
    },
    { // 27 - single quote
      __X__,
      __X__,
      __X__,
      _____,
      _____,
      _____,
      _____
    },
    { // 28 - left parenthesis
      ___X_,
      __X__,
      _X___,
      _X___,
      _X___,
      __X__,
      ___X_
    },
    { // 29 - right parenthesis
      _X___,
      __X__,
      ___X_,
      ___X_,
      ___X_,
      __X__,
      _X___
    },
    { // 2a - splat
      __X__,
      X_X_X,
      _XXX_,
      __X__,
      _XXX_,
      X_X_X,
      __X__
    },
    { // 2b - plus
      _____,
      __X__,
      __X__,
      XXXXX,
      __X__,
      __X__,
      _____
    },
    { // 2c - comma
      _____,
      _____,
      _____,
      _____,
      __X__,
      __X__,
      _X___
    },
    { // 2d - hyphen
      _____,
      _____,
      _____,
      XXXXX,
      _____,
      _____,
      _____
    },
    { // 2e - period
      _____,
      _____,
      _____,
      _____,
      _____,
      _____,
      __X__
    },
    { // 2f - slash
      _____,
      ____X,
      ___X_,
      __X__,
      _X___,
      X____,
      _____
    },
    { // 30 - zero
      _XXX_,
      X___X,
      X__XX,
      X_X_X,
      XX__X,
      X___X,
      _XXX_
    },
    { // 31 - one
      __X__,
      _XX__,
      __X__,
      __X__,
      __X__,
      __X__,
      _XXX_
    },
    { // 32 - two
      _XXX_,
      X___X,
      ____X,
      __XX_,
      _X___,
      X____,
      XXXXX
    },
    { // 33 - three
      XXXXX,
      ____X,
      ___X_,
      __XX_,
      ____X,
      X___X,
      _XXX_
    },
    { // 34 - four
      ___X_,
      __XX_,
      _X_X_,
      X__X_,
      XXXXX,
      ___X_,
      ___X_
    },
    { // 35 - five
      XXXXX,
      X____,
      XXXX_,
      ____X,
      ____X,
      X___X,
      _XXX_
    },
    { // 36 - six
      __XXX,
      _X___,
      X____,
      XXXX_,
      X___X,
      X___X,
      _XXX_
    },
    { // 37 - seven
      XXXXX,
      ____X,
      ___X_,
      __X__,
      _X___,
      _X___,
      _X___
    },
    { // 38 - eight
      _XXX_,
      X___X,
      X___X,
      _XXX_,
      X___X,
      X___X,
      _XXX_
    },
    { // 39 - nine
      _XXX_,
      X___X,
      X___X,
      _XXXX,
      ____X,
      ___X_,
      XXX__
    },
    { // 3a - colon
      _____,
      _____,
      __X__,
      _____,
      __X__,
      _____,
      _____
    },
    { // 3b - semicolon
      _____,
      _____,
      __X__,
      _____,
      __X__,
      __X__,
      _X___
    },
    { // 3c - less than
      ___X_,
      __X__,
      _X___,
      X____,
      _X___,
      __X__,
      ___X_
    },
    { // 3d - equal
      _____,
      _____,
      XXXXX,
      _____,
      XXXXX,
      _____,
      _____
    },
    { // 3e - greater
      _X___,
      __X__,
      ___X_,
      ____X,
      ___X_,
      __X__,
      _X___
    },
    { // 3f - question mark
      _XXX_,
      X___X,
      ___X_,
      __X__,
      __X__,
      _____,
      __X__
    },
    { // 40 - at sign
      _XXX_,
      X___X,
      X_X_X,
      X_XXX,
      X_XX_,
      X____,
      _XXXX
    },
    { // 41 - A
      __X__,
      _X_X_,
      X___X,
      X___X,
      XXXXX,
      X___X,
      X___X
    },
    { // 42 - B
      XXXX_,
      X___X,
      X___X,
      XXXX_,
      X___X,
      X___X,
      XXXX_
    },
    { // 43 - C
      _XXX_,
      X___X,
      X____,
      X____,
      X____,
      X___X,
      _XXX_
    },
    { // 44 - D
      XXXX_,
      X___X,
      X___X,
      X___X,
      X___X,
      X___X,
      XXXX_
    },
    { // 45 - E
      XXXXX,
      X____,
      X____,
      XXXX_,
      X____,
      X____,
      XXXXX
    },
    { // 46 - F
      XXXXX,
      X____,
      X____,
      XXXX_,
      X____,
      X____,
      X____
    },
    { // 47 - G
      _XXXX,
      X____,
      X____,
      X____,
      X__XX,
      X___X,
      _XXXX
    },
    { // 48 - H
      X___X,
      X___X,
      X___X,
      XXXXX,
      X___X,
      X___X,
      X___X
    },
    { // 49 - I
      _XXX_,
      __X__,
      __X__,
      __X__,
      __X__,
      __X__,
      _XXX_
    },
    { // 4a - J
      ____X,
      ____X,
      ____X,
      ____X,
      ____X,
      X___X,
      _XXX_
    },
    { // 4b - K
      X___X,
      X__X_,
      X_X__,
      XX___,
      X_X__,
      X__X_,
      X___X
    },
    { // 4c - L
      X____,
      X____,
      X____,
      X____,
      X____,
      X____,
      XXXXX
    },
    { // 4d - M
      X___X,
      XX_XX,
      X_X_X,
      X_X_X,
      X___X,
      X___X,
      X___X
    },
    { // 4e - N
      X___X,
      X___X,
      XX__X,
      X_X_X,
      X__XX,
      X___X,
      X___X
    },
    { // 4f - O
      _XXX_,
      X___X,
      X___X,
      X___X,
      X___X,
      X___X,
      _XXX_
    },
    { // 50 - P
      XXXX_,
      X___X,
      X___X,
      XXXX_,
      X____,
      X____,
      X____
    },
    { // 51 - Q
      _XXX_,
      X___X,
      X___X,
      X___X,
      X_X_X,
      X__X_,
      _XX_X
    },
    { // 52 - R
      XXXX_,
      X___X,
      X___X,
      XXXX_,
      X_X__,
      X__X_,
      X___X
    },
    { // 53 - S
      _XXX_,
      X___X,
      X____,
      _XXX_,
      ____X,
      X___X,
      _XXX_
    },
    { // 54 - T
      XXXXX,
      __X__,
      __X__,
      __X__,
      __X__,
      __X__,
      __X__
    },
    { // 55 - U
      X___X,
      X___X,
      X___X,
      X___X,
      X___X,
      X___X,
      _XXX_
    },
    { // 56 - V
      X___X,
      X___X,
      X___X,
      X___X,
      X___X,
      _X_X_,
      __X__
    },
    { // 57 - W
      X___X,
      X___X,
      X___X,
      X_X_X,
      X_X_X,
      XX_XX,
      X___X
    },
    { // 58 - X
      X___X,
      X___X,
      _X_X_,
      __X__,
      _X_X_,
      X___X,
      X___X
    },
    { // 59 - Y
      X___X,
      X___X,
      _X_X_,
      __X__,
      __X__,
      __X__,
      __X__
    },
    { // 5a - Z
      XXXXX,
      ____X,
      ___X_,
      __X__,
      _X___,
      X____,
      XXXXX
    },
    { // 5b - left bracket
      XXXXX,
      XX___,
      XX___,
      XX___,
      XX___,
      XX___,
      XXXXX
    },
    { // 5c - backslash
      _____,
      X____,
      _X___,
      __X__,
      ___X_,
      ____X,
      _____
    },
    { // 5d - right bracket
      XXXXX,
      ___XX,
      ___XX,
      ___XX,
      ___XX,
      ___XX,
      XXXXX
    },
    { // 5e - caret
      _____,
      _____,
      __X__,
      _X_X_,
      X___X,
      _____,
      _____
    },
    { // 5f - underscore
      _____,
      _____,
      _____,
      _____,
      _____,
      _____,
      XXXXX
    }
  };


FILE *serial_open (char *port, int rate)
{
  FILE *f;

  f = fopen (port, "w+b");
  if (! f)
    return (NULL);

  // $$$ set bit rate
  // $$$ set raw mode
  // $$$ set RTS/CTS flow control

  setbuf(f,NULL);
  return (f);
}


void punch_column (FILE *outf, uint8_t val)
{
  fputc (val, outf);
}


void punch_zeros (FILE *outf, int count)
{
  while (count--)
    punch_column (outf, 0);
}

void punch_e9s (FILE *outf, int count)
{
  while (count--)
    punch_column (outf, 0xe9);
}


void punch_character (FILE *outf, unsigned char c)
{
  int row, col;
  uint8_t val;

  if ((c < 0x20) || (c > 0x7e))
    return;

  if (c >= 0x60)
    c -= 0x20;

  c -= 0x20;

  for (col = 0; col < 5; col++)
    {
      val = 0;
      for (row = 6; row >= 0; row--)
	{
	  val <<= 1;
	  val |= ((chargen [c][row] & (0x10 >> col)) != 0);
	}
      punch_column (outf, val);
    }
  punch_zeros (outf, INTERCHARACTER_SPACING);
}


void punch_string (FILE *outf, char *s)
{
  while (*s)
    punch_character (outf, (unsigned char) *(s++));
}


void punch_file (FILE *outf, FILE *in)
{
  int ch;
  int zero_count = 0;
  bool first_nonzero_char = true;

  // Leading zeros will be discarded.  If there is any non-zero
  // input, we'll punch GAP_LENGTH zero frames before it.

  while ((ch = fgetc (in)) >= 0)
    {
/*
          if (ch == '\0')
	        {
	          zero_count++;
	          continue;
	        }
          if (zero_count)
	        {
	          if (first_nonzero_char)
	            {
	              first_nonzero_char = false;
	              punch_zeros (outf, GAP_LENGTH);
	            }
	          else
	            punch_zeros (outf, zero_count);
	          zero_count = 0;
	        }
*/
          punch_column (outf, ch);
    }
  // trailing zeros will be discarded
  // should check for error vs. eof
}


int main (int argc, char *argv [])
{
  char *outfn = DEFAULT_SERIAL_PORT;
  int rate = DEFAULT_BIT_RATE;

  FILE *outf = serial_open (outfn, rate);

  if (argc != 2)
    {
      fprintf (stderr, "usage: %s string\n", argv [0]);
      exit (1);
    }

  punch_zeros (outf, LEADER_LENGTH);

  punch_string (outf, argv [1]);

  punch_zeros (outf, GAP_LENGTH);

  punch_e9s (outf, E9_LENGTH);

  punch_file (outf, stdin);

  punch_zeros (outf, TRAILER_LENGTH);

  exit (0);
}
