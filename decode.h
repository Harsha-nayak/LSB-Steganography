#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/*
  structure to store the data retrieved from stegged bmp file 
*/


typedef struct _DecodeInfo
{
    char *stego_image;
    FILE *fptr_stego_image;
    
    char *output_fname;
    char output_name[11];
    FILE *fptr_output_txt;
    
    int extn_secret_file_size;
    int size_secret_file;

}DecodeInfo;

/* This function will check output arguments are properly provided */

Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo);    

/* This function will initiate decoding process */

Status do_decoding(DecodeInfo *decInfo);

/*This function will check whether the files exits */

Status open_decode_files(DecodeInfo *decInfo);

/* This function will decode magic string from stego.bmp */

Status decode_magic_string(DecodeInfo *decInfo);

/* This function fetches lsb's of each byte from bmp file */

Status decode_lsb_to_byte(char *temp_arr,char *image_buffer);

/* This function fetches the file extension size */

Status decode_file_extn_size(DecodeInfo *decInfo);

/* This function decodes the data from image buffer and converts into number */

Status decode_lsb_to_num(uint num,char *image_buffer);

/* This function decodes the file extension of secret text */

Status decode_file_extn(DecodeInfo *decInfo);

/* This function obtains the file size of secret file */

Status decode_secret_file_size(DecodeInfo *decInfo);

/* This function decodes the actual secret data from image file and reads to output file */

Status decode_secret_data(DecodeInfo *decInfo);

#endif
