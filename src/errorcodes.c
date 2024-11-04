#include "errorcodes.h"

static const char Interpreter_Error[] = "IE";
static const char Runtime_Error[] = "RE";
static const char Parser_Error[] = "PE";
const int invalid_argument_error = 0;
const int file_not_found = 1;
const int error_reading_from_file = 2;
const int invalid_ppm_header = 3;
const char* Error_Table[][3] =
{
{Interpreter_Error,"IA","Invalid Interpreter Argument"},
{Parser_Error,"FN","File Not Found Or Could Not Be Opened"},
{Parser_Error,"ER","Error Reading From File Or EOF"},
{Parser_Error,"IH", "Invalid PPM Header"}
};


void Print_Error(int error_code, bool show_full_message)
{
  if(show_full_message)
    printf("%s:%s - %s\n",Error_Table[error_code][0],Error_Table[error_code][1],Error_Table[error_code][2]);
  else
    printf("%s:%s\n",Error_Table[error_code][0],Error_Table[error_code][1]);
  exit(1);
}
