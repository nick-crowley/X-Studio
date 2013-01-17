
/// ///////////////////////////////////////////////////////
///                    RESOURCE FILES
/// ///////////////////////////////////////////////////////

// Accessed using strings:

// 'COMMAND_SYNTAX'       --  Command Syntax.txt
// 'OBJECT_DESCRIPTIONS'  --  Object Descriptions.xml

/// ///////////////////////////////////////////////////////
///                    BITMAP
/// ///////////////////////////////////////////////////////

#define IDB_MAIN_TOOLBAR                                 100
#define IDB_LANGUAGE_TOOLBAR                             101
#define IDB_FILE_TOOLBAR                                 102
#define IDB_MESSAGE_TOOLBAR                              103

/// ///////////////////////////////////////////////////////
///                  ERROR  MESSAGES
/// ///////////////////////////////////////////////////////

// Initialisation
#define IDS_INIT_FAILED                                  10
#define IDS_INIT_COMMON_CONTROLS_FAILED                  11
#define IDS_INIT_MAIN_WINDOW_FAILED                      12
#define IDS_INIT_WINDOW_CLASSES_FAILED                   13
#define IDS_INIT_REGISTER_CLASS_FAILED                   14
#define IDS_INIT_PREFERENCES_CORRUPT                     15
#define IDS_INIT_PREFERENCES_UPGRADED                    16
#define IDS_INIT_RICH_EDIT_FAILED                        17
#define IDS_INIT_NO_GAME_DATA                            18
#define IDS_INIT_NO_GAME_FOLDER                          19
#define IDS_INIT_GAME_DATA_LOST                          20
#define IDS_INIT_GAME_DATA_FOUND                         21

/// Error messages
// General
#define IDS_ERROR_MESSAGE_NOT_FOUND                      25

// Attachments
#define IDS_ERROR_APPEND_COMMAND                         26
#define IDS_ERROR_APPEND_XML                             27    
#define IDS_ERROR_APPEND_TEXT                            28
#define IDS_ERROR_APPEND_OUTPUT                          29    // [SPECIAL] Has no matching resource string


/// NOTE: The ID 32 is reserved by IDM_SHOW_HIDE_CONSOLE and cannot be used for anything else


/// File System
// Virtual File System
#define IDS_VFS_GAME_FOLDER_NOT_FOUND                    40
#define IDS_VFS_CATALOGUES_NOT_FOUND                     41
#define IDS_VFS_ERROR                                    42

// File I/O
#define IDS_LOAD_FILE_SYSTEM_ERROR                       50
#define IDS_LOAD_FILE_NOT_FOUND                          51
#define IDS_LOAD_ALTERNATE_FILE_NOT_FOUND                52
#define IDS_SAVE_FILE_SYSTEM_ERROR                       53

// GZip
#define IDS_GZIP_MAGIC_NUMBER                            60
#define IDS_GZIP_COMPRESSION_TYPE                        61
#define IDS_GZIP_ZLIB_DEFLATE_ERROR                      62
#define IDS_GZIP_ZLIB_INFLATE_ERROR                      63
#define IDS_GZIP_CRC_FAILURE                             64
#define IDS_FILE_UTF16_UNSUPPORTED                       65
#define IDS_FILE_EMPTY                                   66


/// Script Generation
// Identification
#define IDS_GENERATION_CONDITIONAL_UNRECOGNISED          99
#define IDS_GENERATION_CONDITIONAL_HASH_UNRECOGNISED     100
#define IDS_GENERATION_COMMAND_SYNTAX_NOT_FOUND          101
#define IDS_GENERATION_COMMAND_SYNTAX_INVALID            102


// Return/Reference Objects
#define IDS_GENERATION_RETURN_OBJECT_MISSING             105
#define IDS_GENERATION_RETURN_OBJECT_INVALID             106
#define IDS_GENERATION_REFERENCE_OBJECT_MISSING          107
#define IDS_GENERATION_REFERENCE_OBJECT_INVALID          108

// Operators
#define IDS_GENERATION_ASSIGNMENT_OPERATOR_MISSING       109
#define IDS_GENERATION_REFERENCE_OPERATOR_MISSING        110

// Parameters  
#define IDS_GENERATION_PARAMETER_MISSING                 111
#define IDS_GENERATION_CONSTANT_NOT_FOUND                112
#define IDS_GENERATION_ENUMERATION_NOT_FOUND             113
#define IDS_GENERATION_OPERATOR_NOT_FOUND                114
#define IDS_GENERATION_VARIABLE_ZERO_LENGTH              115
#define IDS_GENERATION_PARAMETER_INVALID                 116
#define IDS_GENERATION_CODEOBJECT_ILLEGAL                117

// Script Calls / Expressions
#define IDS_GENERATION_SCRIPT_CALL_INVALID               120
#define IDS_GENERATION_SCRIPT_CALL_ASSIGNMENT_MISSING    121
#define IDS_GENERATION_EXPRESSION_INVALID                122
#define IDS_GENERATION_EXPRESSION_BRACKET_MISSING        123
#define IDS_GENERATION_EXPRESSION_ASSIGNMENT_MISSING     124
#define IDS_GENERATION_EXPRESSION_OPERAND_INVALID        125
#define IDS_GENERATION_EXPRESSION_OPERAND_MISSING        126
#define IDS_GENERATION_EXPRESSION_OPERATOR_INVALID       127
#define IDS_GENERATION_EXPRESSION_OPERATOR_MISSING       128
#define IDS_GENERATION_EXPRESSION_UNARY_INVALID          129

// Compilation Abort Reasons
#define IDS_GENERATION_ABORT_PROPERTIES                  130
#define IDS_GENERATION_ABORT_SYNTAX                      131
#define IDS_GENERATION_ABORT_CONDITIONALS                132
#define IDS_GENERATION_ABORT_SUBROUTINES                 133
#define IDS_GENERATION_ABORT_XML_TREE                    134
#define IDS_GENERATION_ABORT_FLATTEN_XML                 135

// Branching Logic
#define IDS_GENERATION_UNEXPECTED_END                    140
#define IDS_GENERATION_UNEXPECTED_BREAK                  141
#define IDS_GENERATION_UNEXPECTED_CONTINUE               142
#define IDS_GENERATION_UNEXPECTED_ELSE                   143
#define IDS_GENERATION_UNEXPECTED_ELSE_IF                144
#define IDS_GENERATION_UNCLOSED_CONDITIONAL              145

// Subroutine Logic
#define IDS_GENERATION_UNEXPECTED_END_SUB                146
#define IDS_GENERATION_LABEL_NOT_FOUND                   147
#define IDS_GENERATION_DUPLICATE_LABEL_NAME              148

// General
#define IDS_GENERATION_NO_COMMANDS_FOUND                 150
#define IDS_GENERATION_FINAL_RETURN_MISSING              151
#define IDS_GENERATION_SPECIAL_CASE_PARAMETER_NOT_FOUND  152
#define IDS_GENERATION_RECURSIVE_SKIP_IF                 153
#define IDS_GENERATION_VIRTUAL_SKIP_IF                   154
#define IDS_GENERATION_UNASSIGNED_VARIABLE               155
#define IDS_GENERATION_COMMAND_UNRECOGNISED              156
#define IDS_GENERATION_SCRIPTNAME_MISSING                157

// Macros
#define IDS_GENERATION_MACRO_LOOP_ITERATOR_MISSING       160
#define IDS_GENERATION_MACRO_LOOP_RANGE_MISSING          161
#define IDS_GENERATION_MACRO_LOOP_STEP_MISSING           162
#define IDS_GENERATION_MACRO_LOOP_STEP_ZERO              163
#define IDS_GENERATION_MACRO_LOOP_RANGE_INVALID          164
#define IDS_GENERATION_ARRAY_UNEXPECTED_COMMA            165
#define IDS_GENERATION_ARRAY_UNEXPECTED_OPERATOR         166
#define IDS_GENERATION_ARRAY_UNEXPECTED_PARAMETER        167
#define IDS_GENERATION_ARRAY_UNEXPECTED_CODEOBJECT       168
#define IDS_GENERATION_ARRAY_RETURN_VARIABLE_MISSING     169
#define IDS_GENERATION_FOREACH_ARRAY_MISSING             170
#define IDS_GENERATION_FOREACH_ITERATOR_MISSING          171
#define IDS_GENERATION_FOREACH_COUNTER_MISSING           172

/// Script Translation
// Script structural Errors
#define IDS_SCRIPT_ENGINE_VERSION_HIGHER                 212
#define IDS_SCRIPT_NAME_MISMATCH                         213
#define IDS_SCRIPT_ARGUMENT_TYPE_NAME_INVALID            214
#define IDS_SCRIPT_ARGUMENT_TYPE_ID_INVALID              215
#define IDS_SCRIPT_ARGUMENT_CORRUPT                      216
#define IDS_SCRIPT_PROPERTY_MISSING                      217
#define IDS_SCRIPT_PROPERTY_TAG_MISSING                  218
#define IDS_SCRIPT_COMMAND_ID_NOT_FOUND                  219
#define IDS_SCRIPT_STRUCTURE_INVALID                     220
#define IDS_SCRIPT_TAG_MISSING                           221
#define IDS_SCRIPT_TREE_EMPTY                            222
#define IDS_SCRIPT_TRANSLATION_MINOR_ERROR               223
#define IDS_SCRIPT_TRANSLATION_FAILED                    224
#define IDS_SCRIPT_XML_ERROR                             225

// CodeArray structural errors
#define IDS_CODEARRAY_STANDARD_BRANCH_MISSING            230
#define IDS_CODEARRAY_STANDARD_BRANCH_INVALID            231
#define IDS_CODEARRAY_STANDARD_BRANCH_EMPTY              232
#define IDS_CODEARRAY_AUXILIARY_BRANCH_MISSING           233
#define IDS_CODEARRAY_AUXILIARY_BRANCH_INVALID           234
#define IDS_CODEARRAY_VARIABLES_BRANCH_MISSING           235
#define IDS_CODEARRAY_VARIABLES_BRANCH_INVALID           236
#define IDS_CODEARRAY_LIVE_DATA_MISSING                  237
#define IDS_CODEARRAY_COMMAND_ID_MISSING                 238
#define IDS_CODEARRAY_COMMAND_ID_INVALID                 239
#define IDS_CODEARRAY_INVALID                            240

