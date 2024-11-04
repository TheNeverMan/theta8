#pragma once
#include <stdlib.h>
#include <string.h>
#include "bool.h"
#include "errorcodes.h"
#include "help_messages.h"

struct Interpreter_Data
{
    bool is_in_debug_mode;
    bool is_in_step_by_step_mode;
    bool treat_warnings_as_errors;
    bool display_full_error_messages;
    bool display_warnings;
    bool quiet;
    bool show_prompt_on_ask_command;
    char* filepath;
};

struct Interpreter_Data Generate_Interpreter_Data(const int argv, char **argc);
