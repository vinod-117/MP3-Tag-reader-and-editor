#ifndef COMMON_H
#define COMMON_H

#include "type.h"

typedef struct mp3tag_info
{

    char *mp3_fname;      // MP3 filename 
    FILE *fptr_mp3;       // File pointer to MP3 file 
    
    char *new_mp3;        // New filename for edited MP3 
    FILE *new_ftr;        // File pointer for edited output 

    char tag[3];          // Stores tag header (ID3) 
    char ver[10];         // Stores version string 

    char *orginal;        // Original tag data 
    char *fptr;           // Generic pointer used during edit 


}Mp3Info;

/* Checks the operation type (view/edit/help) */
OperationType check_operation_type(char *argv[]);

/* Validates arguments for view operation */
Status read_and_validate_args(char *argv[], Mp3Info *mp3info);

/* Prints help menu */
int print_help_menu();

/* Validates ID3 version of the MP3 file */
Status check_version(Mp3Info *mp3info);

/* Opens the MP3 file */
Status open_files(Mp3Info *mp3info);

/* Displays tag information */
Status view_tag(char *argv[], Mp3Info *mp3info);

/* Validates arguments for edit operation */
Status read_and_validate_args_edit(char *argv[], Mp3Info *mp3info);

/* Converts integer to big-endian format */
void int_to_big_endian(unsigned char b[4], unsigned int size);

/* Converts big-endian bytes to little-endian integer */
unsigned int big_to_little_edit(unsigned char *b);

/* Edits a specific MP3 tag */
Status edit_tag(char *tag, char *argv[], Mp3Info *mp3Info);

#endif