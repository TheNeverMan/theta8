#include "interpreter.h"

static void Initialize_Interpreter_Data_Struct(struct Interpreter_Data* Data)
{
  Data->is_in_debug_mode = FALSE;
  Data->is_in_step_by_step_mode = FALSE;
  Data->treat_warnings_as_errors = FALSE;
  Data->display_full_error_messages = FALSE;
  Data->display_warnings = FALSE;
  Data->quiet = FALSE;
  Data->show_prompt_on_ask_command = FALSE;
}

struct Interpreter_Data Generate_Interpreter_Data(const int argv, char **argc)
{
  struct Interpreter_Data out;
  int index = 1; /*first argc is program name*/
  char option = argc[index][1];
  Initialize_Interpreter_Data_Struct(&out);
  do
  {
    option = argc[index][1];
    if(index == argv-1)
      break;
    if((strlen(argc[index]) != 2) && (index < (argv-1)))
    {
      Print_Error(invalid_argument_error, TRUE);
    }
    switch(option)
    {
      case 'v':
      {
        Show_Version();
        exit(0);
        break;
      }
      case 'h':
      {
        Show_Help_Message();
        exit(0);
        break;
      }
      case 'q':
      {
        out.quiet = TRUE;
        break;
      }
      case 'd':
      {
        out.is_in_debug_mode = TRUE;
        break;
      }
      case 's':
      {
        out.is_in_step_by_step_mode = TRUE;
        break;
      }
      case 'w':
      {
        out.display_warnings = TRUE;
        break;
      }
      case 'e':
      {
        out.display_full_error_messages = TRUE;
        break;
      }
      case 'p':
      {
        out.display_warnings = TRUE; /* it only makes sense then */
        out.treat_warnings_as_errors = TRUE;
        break;
      }
      case 'a':
      {
        out.show_prompt_on_ask_command = TRUE;
        break;
      }
      default:
      {
        if((index < (argv-1)))
        {
          Print_Error(invalid_argument_error, TRUE);
        }
      }
    }
    index++;

  }
  while(index < (argv-1));
  /*last argument is filename*/
  out.filepath = argc[argv-1];
  return out;
}
