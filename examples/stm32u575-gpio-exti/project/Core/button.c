#include "button.h"

// textbook:begin confirm
bool Button_Confirm(bool *stable_high, bool pin_high,
                    uint32_t now_ms, uint32_t edge_ms)
{
    if ((pin_high == *stable_high) || (now_ms - edge_ms < BUTTON_DEBOUNCE_MS)) {
        return false;
    }

    *stable_high = pin_high;
    return pin_high;
}
// textbook:end confirm
