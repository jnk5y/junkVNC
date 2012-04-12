char hexdigit(int c) {
  if(c<0)
    return '.';
  if(c<10)
    return '0'+c;
  if(c<16)
    return 'a'+(c-10);
  return ',';
}

/* printf's the two hex digits coresponding to a byte */
inline void charhexout(char c) {
  printf("%c%c ",hexdigit((c>>4)&0x0F),hexdigit(c&0x0F));
}

/* charhexout's @a n bytes starting at @a p */
inline void hexout(char *p, size_t n) {
  unsigned int i;
  for(i=0; i<n; i++) {
   charhexout(p[i]);
  }
  printf("\n");
}
