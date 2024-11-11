#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "bool.h"

extern const int invalid_argument_error;
extern const int file_not_found;
extern const int error_reading_from_file;
extern const int invalid_ppm_header;
extern const int runtime_eof;
extern const int runtime_after_eof;
extern const int invalid_command;
extern const int invalid_variable;
extern const int invalid_command_argument;
extern const int invalid_address;
extern const int unused_argument_error;
extern const char* Error_Table[][3];

void Print_Error(int error_code, bool show_full_message);
void Print_Warning(int warning_code, bool show_full_message);
void Print_Warning_As_Error(int warning_code, bool show_full_message);
