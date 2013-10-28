#include "hwloc.h"
#include <stdlib.h>

#include "../../MA-place/include/ma_components.h"

int
main (int argc, char **argv)
{
  ma_place();
  ma_resume();
  ma_machine_topology();
  return 0;
} 