// SourceValue structural errors
#define IDS_SCRIPT_SOURCEVALUE_NOT_INTEGER               270
#define IDS_SCRIPT_SOURCEVALUE_NOT_FOUND                 271
#define IDS_SCRIPT_SOURCEVALUE_NOT_STRING                272
#define IDS_SCRIPT_SOURCEVALUE_TYPE_MISSING              273
#define IDS_SCRIPT_SOURCEVALUE_TYPE_INVALID              274
#define IDS_SCRIPT_SOURCEVALUE_SIZE_MISMATCH             275

// COMMAND translation errors
#define IDS_SCRIPT_COMMAND_COMMENT_ID_MISSING            280
#define IDS_SCRIPT_COMMAND_COMMENT_INVALID               281
#define IDS_SCRIPT_COMMAND_ID_MISSING                    282
#define IDS_SCRIPT_COMMAND_ID_INDEX_MISSING              283
#define IDS_SCRIPT_COMMAND_INDEX_MISSING                 284
#define IDS_SCRIPT_COMMAND_NODE_COUNT_MISMATCH           285
#define IDS_SCRIPT_COMMAND_NODE_COUNT_INCORRECT          286
#define IDS_SCRIPT_COMMAND_STRUCTURE_INVALID             287
#define IDS_SCRIPT_COMMAND_TRANSLATION_FAILED            288
#define IDS_SCRIPT_COMMAND_PARAMETER_NOT_FOUND           289
#define IDS_SCRIPT_COMMAND_INCOMPATIBLE                  290

// COMMAND syntax errors
#define IDS_COMMAND_SYNTAX_NOT_FOUND                     295
#define IDS_COMMAND_SYNTAX_INCOMPATIBLE                  296
#define IDS_COMMAND_SYNTAX_VARIATIONS_INCOMPATIBLE       297

// PARAMETER translation errors
#define IDS_SCRIPT_PARAMETER_NODE_TRANSLATION_FAILED     300
#define IDS_SCRIPT_PARAMETER_NODE_STRING_INVALID         301
#define IDS_SCRIPT_PARAMETER_NODE_INTEGER_INVALID        302
#define IDS_SCRIPT_PARAMETER_NODE_TYPE_INVALID           303
#define IDS_SCRIPT_PARAMETER_TUPLE_INVALID               304
#define IDS_SCRIPT_PARAMETER_TUPLE_TYPE_INVALID          305
#define IDS_SCRIPT_PARAMETER_TUPLE_STRING_INVALID        306
#define IDS_SCRIPT_PARAMETER_TUPLE_INTEGER_INVALID       307
#define IDS_SCRIPT_PARAMETER_TUPLE_MISSING               308
#define IDS_SCRIPT_PARAMETER_COUNT_MISSING               309
#define IDS_SCRIPT_PARAMETER_SYNTAX_MISSING              310
#define IDS_SCRIPT_PARAMETER_PARSING_FAILED              311
#define IDS_SCRIPT_PARAMETER_RESOLUTION_FAILED           312
#define IDS_SCRIPT_PARAMETER_TRANSLATION_FAILED          313

// Expressions
#define IDS_SCRIPT_EXPRESSION_INFIX_COUNT_MISMATCH       320
#define IDS_SCRIPT_EXPRESSION_POSTFIX_COUNT_MISMATCH     321
#define IDS_SCRIPT_EXPRESSION_POSTFIX_NOT_FOUND          322

// Labels
#define IDS_SCRIPT_LABEL_NUMBER_INVALID                  330
#define IDS_SCRIPT_LABEL_NUMBER_NOT_FOUND                331
#define IDS_SCRIPT_LABEL_COMMAND_INVALID                 332
#define IDS_SCRIPT_LABEL_TRANSLATION_FAILED              333

// Return Objects
#define IDS_RETURN_OBJECT_VARIABLE_TRANSLATION_FAILED    340
#define IDS_RETURN_OBJECT_CONDITIONAL_TRANSLATION_FAILED 341
#define IDS_RETURN_OBJECT_CONDITIONAL_INVALID            342
#define IDS_RETURN_OBJECT_VARIABLE_INVALID               343

// Translation to String
#define IDS_TRANSLATION_DATATYPE_INVALID                 350
#define IDS_TRANSLATION_RELATION_INVALID                 351
#define IDS_TRANSLATION_DATATYPE_UNSUPPORTED             352
#define IDS_TRANSLATION_WARE_INVALID                     353
#define IDS_TRANSLATION_VARIABLE_INVALID                 354
#define IDS_TRANSLATION_TYPE_CONVERSION_FAILED           355
#define IDS_TRANSLATION_PARAMETER_INVALID                356
#define IDS_TRANSLATION_PARAMETER_FAILED                 357
#define IDS_TRANSLATION_SCRIPT_TARGET_MISSING            358
#define IDS_TRANSLATION_SCRIPT_TARGET_ERRORS             359
#define IDS_TRANSLATION_SCRIPT_TARGET_MINOR_ERRORS       360
#define IDS_TRANSLATION_GAME_STRING_REFERENCE_NOT_FOUND  361
#define IDS_TRANSLATION_DELAYED_COMMAND_DETECTED         362
#define IDS_TRANSLATION_SET_WING_COMMAND_DETECTED        363
#define IDS_TRANSLATION_SPECIAL_CASE_PARAMETER_NOT_FOUND 364


/// Script Validation
#define IDS_VALIDATION_TRANSLATION_FAILED                390
#define IDS_VALIDATION_STRING_PROPERTY_MISMATCH          391
#define IDS_VALIDATION_INTEGER_PROPERTY_MISMATCH         392
#define IDS_VALIDATION_ARGUMENT_COUNT_MISMATCH           393
#define IDS_VALIDATION_VARIABLE_STRING_MISMATCH          394
#define IDS_VALIDATION_VARIABLE_INTEGER_MISMATCH         395
#define IDS_VALIDATION_COMMAND_COUNT_MISMATCH            396
#define IDS_VALIDATION_ARRAY_COUNT_MISMATCH              397
#define IDS_VALIDATION_ARRAY_INTEGER_MISMATCH            398
#define IDS_VALIDATION_ARRAY_STRING_MISMATCH             399
#define IDS_VALIDATION_STD_COMMAND_ERROR                 400
#define IDS_VALIDATION_AUX_COMMAND_ERROR                 401
#define IDS_VALIDATION_COMMAND_ID_NOT_FOUND              402
#define IDS_VALIDATION_VARIABLE_MISSING                  403
#define IDS_VALIDATION_VARIABLES_CASE_INSENSITIVE        404
#define IDS_VALIDATION_COMMAND_TEXT_MISMATCH             405

/// XML Parsing
// Original
#define IDS_XML_TAG_UNKNOWN_PROPERTY                     407
#define IDS_XML_TAG_PARSE_ERROR                          408
#define IDS_XML_UNEXPECTED_SCHEMA_TOKEN                  410
#define IDS_XML_UNEXPECTED_TOKEN                         411
#define IDS_XML_LANGUAGE_MISMATCH                        412
#define IDS_XML_ID_PROPERTY_MISSING                      413

// New
#define IDS_XML_UNEXPECTED_OPENING_BRACKET               420
#define IDS_XML_UNEXPECTED_CLOSING_BRACKET               421
#define IDS_XML_UNEXPECTED_OPENING_TAG                   422
#define IDS_XML_UNEXPECTED_CLOSING_TAG                   423
#define IDS_XML_MISMATCHED_CLOSING_TAG                   424
#define IDS_XML_UNCLOSED_OPENING_TAG                     425
#define IDS_XML_UNEXPECTED_TEXT                          426
#define IDS_XML_MINOR_SYNTAX_ERROR                       427
#define IDS_XML_MINOR_PARSING_ERROR                      428
#define IDS_XML_MINOR_SYNTAX_ERROR_QUESTION              429
#define IDS_XML_MINOR_PARSING_ERROR_QUESTION             430

/// Game Data
// TFiles
#define IDS_TFILE_IO_ERROR                               499
#define IDS_TFILE_VERSION_UNSUPPORTED                    500
#define IDS_TFILE_OBJECT_COUNT_NOT_FOUND                 501
#define IDS_TFILE_OBJECT_COUNT_INCORRECT                 502
#define IDS_TFILE_INSUFFICIENT_PROPERTIES                503
#define IDS_TFILE_SHIP_RACE_NOT_FOUND                    504
#define IDS_TFILE_SHIP_NAME_NOT_FOUND                    505
#define IDS_TFILE_SHIP_VARIATION_NOT_FOUND               506
#define IDS_TFILE_STATION_RACE_NOT_FOUND                 507
#define IDS_TFILE_STATION_NAME_NOT_FOUND                 508
#define IDS_TFILE_STATION_SIZE_NOT_FOUND                 509
#define IDS_TFILE_WARE_NAME_NOT_FOUND                    510
#define IDS_TFILE_OBJECT_NAME_DUPLICATE                  511
#define IDS_TFILE_INCOMPLETE_OBJECT_COUNT                512
#define IDS_TFILE_MINOR_ERROR                            513
#define IDS_TFILE_EXCESSIVE_PROPERTIES                   514

// Media Items
#define IDS_MEDIA_SFX_FILE_IO_ERROR                      520
#define IDS_MEDIA_SFX_MINOR_ERROR                        521
#define IDS_MEDIA_SFX_ENTRY_INVALID                      522
#define IDS_MEDIA_SFX_DUPLICATE_ITEM                     523
#define IDS_MEDIA_SFX_DUPLICATE_PAGE                     524
#define IDS_MEDIA_VIDEO_FILE_IO_ERROR                    525
#define IDS_MEDIA_VIDEO_MINOR_ERROR                      526
#define IDS_MEDIA_VIDEO_ENTRY_INVALID                    527
#define IDS_MEDIA_VIDEO_DUPLICATE_ITEM                   528
#define IDS_MEDIA_VIDEO_DUPLICATE_PAGE                   529

// Command Syntax
#define IDS_SYNTAX_FILE_IO_ERROR                         540
#define IDS_SYNTAX_PARAMETER_NOT_FOUND                   541
#define IDS_SYNTAX_CUSTOM_COMMAND                        542
#define IDS_SYNTAX_MINOR_ERROR                           543

