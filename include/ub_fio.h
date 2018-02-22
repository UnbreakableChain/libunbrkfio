#include <stdio.h>
#include "ub_core.h"
#include "ub_io_err.h"


enum UbIOErr fread_UbChain(UbChain** chain, FILE* file);

enum UbIOErr fwrite_UbChain(UbChain* chain, FILE* file);
