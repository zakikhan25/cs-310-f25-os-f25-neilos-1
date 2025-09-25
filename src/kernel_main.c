#include <stdint.h>

// Multiboot1 header so GRUB knows how to load us
__attribute__((section(".multiboot")))
const uint32_t multiboot_header[] = {
    0x1BADB002,                  // magic
    0x00010003,                  // flags (align + mem info)
    -(0x1BADB002 + 0x00010003)   // checksum (sum to zero)
};

// externs from other files
extern void putc(int data);
extern int esp_printf(void (*printchar)(int), const char *fmt, ...);

// kernel entry point
void kernel_main(void) {
    esp_printf(putc, "Hello World!\n");
    esp_printf(putc, "Execution level: %d\n", 1);
    while (1) { /* loop forever */ }
}


