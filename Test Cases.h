// 
// TestCases.h -- defines IDs of each TestCase
//

#pragma once

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                 CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////

// Defines the groups of TestCases available
enum TEST_CASE_TYPE          { TCT_UNKNOWN, TCT_GAME_DATA, TCT_SCRIPT_TRANSLATION };

// Test Case IDs
enum TEST_CASE
{
   /// [GAME DATA]
   // [Missing data files]
   TC_MISSING_LANGUAGE_FILE              = 1,
   TC_MISSING_SPEECH_FILE                = 2,
   TC_MISSING_SOUND_EFFECT_FILE          = 3,
   TC_MISSING_VIDEO_FILE                 = 4,
   TC_MISSING_DOCKS_FILE                 = 5,
   TC_MISSING_FACTORIES_FILE             = 6,
   TC_MISSING_LASERS_FILE                = 7,
   TC_MISSING_MISSILES_FILE              = 8,
   TC_MISSING_SHIELDS_FILE               = 9,
   TC_MISSING_SHIPS_FILE                 = 10,
   TC_MISSING_WARE_B_FILE                = 11,
   TC_MISSING_WARE_E_FILE                = 12,
   TC_MISSING_WARE_F_FILE                = 13,
   TC_MISSING_WARE_M_FILE                = 14,
   TC_MISSING_WARE_N_FILE                = 15,
   TC_MISSING_WARE_T_FILE                = 16,

   // [Language Files]
   TC_LANGUAGE_BLANK                     = 17,
   TC_LANGUAGE_UNSUPPORTED_SCHEMA_TAG    = 18,
   TC_LANGUAGE_UNSUPPORTED_TAG           = 19,
   TC_LANGUAGE_UNCLOSED_STRING_TAG       = 20,
   TC_LANGUAGE_UNOPENED_STRING_TAG       = 21,
   TC_LANGUAGE_MISSING_STRING_TAG_ID     = 22,
   TC_LANGUAGE_UNSUPPORTED_TAG_PROPERTY  = 23,
   TC_LANGUAGE_MISSING_SCHEMA_TAG        = 24,
   TC_LANGUAGE_MISSING_LANGUAGE_TAG      = 25,
   TC_LANGUAGE_UNSUPPORTED_LANGUAGE_ID   = 26,

   // [TFiles]
   TC_TFILE_MISSING_OBJECT_COUNT         = 27,
   TC_TFILE_INCORRECT_OBJECT_COUNT       = 28,
   TC_TFILE_EMPTY_LINE                   = 29,
   TC_TFILE_UNKNOWN_NAME_ID              = 30,
   TC_TFILE_UNKNOWN_RACE_ID              = 31,

   /// [SCRIPT TRANSLATION]
   // [Missing XML tags]
   TC_MISSING_SCRIPT_TAG                 = 50,
   TC_MISSING_CODEARRAY_TAG              = 51,
   TC_MISSING_NAME_TAG                   = 52,
   TC_MISSING_DESCRIPTION_TAG            = 53,
   TC_MISSING_VERSION_TAG                = 54,
   TC_MISSING_ENGINEVERSION_TAG          = 55,
   TC_MISSING_ARGUMENTS_TAG              = 56,
   TC_MISSING_SOURCETEXT_TAG             = 57,
   
   // [Structural Flaws]
   TC_CODEARRAY_INCORRECT_SIZE           = 58,
   TC_CODEARRAY_INVALID_STANDARD_BRANCH  = 59,
   TC_CODEARRAY_EMPTY_STANDARD_BRANCH    = 60,
   TC_CODEARRAY_INVALID_AUXILIARY_BRANCH = 61,
   TC_CODEARRAY_INVALID_VARIABLES_BRANCH = 62,
   TC_CODEARRAY_MISSING_COMMAND_ID       = 63,

   // [Script Properties]
   TC_MISSING_SCRIPT_NAME                = 64,
   TC_MISSING_SCRIPT_DESCRIPTION         = 65,
   TC_MISSING_SCRIPT_VERSION             = 66,
   TC_MISSING_SCRIPT_ENGINEVERSION       = 67,
   
   // [Expressions]
   TC_EXPRESSION_ASSIGNMENT              = 68,
   TC_EXPRESSION_SIMPLE_SMALL            = 69,
   TC_EXPRESSION_SIMPLE_LARGE            = 70,
   TC_EXPRESSION_SIMPLE_HUGE             = 71,
   TC_EXPRESSION_COMPLEX_SMALL           = 72,
   TC_EXPRESSION_COMPLEX_LARGE           = 73,

   // [Conditionals]
   TC_CONDITIONAL_IF                     = 74,
   TC_CONDITIONAL_IF_NOT                 = 75,
   TC_CONDITIONAL_SKIP_IF                = 76,
   TC_CONDITIONAL_SKIP_IF_NOT            = 77,
   TC_CONDITIONAL_WHILE                  = 78,
   TC_CONDITIONAL_WHILE_NOT              = 79,
   TC_CONDITIONAL_ELSE                   = 80,
   TC_CONDITIONAL_ELSE_IF                = 81,
   TC_CONDITIONAL_ELSE_IF_NOT            = 82,

