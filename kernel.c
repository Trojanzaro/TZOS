#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "string.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

uint16_t* terminal_buffer;

const char* welcome_message = "                  ****************************************\n                  *         WELCOME TO TZOS v1.0         *\n                  ****************************************";

enum vga_colour {
    VGA_COLOUR_BLACK,
    VGA_COLOUR_BLUE,
    VGA_COLOUR_GREEN,
    VGA_COLOUR_CYAN,
    VGA_COLOUR_RED,
    VGA_COLOUR_MAGENTA,
    VGA_COLOUR_BROWN,
    VGA_COLOUR_LIGHT_GREY,
    VGA_COLOUR_DARK_GREY,
    VGA_COLOUR_LIGHT_BLUE,
    VGA_COLOUR_LIGHT_GREEN,
    VGA_COLOUR_LIGHT_CYAN,
    VGA_COLOUR_LIGHT_RED,
    VGA_COLOUR_LIGHT_MAGENTA,
    VGA_COLOUR_LIGHT_BROWN,
    VGA_COLOUR_WHITE,
};

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_colour;

unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};


static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
    /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

static inline uint8_t vga_entry_colour(enum vga_colour foreground, enum vga_colour background){
    return foreground | (background << 4);
}

static inline uint16_t vga_entry(unsigned char c, uint8_t colour){
    return (uint16_t) c | ((uint16_t) colour << 8);
}

void terminal_putcharat(char c, uint16_t colour, size_t x, size_t y){
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, colour);
}

void terminal_putchar(char c){
    if(terminal_column == VGA_WIDTH || c == '\n'){
        terminal_column = 0;
        if(terminal_row == VGA_HEIGHT-1){
            terminal_row = 0;
        } else {
            terminal_row++;
        }
    }
    if(c == '\n') return;
    if(c == '\t') for(size_t i = 0; i < 5; i++) terminal_putcharat(' ', terminal_colour, terminal_column++, terminal_row);
    terminal_putcharat(c, terminal_colour, terminal_column++, terminal_row);
}

void terminal_initialize(){
    terminal_row = 0;
    terminal_column = 0;
    terminal_colour = vga_entry_colour(VGA_COLOUR_WHITE, VGA_COLOUR_BLUE);
    terminal_buffer = (uint16_t *) 0xB8000;
    for(size_t y = 0; y < VGA_HEIGHT; y++){
        for(size_t x = 0; x < VGA_WIDTH; x++){
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_colour);
        }
    }
}

 
void terminal_write(const char* data, size_t size){
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
}
void terminal_writestring(const char* data){
    terminal_write(data, strlen(data));
}

void terminal_scroll_up(){
    int i;
    for(i = 0; i < (VGA_WIDTH*VGA_HEIGHT-80); i++)
        terminal_buffer[i] = terminal_buffer[i+80];
    for(i = 0; i < VGA_WIDTH; i++)
        terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + i] = vga_entry(' ', terminal_colour);
}

void delay(int t){   
    volatile int i,j;
    for(i=0;i<t;i++)
        for(j=0;j<250000;j++)
            __asm__("NOP");
}

void terminal_writeint(unsigned long n){
    if(n/10)
        terminal_writeint(n/10);
    terminal_putchar((n % 10) + '0');
}

void terminal_keyboard_handler(){
	unsigned char scancode;

    /* Read from the keyboard's data buffer */
    scancode = inb(0x60);

    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
    if (inb(0x60) & 0x80 != scancode)
    {
	terminal_putchar(kbdus[scancode]);
        /* You can use this one to see if the user released the
        *  shift, alt, or control keys... */
    }
    else
    {
    }

}

void kernel_main() {
	terminal_initialize();
	terminal_writestring(welcome_message);
	terminal_writestring("\n\n");
	terminal_writestring(">");
	while(1)
		terminal_keyboard_handler();
		
}





