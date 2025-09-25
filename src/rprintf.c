#define VIDEO_MEMORY 0xB8000
#define ROWS 25
#define COLS 80

int cursor_row = 0;
int cursor_col = 0;

void scroll();

// print one character to VGA text buffer
void putc(int data) {
    volatile char *video = (volatile char*) VIDEO_MEMORY;
    int offset = (cursor_row * COLS + cursor_col) * 2;

    if (data == '\n') {
        cursor_row++;
        cursor_col = 0;
    } else {
        video[offset] = (char)data;   // character
        video[offset + 1] = 0x07;     // white on black
        cursor_col++;
    }

    if (cursor_col >= COLS) {
        cursor_col = 0;
        cursor_row++;
    }

    if (cursor_row >= ROWS) {
        scroll();
        cursor_row = ROWS - 1;
        cursor_col = 0;
    }
}

void scroll() {
    volatile char *video = (volatile char*) VIDEO_MEMORY;

    // Shift all rows up by one
    for (int i = 0; i < (ROWS - 1) * COLS * 2; i++) {
        video[i] = video[i + COLS * 2];
    }

    // Clear last row
    for (int i = (ROWS - 1) * COLS * 2; i < ROWS * COLS * 2; i += 2) {
        video[i] = ' ';
        video[i + 1] = 0x07;
    }
}
