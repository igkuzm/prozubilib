/**
 * File              : translit.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 07.03.2022
 * Last Modified Date: 13.12.2025
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

/**
 * translit.h
 * Copyright (c) 2022 Igor V. Sementsov <ig.kuzm@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TRANSLIT_H
#define TRANSLIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/*
 * Return allocated null-terminated c string with ASCII
 * transliterating symbols converted from utf8 russian
 * string
 */
static char *translit_utf8(const char *string);

/*
 * IMP
 */

typedef struct cprecode_t {
  uint32_t utf_code;
  char utf8[5];
  char ascii[4];
  char koi8r;
  char cp1251;
} cprecode_t;

// for binary search we need sorted array
static cprecode_t cprecode_table[] = {
   { 0x00AB, "«", "<<", '"',  0xAB}, // 0
   { 0x00BB, "»", ">>", '"',  0xBB}, // 1
   { 0x0401, "Ё", "YO", 0xB3, 0xA8}, // 2
   { 0x0410, "А", "A",  0xC1, 0xC0}, // 3
   { 0x0411, "Б", "B",  0xC2, 0xC1}, // 4
   { 0x0412, "В", "V",  0xC3, 0xC2}, // 5
   { 0x0413, "Г", "G",  0xC4, 0xC3}, // 6
   { 0x0414, "Д", "D",  0xC5, 0xC4}, // 7
   { 0x0415, "Е", "E",  0xC6, 0xC5}, // 8
   { 0x0416, "Ж", "ZH", 0xC7, 0xC6}, // 9
  { 0x0417, "З", "Z",  0xC8, 0xC7}, // 10
  { 0x0418, "И", "I",  0xC9, 0xC8}, // 11
  { 0x0419, "Й", "Y",  0xCA, 0xC9}, // 12
  { 0x041A, "К", "K",  0xCB, 0xCA}, // 13
  { 0x041B, "Л", "L",  0xCC, 0xCB}, // 14
  { 0x041C, "М", "M",  0xCD, 0xCC}, // 15
  { 0x041D, "Н", "N",  0xCE, 0xCD}, // 16
  { 0x041E, "О", "O",  0xCF, 0xCE}, // 17
  { 0x041F, "П", "P",  0xD0, 0xCF}, // 18
  { 0x0420, "Р", "R",  0xD1, 0xD0}, // 19
  { 0x0421, "С", "S",  0xD2, 0xD1}, // 20
  { 0x0422, "Т", "T",  0xD3, 0xD2}, // 21
  { 0x0423, "У", "U",  0xD4, 0xD3}, // 22
  { 0x0424, "Ф", "PH", 0xD5, 0xD4}, // 23
  { 0x0425, "Х", "KH", 0xD6, 0xD5}, // 24
  { 0x0426, "Ц", "TS", 0xD7, 0xD6}, // 25
  { 0x0427, "Ч", "TCH",0xD8, 0xD7}, // 26
  { 0x0428, "Ш", "SH", 0xD9, 0xD8}, // 27
  { 0x0429, "Щ", "SCH",0xDA, 0xD9}, // 28
  { 0x042A, "Ъ", "'",  0xDB, 0xDA}, // 29
  { 0x042B, "Ы", "Y",  0xDC, 0xDB}, // 30
  { 0x042C, "Ь", "'",  0xDD, 0xDC}, // 31
  { 0x042D, "Э", "E",  0xDE, 0xDD}, // 32
  { 0x042E, "Ю", "YU", 0xDF, 0xDE}, // 33
  { 0x042F, "Я", "YA", 0xE0, 0xDF}, // 34
  { 0x0430, "а", "a",  0xE1, 0xE0}, // 35
  { 0x0431, "б", "b",  0xE2, 0xE1}, // 36
  { 0x0432, "в", "v",  0xE3, 0xE2}, // 37
  { 0x0433, "г", "g",  0xE4, 0xE3}, // 38
  { 0x0434, "д", "d",  0xE5, 0xE4}, // 39
  { 0x0435, "е", "e",  0xE6, 0xE5}, // 40
  { 0x0436, "ж", "zh", 0xE7, 0xE6}, // 41
  { 0x0437, "з", "z",  0xE8, 0xE7}, // 42
  { 0x0438, "и", "i",  0xE9, 0xE8}, // 43
  { 0x0439, "й", "y",  0xEA, 0xE9}, // 44
  { 0x043A, "к", "k",  0xEB, 0xEA}, // 45
  { 0x043B, "л", "l",  0xEC, 0xEB}, // 46
  { 0x043C, "м", "m",  0xED, 0xEC}, // 47
  { 0x043D, "н", "n",  0xEE, 0xED}, // 48
  { 0x043E, "о", "o",  0xEF, 0xEE}, // 49
  { 0x043F, "п", "p",  0xF0, 0xEF}, // 50
  { 0x0440, "р", "r",  0xF1, 0xF0}, // 51
  { 0x0441, "с", "c",  0xF2, 0xF1}, // 52
  { 0x0442, "т", "t",  0xF3, 0xF2}, // 53
  { 0x0443, "у", "u",  0xF4, 0xF3}, // 54
  { 0x0444, "ф", "ph", 0xF5, 0xF4}, // 55
  { 0x0445, "х", "kh", 0xF6, 0xF5}, // 56
  { 0x0446, "ц", "ts", 0xF7, 0xF6}, // 57
  { 0x0447, "ч", "tch",0xF8, 0xF7}, // 58
  { 0x0448, "ш", "sh", 0xF9, 0xF8}, // 59
  { 0x0449, "щ", "sch",0xFA, 0xF9}, // 60
  { 0x044A, "ъ", "'",  0xFB, 0xFA}, // 61
  { 0x044B, "ы", "y",  0xFC, 0xFB}, // 62
  { 0x044C, "ь", "'",  0xFD, 0xFC}, // 63
  { 0x044D, "э", "e",  0xFE, 0xFD}, // 64
  { 0x044E, "ю", "yu", 0xFF, 0xFE}, // 65
  { 0x044F, "я", "ya", 0xE1, 0xFF}, // 66
  { 0x0451, "ё", "yo", 0xA3, 0xB8}, // 67
  { 0x2116, "№", "#",  '#',  0xB9}, // 68
};

