#include <stdarg.h>

// external character output
extern void putc(int data);

int esp_printf(void (*printchar)(int), const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (const char *p = fmt; *p; p++) {
        if (*p == '%') {
            p++;
            if (*p == 'd') { // integer
                int val = va_arg(args, int);
                char buf[16];
                int i = 0;
                if (val == 0) {
                    buf[i++] = '0';
                } else {
                    int neg = 0;
                    if (val < 0) { neg = 1; val = -val; }
                    while (val > 0) { buf[i++] = '0' + (val % 10); val /= 10; }
                    if (neg) buf[i++] = '-';
                }
                while (--i >= 0) printchar(buf[i]);
            } else if (*p == 's') { // string
                char *s = va_arg(args, char*);
                while (*s) printchar(*s++);
            } else if (*p == 'c') { // char
                char c = (char)va_arg(args, int);
                printchar(c);
            } else {                // unknown specifier
                printchar('%');
                printchar(*p);
            }
        } else {
            printchar(*p);
        }
    }

    va_end(args);
    return 0;
}


