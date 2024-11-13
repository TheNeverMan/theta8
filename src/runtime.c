#include "runtime.h"

#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define YELLOW 6
#define WHITE 7

static const byte Command_Length[8] = {3,4,2,3,2,3,2,2};
/*{
  {BLACK,3},
  {BLUE,4},
  {GREEN,2},
  {CYAN,3},
  {RED,2},
  {MAGENTA,2},
  {YELLOW,2},
  {WHITE,2}
};*/

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
  printf("PC: %i (0x%X)\n",Env->program_counter, Env->Program.Program[(Env->program_counter+1)%56]);
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
    return WHITE;
  }
  if(Env->program_counter > 56)
  {
    Trigger_Warning(Env, runtime_after_eof);
    return WHITE;
  }
  return Env->Program.Program[Env->program_counter];
}

static void Increment_Program_Counter_By_Value(struct Runtime* const Env, const byte value)
{
  int index = 0;
  while(index < value)
  {
    Get_Next_Pixel(Env);
    index++;
  }
}

static void Validate_Variable(const struct Runtime* const Env,const byte value)
{
  if(value < 0 || value > 7)
    Trigger_Error(Env, invalid_variable);
}

static void Validate_Adress(const struct Runtime* const Env,const byte value)
{
  if(value < 0 || value > 55)
    Trigger_Error(Env, invalid_address);
}

static void Validate_Argument(const struct Runtime* const Env,const byte value)
{
  if(value < 0 || value > 7)
    Trigger_Error(Env, invalid_command_argument);
}

static byte Get_Next_Var(const struct Runtime* const Env, const byte var)
{
  Validate_Variable(Env,var);
  return Env->Program.Variables[(var+1) % 8];
}

static byte Get_Var(const struct Runtime* const Env, const byte var)
{
  Validate_Variable(Env,var);
  return Env->Program.Variables[var % 8];
}

static void Set_Var(struct Runtime* const Env, const byte var, const byte value)
{
  Validate_Variable(Env,var);
  Env->Program.Variables[var % 8] = value;
}

static byte Get_Addr(const struct Runtime* const Env, const byte addr)
{
  Validate_Adress(Env,addr);
  return Env->Program.Program[addr % 56];
}

static void Set_Addr(struct Runtime* const Env, const byte addr, const byte value)
{
  Validate_Adress(Env,addr);
  Env->Program.Program[addr % 56] = value;
}


static char Get_Variable_Name(const byte variable)
{
  char names[8] = {'L','B','G','C','R','M','Y','W'};
  return names[variable % 8];
}

static char Get_If_Condition_Name(const byte variable)
{
  char names[8] = {'?','<','>',']','=','[','!','?'};
  return names[variable % 8];
}

static char Get_Math_Operation_Name(const byte variable)
{
  char names[8] = {'&','*','-','/','+','%','N','|'};
  return names[variable % 8];
}

static void Command_Print(struct Runtime* const Env)
{
  byte variable_to_print = Get_Next_Pixel(Env);
  if(Env->Flags.is_in_debug_mode)
    printf("Print %c\n",Get_Variable_Name(variable_to_print));
  if(variable_to_print < 4)
    putchar(Get_Var(Env, variable_to_print));
  else
    printf("%d",Get_Var(Env, variable_to_print));
}

static void Command_Ask(struct Runtime* const Env)
{
  byte variable_to_ask = Get_Next_Pixel(Env);
  char format_mask[4] = "%i";
  byte buffer = ' ';
  bool loop = FALSE;
  if(Env->Flags.is_in_debug_mode)
    printf("Ask %c\n",Get_Variable_Name(variable_to_ask));
  if(variable_to_ask < 4)
    strcpy(format_mask, "%1c");
  do
  {
    if(Env->Flags.show_prompt_on_ask_command)
      printf("?");
    loop = scanf(format_mask,&buffer);
    if(!loop)
    {
      byte c;
      do {
        c = getchar();
      }
      while (!isdigit(c));
      ungetc(c, stdin);
    }
  }
  while(!loop);
  Set_Var(Env,variable_to_ask,buffer);
}

static void Command_Set(struct Runtime* const Env)
{
  byte argument = Get_Next_Pixel(Env);
  byte val_1 = Get_Next_Pixel(Env);
  byte val_2 = Get_Next_Pixel(Env);
  Validate_Argument(Env, argument);
  if(Env->Flags.is_in_debug_mode)
    printf("Set %c -%c> %c\n",Get_Variable_Name(val_1),Get_Variable_Name(argument),Get_Variable_Name(val_2));
  switch(argument)
  {
    case RED:
    {
      Set_Var(Env,val_2,Get_Var(Env,val_1));
      break;
    }
    case GREEN:
    {
      Set_Var(Env,val_2,val_1);
      break;
    }
    case BLUE:
    {
      Set_Addr(Env,val_2,Get_Var(Env,val_1));
      break;
    }
    case CYAN:
    {
      Set_Addr(Env,val_2,val_1);
      break;
    }
    case MAGENTA:
    {
      Set_Var(Env,val_2,Get_Addr(Env,val_1));
      break;
    }
    case YELLOW:
    {
      Set_Addr(Env,val_2,Get_Addr(Env,val_1));
      break;
    }
    default:
    {
      Trigger_Error(Env, unused_argument_error);
      break;
    }
  }
}

