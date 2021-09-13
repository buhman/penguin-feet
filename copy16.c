void copy_16(void * dest, void * src, unsigned int n)
{
  while (n > 1) {
    *((unsigned short *)dest) = *((unsigned short *)src);
    dest += 2;
    src += 2;
    n -= 2;
  }
}

void fill_16(void * dest, unsigned short c, unsigned int n)
{
  while (n > 1) {
    *((unsigned short *)dest) = c;
    dest += 2;
    n -= 2;
  }
}
