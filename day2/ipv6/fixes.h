/* Compensate for systems that don't have bounded string functions to avoid buffer
 * overruns
 */

size_t strnlen(const char *s, size_t len) 
{ 
  size_t i; 
  for(i=0; i<len && *(s+i); i++); 
  return i; 
} 

