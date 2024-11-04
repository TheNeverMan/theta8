#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errorcodes.h"
#include "bool.h"

/*this should be probably somewhere else*/
typedef unsigned char byte;

struct RGB
{
  byte R;
  byte G;
  byte B;
};

/*returns char[64] that is ready to be pasted into interpreter*/
byte* Parse_File(char* filename);