// Object Descriptions
#define IDS_DESCRIPTION_FILE_IO_ERROR                    560
#define IDS_DESCRIPTION_FILE_TRANSLATION_FAILED          561
#define IDS_VARIABLES_FILE_IO_ERROR                      562
#define IDS_VARIABLES_FILE_TRANSLATION_FAILED            563
#define IDS_DESCRIPTION_DUPLICATE                        564
#define IDS_DESCRIPTION_OBJECT_NOT_FOUND                 565
#define IDS_DESCRIPTION_SYNTAX_NOT_FOUND                 566
#define IDS_DESCRIPTION_PARAMETER_UNKNOWN                567
#define IDS_DESCRIPTION_VARIABLE_UNKNOWN                 568
#define IDS_DESCRIPTION_VARIABLE_PARAMETER_COUNT         569
#define IDS_DESCRIPTION_VARIABLE_DUPLICATE               570

// Script-Objects
#define IDS_SCRIPT_OBJECT_DUPLICATE                      580

/// Bug Submission
// Preparation
#define IDS_SUBMISSION_LOADING_FAILED                    600
#define IDS_SUBMISSION_COMPRESSION_FAILED                601

// Upload
#define IDS_SUBMISSION_INIT_FAILED                       602
#define IDS_SUBMISSION_CONNECTION_FAILED                 603
#define IDS_SUBMISSION_CREATE_FOLDER_FAILED              604
#define IDS_SUBMISSION_NAVIGATE_FOLDER_FAILED            605
#define IDS_SUBMISSION_UPLOAD_FAILED                     606

/// Project files
#define IDS_PROJECT_LOAD_IO_ERROR                        620
#define IDS_PROJECT_NODE_MISSING                         621
#define IDS_PROJECT_NODE_UNRECOGNISED                    622
#define IDS_PROJECT_DOCUMENT_CORRUPT                     623
#define IDS_PROJECT_DOCUMENT_MISSING                     624
#define IDS_PROJECT_VARIABLE_CORRUPT                     625
#define IDS_PROJECT_TRANSLATION_FAILED                   626
#define IDS_PROJECT_SAVE_IO_ERROR                        627

/// Scripts
// General
#define IDS_SCRIPT_LOAD_IO_ERROR                         640
#define IDS_SCRIPT_CALL_LOAD_IO_ERROR                    641
#define IDS_SCRIPT_LOAD_MINOR_ERROR                      642
#define IDS_SCRIPT_SAVE_PCK_IO_ERROR                     643
#define IDS_SCRIPT_SAVE_IO_ERROR                         644

/// Language Files
// General
#define IDS_LANGUAGE_FILE_IO_ERROR                       650
#define IDS_LANGUAGE_FILE_MINOR_ERROR                    651
#define IDS_LANGUAGE_FILE_TRANSLATION_FAILED             652
#define IDS_LANGUAGE_MASTER_FILE_FAILED                  653
#define IDS_LANGUAGE_LOAD_ERROR                          654
#define IDS_LANGUAGE_SAVE_ERROR                          655
#define IDS_LANGUAGE_SUPPLEMENTARY_FILE_FAILED           658
#define IDS_LANGUAGE_SUPPLEMENTARY_FILE_MERGED           659

// Parsing
#define IDS_LANGUAGE_TAG_MISSING                         670
#define IDS_LANGUAGE_TAG_MISMATCH                        671
#define IDS_LANGUAGE_TAG_ID_MISSING                      672
#define IDS_LANGUAGE_UNEXPECTED_TAG                      673
#define IDS_LANGUAGE_FILE_STRING_MERGED                  674
#define IDS_LANGUAGE_SUPPLEMENTARY_STRING_MERGED         675
#define IDS_LANGUAGE_SUPPLEMENTARY_COMMAND_MERGED        676

// Game Strings
#define IDS_GAME_STRING_SUBSTRING_RECURSIVE              680
#define IDS_GAME_STRING_SUBSTRING_NOT_FOUND              681
#define IDS_GAME_STRING_SUBSTRING_CORRUPT                682
#define IDS_GAME_STRING_SUBSTRING_MISSING_COUNT          683

// Language string formatting
#define IDS_RICHTEXT_OPEN_TAGS_REMAINING                 690
#define IDS_RICHTEXT_EXCESS_CLOSING_TAGS                 691
#define IDS_RICHTEXT_UNSUPPORTED_LANGUAGE_TAG            692
#define IDS_RICHTEXT_UNSUPPORTED_TOOLTIP_TAG             693
#define IDS_RICHTEXT_UNKNOWN_TAG                         694

/// Main Window
// Exception types
#define IDS_UNHANDLED_EXCEPTION                          700
#define IDS_UNHANDLED_ACCESS_VIOLATION                   701
#define IDS_UNHANDLED_DEBUG_ASSERTION                    702

// Thread Exception handlers
#define IDS_EXCEPTION_LOAD_GAME_DATA                     705
#define IDS_EXCEPTION_LOAD_PROJECT_FILE                  706
#define IDS_EXCEPTION_SAVE_PROJECT_FILE                  707
#define IDS_EXCEPTION_LOAD_SCRIPT_FILE                   708
#define IDS_EXCEPTION_SAVE_SCRIPT_FILE                   709
#define IDS_EXCEPTION_SUBMIT_REPORT                      710
#define IDS_EXCEPTION_VALIDATE_SCRIPT_FILE               711
#define IDS_EXCEPTION_SEARCH_SCRIPT_CALLS                712

// Window Exception handlers
#define IDS_EXCEPTION_MAIN_WINDOW                        720
#define IDS_EXCEPTION_CODE_EDIT                          721
#define IDS_EXCEPTION_PROGRESS_DIALOG                    722
#define IDS_EXCEPTION_SCRIPT_DOCUMENT                    723
#define IDS_EXCEPTION_SEARCH_RESULTS_DIALOG              724
#define IDS_EXCEPTION_SEARCH_TAB_DIALOG                  725
#define IDS_EXCEPTION_OUTPUT_DIALOG                      726
#define IDS_EXCEPTION_OPERATION_POOL                     727
#define IDS_EXCEPTION_FILE_DIALOG                        728
#define IDS_EXCEPTION_PROPERTIES_DIALOG                  729
#define IDS_EXCEPTION_DOCUMENTS_WINDOW                   730
#define IDS_EXCEPTION_SPLASH_DIALOG                      731
#define IDS_EXCEPTION_PREFERENCES_BASE                   732
#define IDS_EXCEPTION_PREFERENCES_DIALOG                 733
#define IDS_EXCEPTION_LANGUAGE_DOCUMENT                  734
#define IDS_EXCEPTION_FIRST_RUN_DIALOG                   735

// Function Exception Handlers
#define IDS_EXCEPTION_MESSAGE_PUMP                       740
#define IDS_EXCEPTION_GENERATE_XML_TREE                  741
#define IDS_EXCEPTION_DISPLAY_PREFERENCES                742
#define IDS_EXCEPTION_TREE_WORKER_THREAD                 743
#define IDS_EXCEPTION_LOAD_GAME_STRING_TREES             744
#define IDS_EXCEPTION_RESOLVE_SUBSTRINGS                 745
#define IDS_EXCEPTION_LOAD_SCRIPT_OBJECTS                746
#define IDS_EXCEPTION_UPDATE_LINE_COMMAND                747
#define IDS_EXCEPTION_OPERATION_COMPLETE                 748
#define IDS_EXCEPTION_UPDATE_RESULTS_DIALOG              749
#define IDS_EXCEPTION_DESTROY_DOCUMENTS_CTRL             750
#define IDS_EXCEPTION_INIT_APPLICATION                   751
#define IDS_EXCEPTION_DELETE_DOCUMENT                    752

/// ///////////////////////////////////////////////////////
///                   DIALOG STRINGS
/// ///////////////////////////////////////////////////////

/// Status Bar
#define IDS_MAIN_STATUS_WELCOME                          790
#define IDS_MAIN_STATUS_READY                            791
#define IDS_MAIN_STATUS_NO_DATA_LOADED                   792
#define IDS_MAIN_STATUS_GAME_DATA_LOADED                 793

/// File dialog:
// Columns
#define IDS_FILE_DIALOG_COLUMN_NAME                      850
#define IDS_FILE_DIALOG_COLUMN_DESCRIPTION               851
#define IDS_FILE_DIALOG_COLUMN_VERSION                   852
#define IDS_FILE_DIALOG_COLUMN_TYPE                      853
#define IDS_FILE_DIALOG_COLUMN_SIZE                      854

// General
#define IDS_FILE_DIALOG_UPDATING                         857

// Filters
#define IDS_FILE_DIALOG_FILTER_SCRIPT_FILES              860
#define IDS_FILE_DIALOG_FILTER_PCK_SCRIPT_FILES          861
#define IDS_FILE_DIALOG_FILTER_XML_SCRIPT_FILES          862
#define IDS_FILE_DIALOG_FILTER_LANGUAGE_FILES            863
#define IDS_FILE_DIALOG_FILTER_MISSION_FILES             864
#define IDS_FILE_DIALOG_FILTER_PROJECT_FILES             865
#define IDS_FILE_DIALOG_FILTER_DOCUMENT_FILES            866
#define IDS_FILE_DIALOG_FILTER_ALL_FILES                 867

// File Types
#define IDS_SCRIPT_FILE_DISPLAY_TYPE                     870
#define IDS_LANGUAGE_FILE_DISPLAY_TYPE                   871
#define IDS_MISSION_FILE_DISPLAY_TYPE                    872

/// Properties Dialog:
// Titles
#define IDS_PROPERTIES_SHEET_TITLE                       890
#define IDS_SCRIPT_GENERAL_PAGE_TITLE                    891
#define IDS_SCRIPT_ARGUMENTS_PAGE_TITLE                  892
#define IDS_SCRIPT_DEPENDENCIES_PAGE_TITLE               893
#define IDS_SCRIPT_VARIABLES_PAGE_TITLE                  894
#define IDS_SCRIPT_STRINGS_PAGE_TITLE                    895
#define IDS_LANGUAGE_GENERAL_PAGE_TITLE                  896
#define IDS_LANGUAGE_COLUMNS_PAGE_TITLE                  897
#define IDS_LANGUAGE_BUTTON_PAGE_TITLE                   898
#define IDS_LANGUAGE_SPECIAL_PAGE_TITLE                  899
#define IDS_NONE_PAGE_TITLE                              900

