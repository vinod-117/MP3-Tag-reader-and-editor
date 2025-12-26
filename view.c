#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "type.h"

Status read_and_validate_args(char *argv[], Mp3Info *mp3info)
{

    //Check for --v (view) operation
    if (strcmp(argv[1], "-v") == 0)
    {
        printf("View operation selected.\n");
    }

    //Check for --e (edit) operation
    else if (strcmp(argv[1], "-e") == 0)
    {
        printf("Edit operation selected.\n");
    }

    //Check for --help
    else if (strcmp(argv[1], "--help") == 0)
    {
        print_help_menu();     //to display help menu
        return e_success;
    }

    else
    {
        printf("Error: Invalid argument. Use '--help' for correct usage.\n");
        return e_failure;
    }

        //Check for valid .mp3 file
    if (strstr(argv[2], ".mp3") != NULL)
    {
        printf("MP3 file detected: %s\n", argv[2]);
        mp3info->mp3_fname = argv[2];
    }
    else
    {
        printf("Error: No valid .mp3 file provided.\n");
        return e_failure;
    }
    return e_success;
}

//to display help menu
int print_help_menu()
{
        printf("Help menu for Mp3 Tag Reader and Editor:\n");
        printf("$ mp3tag --help\n");
        printf("Usage: mp3tag[option] value filename.mp3\n");
        printf("-t      Modifies a Title tag\n");
        printf("-T      Modifies a Track tag\n");
        printf("-a      Modifies an Artist tag\n");
        printf("-A      Modifies an Album tag\n");
        printf("-y      Modifies a Year tag\n");
        printf("-C      Modifies a Comment tag\n");
        printf("-g      Modifies a Genre tag\n");
        printf("-h      Displays this help info\n");
        printf("-V      Prints version info\n");

        return e_success;
}
// Opens the MP3 file in read mode.
Status open_files(Mp3Info *mp3info)
{
    mp3info->fptr_mp3 = fopen(mp3info->mp3_fname, "r");

    if (mp3info->fptr_mp3 == NULL)
    {
        printf("sample.mp3 is not present\n");
        return e_failure;
    }
    return e_success;
}

//To Opens the MP3 file in read mode.
Status check_version(Mp3Info *mp3info)
{
    fseek(mp3info->fptr_mp3,0,SEEK_SET);
    char arr[3];
    fread(arr,sizeof(char),3,mp3info->fptr_mp3);
    //printf("--%s\n",arr);
    if(strcmp(arr,"ID3")==0)
    {
        printf("Its is ID3 version\n");
    }
    else
    {
        printf("Its not ID3 version");
    }
    unsigned char ver[2];
    fread(ver,sizeof(char),2,mp3info->fptr_mp3);
    printf("ID3 version is v2.%u.%u\n", ver[0], ver[1]);

    return e_success;
}

//function to Converts big-endian 4-byte size to little-endian integer.
unsigned int big_to_little(unsigned char *b)
{
    return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
}

//Reads and prints the first 6 ID3 frames:
Status view_tag(char *argv[], Mp3Info *mp3info)
{
    rewind(mp3info->fptr_mp3);
    fseek(mp3info->fptr_mp3, 10, SEEK_SET);    //skip header which is of 10 bytes


    printf("\n----- VIEWING MP3 TAGS -----\n");

    printf("Filename: %s\n",argv[2]);
    printf("Version:    ID3 V2.3\n");

    printf("------------------------------\n");


    for (int i = 0; i < 6; i++)  //for all tags TIT2, TPE1, TALB, TYER, TCON, TCOM
    {
        unsigned char frame_id[5];
        unsigned char size_bytes[4];
        unsigned char flags[2];

        // Read 4 bytes of Frame ID
        fread(frame_id, 1, 4, mp3info->fptr_mp3);
        frame_id[4] = '\0';


        // Read 4 bytes of frame size
        fread(size_bytes, 1, 4, mp3info->fptr_mp3);

        unsigned int frame_size = big_to_little(size_bytes);

        // Read 2 bytes of flags
        fread(flags,1, 2, mp3info->fptr_mp3);
        
        // Read actual frame data
        char data[frame_size];

        fread(data,1, frame_size, mp3info->fptr_mp3);
        data[frame_size] = '\0';

        printf("%s  :   ",frame_id);       //Print frame info
        for(int i=0;i<frame_size;i++)      //Print frame content
        {
            printf("%c",data[i]);
        }
        printf("\n");
    }

    printf("------------------------------\n");
    return e_success;
}
