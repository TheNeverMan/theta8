#include "runtime.h"

#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define YELLOW 6
#define WHITE 7

static void Pretty_Display_Variable_Values(const struct Runtime* const Env)
{
  int var_index = 0;
  char names[8] = {'L','B','G','C','R','M','Y','W'};
  puts("Loaded variables:");
  while(var_index < 8)
  {
    printf("%c: %i (0x%X) '%c'\n", names[var_index],Env->Program.Variables[var_index],Env->Program.Variables[var_index],Env->Program.Variables[var_index]);
    var_index++;
  }
}

static void Trigger_Warning(const struct Runtime* const Env, const int warning_code)
{
  if(!(Env->Flags.display_warnings))
    return;
  if(!(Env->Flags.quiet))
    printf("Warning At %d:\n",Env->program_counter);
  if(Env->Flags.treat_warnings_as_errors)
    Print_Warning_As_Error(warning_code,Env->Flags.display_full_error_messages);
  else
    Print_Warning(warning_code,Env->Flags.display_full_error_messages);
}

static void Trigger_Error(const struct Runtime* const Env, const int error_code)
{
  if(!(Env->Flags.quiet))
    printf("Error At %d:\n",Env->program_counter);
  Print_Error(error_code, Env->Flags.display_full_error_messages);
}

static byte Get_Next_Pixel(struct Runtime* const Env)
{
  Env->program_counter++;
  if(Env->program_counter == 56)
  {
    Trigger_Warning(Env, runtime_eof);
    return RED;
  }
  if(Env->program_counter > 56)
  {
    Trigger_Warning(Env, runtime_after_eof);
    return RED;
  }
  return Env->Program.Program[Env->program_counter];
}

static void Validate_Variable(const struct Runtime* const Env,const byte value)
{
  if(value < 0 || value > 7)
    Trigger_Error(Env, invalid_variable);
}

static byte Get_Var(const struct Runtime* const Env, const byte var)
{
  return Env->Program.Variables[var % 8];
}

static void Command_Print(struct Runtime* const Env)
{
  byte variable_to_print = Get_Next_Pixel(Env);
  Validate_Variable(Env,variable_to_print);
  if(variable_to_print < 4)
    putchar(Get_Var(Env, variable_to_print));
  else
    printf("%d",Get_Var(Env, variable_to_print));
}

static bool Interpret_Command(struct Runtime* const Env)
{
  if(Env->Flags.is_in_debug_mode)
    Pretty_Display_Variable_Values(Env);
  const byte command = Get_Next_Pixel(Env);
  bool loop = TRUE;
  switch(command)
  {
    case BLACK:
    {
      break;
    }
    case BLUE:
    {
      break;
    }
    case GREEN:
    {
      break;
    }
    case CYAN:
    {
      break;
    }
    case RED:
    {
      Command_Print(Env);
      break;
    }
    case MAGENTA:
    {
      break;
    }
    case YELLOW:
    {
      break;
    }
    case WHITE:
    {
      break;
    }
    default:
    {
      Trigger_Error(Env, invalid_command);
      break;
    }
  }
  return loop;
}

void Interpret(struct Interpreter_Data Flags, struct Program_Data Program)
{
  struct Runtime Env = {.Flags = Flags, .Program = Program, .program_counter = 0, .is_out_of_file = 0};
  bool loop = TRUE;
  while(loop && !(Env.is_out_of_file))
  {
    loop = Interpret_Command(&Env);
  }
  free(Program.Variables);
  free(Program.Program);
}
