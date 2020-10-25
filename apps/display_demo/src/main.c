#include "status.h"

void main(void) {
  atomic_set(&status, STATUS_CONNECTING);

  while (true) {
    k_sleep(K_MSEC(1000));
  }
}
