#include <stdio.h>
#include <string.h>
#include "common.h"
#include "type.h"

int main(int argc, char *argv[])
{
    Mp3Info mp3info;
    if (argc < 2)    //Check minimum number of arguments.
    {
        printf("Usage:\n");
        printf("For help menu: %s --help\n",argv[0]);
        printf("For viewing: %s -v <filename.mp3>\n", argv[0]);
        printf("For editing: %s -e <modifier> <new content> <file name>\n", argv[0]);
        return e_failure;
    }
    /* Determine which operation the user selected */
    if (check_operation_type(argv) == view)
    {
            //Validate input arguments for VIEW operation
        if (read_and_validate_args(argv, &mp3info) == e_success)
        {
            printf("Read and validate successful\n");
            if (open_files(&mp3info) == e_success)
            {
                printf("sample.mp3 is present\n");
            }
            else
            {
                printf("sample.mp3 is not present\n");
                return e_failure;
            }
            /* Check if MP3 version is supported (ID3v2 expected) */
            if (check_version(&mp3info) == e_success)
            {
                printf("Version validatation successful\n");
            }
            else
            {
                printf("Not a suitable version\n");
            }
            /* Display MP3 tag information */
            if (view_tag(argv, &mp3info) == e_success)
            {
                printf("DETAILS DISPLAYED SUCCESSFULLY\n");
            }
            else
            {
                printf("Error:To display MP3 tags\n");
                return e_failure;
            }
        }
        else
        {
            printf("Invalid input or operation failed.\n");
            return e_failure;
        }
    }
    //EDIT OPERATION 
    else if (check_operation_type(argv) == edit)
    {
        printf("You have chosen EDIT operation.\n");
        if (read_and_validate_args_edit(argv, &mp3info) == e_success)
        {
            printf("Edit operation successfull\n");
        }
        else
        {
            printf("Failed to read and validation for edit operation\n");
            return e_failure;
        }

    }
    //For help menu
    else if (check_operation_type(argv) == help)
    {
        if (read_and_validate_args(argv, &mp3info) == e_success)
        {
            printf("Help menu selected.\n");
        }
        else
        {
            printf("Invalid input or operation failed.\n");
            return e_failure;
        }
    }
    else
    {
        printf("Unsupported operation.\n");
        return e_failure;
    }

    return e_success;
}

// Determines whether the user wants to perform view or edit.
OperationType check_operation_type(char *argv[])
{
    Mp3Info mp3info;
    if (strcmp(argv[1], "-v") == 0)
    {
        return view;
    }
    else if (strcmp(argv[1], "-e") == 0)
    {
        return edit;
    }
    else if (strcmp(argv[1], "--help") == 0)
    {
        return help;
    }
    else
    {
        return unsupported;
    }
}

