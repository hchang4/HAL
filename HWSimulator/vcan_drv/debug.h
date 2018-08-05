#ifndef DEBUG_H_
#define DEBUG_H_

int DBGprintf(const char *format, ... ); 

#ifdef DEBUG_EN
#define DEBUG printk
#else
#define DEBUG(fmt, args...)
#endif


#endif //DEBUG_H_
