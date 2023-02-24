#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#define BUFSIZE 1024


#define ASCII_MIN 0
#define ASCII_MAX 127

#include <stdint.h>

uint32_t decode_utf8(const unsigned char *input, size_t length) {
  uint32_t code_point = 0;
  size_t i = 0;

  if (length < 1) {
    return 0;
  }

  // Determine the number of bytes in the sequence
  if (input[i] < 0x80) {
    // Single-byte sequence
    code_point = input[i];
    i += 1;
  } else if (input[i] < 0xE0) {
    // Two-byte sequence
    code_point = ((input[i] & 0x1F) << 6) | (input[i + 1] & 0x3F);
    i += 2;
  } else if (input[i] < 0xF0) {
    // Three-byte sequence
    code_point = ((input[i] & 0x0F) << 12) | ((input[i + 1] & 0x3F) << 6) | (input[i + 2] & 0x3F);
    i += 3;
  } else if (input[i] < 0xF8) {
    // Four-byte sequence
    code_point = ((input[i] & 0x07) << 18) | ((input[i + 1] & 0x3F) << 12) | ((input[i + 2] & 0x3F) << 6) | (input[i + 3] & 0x3F);
    i += 4;
  } else {
    // Invalid byte sequence
    return 0;
  }

  return code_point;
}

void print_valid_utf8_choose(const unsigned char *input, size_t length, int offset, int flag) {
  size_t i = 0;
  while (i < length) {
    unsigned char c = input[i];

    if (c <= 0x7F && (flag == 1 || flag == 2)) {
      // Found a valid 1-byte sequence
      uint32_t code_point = c;
      printf("%c : U+%04X \n", c, code_point);
      i += 1;
    } else if ((c & 0xE0) == 0xC0 && (flag == 1 || flag == 3)) {
      if (i + 1 >= length || (input[i + 1] & 0xC0) != 0x80) {
        // Invalid 2-byte sequence, skip over it
        i += 2;
      } else {
        // Found a valid 2-byte sequence
        uint32_t code_point = decode_utf8(&input[i], 2);
        printf("%c%c : U+%04X \n", c, input[i+1], code_point);
        i += 2;
      }
    } else if ((c & 0xF0) == 0xE0 && (flag == 1 || flag == 3)) {
      if (i + 2 >= length || (input[i + 1] & 0xC0) != 0x80 || (input[i + 2] & 0xC0) != 0x80) {
        // Invalid 3-byte sequence, skip over it
        i += 3;
      } else {
        // Found a valid 3-byte sequence
        uint32_t code_point = decode_utf8(&input[i], 3);
        printf("%c%c%c : U+%04X \n", c, input[i+1], input[i+2], code_point);
        i += 3;
      }
    } else if ((c & 0xF8) == 0xF0 && (flag == 1 || flag == 3)) {
      if (i + 3 >= length || (input[i + 1] & 0xC0) != 0x80 || (input[i + 2] & 0xC0) != 0x80 || (input[i + 3] & 0xC0) != 0x80) {
        // Invalid 4-byte sequence, skip over it
        i += 4;
      } else {
        // Found a valid 4-byte sequence
        uint32_t code_point = decode_utf8(&input[i], 4);
        printf("%c%c%c%c : U+%04X \n", c, input[i+1], input[i+2], input[i+3], code_point);
        i += 4;
      }
    } else {
      // Invalid byte, skip over it
      i += 1;
    }
    offset++;
  }
}

