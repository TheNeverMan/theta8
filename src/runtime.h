#pragma once
#include <ctype.h>
#include "interpreter.h"
#include "parser.h"
#include "errorcodes.h"

struct Runtime
{
  const struct Interpreter_Data Flags;
  struct Program_Data Program;
  int program_counter;
  bool is_out_of_file;
};

void Interpret(struct Interpreter_Data Flags, struct Program_Data Program);