// Script: Arguments
#define IDS_ARGUMENTS_COLUMN_NAME                        920
#define IDS_ARGUMENTS_COLUMN_TYPE                        921
#define IDS_ARGUMENTS_COLUMN_DESCRIPTION                 922

// Script: Dependencies
#define IDS_DEPENDENCIES_COLUMN_NAME                     930
#define IDS_DEPENDENCIES_COLUMN_CALLS                    931

// Script: Variables
#define IDS_VARIABLES_COLUMN_NAME                        933
#define IDS_VARIABLES_COLUMN_TYPE                        934
#define IDS_VARIABLES_COLUMN_USAGE                       935

// Script: Strings
#define IDS_STRINGS_COLUMN_PAGE                          936
#define IDS_STRINGS_COLUMN_ID                            937
#define IDS_STRINGS_COLUMN_TEXT                          938

// Language: Buttons
#define IDS_BUTTONS_COLUMN_ID                            940
#define IDS_BUTTONS_COLUMN_TEXT                          941

/// Search Dialog
// Tab Titles
#define IDS_SEARCH_COMMANDS                              970
#define IDS_SEARCH_GAME_OBJECTS                          971
#define IDS_SEARCH_SCRIPT_OBJECTS                        972

/// SearchResults Dialog
// Columns
#define IDS_COMMAND_LIST_COLUMN_NAME                     974
#define IDS_COMMAND_LIST_COLUMN_VERSION                  975
#define IDS_AUTOCOMPLETE_COLUMN_NAME                     976
#define IDS_AUTOCOMPLETE_COLUMN_TYPE                     977

// Command Group Names
#define IDS_COMMAND_GROUP_ARRAY			                   979
#define IDS_COMMAND_GROUP_CUSTOM                         980
#define IDS_COMMAND_GROUP_FLEET						          981
#define IDS_COMMAND_GROUP_FLOW_CONTROL							 982
#define IDS_COMMAND_GROUP_GAME_ENGINE							 983
#define IDS_COMMAND_GROUP_GRAPH							       984 
#define IDS_COMMAND_GROUP_MACRO                          985
#define IDS_COMMAND_GROUP_MARINE							       986
#define IDS_COMMAND_GROUP_MATHS							       987
#define IDS_COMMAND_GROUP_MERCHANT							    988
#define IDS_COMMAND_GROUP_NON_PLAYER							 989
#define IDS_COMMAND_GROUP_OBJECT_ACTION						 990
#define IDS_COMMAND_GROUP_OBJECT_PROPERTY						 991
///#define IDS_COMMAND_GROUP_OBJECT_TRADE					    991     REM: Removed
#define IDS_COMMAND_GROUP_PASSENGER							    992
#define IDS_COMMAND_GROUP_PILOT							       993
#define IDS_COMMAND_GROUP_PLAYER							       994
#define IDS_COMMAND_GROUP_SCRIPT_PROPERTY						 995
#define IDS_COMMAND_GROUP_SHIP_ACTION							 996
#define IDS_COMMAND_GROUP_SHIP_PROPERTY						 997
#define IDS_COMMAND_GROUP_SHIP_TRADE							 998
#define IDS_COMMAND_GROUP_SHIP_WING							    999
///#define IDS_COMMAND_GROUP_STATION_ACTION					 999     REM: Removed
#define IDS_COMMAND_GROUP_STATION_PROPERTY					 1000
#define IDS_COMMAND_GROUP_STATION_TRADE						 1001
#define IDS_COMMAND_GROUP_STOCK_EXCHANGE						 1002
#define IDS_COMMAND_GROUP_STRING							       1003
#define IDS_COMMAND_GROUP_SYSTEM_PROPERTY						 1004
#define IDS_COMMAND_GROUP_UNIVERSE_DATA						 1005
#define IDS_COMMAND_GROUP_UNIVERSE_PROPERTY					 1006
#define IDS_COMMAND_GROUP_USER_INTERFACE						 1007
#define IDS_COMMAND_GROUP_WAR					                1008
#define IDS_COMMAND_GROUP_WARE_PROPERTY						 1009
#define IDS_COMMAND_GROUP_WEAPON_PROPERTY						 1010
#define IDS_COMMAND_GROUP_HIDDEN		                      1011

// GameObject Group names
#define IDS_GAME_OBJECT_GROUP_DOCK                       1020         // Dock
#define IDS_GAME_OBJECT_GROUP_FACTORY                    1021         // Factory
#define IDS_GAME_OBJECT_GROUP_LASER                      1022         // Laser
#define IDS_GAME_OBJECT_GROUP_MISSILE                    1023         // Missile
#define IDS_GAME_OBJECT_GROUP_SHIP                       1024         // Ship
#define IDS_GAME_OBJECT_GROUP_SHIELD                     1025         // Shield
#define IDS_GAME_OBJECT_GROUP_WARE                       1026         // Ware

// ScriptObject Group names
#define IDS_SCRIPT_OBJECT_GROUP_CONSTANT                 1027      // Script Constant
#define IDS_SCRIPT_OBJECT_GROUP_DATA_TYPE                1028      // Variable Data Type
#define IDS_SCRIPT_OBJECT_GROUP_FLIGHT_RETURN            1029      // Flight Return Code
#define IDS_SCRIPT_OBJECT_GROUP_FORMATION                1030      // Ship Formation
#define IDS_SCRIPT_OBJECT_GROUP_OBJECT_CLASS             1031      // Object Class
#define IDS_SCRIPT_OBJECT_GROUP_RACE                     1032      // Race
#define IDS_SCRIPT_OBJECT_GROUP_RELATION                 1033      // Relation
#define IDS_SCRIPT_OBJECT_GROUP_PARAMETER_TYPE           1034      // Parameter Syntax
#define IDS_SCRIPT_OBJECT_GROUP_SECTOR                   1035      // Sector
#define IDS_SCRIPT_OBJECT_GROUP_STATION_SERIAL           1036      // Station Serial
#define IDS_SCRIPT_OBJECT_GROUP_OBJECT_COMMAND           1037      // Object Command
#define IDS_SCRIPT_OBJECT_GROUP_WING_COMMAND             1038      // Wing Command
#define IDS_SCRIPT_OBJECT_GROUP_SIGNAL                   1039      // Signal
#define IDS_SCRIPT_OBJECT_GROUP_TRANSPORT_CLASS          1040      // Transport Class
#define IDS_SCRIPT_OBJECT_GROUP_OPERATOR                 1041      // Operator

/// Preferences Dialog:
// Titles
#define IDS_PREFERENCES_SHEET_TITLE                      1050
#define IDS_GENERAL_PAGE_TITLE                           1051
#define IDS_FOLDERS_PAGE_TITLE                           1052
#define IDS_APPEARANCE_PAGE_TITLE                        1053
#define IDS_SYNTAX_PAGE_TITLE                            1054
#define IDS_MISC_PAGE_TITLE                              1055

// Error Handling
#define IDS_ERROR_HANDLING_ABORT                         1060
#define IDS_ERROR_HANDLING_IGNORE                        1061
#define IDS_ERROR_HANDLING_PROMPT                        1062

// Code Objects
#define IDS_CODE_OBJECT_ARGUMENT                         1070
#define IDS_CODE_OBJECT_WORD                             1071
#define IDS_CODE_OBJECT_COMMENT                          1072
#define IDS_CODE_OBJECT_ENUMERATION                      1073
#define IDS_CODE_OBJECT_KEYWORD                          1074
#define IDS_CODE_OBJECT_LABEL                            1075
#define IDS_CODE_OBJECT_NUMBER                           1076
#define IDS_CODE_OBJECT_OPERATOR                         1077
#define IDS_CODE_OBJECT_CONSTANT                         1078
#define IDS_CODE_OBJECT_STRING                           1079
#define IDS_CODE_OBJECT_VARIABLE                         1080
#define IDS_CODE_OBJECT_NULL                             1081
#define IDS_CODE_OBJECT_WHITESPACE                       1082

// Syntax
#define IDS_SYNTAX_COLUMN_COMPONENT                      1090
#define IDS_CODE_EDIT_PREVIEW_TEXT                       1091

// Folders
#define IDS_GAME_FOLDER_BROWSE_TEXT                      1095


/// Language Document
// Columns
#define IDS_LANGUAGE_PAGE_COLUMN_ID                      1100
#define IDS_LANGUAGE_PAGE_COLUMN_TITLE                   1101
#define IDS_LANGUAGE_PAGE_COLUMN_DESCRIPTION             1102
#define IDS_LANGUAGE_STRING_COLUMN_ID                    1103
#define IDS_LANGUAGE_STRING_COLUMN_TEXT                  1104

/// Media Document
// Columns
#define IDS_MEDIA_GROUP_COLUMN_ID                        1110
#define IDS_MEDIA_GROUP_COLUMN_TITLE                     1111
#define IDS_MEDIA_ITEM_COLUMN_ID                         1112
#define IDS_MEDIA_ITEM_COLUMN_DESCRIPTION                1113
#define IDS_MEDIA_ITEM_COLUMN_DURATION                   1114

// MediaItems
#define IDS_MEDIA_TYPE_SOUND_EFFECT                      1120
#define IDS_MEDIA_TYPE_VIDEO_CLIP                        1121
#define IDS_MEDIA_TYPE_SPEECH_CLIP                       1122

/// InsertDocument Dialog
// Types
#define IDS_DOCUMENT_TYPE_SCRIPT                         1130
#define IDS_DOCUMENT_TYPE_LANGUAGE                       1131
#define IDS_DOCUMENT_TYPE_MISSION                        1132
#define IDS_DOCUMENT_TYPE_PROJECT                        1133

// Descriptions
#define IDS_DOCUMENT_DESCRIPTION_SCRIPT                  1140
#define IDS_DOCUMENT_DESCRIPTION_LANGUAGE                1141
#define IDS_DOCUMENT_DESCRIPTION_MISSION                 1142
#define IDS_DOCUMENT_DESCRIPTION_PROJECT                 1143

