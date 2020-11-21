#pragma once

#include "pin_io.h"

#define CMDSUCCESS 1
#define CMDFAIL 0
#define NUMTOKS 6
#define BUFFLEN 256


char* cmd(char* buf);
