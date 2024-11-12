#include <stdio.h>
#include <stdlib.h>
#include "help_messages.h"
#include "interpreter.h"
#include "parser.h"
#include "runtime.h"

void Destroy_Objects()
{
  /* wytępić całe to bydło */
  /* tzn free memory */
  /* actually techically speaking os frees all the memory allocated by program
  at the exit, and this function was supposed to free memory after error call
  which use exit() functions to end the program. it is not possible to free
  allocated memory from there, but it turns out that there is no need to do that
  so yeah it can just be empty, as normal memory management is done normally*/
}

int main(int argv, char** argc)
{
  struct Interpreter_Data Interpreter_Flags;
  struct Program_Data Loaded_Program_Data;
  atexit(Destroy_Objects);
  if(argv == 1)
  {
    Show_Help_Message();
    return 0;
  }
  Interpreter_Flags = Generate_Interpreter_Data(argv, argc);
  if(!Interpreter_Flags.quiet)
    puts(Interpreter_Flags.filepath);
  Loaded_Program_Data = Parse_File(Interpreter_Flags.filepath);
  Interpret(Interpreter_Flags, Loaded_Program_Data);
  return 0;

}

/*TheNeverMan 2024*/