/// Project Variables dialog
// Column names
#define IDS_PROJECT_VARIABLES_COLUMN_NAME                1150
#define IDS_PROJECT_VARIABLES_COLUMN_VALUE               1151

/// Project dialog
// Folder names
#define IDS_PROJECT_FOLDER_SCRIPT                        1160
#define IDS_PROJECT_FOLDER_LANGUAGE                      1161
#define IDS_PROJECT_FOLDER_MISSION                       1162

/// Output dialog
#define IDS_OUTPUT_COLUMN                                1170
#define IDS_OUTPUT_ERROR_CORRUPTED                       1171

// Loading GameData
#define IDS_OUTPUT_GAME_DATA_LOADING                     1220
#define IDS_OUTPUT_LOADING_FILE_SYSTEM                   1221
#define IDS_OUTPUT_LOADING_MASTER_LANGUAGE               1222
#define IDS_OUTPUT_LOADING_SUPPLEMENTARY_LANGUAGE        1223
#define IDS_OUTPUT_RESOLVING_SUBSTRINGS                  1224
#define IDS_OUTPUT_GENERATING_SCRIPT_OBJECTS             1225
#define IDS_OUTPUT_LOADING_TFILE                         1226
#define IDS_OUTPUT_LOADING_SPEECH_CLIPS                  1227
#define IDS_OUTPUT_LOADING_SOUND_EFFECTS                 1228
#define IDS_OUTPUT_LOADING_VIDEO_CLIPS                   1229
#define IDS_OUTPUT_LOADING_SYNTAX                        1230
#define IDS_OUTPUT_LOADING_CUSTOM_SYNTAX                 1231

#define IDS_OUTPUT_GAME_DATA_SUCCESS                     1233   // NB: The order of success/failure/abort is required by printOperationResultToOutputDialog
#define IDS_OUTPUT_GAME_DATA_FAILURE                     1234
#define IDS_OUTPUT_GAME_DATA_ABORTED                     1235

// Loading Script
#define IDS_OUTPUT_SCRIPT_LOADING                        1240
#define IDS_OUTPUT_LOADING_SCRIPT_XML                    1241
#define IDS_OUTPUT_PARSING_SCRIPT_XML                    1242
#define IDS_OUTPUT_TRANSLATING_SCRIPT_PROPERTIES         1243
#define IDS_OUTPUT_TRANSLATING_SCRIPT_COMMANDS           1244

#define IDS_OUTPUT_SCRIPT_LOAD_SUCCESS                   1245   // NB: The order of success/failure/abort is required by printOperationResultToOutputDialog
#define IDS_OUTPUT_SCRIPT_LOAD_FAILURE                   1246
#define IDS_OUTPUT_SCRIPT_LOAD_ABORTED                   1247

// Saving Script
#define IDS_OUTPUT_SCRIPT_SAVING                         1250
#define IDS_OUTPUT_PARSING_SCRIPT_COMMANDS               1251
#define IDS_OUTPUT_GENERATING_BRANCHING_LOGIC            1252
#define IDS_OUTPUT_GENERATING_XML_TREE                   1253
#define IDS_OUTPUT_SAVING_SCRIPT_PCK                     1254
#define IDS_OUTPUT_SAVING_SCRIPT_XML                     1255

#define IDS_OUTPUT_SCRIPT_SAVE_SUCCESS                   1256   // NB: The order of success/failure/abort is required by printOperationResultToOutputDialog
#define IDS_OUTPUT_SCRIPT_SAVE_FAILURE                   1257
#define IDS_OUTPUT_SCRIPT_SAVE_ABORTED                   1258

// Loading Language File
#define IDS_OUTPUT_LANGUAGE_LOADING                      1260

#define IDS_OUTPUT_LANGUAGE_LOAD_SUCCESS                 1261   // NB: The order of success/failure/abort is required by printOperationResultToOutputDialog
#define IDS_OUTPUT_LANGUAGE_LOAD_FAILURE                 1262
#define IDS_OUTPUT_LANGUAGE_LOAD_ABORTED                 1263

// Saving Language File
#define IDS_OUTPUT_LANGUAGE_SAVING                       1270

#define IDS_OUTPUT_LANGUAGE_SAVE_SUCCESS                 1271   // NB: The order of success/failure/abort is required by printOperationResultToOutputDialog
#define IDS_OUTPUT_LANGUAGE_SAVE_FAILURE                 1272
#define IDS_OUTPUT_LANGUAGE_SAVE_ABORTED                 1273

// Bug Submission
#define IDS_OUTPUT_SUBMIT_REPORT                         1280

#define IDS_OUTPUT_SUBMIT_REPORT_SUCCESS                 1281   // NB: The order of success/failure/abort is required by printOperationResultToOutputDialog
#define IDS_OUTPUT_SUBMIT_REPORT_FAILURE                 1282
#define IDS_OUTPUT_SUBMIT_REPORT_ABORTED                 1283

// Correction Submittion
#define IDS_OUTPUT_CORRECTION                            1285

#define IDS_OUTPUT_CORRECTION_SUCCESS                    1286   // NB: The order of success/failure/abort is required by printOperationResultToOutputDialog
#define IDS_OUTPUT_CORRECTION_FAILURE                    1287
#define IDS_OUTPUT_CORRECTION_ABORTED                    1288

// Loading project
#define IDS_OUTPUT_PROJECT_LOADING                       1290
#define IDS_OUTPUT_LOADING_PROJECT_XML                   1291

#define IDS_OUTPUT_PROJECT_LOAD_SUCCESS                  1292   // NB: The order of success/failure/abort is required by printOperationResultToOutputDialog
#define IDS_OUTPUT_PROJECT_LOAD_FAILURE                  1293  
#define IDS_OUTPUT_PROJECT_LOAD_ABORTED                  1294  

// Saving project
#define IDS_OUTPUT_PROJECT_SAVING                        1295
#define IDS_OUTPUT_SAVING_PROJECT_XML                    1296

#define IDS_OUTPUT_PROJECT_SAVE_SUCCESS                  1297   // NB: The order of success/failure/abort is required by printOperationResultToOutputDialog
#define IDS_OUTPUT_PROJECT_SAVE_FAILURE                  1298  
#define IDS_OUTPUT_PROJECT_SAVE_ABORTED                  1299  

// Validation script
#define IDS_OUTPUT_VALIDATING_SCRIPT                     1300
#define IDS_OUTPUT_LOADING_VALIDATION_SCRIPT             1301
#define IDS_OUTPUT_VALIDATING_SCRIPT_PROPERTIES          1302
#define IDS_OUTPUT_VALIDATING_SCRIPT_COMMANDS            1303
#define IDS_OUTPUT_VALIDATING_SCRIPT_CODEARRAY           1304

#define IDS_OUTPUT_VALIDATION_SUCCESS                    1305   // NB: The order of success/failure/abort is required by printOperationResultToOutputDialog
#define IDS_OUTPUT_VALIDATION_FAILURE                    1306
#define IDS_OUTPUT_VALIDATION_ABORTED                    1307

// External Dependency Search
#define IDS_OUTPUT_SEARCHING_FOLDER                      1310

#define IDS_OUTPUT_SCRIPT_SEARCH_SUCCESS                 1312
#define IDS_OUTPUT_SCRIPT_SEARCH_FAILURE                 1313
#define IDS_OUTPUT_SCRIPT_SEARCH_ABORTED                 1314



///                                                  **FUTURE STAGES**

/// Progress dialog
// Stage descriptions:
#define IDS_PROGRESS_MULTIPLE                               1340

// Load Game Data
#define IDS_PROGRESS_LOADING_CATALOGUES                     1341
#define IDS_PROGRESS_LOADING_MASTER_STRINGS                 1342
#define IDS_PROGRESS_STORING_MASTER_STRINGS                 1343
#define IDS_PROGRESS_CONVERTING_MASTER_STRINGS              1344
#define IDS_PROGRESS_RESOLVING_SUBSTRINGS                   1345
#define IDS_PROGRESS_LOADING_SUPPLEMENTARY_STRINGS          1347
#define IDS_PROGRESS_LOADING_OBJECT_NAMES                   1348
#define IDS_PROGRESS_LOADING_SPEECH_CLIPS                   1349
#define IDS_PROGRESS_TRANSLATING_SPEECH_CLIPS               1350
#define IDS_PROGRESS_LOADING_MEDIA_ITEMS                    1351
#define IDS_PROGRESS_LOADING_COMMAND_SYNTAX                 1352
#define IDS_PROGRESS_LOADING_COMMAND_DESCRIPTIONS           1353

// Load Script
#define IDS_PROGRESS_PARSING_SCRIPT                         1355
#define IDS_PROGRESS_TRANSLATING_SCRIPT                     1356
#define IDS_PROGRESS_VALIDATING_SCRIPT                      1357

// Save script
#define IDS_PROGRESS_PARSING_COMMANDS                       1360
#define IDS_PROGRESS_GENERATING_SCRIPT                      1361
#define IDS_PROGRESS_COMPRESSING_SCRIPT                     1362

// Submit Report
#define IDS_PROGRESS_PREPARING_REPORT                       1365
#define IDS_PROGRESS_CONNECTING_SERVER                      1366
#define IDS_PROGRESS_UPLOADING_REPORT                       1367

// File System Search
#define IDS_PROGRESS_SEARCHING_PHYSICAL                     1370
#define IDS_PROGRESS_SEARCHING_VIRTUAL                      1371

// Load/Save project
#define IDS_PROGRESS_LOADING_PROJECT                        1375
#define IDS_PROGRESS_SAVING_PROJECT                         1376

// External Dependencies Search
#define IDS_PROGRESS_SEARCHING_SCRIPTS                      1378

/// ///////////////////////////////////////////////////////
///                  GENERAL STRINGS
/// ///////////////////////////////////////////////////////

/// Application
#define IDS_APPLICATION_NAME_DEBUG                       1500
#define IDS_APPLICATION_NAME_RELEASE                     1501

#ifdef _DEBUG
   #define IDS_APPLICATION_NAME    IDS_APPLICATION_NAME_DEBUG
#else
   #define IDS_APPLICATION_NAME    IDS_APPLICATION_NAME_RELEASE
#endif

/// Game versions
#define IDS_GAME_VERSION_THREAT                          1505
#define IDS_GAME_VERSION_REUNION                         1506
#define IDS_GAME_VERSION_TERRAN_CONFLICT                 1507
#define IDS_GAME_VERSION_ALBION_PRELUDE                  1508