// function for binary search - compare utf codes in table
static int 
translit_compare(const void *s1, const void *s2) 
{
  return 
		((cprecode_t *)s1)->utf_code - 
		((cprecode_t *)s2)->utf_code;
}


char *
translit_utf8(const char *str) 
{
  // get size of string
  int len = strlen(str);

  // allocate return string
  char *ret = (char *)malloc(len * 3 + 1);
  if (!ret)
    return NULL;

  // get size of translit table
  int translit_table_size = 
		sizeof(cprecode_table) / sizeof(cprecode_t);

  int i,     // itearator
			k = 0; // len of return string 

  char *ptr = (char *)str; // pointer to string
  
	// check utf8 char size
  while (*ptr) {
    int buflen = 0;
    char buf[7];
		// 0b11111100 is 252
		if ((*ptr & 252) == 252) {        // 6 byte
      for (i = 0; i < 6; i++) {
        buf[i] = *ptr++;
        buflen++;
      }
		// 0b11111000 is 248
		} else if ((*ptr & 248) == 248) { // 5 byte
      for (i = 0; i < 5; i++) {
        buf[i] = *ptr++;
        buflen++;
      }
		// 0b11110000 is 240
		} else if ((*ptr & 240) == 240) { // 4 byte
      for (i = 0; i < 4; i++) {
        buf[i] = *ptr++;
        buflen++;
      }
		// 0b11100000 is 224
    } else if ((*ptr & 224) == 224) { // 3 byte
      for (i = 0; i < 3; i++) {
        buf[i] = *ptr++;
        buflen++;
      }
		// 0b11000000 is 192
    } else if ((*ptr & 192) == 192) { // 2 byte
      for (i = 0; i < 2; i++) {
        buf[i] = *ptr++;
        buflen++;
      }
    } else                            // 1 byte - ANSY
      buf[0] = *ptr++;
    

    // find utf coded char
    cprecode_t key = { 0 };
    switch (buflen) {
		case 6: {
			// 0b00000001 is 1 and 0b00111111 is 63
      key.utf_code = (
					 ((buf[0] & 1)  << 30) | 
					 ((buf[1] & 63) << 24) | 
					 ((buf[2] & 63) << 18) | 
					 ((buf[3] & 63) << 12) |
           ((buf[4] & 63) << 6)  | 
					  (buf[5] & 63));
      break;
    }
		case 5: {
			// 0b00000011 is 3 and 0b00111111 is 63
      key.utf_code = (
					 ((buf[0] & 3)  << 24) | 
					 ((buf[1] & 63) << 18) | 
					 ((buf[2] & 63) << 12) |
           ((buf[3] & 63) << 6)  | 
					  (buf[4] & 63));
      break;
    }
    case 4: {
			// 0b00000111 is 7 and 0b00111111 is 63
      key.utf_code = (
					((buf[0] & 7)  << 18) | 
					((buf[1] & 63) << 12) |
          ((buf[2] & 63) << 6)  | 
					 (buf[3] & 63));
      break;
    }
    case 3: {
			// 0b00001111 is 15 and 0b00111111 is 63
      key.utf_code = (
					((buf[0] & 15) << 12) |
          ((buf[1] & 63) << 6)  | 
					 (buf[2] & 63));
      break;
    }
    case 2: {
			// 0b00011111 is 31 and 0b00111111 is 63
      key.utf_code = (
					((buf[0] & 31) << 6) | 
					 (buf[1] & 63));
      break;
    }

    default:
      break;
    }

		// find  ASCII transliterating in table with
		// binary search
    if (key.utf_code) {
      cprecode_t *translit = (cprecode_t *)bsearch(
					&key, cprecode_table,
				 	translit_table_size,
					sizeof(cprecode_t), translit_compare);

      if (translit)
				// copy transilt value to return string
				for (i=0; translit->ascii[i]; ++i)
					ret[k++] = translit->ascii[i];
    } else
				// copy ASCII char to return string
				ret[k++] = buf[0];
  }

  return ret;
}

#ifdef __cplusplus
}
#endif

#endif // TRANSLIT_H
