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

struct Program_Data
{
  byte* Variables;
  byte* Program;
};

/* returns Program_Data struct that has split to variables and program*/
struct Program_Data Parse_File(char* filename);