/// Languages
#define IDS_LANGUAGE_ENGLISH                             1510
#define IDS_LANGUAGE_FRENCH                              1511
#define IDS_LANGUAGE_GERMAN                              1512
#define IDS_LANGUAGE_ITALIAN                             1513
#define IDS_LANGUAGE_POLISH                              1514
#define IDS_LANGUAGE_RUSSIAN                             1515
#define IDS_LANGUAGE_SPANISH                             1516

/// Display Colours
#define IDS_COLOUR_BLACK                                 1520
#define IDS_COLOUR_DARK_BLUE                             1521
#define IDS_COLOUR_DARK_GREEN                            1522
#define IDS_COLOUR_DARK_GREY                             1523
#define IDS_COLOUR_DARK_RED                              1524
#define IDS_COLOUR_LIGHT_BLUE                            1525
#define IDS_COLOUR_LIGHT_GREEN                           1526
#define IDS_COLOUR_LIGHT_GREY                            1527
#define IDS_COLOUR_LIGHT_RED                             1528
#define IDS_COLOUR_ORANGE                                1529
#define IDS_COLOUR_PURPLE                                1530
#define IDS_COLOUR_YELLOW                                1531
#define IDS_COLOUR_WHITE                                 1532


// General
#define IDS_FEATURE_NOT_IMPLEMENTED                      1540
#define IDS_FEATURE_DISABLED                             1541
#define IDS_TITLE_NOT_IMPLEMENTED                        1542
#define IDS_TITLE_EXCEPTION                              1543

#define IDS_GENERAL_GALAXY_MAPS_UNSUPPORTED              1550
#define IDS_GENERAL_UNKNOWN_FILE_TYPE                    1551
#define IDS_GENERAL_DEPENDENCY_NOT_FOUND                 1552
#define IDS_GENERAL_OVERWRITE_PHYSICAL_FILE              1553
#define IDS_GENERAL_OVERWRITE_VIRTUAL_FILE               1554
#define IDS_GENERAL_FILE_NOT_FOUND                       1555
#define IDS_GENERAL_LANGUAGE_RESTART                     1556
#define IDS_GENERAL_FIRST_RUN                            1557
#define IDS_GENERAL_DOCUMENT_CLOSE_CONFIRMATION          1558
#define IDS_GENERAL_PROJECT_CLOSE_CONFIRMATION           1559
#define IDS_GENERAL_CONFIRM_DEPENDENCY_SEARCH            1560
#define IDS_GENERAL_VALIDATION_DOCUMENTS_OPEN            1561
#define IDS_GENERAL_RELOAD_DOCUMENTS_OPEN                1562
#define IDS_GENERAL_PROJECT_EXPORT_BROWSE                1563
#define IDS_GENERAL_PROJECT_EXPORT_SUCCESS               1564

/// ///////////////////////////////////////////////////////
///                     MENU INDICIES
/// ///////////////////////////////////////////////////////

// Dialog context menus IDs
#define  IDM_DOCUMENT_POPUP                              0
#define  IDM_OUTPUT_POPUP                                1
#define  IDM_PROJECT_POPUP                               2
#define  IDM_PROJECT_VARIABLES_POPUP                     3
#define  IDM_RESULTS_POPUP                               4

// Language context menus IDs
#define  IDM_GAMEPAGE_POPUP                              0
#define  IDM_GAMESTRING_POPUP                            1
#define  IDM_RICHEDIT_POPUP                              2
#define  IDM_COLOUR_POPUP                                3
#define  IDM_BUTTON_POPUP                                4

// Script context menus IDs
#define  IDM_CODE_EDIT_POPUP                             0
#define  IDM_ARGUMENT_POPUP                              1
#define  IDM_DEPENDENCIES_POPUP                          2
#define  IDM_VARIABLES_POPUP                             3
#define  IDM_STRINGS_POPUP                               4

/// ///////////////////////////////////////////////////////
///                    MENU ITEM IDs
/// ///////////////////////////////////////////////////////

/// BearScript Window
// File Menu
#define IDM_FILE_NEW                                     1940
#define IDM_FILE_NEW_SCRIPT                              1941 
#define IDM_FILE_NEW_LANGUAGE                            1942 
#define IDM_FILE_NEW_MISSION                             1943
#define IDM_FILE_NEW_PROJECT                             1944
#define IDM_FILE_OPEN                                    1945
#define IDM_FILE_BROWSE                                  1946
#define IDM_FILE_CLOSE                                   1947
#define IDM_FILE_CLOSE_PROJECT                           1948
#define IDM_FILE_SAVE                                    1949
#define IDM_FILE_SAVE_AS                                 1950
#define IDM_FILE_SAVE_ALL                                1951
#define IDM_FILE_EXPORT_PROJECT                          1952
#define IDM_FILE_EXIT                                    1953

// Recent menu
#define IDM_FILE_RECENT_EMPTY                            1955
#define IDM_FILE_RECENT_FIRST                            1956
#define IDM_FILE_RECENT_LAST                             1966

// Edit Menu
#define IDM_EDIT_UNDO                                    1970
#define IDM_EDIT_REDO                                    1971
#define IDM_EDIT_CUT                                     1972
#define IDM_EDIT_COPY                                    1973
#define IDM_EDIT_PASTE                                   1974
#define IDM_EDIT_DELETE                                  1975
#define IDM_EDIT_SELECT_ALL                              1976
#define IDM_EDIT_COMMENT                                 1977
#define IDM_EDIT_FIND                                    1978

// Edit interface
#define IDM_EDIT_WORD_LEFT                               1980
#define IDM_EDIT_WORD_RIGHT                              1981
#define IDM_EDIT_LINE_UP                                 1982
#define IDM_EDIT_LINE_DOWN                               1983

// View Menu
#define IDM_VIEW_COMMAND_LIST                            1990
#define IDM_VIEW_GAME_OBJECTS_LIST                       1991
#define IDM_VIEW_SCRIPT_OBJECTS_LIST                     1992
#define IDM_VIEW_COMPILER_OUTPUT                         1993
#define IDM_VIEW_DOCUMENT_PROPERTIES                     1994
#define IDM_VIEW_PROJECT_EXPLORER                        1995
#define IDM_VIEW_PREFERENCES                             1996

// Tools Menu
#define IDM_TOOLS_GAME_STRINGS                           2010
#define IDM_TOOLS_MEDIA_BROWSER                          2011
#define IDM_TOOLS_MISSION_HIERARCHY                      2012
#define IDM_TOOLS_CONVERSATION_BROWSER                   2013
#define IDM_TOOLS_RELOAD_GAME_DATA                       2014

// Test Menu
#define IDM_TEST_ORIGINAL_SCRIPT                         2020
#define IDM_TEST_OUTPUT_SCRIPT                           2021
#define IDM_TEST_COMMAND_DESCRIPTIONS                    2022
#define IDM_TEST_VALIDATE_XML_SCRIPTS                    2023
#define IDM_TEST_GAME_DATA                               2024
#define IDM_TEST_SCRIPT_TRANSLATION                      2025
#define IDM_TEST_VALIDATE_ALL_SCRIPTS                    2026
#define IDM_TEST_INTERPRET_DOCUMENT                      2027
#define IDM_TEST_CURRENT_CODE                            2028

// Window Menu
#define IDM_WINDOW_CLOSE_ALL_DOCUMENTS                   2030
#define IDM_WINDOW_CLOSE_OTHER_DOCUMENTS                 2031
#define IDM_WINDOW_NEXT_DOCUMENT                         2032
#define IDM_WINDOW_FIRST_DOCUMENT                        2033
#define IDM_WINDOW_LAST_DOCUMENT                         2077
//                                            The IDs 2033-2077 are reserved for documents

// Help Menu
#define IDM_HELP_HELP                                    2078
#define IDM_HELP_UPDATES                                 2079
#define IDM_HELP_FORUMS                                  2080
#define IDM_HELP_SUBMIT_FILE                             2081
#define IDM_HELP_ABOUT                                   2082
#define IDM_HELP_TUTORIAL_OPEN_FILE                      2083
#define IDM_HELP_TUTORIAL_FILE_OPTIONS                   2084
#define IDM_HELP_TUTORIAL_GAME_DATA                      2085
#define IDM_HELP_TUTORIAL_GAME_FOLDER                    2086
#define IDM_HELP_TUTORIAL_GAME_OBJECTS                   2087
#define IDM_HELP_TUTORIAL_EDITOR                         2088
#define IDM_HELP_TUTORIAL_PROJECTS                       2089
#define IDM_HELP_TUTORIAL_COMMANDS                       2090
#define IDM_HELP_TUTORIAL_SCRIPT_OBJECTS                 2091

/// Documents control
// Context Menu
#define IDM_DOCUMENT_ADD_PROJECT                         2092
#define IDM_DOCUMENT_REMOVE_PROJECT                      2093

/// Output dialog
// Context Menu
#define IDM_OUTPUT_CLEAR                                 2094

/// Project dialog
// Context menu
#define IDM_PROJECT_OPEN_DOCUMENT                        2096
#define IDM_PROJECT_REMOVE_DOCUMENT                      2097
#define IDM_PROJECT_ADD_ALL_DOCUMENTS                    2098
#define IDM_PROJECT_EDIT_VARIABLES                       2099

/// ProjectVariables dialog
// Context menu
#define IDM_PROJECT_VARIABLES_ADD                        2100
#define IDM_PROJECT_VARIABLES_REMOVE                     2101

/// Results dialog
// Context menu
#define IDM_RESULTS_INSERT_RESULT                        2105
#define IDM_RESULTS_MSCI_LOOKUP                          2106
#define IDM_RESULTS_SUBMIT_CORRECTION                    2107

/// CodeEdit Window
// Context Menu
#define IDM_CODE_EDIT_CUT                                2115
#define IDM_CODE_EDIT_COPY                               2116
#define IDM_CODE_EDIT_PASTE                              2117
#define IDM_CODE_EDIT_DELETE                             2118
#define IDM_CODE_EDIT_VIEW_SUGGESTIONS                   2119
#define IDM_CODE_EDIT_LOOKUP_COMMAND                     2120
#define IDM_CODE_EDIT_OPEN_TARGET_SCRIPT                 2121
#define IDM_CODE_EDIT_GOTO_LABEL                         2122
#define IDM_CODE_EDIT_EDIT_LANGUAGE_STRING               2123
#define IDM_CODE_EDIT_VIEW_ERROR                         2124
#define IDM_CODE_EDIT_VIEW_WARNING                       2125
#define IDM_CODE_EDIT_PROPERTIES                         2126

