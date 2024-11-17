#pragma once
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include "interpreter.h"
#include "parser.h"
#include "errorcodes.h"

struct Runtime
{
  const struct Interpreter_Data Flags;
  struct Program_Data Program;
  byte Default_Variables[8];
  int program_counter;
  bool is_out_of_file;
  int stack_pointer;
  byte Stack[16];
  byte Default_Code[56];
};

void Interpret(struct Interpreter_Data Flags, struct Program_Data Program);
