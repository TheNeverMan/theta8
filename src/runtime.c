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
    printf("%c: %i (0x%X)", names[var_index],Env->Program.Variables[var_index],Env->Program.Variables[var_index]);
    if(Env->Program.Variables[var_index] > 31)
      printf(" '%c'",Env->Program.Variables[var_index]);
    printf("\n");
    var_index++;
  }
  printf("PC: %i (0x%X)\n",Env->program_counter, Env->Program.Program[(Env->program_counter+1)%56]);
  printf("SP: %i (0x%X) SD: %i\n",Env->Stack[Env->stack_pointer],Env->Stack[Env->stack_pointer],Env->stack_pointer);
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

static char Get_End_Mode_Name(const byte variable)
{
  char names[8] = {'J','?','R','r','E','?','l','E'};
  return names[variable % 8];
}

static char Get_RID_Operation_Name(const byte variable)
{
  char names[8] = {'^','<','-','>','+','~','r','?'};
  return names[variable % 8];
}

static void Push_Current_Address_To_Stack(struct Runtime* const Env)
{
  if(Env->stack_pointer == 15)
    Trigger_Warning(Env,stack_full);
  else
    Env->stack_pointer++;
  Env->Stack[Env->stack_pointer] = Env->program_counter;
}

static byte Pop_Address_From_Stack(struct Runtime* const Env)
{
  byte out = 0;
  if(!(Env->stack_pointer))
  {
    Trigger_Warning(Env,stack_empty);
    return Env->program_counter;
  }
  out = Env->Stack[Env->stack_pointer];
  Env->stack_pointer--;
  return out;
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
  const byte variable_to_ask = Get_Next_Pixel(Env);
  unsigned int tmp = 0;
  bool loop = FALSE;
  if(Env->Flags.is_in_debug_mode)
    printf("Ask %c\n",Get_Variable_Name(variable_to_ask));

  if(Env->Flags.show_prompt_on_ask_command)
    printf("?");
  /* i hate c io i hate c io i hate c io */
  /*fflush(stdin);
  fflush(stdout);*/
  if(variable_to_ask < 4)
  {
    tmp = getchar();
  }
  else
  {
    do
    {
      loop = scanf("%u",&tmp);
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
  }
  Set_Var(Env,variable_to_ask,(byte)tmp);
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
      Set_Var(Env,(var_1+1) % 8,(val_2 + val_1));
      break;
    }
    case GREEN:
    {
      Set_Var(Env,(var_1+1) % 8,(val_2 - val_1));
      break;
    }
    case BLUE:
    {
      Set_Var(Env,(var_1+1) % 8,(val_2 * val_1));
      break;
    }
    case CYAN:
    {
      if(!val_1)
        Trigger_Error(Env,division_by_zero);
      Set_Var(Env,(var_1+1) % 8,(val_2 / val_1));
      break;
    }
    case MAGENTA:
    {
      if(!val_1)
        Trigger_Error(Env,division_by_zero);
      Set_Var(Env,(var_1+1) % 8,(val_2 % val_1));
      break;
    }
    case YELLOW:
    {
      Set_Var(Env,(var_1+1) % 8,~(val_2 & val_1));
      break;
    }
    case BLACK:
    {
      Set_Var(Env,(var_1+1) % 8,(val_2 & val_1));
      break;
    }
    case WHITE:
    {
      Set_Var(Env,(var_1+1) % 8,(val_2 | val_1));
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
  byte address = Get_Next_Pixel(Env);
  Validate_Adress(Env,address);
  if(Env->Flags.is_in_debug_mode)
    printf("Jump %i\n",address);
  if(!address)
  {
    Env->program_counter = Pop_Address_From_Stack(Env);
    return;
  }
  Push_Current_Address_To_Stack(Env);
  Env->program_counter = address - 1;
}

static bool Command_End(struct Runtime* const Env)
{
  bool out = TRUE;
  byte operation = Get_Next_Pixel(Env);
  Validate_Argument(Env, operation);
  if(Env->Flags.is_in_debug_mode)
    printf("End %c\n",Get_End_Mode_Name(operation));
  switch(operation)
  {
    case RED:
    {
      out = FALSE;
      break;
    }
    case GREEN:
    {
      memcpy(Env->Program.Variables, Env->Default_Variables, sizeof(byte)* 8);
      memcpy(Env->Program.Program, Env->Default_Code, sizeof(byte)* 56);
      Env->stack_pointer = 0;
      Env->program_counter = -1;
      break;
    }
    case BLUE:
    {
      Trigger_Error(Env, unused_argument_error);
      break;
    }
    case CYAN:
    {
      memcpy(Env->Program.Program, Env->Default_Code, sizeof(byte)* 56);
      Env->stack_pointer = 0;
      Env->program_counter = -1;
      break;
    }
    case MAGENTA:
    {
      Trigger_Error(Env, unused_argument_error);
      break;
    }
    case YELLOW:
    {
      memcpy(Env->Program.Program, Env->Default_Code, sizeof(byte)* 56);
      Set_Var(Env,BLACK,rand() % 256);
      Env->stack_pointer = 0;
      Env->program_counter = -1;
      break;
    }
    case BLACK:
    {
      Push_Current_Address_To_Stack(Env);
      Env->program_counter = -1;
      break;
    }
    case WHITE:
    {
      out = FALSE;
      break;
    }
    default:
    {
      Trigger_Error(Env, unused_argument_error);
    }
  }
  return out;
}

static void Command_RID(struct Runtime* const Env)
{
  byte operation = Get_Next_Pixel(Env);
  byte var_1 = Get_Next_Pixel(Env);
  byte val_1 = Get_Var(Env,var_1);
  Validate_Argument(Env, operation);
  if(Env->Flags.is_in_debug_mode)
    printf("RID %c %c\n",Get_Variable_Name(var_1),Get_RID_Operation_Name(operation));
  switch(operation)
  {
    case RED:
    {
      Set_Var(Env,var_1,++val_1);
      break;
    }
    case GREEN:
    {
      Set_Var(Env,var_1,--val_1);
      break;
    }
    case BLUE:
    {
      Set_Var(Env,var_1,val_1 << 1);
      break;
    }
    case CYAN:
    {
      Set_Var(Env,var_1,val_1 >> 1);
      break;
    }
    case MAGENTA:
    {
      Set_Var(Env,var_1,~val_1);
      break;
    }
    case YELLOW:
    {
      Set_Var(Env,var_1,rand()%256);
      break;
    }
    case BLACK:
    {
      byte tmp = Get_Next_Var(Env,var_1);
      Set_Var(Env,((var_1 + 1)%8),val_1);
      Set_Var(Env, var_1, tmp);
      break;
    }
    case WHITE:
    {
      Trigger_Error(Env, unused_argument_error);
      break;
    }
    default:
    {
      Trigger_Error(Env, unused_argument_error);
    }
  }
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
      loop = Command_End(Env);
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
  struct Runtime Env = {Flags,Program,{0},-1,0,0,{0},{0}};
  bool loop = TRUE;
  srand(time(NULL));
  memcpy(Env.Default_Variables, Env.Program.Variables, sizeof(byte)* 8);
  memcpy(Env.Default_Code, Env.Program.Program, sizeof(byte)* 56);
  if(!Env.Flags.quiet)
  {
    int index = 0;
    while(index < 56)
      printf("%i ",Env.Program.Program[index++]);
    printf("\n");
  }

  while(loop && !(Env.is_out_of_file))
  {
    char c;
    loop = Interpret_Command(&Env);
    if(Env.Flags.is_in_step_by_step_mode)
      while ((c = getchar()) != '\n' && c != EOF) { }
      /*getchar();*/
    fflush(stdout);
  }
  free(Program.Variables);
  free(Program.Program);
}