/// Language Document Window
// RichEdit Toolbar
#define IDM_RICHEDIT_CUT                                 2130
#define IDM_RICHEDIT_COPY                                2131
#define IDM_RICHEDIT_PASTE                               2132
#define IDM_RICHEDIT_DELETE                              2133
#define IDM_RICHEDIT_LEFT                                2134
#define IDM_RICHEDIT_CENTRE                              2135
#define IDM_RICHEDIT_RIGHT                               2136
#define IDM_RICHEDIT_JUSTIFY                             2137
#define IDM_RICHEDIT_BOLD                                2138
#define IDM_RICHEDIT_ITALIC                              2139
#define IDM_RICHEDIT_UNDERLINE                           2140
#define IDM_RICHEDIT_COLOUR                              2141
#define IDM_RICHEDIT_BUTTON                              2142
#define IDM_RICHEDIT_PROPERTIES                          2143

// Colour Popup Menu
#define IDM_COLOUR_BLACK                                 2150
#define IDM_COLOUR_BLUE                                  2151
#define IDM_COLOUR_CYAN                                  2152
#define IDM_COLOUR_DEFAULT                               2153
#define IDM_COLOUR_GREEN                                 2154
#define IDM_COLOUR_ORANGE                                2155
#define IDM_COLOUR_PURPLE                                2156
#define IDM_COLOUR_RED                                   2157
#define IDM_COLOUR_WHITE                                 2158
#define IDM_COLOUR_YELLOW                                2159

// GamePage Popup Menu
#define IDM_GAMEPAGE_INSERT                              2170
#define IDM_GAMEPAGE_EDIT                                2171
#define IDM_GAMEPAGE_DELETE                              2172
#define IDM_GAMEPAGE_VOICED                              2173

// GameString Popup Menu
#define IDM_GAMESTRING_INSERT                            2180
#define IDM_GAMESTRING_EDIT                              2181
#define IDM_GAMESTRING_DELETE                            2182
#define IDM_GAMESTRING_VIEW_ERROR                        2183
#define IDM_GAMESTRING_PROPERTIES                        2184

/// Script Document Properties
// Arguments Menu
#define IDM_ARGUMENT_INSERT                              2220
#define IDM_ARGUMENT_EDIT                                2221
#define IDM_ARGUMENT_DELETE                              2222

// Dependencies Menu
#define IDM_DEPENDENCIES_LOAD                            2230
#define IDM_DEPENDENCIES_REFRESH                         2231

// Variables Menu
#define IDM_VARIABLES_REFRESH                            2233

// Strings Menu
#define IDM_STRINGS_REFRESH                              2235

/// Language Document Properties
#define IDM_BUTTON_EDIT                                  2250
#define IDM_BUTTON_DELETE                                2251

/// Message Dialog
#define IDM_FIRST_ATTACHMENT                             2270

/// SPECIAL: System Menu  (Place last in file)
#define IDM_SHOW_HIDE_CONSOLE                           32
#define SC_SIZE                                         0xF000
#define SC_MOVE                                         0xF010
#define SC_MINIMIZE                                     0xF020
#define SC_MAXIMIZE                                     0xF030
#define SC_CLOSE                                        0xF060
#define SC_RESTORE                                      0xF120


/// ///////////////////////////////////////////////////////
///                    CONTROL IDs
/// ///////////////////////////////////////////////////////

// Generic
#define IDC_DIALOG_ICON                                  2990
#define IDC_DIALOG_TITLE                                 2991
#define IDC_DIALOG_HEADING_1                             2992
#define IDC_DIALOG_HEADING_2                             2993
#define IDC_DIALOG_HEADING_3                             2994

/// Main Window
#define IDC_DOCUMENTS_TAB                                3000
#define IDC_STATUS_BAR                                   3001
#define IDC_SPLITTER                                     3002
#define IDC_MAIN_TOOLBAR                                 3003
#define IDC_OPERATION_POOL                               3004
#define IDC_WORKSPACE                                    3005

/// Dialogs
// File Dialog
#define IDC_FILE_LIST                                    3020
#define IDC_FILE_OPTIONS                                 3021
#define IDC_FILE_TOOLBAR                                 3022
#define IDC_FILENAME_EDIT                                3023
#define IDC_FILENAME_STATIC                              3024
#define IDC_FILETYPE_COMBO                               3025
#define IDC_FILETYPE_STATIC                              3026
#define IDC_LOCATION_COMBO                               3027
#define IDC_LOCATION_REFRESH                             3028
#define IDC_LOCATION_UP                                  3029

#define IDC_LOCATION_JUMP1                               3030
#define IDC_LOCATION_JUMP2                               3031
#define IDC_LOCATION_JUMP3                               3032
#define IDC_LOCATION_JUMP4                               3033
#define IDC_LOCATION_JUMP5                               3034
#define IDC_LOCATION_JUMP6                               3035

// File Options Dialog
#define IDC_FILE_OPTIONS_SCRIPTCALL_CHECK                3040
#define IDC_FILE_OPTIONS_RECURSION_CHECK                 3041
#define IDC_FILE_OPTIONS_RECURSION_SLIDER                3042
#define IDC_FILE_OPTIONS_DEPTH_STATIC                    3043
#define IDC_FILE_OPTIONS_COMPILER_CHECK                  3044
#define IDC_FILE_OPTIONS_GENERATION_RADIO                3045
#define IDC_FILE_OPTIONS_VALIDATION_RADIO                3046

// Progress Dialog
#define IDC_PROGRESS_DIALOG_ICON                         3060
#define IDC_PROGRESS_DIALOG_TITLE                        3061
#define IDC_PROGRESS_DIALOG_HEADING                      3062
#define IDC_PROGRESS_STAGE_STATIC                        3063
#define IDC_PROGRESS_BAR                                 3064

// About Dialog
#define IDC_ABOUT_DIALOG_ICON                            3080
#define IDC_ABOUT_DIALOG_TITLE                           3081
#define IDC_ABOUT_DIALOG_VERSION_HEADING                 3082
#define IDC_ABOUT_DIALOG_THANKS_HEADING                  3083
#define IDC_ABOUT_LINK_EGOSOFT                           3084
#define IDC_ABOUT_LINK_FORUMS                            3085
#define IDC_ABOUT_EXSCRIPTOR_ICON                        3086
#define IDC_ABOUT_DOUBLESHADOW_ICON                      3087
#define IDC_ABOUT_LICENSE                                3088

// About-License Dialog
#define IDC_LICENSE_EDIT                                 3090

// Message Dialog
#define IDC_MESSAGE_TEXT_STATIC                          3100
#define IDC_MESSAGE_CODE_STATIC                          3101
#define IDC_MESSAGE_TOOLBAR                              3102
#define IDC_MESSAGE_BUTTON_1                             3103
#define IDC_MESSAGE_BUTTON_2                             3104
#define IDC_MESSAGE_BUTTON_3                             3105

#define IDC_MESSAGE_COPY                                 3110
#define IDC_MESSAGE_ATTACHMENTS                          3111
#define IDC_MESSAGE_LESS                                 3112
#define IDC_MESSAGE_MORE                                 3113

// Message-Attachment Dialog
#define IDC_ATTACHMENTS                                  3131
#define IDC_WORDWRAP_CHECK                               3141

// Game Version Dialog
#define IDC_VERSION_DIALOG_ICON                          3150
#define IDC_VERSION_DIALOG_TITLE                         3151
#define IDC_VERSION_AVAILABLE_HEADING                    3152
#define IDC_VERSION_THREAT_ICON                          3153
#define IDC_VERSION_REUNION_ICON                         3154
#define IDC_VERSION_TERRAN_CONFLICT_ICON                 3155
#define IDC_VERSION_ALBION_PRELUDE_ICON                  3156
#define IDC_VERSION_THREAT                               3157
#define IDC_VERSION_REUNION                              3158
#define IDC_VERSION_TERRAN_CONFLICT                      3159
#define IDC_VERSION_ALBION_PRELUDE                       3160

// Output Dialog
#define IDC_OUTPUT_LIST                                  3165

// Project Dialog
#define IDC_PROJECT_TREE                                 3170

// Project Variables Dialog
#define IDC_VARIABLES_STATIC                             3171

// ScriptCall Dialog
#define IDC_SCRIPTCALL_TEXT_STATIC                       3175

// Results Dialog
#define IDC_RESULTS_LIST                                 3180
#define IDC_RESULTS_SEARCH                               3181
#define IDC_RESULTS_FILTER                               3182
#define IDC_RESULTS_COMPATIBLE_CHECK                     3183

// Search Dialog
#define IDC_SEARCH_TAB                                   3190

// First-Time Dialog
#define IDC_FIRST_DIALOG_ICON                            3200
#define IDC_FIRST_WELCOME_STATIC                         3201
#define IDC_FIRST_LANGUAGE_STATIC                        3202
#define IDC_FIRST_LANGUAGE_COMBO                         3203

// Tutorial Dialogs
#define IDC_TUTORIAL_DIALOG_ICON                         3210
#define IDC_TUTORIAL_DIALOG_TITLE                        3211
#define IDC_TUTORIAL_HEADING_1                           3212
#define IDC_TUTORIAL_HEADING_2                           3213
#define IDC_TUTORIAL_HEADING_3                           3214
#define IDC_TUTORIAL_COMMANDS_BITMAP                     3215
#define IDC_TUTORIAL_FILE_OPTIONS_BITMAP                 3216
#define IDC_TUTORIAL_GAME_DATA_BITMAP                    3217
#define IDC_TUTORIAL_GAME_FOLDER_BITMAP                  3218
#define IDC_TUTORIAL_GAME_OBJECT_BITMAP                  3219
#define IDC_TUTORIAL_OPEN_FILE_BITMAP                    3220
#define IDC_TUTORIAL_PROJECT_BITMAP                      3221
#define IDC_TUTORIAL_SCRIPT_OBJECT_BITMAP                3222
#define IDC_TUTORIAL_SCRIPT_EDITOR_BITMAP                3223

