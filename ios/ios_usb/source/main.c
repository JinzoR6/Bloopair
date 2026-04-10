#include <stdint.h>
#include <string.h>

#define IOS_USB_SCAN_START  0x10100000
#define IOS_USB_SCAN_END    0x10200000
#define WII_PORTAL_VID      0x1430
#define WII_PORTAL_PID      0x0150
#define XBOX_PORTAL_PID     0x1F17

void _main()
{
    void(*ios_shutdown)(int) = (void(*)(int)) 0x1012EE4C;
    int(*reply)(int, int) = (int(*)(int, int)) 0x1012ED04;

    int saved_handle = *(volatile int*) 0x0012F000;
    int myret = reply(saved_handle, 0);
    if (myret != 0) {
        ios_shutdown(1);
    }

    // Scan IOS-USB memory for VID/PID filter pattern and patch it
    uint16_t *ptr = (uint16_t *) IOS_USB_SCAN_START;
    uint16_t *end = (uint16_t *) IOS_USB_SCAN_END;

    while (ptr < end) {
        // Look for: VID 0x1430 followed closely by PID 0x0150
        if (ptr[0] == WII_PORTAL_VID && ptr[1] == WII_PORTAL_PID) {
            // Patch: change 0x0150 to 0x1F17 so Xbox portal is accepted
            ptr[1] = XBOX_PORTAL_PID;
            break;
        }
        ptr++;
    }

    asm("LDR SP, newsp\n"
        "LDR R0, newr0\n"
        "LDR LR, newlr\n"
        "LDR PC, newpc\n"
        "newsp: .word 0x1016AE30\n"
        "newlr: .word 0x1012EACC\n"
        "newr0: .word 0x10146080\n"
        "newpc: .word 0x10111164\n");
}
