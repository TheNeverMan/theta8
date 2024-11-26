#!/bin/awk -f
#This program transcodes intermediate MLang programs to NetPBM bitmaps
# Intermediate notation standard
# label command arguments (in MLang order)
# labels: *labelname
# commands:
# Print - p
# p var
# Ask   - ask
# ask var
# Set   - s+
#s+ var1 -op> var2
# If    - if
# if var1 op [var2]
# op: == < > <= >= !=
# var2 is not parsed but it is usefull to write it anyway for readibility
# Math  - =
# [var2] = [var2] op var1
# var2 technically can be anything but MUST be something
# op: + - * / % ~& & |
# Jump  - jump
# jump [number|label]
# RID   - rid
# op var
# op: ++ -- << >> ~ rand <>
# End   - end
# end op
# op: normal, restart, reset, reset_random, stack_jump
# arguments:
# L B G C R M Y W - variables
# 56 - number
# ax - ASCII character
# variable initialization
# :x [number|ascii]
# arbitrary constant value in program
# . constant
# comments
# ;sdasdadsads

function Get_Char_In_ASCII(STRING,INDEX,   TMP_ARR)
{
  split(STRING,TMP_ARR,"")
  return ASCII[TMP_ARR[INDEX]]
}

function Change_To_b_Value(ARG)
{
  if(ARG ~ /^\*/)
    return ARG
  if(ARG ~ /^a/)
    return "b" Get_Char_In_ASCII(ARG,2)
  if(ARG ~ /^[0-9]{1,3}$/)
    return "b" ARG
  return COMMANDS[ARG]
}

function Validate_Variable(VAR)
{
  if(VAR !~ /[RGBCMYWL]/)
  {
    ERROR = "IV"
    ERROR_DATA = VAR
    exit -1
  }
}

function Validate_Set_Operation_And_Extract(OPERATION,   TMP_ARR)
{
  if(OPERATION !~ /-[RGBCMYWL]>/)
  {
    ERROR = "IS"
    ERROR_DATA = OPERATION
    exit -1
  }
  split(OPERATION,TMP_ARR,"")
  return TMP_ARR[2]
}

function Validate_If_Condition_And_Extract(CONDITION)
{
  if(CONDITION !~ /[(==)(<)(>)(<=)(>=)(!=)]/)
  {
    ERROR = "II"
    ERROR_DATA = CONDITION
    exit -1
  }
  return COMMANDS[CONDITION]
}

function Validate_Math_Operation_And_Extract(OPERATION)
{
  if(OPERATION !~ /[(+)(/)(-)(\*)(%)(&)(~&)(|)]/)
  {
    ERROR = "IM"
    ERROR_DATA = OPERATION
    exit -1
  }
  return COMMANDS[OPERATION]
}

function Validate_Address_And_Extract(ADDRESS)
{
  if(ADDRESS ~ /^\*/)
  {
#    if(LABELS[ADDRESS] == "")
#    {
#      ERROR = "IL"
#      ERROR_DATA = ADDRESS
#      exit -1
#    }
    return ADDRESS
  }
  if(ADDRESS ~ /^[[:digit:]]*$/)
  {
    if(ADDRESS < 56)
      return "b" ADDRESS
  }
  ERROR = "IA"
  ERROR_DATA = ADDRESS
  exit -1
}

function Validate_End_Mode_And_Extract(MODE)
{
  if(MODE !~ /[(normal)(restart)(reset)(reset_random)(stack_jump)]/)
  {
    ERROR = "IE"
    ERROR_DATA = MODE
    exit -1
  }
  return COMMANDS[MODE]
}

function Count_Words(STRING,   OUTZ)
{
  OUTZ = 1
  split(STRING,TMP_ARR," ")
  while(TMP_ARR[OUTZ] != "")
    OUTZ++
  return OUTZ-1
}

function Count_Bytes_To_Command(COMMAND_INDEX,   TMP,OUT)
{
  TMP = 0
  OUT = 0
  while(TMP < COMMAND_INDEX)
  {
    OUT = OUT + Count_Words(PROGRAM[TMP])
    TMP++
  }
  return OUT
}

