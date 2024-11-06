#include "errorcodes.h"

static const char Interpreter_Error[] = "IE";
static const char Runtime_Error[] = "RE";
static const char Parser_Error[] = "PE";
static const char Runtime_Warning[] = "RW";
const int invalid_argument_error = 0;
const int file_not_found = 1;
const int error_reading_from_file = 2;
const int invalid_ppm_header = 3;
const int runtime_eof = 4;
const int runtime_after_eof = 5;
const int invalid_command = 6;
const int invalid_variable = 7;

const char* Error_Table[][3] =
{
{Interpreter_Error,"IA","Invalid Interpreter Argument"},
{Parser_Error,"FN","File Not Found Or Could Not Be Opened"},
{Parser_Error,"ER","Error Reading From File Or EOF"},
{Parser_Error,"IH", "Invalid PPM Header"},
{Runtime_Warning, "EF", "Program Counter Reached EOF"},
{Runtime_Warning, "AE", "Program Counter Is After EOF - Returning Default Value"},
{Runtime_Error,"IC","Invalid Command"},
{Runtime_Error,"IV","Invalid Variable"}
};


void Print_Error(int error_code, bool show_full_message)
{
  if(show_full_message)
    printf("%s:%s - %s\n",Error_Table[error_code][0],Error_Table[error_code][1],Error_Table[error_code][2]);
  else
    printf("%s:%s\n",Error_Table[error_code][0],Error_Table[error_code][1]);
  exit(1);
}

void Print_Warning(int warning_code, bool show_full_message)
{
  if(show_full_message)
    printf("%s:%s - %s\n",Error_Table[warning_code][0],Error_Table[warning_code][1],Error_Table[warning_code][2]);
  else
    printf("%s:%s\n",Error_Table[warning_code][0],Error_Table[warning_code][1]);
}

void Print_Warning_As_Error(int warning_code, bool show_full_message)
{
  Print_Warning(warning_code, show_full_message);
  exit(1);
}
