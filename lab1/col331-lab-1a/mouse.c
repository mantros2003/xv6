#include "types.h"
#include "defs.h"
#include "x86.h"
#include "mouse.h"
#include "traps.h"

// Wait until the mouse controller is ready for us to send a packet
void 
mousewait_send(void) 
{
    // Implement your code here
    while (inb(MSSTATP) & 0x02)
        ;
    return;
}

// Wait until the mouse controller has data for us to receive
void 
mousewait_recv(void) 
{
    // Implement your code here
    while ((inb(MSSTATP) & 1) == 0)
        ;
    return;
}

// Send a one-byte command to the mouse controller, and wait for it
// to be properly acknowledged
void 
mousecmd(uchar cmd) 
{
    // Implement your code here
    mousewait_send();
    outb(MSSTATP, 0xD4);

    mousewait_send();
    outb(MSDATAP, cmd);

    mousewait_recv();
    uchar ack = inb(MSDATAP);
    
    return;
}

void
mouseinit(void)
{
    // Implement your code here
    mousewait_send();
    outb(MSSTATP, MSEN);

    mousewait_send();
    outb(MSSTATP, 0x20);

    mousewait_recv();
    uchar status = inb(MSDATAP);
    status = status | 0x02;

    mousewait_send();
    outb(MSSTATP, 0x60);

    mousewait_send();
    outb(MSDATAP, status);

    mousecmd(0xF6);
    mousecmd(0xF4);

    ioapicenable(IRQ_MOUSE, 0);

    return;
}

void
mouseintr(void)
{
    // Implement your code here
    static uchar byte = 0;
    static uchar buffer[3];

    while (inb(MSSTATP) & 0x01) {
        mousewait_recv();
        buffer[byte++] = inb(MSDATAP);

        if (byte == 3) {
            byte = 0;
            
            if (buffer[0] & 0x01) cprintf("LEFT\n");
            if (buffer[0] & 0x02) cprintf("RIGHT\n");
            if (buffer[0] & 0x04) cprintf("MID\n");
        }
    }

    return;
}