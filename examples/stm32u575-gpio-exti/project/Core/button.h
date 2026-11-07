#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include <stdint.h>

#define BUTTON_DEBOUNCE_MS 20U

/* Update the stable level and return true only for a confirmed press. */
bool Button_Confirm(bool *stable_high, bool pin_high,
                    uint32_t now_ms, uint32_t edge_ms);

#endif