function Resolve_Labels()
{
  for(LABEL in LABELS)
  {
    LABELS_ADDRESS[LABEL] += Count_Bytes_To_Command(LABELS[LABEL])
  }
}

function Get_Constant(VAL)
{
  if(VAL ~ /^a/)
  {
    VAL = Get_Char_In_ASCII(VAL,2)
    return VAL
  }
  if(VAL ~ /^[0-9]{1,3}$/)
  {
    return VAL
  }
  if(VAL ~ /^\*/)
  {
    return VAL
  }
  ERROR = "IC"
  ERROR_DATA = VAL
  exit -1
}

function Print_Pixel(STRING)
{
  if(STRING ~ /^c/)
  {
    split(STRING,TMP_ARR,"")
    if(TMP_ARR[2] == "0")
      print "0 0 0"
    if(TMP_ARR[2] == "1")
      print "0 0 255"
    if(TMP_ARR[2] == "2")
      print "0 255 0"
    if(TMP_ARR[2] == "3")
      print "0 255 255"
    if(TMP_ARR[2] == "4")
      print "255 0 0"
    if(TMP_ARR[2] == "5")
      print "255 0 255"
    if(TMP_ARR[2] == "6")
      print "255 255 0"
    if(TMP_ARR[2] == "7")
      print "255 255 255"
    return
  }
  if(STRING ~ /^b/)
  {
    split(STRING,TMP_ARR,"b")
    print TMP_ARR[2] " 0 0"
    return
  }
  if(STRING ~ /^\*/)
  {
    print LABELS_ADDRESS[STRING] " 0 0"
    return
  }
  ERROR = "IP"
  ERROR_DATA = STRING
  exit -1
}

function Get_Variable_Numeric(VAR)
{
  if(VARIABLES[VAR] ~ /^\*/)
    return LABELS_ADDRESS[VARIABLES[VAR]]
  return VARIABLES[VAR]
}

function Print_Var()
{
  if(PIXEL_COUNTER == 11)
  {
    Print_Pixel("b" Get_Variable_Numeric("L"))
    PIXEL_COUNTER++
  }
  if(PIXEL_COUNTER == 19)
  {
    Print_Pixel("b" Get_Variable_Numeric("B"))
    PIXEL_COUNTER++
  }
  if(PIXEL_COUNTER == 29)
  {
    Print_Pixel("b" Get_Variable_Numeric("G"))
    PIXEL_COUNTER++
  }
  if(PIXEL_COUNTER == 30)
  {
    Print_Pixel("b" Get_Variable_Numeric("C"))
    PIXEL_COUNTER++
  }
  if(PIXEL_COUNTER == 33)
  {
    Print_Pixel("b" Get_Variable_Numeric("R"))
    PIXEL_COUNTER++
  }
  if(PIXEL_COUNTER == 34)
  {
    Print_Pixel("b" Get_Variable_Numeric("M"))
    PIXEL_COUNTER++
  }
  if(PIXEL_COUNTER == 44)
  {
    Print_Pixel("b" Get_Variable_Numeric("Y"))
    PIXEL_COUNTER++
  }
  if(PIXEL_COUNTER == 52)
  {
    Print_Pixel("b" Get_Variable_Numeric("W"))
    PIXEL_COUNTER++
  }
}

function Output_PPM()
{
  #NetPBM Header
  print "P3"
  print "8 8"
  print "255"
  PIXEL_COUNTER = 0
  SIZE = 0
  for(COMMAND_INDEX in PROGRAM)
  {
    split(PROGRAM[COMMAND_INDEX],COMMAND," ")
    for(BYTE in COMMAND)
    {
      Print_Var()
      Print_Var()
      Print_Pixel(COMMAND[BYTE])
      SIZE++
      PIXEL_COUNTER++
    }
  }
  #fill remaining empty space with w h i t e
  print "Size " SIZE "/56 bytes" > "/dev/stderr"
  if(PIXEL_COUNTER > 64)
    print "TW:FS - Program Is Too Big - " PIXEL_COUNTER
  while(PIXEL_COUNTER < 64)
  {
    Print_Var()
    Print_Var()
    print "255 255 255"
    PIXEL_COUNTER++
  }
}

