/*
 * bin2c: write the bytes of a file as a C initializer list.
 *
 * The output is meant to be included inside an array definition:
 *
 *   const unsigned char data[] = {
 *   #include "data.inc"
 *   };
 *
 * Usage: bin2c <input> <output>
 */

#include <stdio.h>

#define BYTES_PER_LINE 12

int
main(int argc, char **argv)
{
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <input> <output>\n", argv[0]);
    return 1;
  }

  FILE *in = fopen(argv[1], "rb");
  if (!in) {
    fprintf(stderr, "bin2c: failed to open %s\n", argv[1]);
    return 1;
  }

  FILE *out = fopen(argv[2], "w");
  if (!out) {
    fprintf(stderr, "bin2c: failed to create %s\n", argv[2]);
    fclose(in);
    return 1;
  }

  unsigned long count = 0;
  int c;
  while ((c = fgetc(in)) != EOF) {
    fprintf(out, "0x%02x,", (unsigned int)c);
    count++;
    fputc(count % BYTES_PER_LINE == 0 ? '\n' : ' ', out);
  }
  fputc('\n', out);

  int failed = ferror(in) || ferror(out);
  fclose(in);
  if (fclose(out) != 0) {
    failed = 1;
  }

  if (failed) {
    fprintf(stderr, "bin2c: failed to convert %s\n", argv[1]);
    remove(argv[2]);
    return 1;
  }

  return 0;
}
