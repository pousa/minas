#include "ma_components.h"

int main()
{
  int my_core;
  ma_place();

  ma_phys_pu_topology();

  my_core = ma_proc_core();
}
