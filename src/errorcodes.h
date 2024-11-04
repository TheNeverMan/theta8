#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "bool.h"

extern const int invalid_argument_error;
extern const int file_not_found;
extern const int error_reading_from_file;
extern const int invalid_ppm_header;
extern const char* Error_Table[][3];

void Print_Error(int error_code, bool show_full_message);
