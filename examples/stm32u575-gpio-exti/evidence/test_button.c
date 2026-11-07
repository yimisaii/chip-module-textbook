#include "button.h"

#include <assert.h>
#include <stdio.h>

int main(void)
{
    bool high = false;

    assert(!Button_Confirm(&high, true, 10, 10));
    assert(!Button_Confirm(&high, false, 12, 12));
    assert(!Button_Confirm(&high, true, 16, 16));
    assert(!Button_Confirm(&high, true, 35, 16));
    assert(!high);
    assert(Button_Confirm(&high, true, 36, 16));
    assert(high);
    puts("PASS bouncing press and exact 19/20 tick threshold");

    assert(!Button_Confirm(&high, true, 900, 16));
    assert(high);
    puts("PASS held press does not repeat");

    assert(!Button_Confirm(&high, false, 1019, 1000));
    assert(high);
    assert(!Button_Confirm(&high, true, 1021, 1021));
    assert(!Button_Confirm(&high, false, 1045, 1025));
    assert(!high);
    assert(Button_Confirm(&high, true, 1100, 1080));
    puts("PASS bouncing release, rearm and second press");

    high = false;
    assert(!Button_Confirm(&high, true, 2007, 2000));
    assert(!Button_Confirm(&high, false, 2028, 2008));
    assert(!high);
    puts("PASS short high pulse rejected");

    high = true;
    assert(!Button_Confirm(&high, true, 100, 0));
    assert(high);
    puts("PASS held at startup does not count as a press");

    high = false;
    assert(!Button_Confirm(&high, true, 9, UINT32_MAX - 9U));
    assert(Button_Confirm(&high, true, 10, UINT32_MAX - 9U));
    puts("PASS unsigned tick rollover");
    return 0;
}