   // [Commands]
   TC_COMMAND_COMMENT                    = 83,
   TC_COMMAND_LABEL                      = 84,
   TC_COMMAND_INCREMENT                  = 85,
   TC_COMMAND_DECREMENT                  = 86,
   TC_COMMAND_WAIT                       = 87,
   TC_COMMAND_GOTO_LABEL                 = 88,
   TC_COMMAND_GOTO_SUBROUTINE            = 89,
   TC_COMMAND_BREAK                      = 90,
   TC_COMMAND_CONTINUE                   = 91,
   TC_COMMAND_END                        = 92,
   TC_COMMAND_RETURN                     = 93,

   // [Structural]
   TC_COMMAND_NODE_COUNT_MISMATCH        = 94,
   TC_COMMAND_NODE_COUNT_INCORRECT       = 95,

   // [Invalid Structural Values]
   TC_PARAMETER_INVALID_TYPE             = 96,
   TC_PARAMETER_INVALID_CONDITIONAL      = 97,
   TC_PARAMETER_INVALID_VARIABLE_ID      = 98,
   TC_PARAMETER_INVALID_COMMAND_ID       = 99,
   TC_PARAMETER_INVALID_LABEL_ID         = 100,
   TC_PARAMETER_INVALID_RETURN_VARIABLE  = 101,

   // [Valid Parameter Values]
   TC_PARAMETER_VALID_CONSTANT           = 102,
   TC_PARAMETER_VALID_DATATYPE           = 103,
   TC_PARAMETER_VALID_FLIGHT_RETURN      = 104,
   TC_PARAMETER_VALID_OBJECT_CLASS       = 105,
   TC_PARAMETER_VALID_OBJECT_COMMAND     = 106,
   TC_PARAMETER_VALID_OPERATOR           = 107,
   TC_PARAMETER_VALID_SCRIPTDEF          = 108,
   TC_PARAMETER_VALID_SECTOR             = 109,
   TC_PARAMETER_VALID_SHIP               = 110,
   TC_PARAMETER_VALID_STATION            = 111,
   TC_PARAMETER_VALID_STATION_SERIAL     = 112,
   TC_PARAMETER_VALID_TRANSPORT_CLASS    = 113,
   TC_PARAMETER_VALID_RACE               = 114,
   TC_PARAMETER_VALID_RELATION           = 115,
   TC_PARAMETER_VALID_WARE               = 116,
   TC_PARAMETER_VALID_WING_COMMAND       = 117,

   // [Invalid Parameter Values]
   TC_PARAMETER_INVALID_CONSTANT         = 118,
   TC_PARAMETER_INVALID_DATATYPE         = 119,
   TC_PARAMETER_INVALID_FLIGHT_RETURN    = 120,
   TC_PARAMETER_INVALID_OBJECT_CLASS     = 121,
   TC_PARAMETER_INVALID_OBJECT_COMMAND   = 122,
   TC_PARAMETER_INVALID_OPERATOR         = 123,
   TC_PARAMETER_INVALID_SCRIPTDEF        = 124,
   TC_PARAMETER_INVALID_SECTOR           = 125,
   TC_PARAMETER_INVALID_SHIP             = 126,
   TC_PARAMETER_INVALID_STATION          = 127,
   TC_PARAMETER_INVALID_STATION_SERIAL   = 128,
   TC_PARAMETER_INVALID_TRANSPORT_CLASS  = 129,
   TC_PARAMETER_INVALID_RACE             = 130,
   TC_PARAMETER_INVALID_RELATION         = 131,
   TC_PARAMETER_INVALID_WARE_MAINTYPE    = 132,
   TC_PARAMETER_INVALID_WARE_SUBTYPE     = 133,
   TC_PARAMETER_INVALID_WING_COMMAND     = 134,

   // [Illegal Characters]
   TC_PARAMETER_STRING_HUGE              = 135,
   TC_PARAMETER_STRING_ILLEGAL_CHARS     = 136,
   TC_PARAMETER_COMMENT_HUGE             = 137,
   TC_PARAMETER_COMMENT_ILLEGAL_CHARS    = 138,
   TC_LABEL_NAME_ILLEGAL_CHARS           = 139,
   TC_SCRIPT_NAME_ILLEGAL_CHARS          = 140,
   TC_SCRIPT_DESCRIPTION_ILLEGAL_CHARS   = 141,
   TC_ARGUMENT_NAME_ILLEGAL_CHARS        = 142,
   TC_ARGUMENT_DESCRIPTION_ILLEGAL_CHARS = 143,
   TC_SOURCE_TEXT_ILLEGAL_CHARS          = 144,

   // [Invalid Arguments]
   TC_ARGUMENT_INVALID_INDEX             = 145,
   TC_ARGUMENT_INVALID_TYPE              = 146,

   // [Feature Testing]
   TC_ENGINE_VERSION_UNSUPPORTED         = 147,
   TC_SCRIPT_CALL_TARGET_MISSING         = 148,
   TC_SCRIPT_CALL_TARGET_CORRUPT         = 149,
   TC_SCRIPT_CALL_TARGET_VALID           = 150,
   TC_GAME_STRING_REFERENCE_NOT_FOUND    = 151
};