bool is_valid_utf8(const unsigned char *input, size_t length, int offset) {
  size_t i = 0;
   char* format = "There is a single ill-formed byte at offset:";
   char* space = " ";
  while (i < length) {
    unsigned char c = input[i];

    if (c <= 0x7F) {
      i += 1;
    } else if ((c & 0xE0) == 0xC0) {
      if (i + 1 >= length || (input[i + 1] & 0xC0) != 0x80) {
        // write(1, format, strlen(format));
         //write(1, )
         printf("%s %d:", format, offset);
         for(size_t j = 0; j <=i+1; j++){
            //printf("%s", "1");
            //write(1, )
            printf(" %02x", input[j]);
         }
        return false;
      }
      i += 2;
    } else if ((c & 0xF0) == 0xE0) {
      if (i + 2 >= length || (input[i + 1] & 0xC0) != 0x80 || (input[i + 2] & 0xC0) != 0x80) {
        printf("%s %d:", format, offset);
         for(size_t j = 0; j <=i+1; j++){
            //printf("%s", "1");
            //write(1, )
            printf(" %02x", input[j]);
         }
        return false;
      }
      i += 3;
    } else if ((c & 0xF8) == 0xF0) {
      if (i + 3 >= length || (input[i + 1] & 0xC0) != 0x80 || (input[i + 2] & 0xC0) != 0x80 || (input[i + 3] & 0xC0) != 0x80) {
        printf("%s %d:", format, offset);
         for(size_t j = 0; j <=i+1; j++){
            //printf("%s", "1");
            //write(1, )
            printf(" %02x", input[j]);
         }
        return false;
      }
      i += 4;
    } else {
      printf("%s %d:", format, offset);
         for(size_t j = 0; j <=i; j++){
            //printf("%s", "1");
            //write(1, )
            printf(" %02x", input[j]);
         }
      return false;
    }
     offset++;
  }

  return true;
}

bool isUTF8(unsigned char c) {
    return c >= ASCII_MIN && c <= ASCII_MAX;
}

int main(int argc, char *argv[]) {
    int display_utf8 = 0;
   int display_ascii = 0;
   int problems = 0;
   int display_original = 0;
   int verbose = 0;
   FILE *fp;
  unsigned char buffer[BUFSIZE];
  size_t bytes_read;
  int i;



    for (int i = 1; i < argc; i++) {
      if (strcmp(argv[i], "-u") == 0) {
         display_utf8 = 1;
      }
      if (strcmp(argv[i], "-a") == 0) {
         display_ascii = 1;
      }
      if (strcmp(argv[i], "-p") == 0) {
         problems = 1;
      }

      if (strcmp(argv[i], "-o") == 0) {
         display_original = 1;
      }
      if (strcmp(argv[i], "-v") == 0) {
         verbose = 1;
      }
   }

   int c;
   unsigned char uc;
   int16_t offset = 0;
   char* format = "There is a single ill-formed byte at offset:";
   unsigned char prev_bytes;
   bool prev = false;

    if(problems == 1){
       unsigned char input[1024];
  size_t length = 0;

  while (fread(input + length, 1, 1, stdin)) {
    length++;
    if (length >= 1024) {
      break;
    }
  }

   //print_valid_utf82(input,length,offset);
    is_valid_utf8(input,length,offset);
    }


     else if(verbose == 1 && display_ascii == 1 && display_utf8 == 1){
       unsigned char input[1024];
  size_t length = 0;

  while (fread(input + length, 1, 1, stdin)) {
    length++;
    if (length >= 1024) {
      break;
    }
  }

   //print_valid_utf8(input,length,offset);
    print_valid_utf8_choose(input,length,offset, 1);
    }
     else if(verbose == 1 && display_ascii == 1){
       unsigned char input[1024];
  size_t length = 0;

  while (fread(input + length, 1, 1, stdin)) {
    length++;
    if (length >= 1024) {
      break;
    }
  }

   //print_valid_utf8(input,length,offset);
    print_valid_utf8_choose(input,length,offset, 2);
    }
     else if(verbose == 1 && display_utf8 == 1){
       unsigned char input[1024];
  size_t length = 0;

  while (fread(input + length, 1, 1, stdin)) {
    length++;
    if (length >= 1024) {
      break;
    }
  }

   //print_valid_utf8(input,length,offset);
    print_valid_utf8_choose(input,length,offset, 3);
    }


    if(display_ascii && display_utf8){
         while ((c = getchar()) != EOF) {
                putchar(c);   
        }
    }
    else if(display_utf8){
        //printf("UTF-8 characters:\n");
        while ((c = getchar()) != EOF) {
            if (!isUTF8(c)) {
                putchar(c);
               
            }
        }
    }
    else if(display_ascii){
        //printf("\nASCII characters:\n");
        while ((c = getchar()) != EOF) {
            if (isUTF8(c)) {
                putchar(c);
            }
        }
    }

    return 0;
    }