static void Command_If(struct Runtime* const Env)
{
  byte condition = Get_Next_Pixel(Env);
  byte var_1 = Get_Next_Pixel(Env);
  byte val_1 = Get_Var(Env,var_1);
  byte val_2 = Get_Next_Var(Env,var_1);
  bool skip_next_command = TRUE;
  Validate_Argument(Env, condition);
  if(Env->Flags.is_in_debug_mode)
    printf("If %c %c %c\n",Get_Variable_Name(var_1),Get_If_Condition_Name(condition),Get_Variable_Name(var_1+1));
  switch(condition)
  {
    case RED:
    {
      if(val_1 == val_2)
        skip_next_command = FALSE;
      break;
    }
    case GREEN:
    {
      if(val_1 < val_2)
        skip_next_command = FALSE;
      break;
    }
    case BLUE:
    {
      if(val_1 > val_2)
        skip_next_command = FALSE;
      break;
    }
    case CYAN:
    {
      if(val_1 <= val_2)
        skip_next_command = FALSE;
      break;
    }
    case MAGENTA:
    {
      if(val_1 >= val_2)
        skip_next_command = FALSE;
      break;
    }
    case YELLOW:
    {
      if(val_1 != val_2)
        skip_next_command = FALSE;
      break;
    }
    default:
    {
      Trigger_Error(Env, unused_argument_error);
    }
  }
  if(skip_next_command)
  {
    byte next_com = Get_Next_Pixel(Env);
    Validate_Variable(Env,next_com);
    /* already decremented by Get_Next_Pixel*/
    Increment_Program_Counter_By_Value(Env,Command_Length[next_com]-1);
  }
}

static void Command_Math(struct Runtime* const Env)
{
  byte operation = Get_Next_Pixel(Env);
  byte var_1 = Get_Next_Pixel(Env);
  byte val_1 = Get_Var(Env,var_1);
  byte val_2 = Get_Next_Var(Env,var_1);
  Validate_Argument(Env, operation);
  if(Env->Flags.is_in_debug_mode)
    printf("Math %c = %c %c %c\n",Get_Variable_Name(var_1+1),Get_Variable_Name(var_1+1),Get_Math_Operation_Name(operation),Get_Variable_Name(var_1));
  switch(operation)
  {
    case RED:
    {
      Set_Var(Env,var_1+1,(val_2 + val_1));
      break;
    }
    case GREEN:
    {
      Set_Var(Env,var_1+1,(val_2 - val_1));
      break;
    }
    case BLUE:
    {
      Set_Var(Env,var_1+1,(val_2 * val_1));
      break;
    }
    case CYAN:
    {
      Set_Var(Env,var_1+1,(val_2 / val_1));
      break;
    }
    case MAGENTA:
    {
      Set_Var(Env,var_1+1,(val_2 % val_1));
      break;
    }
    case YELLOW:
    {
      Set_Var(Env,var_1+1,~(val_2 & val_1));
      break;
    }
    case BLACK:
    {
      Set_Var(Env,var_1+1,(val_2 & val_1));
      break;
    }
    case WHITE:
    {
      Set_Var(Env,var_1+1,~(val_2 | val_1));
      break;
    }
    default:
    {
      Trigger_Error(Env, unused_argument_error);
    }
  }
}

static void Command_Jump(struct Runtime* const Env)
{

}

static void Command_End(struct Runtime* const Env)
{

}

static void Command_RID(struct Runtime* const Env)
{
  
}

static bool Interpret_Command(struct Runtime* const Env)
{
  const byte command = Get_Next_Pixel(Env);
  bool loop = TRUE;
  if(Env->Flags.is_in_debug_mode)
    Pretty_Display_Variable_Values(Env);
  switch(command)
  {
    case BLACK:
    {
      Command_RID(Env);
      break;
    }
    case BLUE:
    {
      Command_Set(Env);
      break;
    }
    case GREEN:
    {
      Command_Ask(Env);
      break;
    }
    case CYAN:
    {
      Command_If(Env);
      break;
    }
    case RED:
    {
      Command_Print(Env);
      break;
    }
    case MAGENTA:
    {
      Command_Math(Env);
      break;
    }
    case YELLOW:
    {
      Command_Jump(Env);
      break;
    }
    case WHITE:
    {
      Command_End(Env);
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
  struct Runtime Env = {.Flags = Flags, .Program = Program, .program_counter = -1, .is_out_of_file = 0};
  bool loop = TRUE;
  while(loop && !(Env.is_out_of_file))
  {
    loop = Interpret_Command(&Env);
    if(Env.Flags.is_in_step_by_step_mode)
      getchar();
  }
  free(Program.Variables);
  free(Program.Program);
}
