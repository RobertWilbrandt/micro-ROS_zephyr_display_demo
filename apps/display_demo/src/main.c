#include "display.h"
#include "status.h"

void main(void) {
  atomic_set(&status, STATUS_CONNECTING);

  if (!display_init()) {
    atomic_set(&status, STATUS_ERROR);
    return;
  }

  while (true) {
    display_update();
    k_sleep(K_MSEC(25));
  }
}