BEGIN {
  VARIABLES["L"] = 0
  VARIABLES["B"] = 0
  VARIABLES["G"] = 0
  VARIABLES["C"] = 0
  VARIABLES["R"] = 0
  VARIABLES["M"] = 0
  VARIABLES["Y"] = 0
  VARIABLES["W"] = 0
  COMMANDS["L"] = "c0"
  COMMANDS["B"] = "c1"
  COMMANDS["G"] = "c2"
  COMMANDS["C"] = "c3"
  COMMANDS["R"] = "c4"
  COMMANDS["M"] = "c5"
  COMMANDS["Y"] = "c6"
  COMMANDS["W"] = "c7"
  COMMANDS["stack_jump"] = "c0"
  COMMANDS["restart"] = "c2"
  COMMANDS["reset"] = "c3"
  COMMANDS["normal"] = "c4"
  COMMANDS["reset_random"] = "c6"
  COMMANDS["=="] = "c4"
  COMMANDS["<"] = "c2"
  COMMANDS[">"] = "c1"
  COMMANDS["<="] = "c3"
  COMMANDS[">="] = "c5"
  COMMANDS["!="] = "c6"
  COMMANDS["&"] = "c0"
  COMMANDS["*"] = "c1"
  COMMANDS["-"] = "c2"
  COMMANDS["/"] = "c3"
  COMMANDS["+"] = "c4"
  COMMANDS["%"] = "c5"
  COMMANDS["~&"] = "c6"
  COMMANDS["|"] = "c7"
  COMMANDS["<>"] = "c0"
  COMMANDS["<<"] = "c1"
  COMMANDS["--"] = "c2"
  COMMANDS[">>"] = "c3"
  COMMANDS["++"] = "c4"
  COMMANDS["~"] = "c5"
  COMMANDS["rand"] = "c6"
  ERROR = 0
  ERROR_DATA = 0
  ERRORS["IV"] = "Invalid Variable"
  ERRORS["IE"] = "Invalid End Mode"
  ERRORS["IS"] = "Invalid Set Operation"
  ERRORS["IM"] = "Invalid Math Operation"
  ERRORS["II"] = "Invalid If Condition"
  ERRORS["IL"] = "Invalid Label"
  ERRORS["IA"] = "Invalid Address"
  ERRORS["IC"] = "Invalid Constant, Expected Number or Character"
  ERRORS["IO"] = "Invalid Command"
  ERRORS["IP"] = "Invalid Pixel"
  COMMAND_INDEX = 0
  REWIND = 1
  for(n=0;n<256;n++) ASCII[sprintf("%c",n)]=n
}

#extract variable data
/^:/ {
  split($1,VAR,"");
  VAL = $2
  VARIABLES[VAR[2]] = Get_Constant(VAL)
}