/// //////////////////////////////////////////////////////////////////////////////

// Insert Language GamePage
#define IDC_PAGE_ID_EDIT                                 4100
#define IDC_PAGE_ID_ICON                                 4101
#define IDC_PAGE_TITLE_EDIT                              4102
#define IDC_PAGE_DESCRIPTION_EDIT                        4103
#define IDC_PAGE_VOICED_CHECK                            4104

// Insert Language GameString
#define IDC_STRING_ID_EDIT                               4106
#define IDC_STRING_ID_ICON                               4107

// Find Text Dialog
#define IDC_FIND_SEARCH_EDIT                             4110
#define IDC_FIND_REPLACE_EDIT                            4111
#define IDC_FIND_NEXT                                    4112
#define IDC_FIND_REPLACE                                 4113
#define IDC_FIND_CASE_SENSITIVE_CHECK                    4114
#define IDC_FIND_FROM_TOP_CHECK                          4115
#define IDC_FIND_COMPLETE_STATIC                         4116

// Insert Script Argument
#define IDC_ARGUMENT_DIALOG_ICON                         4120
#define IDC_ARGUMENT_TITLE_STATIC                        4121
#define IDC_ARGUMENT_REQUIRED_STATIC                     4122
#define IDC_ARGUMENT_OPTIONAL_STATIC                     4123
#define IDC_ARGUMENT_NAME_EDIT                           4124
#define IDC_ARGUMENT_NAME_ICON                           4125
#define IDC_ARGUMENT_TYPE_COMBO                          4126
#define IDC_ARGUMENT_DESCRIPTION_EDIT                    4127

// Insert Project Variable
#define IDC_VARIABLE_NAME_EDIT                           4130
#define IDC_VARIABLE_NAME_ICON                           4131
#define IDC_VARIABLE_VALUE_EDIT                          4132

// Project Variables
#define IDC_ADD_VARIABLE                                 4133
#define IDC_REMOVE_VARIABLE                              4134

// New Document
#define IDC_DOCUMENT_DIALOG_ICON                         4140
#define IDC_DOCUMENT_TITLE_STATIC                        4141
#define IDC_DOCUMENT_TYPE_STATIC                         4142
#define IDC_DOCUMENT_LOCATION_STATIC                     4143
#define IDC_DOCUMENT_LIST                                4144
#define IDC_DOCUMENT_DESCRIPTION_STATIC                  4145
#define IDC_DOCUMENT_FILENAME_EDIT                       4146
#define IDC_DOCUMENT_LOCATION_COMBO                      4147

// Bug Report Dialog
#define IDC_SUBMISSION_DESCRIPTION_STATIC                4150
#define IDC_SUBMISSION_ACCESS_VIOLATION_CHECK            4151
#define IDC_SUBMISSION_DEBUG_ASSERTION_CHECK             4152
#define IDC_SUBMISSION_TERMINATION_CHECK                 4153
#define IDC_SUBMISSION_EXCEPTION_CHECK                   4154
#define IDC_SUBMISSION_LOAD_GAME_DATA_CHECK              4155
#define IDC_SUBMISSION_LOAD_SCRIPT_CHECK                 4156
#define IDC_SUBMISSION_FORUM_USERNAME_EDIT               4157
#define IDC_SUBMISSION_INCLUDE_SCRIPT_CHECK              4158
#define IDC_SUBMISSION_VIEW_REPORT                       4159

// Correction Dialog
#define IDC_CORRECTION_DESCRIPTION_RICHEDIT              4165
#define IDC_CORRECTION_SUBMISSION_EDIT                   4166

/// Preferences Property Pages
// 'General' Preferences PropertyPage
#define IDC_APPLICATION_LANGUAGE_COMBO                   4198
#define IDC_PRESERVE_SESSION_CHECK                       4199
#define IDC_USE_SYSTEM_DIALOG_CHECK                      4200
#define IDC_PROPERTIES_TRANSPARENCY_CHECK                4201
#define IDC_TUTORIAL_MODE_CHECK                          4202

#define IDC_CODE_INDENTATION_CHECK                       4203
#define IDC_CODE_MACROS_CHECK                            4204
#define IDC_CODE_DO_IF_CHECK                             4205
#define IDC_CODE_TOOLTIPS_CHECK                          4206
#define IDC_CODE_TOOLTIPS_SLIDER                         4207
#define IDC_CODE_TOOLTIPS_STATIC                         4208
#define IDC_SEARCH_TOOLTIPS_CHECK                        4209
#define IDC_SEARCH_TOOLTIPS_SLIDER                       4210
#define IDC_SEARCH_TOOLTIPS_STATIC                       4211
#define IDC_SUGGEST_COMMANDS_CHECK                       4212
#define IDC_SUGGEST_GAME_OBJECTS_CHECK                   4213
#define IDC_SUGGEST_SCRIPT_OBJECTS_CHECK                 4214
#define IDC_SUGGEST_VARIABLES_CHECK                      4215

// 'Folders' Preferences PropertyPage
#define IDC_GAME_FOLDER_COMBO                            4220
#define IDC_GAME_FOLDER_STATUS                           4221
#define IDC_GAME_FOLDER_BROWSE                           4222
#define IDC_SCRIPT_FOLDER_EDIT                           4223
#define IDC_GAME_LANGUAGE_COMBO                          4224
#define IDC_REPORT_LANGUAGE_OVERWRITES_CHECK             4225
#define IDC_REPORT_MISSING_SUBSTRINGS_CHECK              4226
#define IDC_REPORT_INCOMPLETE_GAMEOBJECTS_CHECK          4227

// 'Apperance' Preferences PropertyPage
#define IDC_FONT_NAME_COMBO                              4240
#define IDC_FONT_SIZE_COMBO                              4241
#define IDC_BOLD_FONT_CHECK                              4242
#define IDC_COLOUR_SCHEME_SAVE                           4243
#define IDC_COLOUR_SCHEME_COMBO                          4244

// 'Syntax' Preferences PropertyPage
#define IDC_CODE_OBJECT_LIST                             4260
#define IDC_TEXT_COLOUR_COMBO                            4261
#define IDC_BACKGROUND_COLOUR_COMBO                      4262
#define IDC_PREVIEW_CODE_EDIT                            4263

// 'Misc' Preferences PropertyPage
#define IDC_ERROR_HANDLING_COMBO                         4280
#define IDC_BUG_REPORTS_CHECK                            4281
#define IDC_PASSIVE_FTP_CHECK                            4282
#define IDC_AUTOMATIC_UPDATES_CHECK                      4283
#define IDC_FORUM_USERNAME_EDIT                          4284


/// Language 'Document Properties'
// 'General' Property Page
#define IDC_AUTHOR_EDIT                                  4320
#define IDC_TITLE_EDIT                                   4321
#define IDC_COMPATIBILITY_COMBO                          4322

// 'Columns' Property Page
#define IDC_COLUMN_ONE_RADIO                             4340
#define IDC_COLUMN_TWO_RADIO                             4341
#define IDC_COLUMN_THREE_RADIO                           4342
#define IDC_COLUMN_WIDTH_CHECK                           4343
#define IDC_COLUMN_WIDTH_SLIDER                          4344
#define IDC_COLUMN_WIDTH_STATIC                          4345 
#define IDC_COLUMN_SPACING_SLIDER                        4346
#define IDC_COLUMN_SPACING_STATIC                        4347
#define IDC_COLUMN_SPACING_CHECK                         4348

// 'Button' Property Page
#define IDC_BUTTONS_LIST                                 4360

// 'Special' Property Page
#define IDC_RANK_TYPE_COMBO                              4380
#define IDC_RANK_TITLE_EDIT                              4381
#define IDC_RANK_CHECK                                   4382
#define IDC_ARTICLE_CHECK                                4383



/// Script Document Properties
// Script 'General' page
#define IDC_SCRIPT_NAME                                  4400
#define IDC_SCRIPT_SIGNATURE                             4401
#define IDC_SCRIPT_DESCRIPTION                           4402
#define IDC_SCRIPT_COMMAND                               4403
#define IDC_SCRIPT_VERSION                               4404
#define IDC_SCRIPT_ENGINE_VERSION                        4405
#define IDC_SCRIPT_COMMENT_RATIO                         4406
#define IDC_SCRIPT_FOLDER                                4407

// Script 'Arguments' page
#define IDC_ARGUMENTS_LIST                               4420
#define IDC_ADD_ARGUMENT                                 4421
#define IDC_REMOVE_ARGUMENT                              4422
#define IDC_INPLACE_ARGUMENT_EDIT                        4423
#define IDC_INPLACE_ARGUMENT_COMBO                       4424

// Script 'Dependencies' page
#define IDC_DEPENDENCIES_LIST                            4440
#define IDC_REFRESH_DEPENDENCIES                         4441
#define IDC_SEARCH_DEPENDENCIES                          4442

// Script 'Variables' page
#define IDC_VARIABLES_LIST                               4444
#define IDC_REFRESH_VARIABLES                            4445
#define IDC_PROJECT_VARIABLES                            4446

// Script 'Strings' page
#define IDC_STRINGS_LIST                                 4447
#define IDC_REFRESH_STRINGS                              4448


/// Document Windows
// Language Document
#define IDC_PAGE_LIST                                    4600
#define IDC_STRING_LIST                                  4601
#define IDC_EDIT_DIALOG                                  4602
#define IDC_LANGUAGE_EDIT                                4603
#define IDC_LANGUAGE_TOOLBAR                             4604

// Script Document
#define IDC_LABELS_COMBO                                 4620
#define IDC_VARIABLES_COMBO                              4621
#define IDC_CODE_EDIT                                    4622
#define IDC_SUGGESTION_LIST                              4623
#define IDC_CODE_EDIT_TOOLTIP                            4624

// Media Document
#define IDC_GROUP_LIST                                   4640
#define IDC_ITEM_LIST                                    4641
#define IDC_ITEM_DIALOG                                  4642
#define IDC_MEDIA_VIDEO                                  4643
#define IDC_MEDIA_PROGRESS                               4644
#define IDC_MEDIA_PLAY                                   4645
#define IDC_MEDIA_PAUSE                                  4646





