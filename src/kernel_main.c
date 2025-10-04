#include <stdint.h>
#include "rprintf.h"

#define MULTIBOOT2_HEADER_MAGIC 0xe85250d6
#define VIDEO_MEMORY 0xB8000
#define ROWS 25
#define COLS 80
#define SCREEN_SIZE (ROWS * COLS)

// Global cursor position
int cursor = 0;

// Helper function to scroll the screen up by one line
void scroll_screen(void) {
    char *video = (char*) VIDEO_MEMORY;
    int i;
    
    // Copy each line to the previous line (starting from line 1)
    for (i = 0; i < (ROWS - 1) * COLS * 2; i++) {
        video[i] = video[i + COLS * 2];
    }
    
    // Clear the last line
    for (i = (ROWS - 1) * COLS * 2; i < ROWS * COLS * 2; i += 2) {
        video[i] = ' ';           // Space character
        video[i + 1] = 0x07;      // Light gray on black
    }
    
    // Move cursor to the beginning of the last line
    cursor = (ROWS - 1) * COLS;
}

// Write one character to the screen
void putc(int data) {
    char *video = (char*) VIDEO_MEMORY;
    
    if (data == '\n') {
        // Move to start of next line
        cursor += COLS - (cursor % COLS);
    } else if (data == '\r') {
        // Carriage return: move to beginning of current line
        cursor = (cursor / COLS) * COLS;
    } else if (data == '\t') {
        // Tab: move to next multiple of 8
        int spaces = 8 - (cursor % 8);
        for (int i = 0; i < spaces; i++) {
            putc(' ');
        }
        return;
    } else if (data == '\b') {
        // Backspace: move back one position
        if (cursor > 0) {
            cursor--;
            video[cursor * 2] = ' ';
            video[cursor * 2 + 1] = 0x07;
        }
        return;
    } else {
        // Regular character
        video[cursor * 2] = (char) data;   // ASCII character
        video[cursor * 2 + 1] = 0x07;      // Light gray on black
        cursor++;
    }
    
    // Check if we need to scroll
    if (cursor >= SCREEN_SIZE) {
        scroll_screen();
    }
}

// Clear the screen
void clear_screen(void) {
    char *video = (char*) VIDEO_MEMORY;
    int i;
    
    for (i = 0; i < SCREEN_SIZE * 2; i += 2) {
        video[i] = ' ';
        video[i + 1] = 0x07;
    }
    
    cursor = 0;
}

// Get current execution level (privilege level from CS register)
uint32_t get_execution_level(void) {
    uint32_t cs;
    __asm__ __volatile__("mov %%cs, %0" : "=r"(cs));
    return cs & 0x3;  // Lower 2 bits contain the privilege level (CPL)
}

// Port I/O
uint8_t inb(uint16_t _port) {
    uint8_t rv;
    __asm__ __volatile__("inb %1, %0" : "=a"(rv) : "dN"(_port));
    return rv;
}

// Kernel entry point
void main(void) {
    uint32_t exec_level;
    
    // Clear the screen first
    clear_screen();
    
    // Print welcome message
    esp_printf(putc, "CS 310 Terminal Driver\r\n");
    esp_printf(putc, "======================\r\n\r\n");
    
    // Get and print the current execution level
    exec_level = get_execution_level();
    esp_printf(putc, "Current execution level: %d\r\n\r\n", exec_level);
    
    // Test basic output
    esp_printf(putc, "Hello World!\r\n");
    esp_printf(putc, "Terminal driver initialized successfully!\r\n");
    esp_printf(putc, "Video memory at: 0x%x\r\n\r\n", VIDEO_MEMORY);
    
    // Test scrolling by printing many lines
    esp_printf(putc, "Testing screen scrolling:\r\n");
    for (int i = 1; i <= 30; i++) {
        esp_printf(putc, "Line %d: The quick brown fox jumps over the lazy dog\r\n", i);
    }
    
    esp_printf(putc, "\r\nScrolling test complete!\r\n");
    esp_printf(putc, "Entering keyboard loop...\r\n\r\n");
    
    // Keyboard polling loop
    while (1) {
        uint8_t status = inb(0x64);
        if (status & 1) {
            uint8_t scancode = inb(0x60);
            // Later: translate scancode to char, call putc()
            // For now, just print the scancode in hex
            esp_printf(putc, "Scancode: 0x%x\r\n", scancode);
        }
    }
}