#read code
 /^[^:;]/ {
  #extract and save labels if any
  split($0, COMMAND," ");
  TMP = 1
  if(COMMAND[TMP] ~ /^\*/)
  {
    LABEL_NAME = COMMAND[TMP]
    LABEL_OFFSET = 0
    if(LABEL_NAME ~ /,*/)
    {
      split(COMMAND[TMP],LABEL_DEF,",")
      LABEL_NAME = LABEL_DEF[1]
      if(LABEL_DEF[2] != "")
        LABEL_OFFSET = LABEL_DEF[2]
    }
    LABELS[LABEL_NAME] = COMMAND_INDEX
    LABELS_ADDRESS[LABEL_NAME] = LABEL_OFFSET
    print "Found label " LABEL_NAME " at " COMMAND_INDEX " with offset " LABBEL_OFFSET > "/dev/stderr"
    TMP++
  }
  if(COMMAND[TMP] == "p")
  {
    VARIABLE_TO_PRINT = COMMAND[++TMP]
    Validate_Variable(VARIABLE_TO_PRINT)
    PROGRAM[COMMAND_INDEX] = COMMANDS["R"] " " Change_To_b_Value(VARIABLE_TO_PRINT)
    COMMAND_INDEX++
    next
  }
  if(COMMAND[TMP] == "ask")
  {
    VARIABLE_TO_ASK = COMMAND[++TMP]
    Validate_Variable(VARIABLE_TO_ASK)
    PROGRAM[COMMAND_INDEX] = COMMANDS["G"] " " Change_To_b_Value(VARIABLE_TO_ASK)
    COMMAND_INDEX++
    next
  }
  if(COMMAND[TMP] == "s+")
  {
    VARIABLE_FROM = COMMAND[++TMP]
    #-X> where X is variable operation
    OPERATION = COMMAND[++TMP]
    VARIABLE_TO = COMMAND[++TMP]
    OPERATION = Validate_Set_Operation_And_Extract(OPERATION)
    PROGRAM[COMMAND_INDEX] = COMMANDS["B"] " " Change_To_b_Value(OPERATION) " " Change_To_b_Value(VARIABLE_FROM) " " Change_To_b_Value(VARIABLE_TO)
    COMMAND_INDEX++
    next
  }
  if(COMMAND[TMP] == "if")
  {
    VAR_IF = COMMAND[++TMP]
    CONDITION = COMMAND[++TMP]
    Validate_Variable(VAR_IF)
    CONDITION = Validate_If_Condition_And_Extract(CONDITION)
    PROGRAM[COMMAND_INDEX] = COMMANDS["C"] " " CONDITION " " Change_To_b_Value(VAR_IF)
    COMMAND_INDEX++
    next
  }
  if(COMMAND[TMP+1] == "=")
  {
    #2 = 2 + 1
    TMP += 3
    OPERATION = COMMAND[TMP++]
    VAR_MATH = COMMAND[TMP]
    Validate_Variable(VAR_MATH)
    OPERATION = Validate_Math_Operation_And_Extract(OPERATION)
    PROGRAM[COMMAND_INDEX] = COMMANDS["M"] " " OPERATION " " Change_To_b_Value(VAR_MATH)
    COMMAND_INDEX++
    next
  }
  if(COMMAND[TMP] == "jump")
  {
    ADDRESS = COMMAND[++TMP]
    ADDRESS = Validate_Address_And_Extract(ADDRESS)
    PROGRAM[COMMAND_INDEX] = COMMANDS["Y"] " " ADDRESS
    COMMAND_INDEX++
    next
  }
  if(COMMAND[TMP] == "end")
  {
    MODE = COMMAND[++TMP]
    MODE = Validate_End_Mode_And_Extract(MODE)
    PROGRAM[COMMAND_INDEX] = COMMANDS["W"] " " MODE
    COMMAND_INDEX++
    next
  }
  if(COMMAND[TMP] ~ /^[(++)(--)(<<)(>>)(~)(rand)(<>)]/)
  {
    OPERATION = COMMAND[TMP++]
    RID_VAR = COMMAND[TMP]
    Validate_Variable(RID_VAR)
    PROGRAM[COMMAND_INDEX] = COMMANDS["L"] " " COMMANDS[OPERATION] " " Change_To_b_Value(RID_VAR)
    COMMAND_INDEX++
    next
  }
  if(COMMAND[TMP] ~ /^\.$/)
  {
    CONSTANT = Get_Constant(COMMAND[++TMP])
    PROGRAM[COMMAND_INDEX] = Change_To_b_Value(CONSTANT)
    COMMAND_INDEX++
    next
  }
  ERROR = "IO"
  ERROR_DATA = $0
  exit -1
}

END {
  if(ERROR != 0)
  {
    print "TE:" ERROR " " ERRORS[ERROR] " - " ERROR_DATA > "/dev/stderr"
    exit -1
  }
  print "Variables:" > "/dev/stderr"
  for(INDEX in VARIABLES)
  {
    print INDEX " " VARIABLES[INDEX] > "/dev/stderr"
  }
  print "Program:" > "/dev/stderr"
  for(INDEX in PROGRAM)
  {
    print INDEX " " PROGRAM[INDEX] > "/dev/stderr"
  }
  print "Labels:" > "/dev/stderr"
  Resolve_Labels();
  Output_PPM();
}
