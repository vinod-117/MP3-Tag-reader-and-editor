#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "common.h"
#include "type.h"


Status read_and_validate_args_edit(char *argv[], Mp3Info *mp3info)
{
    //Check for edit operation
    if (strcmp(argv[1], "-e") == 0)
    {
        printf("Edit operation selected.\n");
        // Validate MP3 file name 
        if(strstr(argv[4], ".mp3") != NULL)
        {
            printf("MP3 file detected\n");
            mp3info->mp3_fname = argv[4];
            //mp3info->new_mp3 = argv[3];
            mp3info->orginal = argv[4];
        }
        else
        {
            printf("Error: Invalid file. Please provide a .mp3 file.\n");
        }
        //Open original MP3 file
        mp3info->fptr_mp3 = fopen(mp3info->mp3_fname, "rb");
        if (mp3info->fptr_mp3 == NULL)
        {
            printf("Error: Unable to open source MP3 file %s\n", mp3info->mp3_fname);
            return e_failure;
        }
        else
        {
            printf("Opened source MP3 file successfully.\n");
        }
        
        char newfile[10];
        mp3info->new_mp3="new.mp3";
        mp3info->new_ftr = fopen(mp3info->new_mp3, "wb");

        if (mp3info->new_ftr == NULL)
        {
            printf("new file is not present\n");
            return e_failure;
        }
        else
        {
            printf("new mp3 is created\n");
        }
        //Choose tag modify
        if (strcmp(argv[2], "-t") == 0)
        {
            printf("To modify title\n");
            edit_tag("TIT2", argv, mp3info);
            return e_success;
        }
        else if (strcmp(argv[2], "-a") == 0)
        {
            printf("To modify Artist\n");
            edit_tag("TPE1", argv, mp3info); 
            return e_success;
        }
        else if (strcmp(argv[2], "-A") == 0)
        {
            printf("To modify Album\n");
            edit_tag("TALB", argv, mp3info);
            return e_success;
        }
        else if (strcmp(argv[2], "-y") == 0)
        {
            printf("To modify year\n");
            edit_tag("TYER", argv, mp3info);
            return e_success;
        }
        else if (strcmp(argv[2], "-m") == 0)
        {
            printf("To modify Content\n");
            edit_tag("TCON", argv, mp3info);
            return e_success;
        }
        else if (strcmp(argv[2], "-c") == 0)
        {
            printf("To modify comments\n");
            edit_tag("COMM", argv, mp3info);
            return e_success;
        }
        else
        {
            printf("Error: Unknown tag option '%s'\n", argv[2]);
            return e_failure;
        }
    }

    else
    {
        printf("Error: Unsupported operation.\n");
        printf("correct Usage to edit ./aout -e <modifier> <new content> <file name>:");
        return e_failure;
    }
}

// Convert big-endian to integer
unsigned int big_to_little_edit(unsigned char *b)
{
    return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
}

//Convert integer to 4 bytes big-endian format
void int_to_big_endian(unsigned char b[4], unsigned int size)
{
    b[0] = (size >> 24) & 0xFF;
    b[1] = (size >> 16) & 0xFF;
    b[2] = (size >> 8) & 0xFF;
    b[3] =size&0xFF;
}

Status edit_tag(char *tag, char *argv[], Mp3Info *mp3Info)
{
    rewind(mp3Info->fptr_mp3);   //resetting file pointer
    rewind(mp3Info->new_ftr);      //resetting new pointer

    char new_content[100];     //new data to modify
    strcpy(new_content, argv[3]);
    unsigned int size_content = strlen(new_content) + 1;    //size of new data

    char header[10];
    if (fread(header, 1, 10, mp3Info->fptr_mp3) != 10)    //Read ID3 header
    {
        printf("Error reading ID3 header\n");
        return e_failure;
    }
    fwrite(header, 1, 10, mp3Info->new_ftr);    //copy header

    char frame_id[5];
    unsigned char size[4], flag[2];
    int found = 0;

    //Read frame-by-frame
    while (1)
    {
        if(fread(frame_id, 1, 4, mp3Info->fptr_mp3)!=4)
        {
            printf("Error in reading frame ID\n");
        }
        frame_id[4] = '\0';

        
        if (!isalpha(frame_id[0]))      // Stop if frame ID doesn't start with an alphabet (end of ID3 tag frames) 
            break;

        if (fread(size, 1, 4, mp3Info->fptr_mp3) != 4)
        {
            printf("Error in reading size\n");
        }
        if (fread(flag, 1, 2, mp3Info->fptr_mp3) != 2)
        {
            printf("Error in reading flag\n");
        } 

        unsigned int frame_size = big_to_little_edit(size);

        if (frame_size > 1000000)        //1000000 = 1MB
        {
            printf("Frame size too large\n");
            break;
        }
        //Read frame data
        char *data = malloc(frame_size + 1);
        if (fread(data, 1, frame_size, mp3Info->fptr_mp3) != frame_size)
        {
            printf("Error reading frame data\n");
            free(data);
            break;
        }
        data[frame_size] = '\0';
        //Check if this is the tag to be edited
        if (strcmp(frame_id, tag) == 0)
        {
            found = 1;

            unsigned char size_bytes[4];
            int_to_big_endian(size_bytes, size_content);

            fwrite(frame_id, 1, 4, mp3Info->new_ftr);
            fwrite(size_bytes, 1, 4, mp3Info->new_ftr);
            fwrite(flag, 1, 2, mp3Info->new_ftr);
            fwrite(new_content, 1, size_content, mp3Info->new_ftr);
        }
        else
        {   //Copy other frame without changes 
            fwrite(frame_id, 1, 4, mp3Info->new_ftr);
            fwrite(size, 1, 4, mp3Info->new_ftr);
            fwrite(flag, 1, 2, mp3Info->new_ftr);
            fwrite(data, 1, frame_size, mp3Info->new_ftr);
        }

        free(data);
    }

    // Copy remaining audio data
    int ch;
    while ((ch = fgetc(mp3Info->fptr_mp3)) != EOF)
    {
        fputc(ch, mp3Info->new_ftr);
    }
    fclose(mp3Info->fptr_mp3);  //closes file pointer
    fclose(mp3Info->new_ftr);   //closes new file pointer

    remove(mp3Info->orginal);      //remove old file
    rename("new.mp3", mp3Info->orginal);   //rename original file 

    if (found)
    {
        printf("Tag '%s' edited successfully.\n", tag);
        return e_success;
    }
    else
    {
        printf("Tag '%s' not found in file.\n", tag);
        return e_failure;
    }
}

