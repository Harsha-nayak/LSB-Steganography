#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include "common.h"
/* Function Definitions */

extern int flag;

Status do_decoding(DecodeInfo *decInfo)                                   //start decoding
{
    printf("INFO: ## Decoding Procedure Started ##\n");
    if(open_decode_files(decInfo) == e_success)                           //check if files exist
    {
	if(decode_magic_string(decInfo) == e_success)                     //decode magic string
	{
	    if(decode_file_extn_size(decInfo) == e_success)               //decode secret file extension size
	    {
		if(decode_file_extn(decInfo) == e_success)                //decode secret file extension
		{
		    if(decode_secret_file_size(decInfo) == e_success)      //decode secret size file
		    {
			if(decode_secret_data(decInfo) == e_success)       //decode secret data
			{
			    printf("INFO: ## Decoding Done Successfully ##\n");
			}
			else
			{
			    printf("Error:Decoding secret data failed!\n");
			}
		    }
		    else
		    {
			printf("Error:Secret file size not decoded\n");
		    }
		}
		else
		    printf("Error:Secret file extension not decoded\n");
	    }
	    else
	    {
		printf("Error:Secret file extension size not decoded\n");
	    }
	}
	else
	{
	    printf("Error:Magic string not decoded\n");
	}
    }
}

Status open_decode_files(DecodeInfo *decInfo)                 //opening files
{
    printf("INFO:Opening required files\n");
    decInfo->fptr_stego_image = fopen(decInfo->stego_image,"r");
    if(decInfo->fptr_stego_image == NULL)
    {
	printf("Given image file file doesnt exist\n");
	return e_failure;
    }
    if(flag == 0)
    {
	decInfo->fptr_output_txt = fopen(decInfo->output_fname,"w");
	if(decInfo->fptr_output_txt == NULL)
	{
	    printf("%s file doesnt exist\n",decInfo->output_fname);
	    return e_failure;
	}
    }
    printf("INFO:Done\n");
    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)              //decode magic string
{
    printf("INFO:Decoding magic string\n");
    char buf[8];
    char magic_string[3];
    fseek(decInfo->fptr_stego_image,54,SEEK_SET);
    for(int i=0;i<2;i++)
    {
	fread(buf,8,1,decInfo->fptr_stego_image);
	decode_lsb_to_byte(&magic_string[i],buf);
    }
    if(!strcmp(magic_string,MAGIC_STRING))                                      //checking whether magic string is correct or not
    {	
	printf("INFO:Done\n");
	return e_success;
    }
    else
	return e_failure;
}


Status decode_file_extn_size(DecodeInfo *decInfo)                                           //file extension size
{
    uint num = 0;
    char buf[32];
    fread(buf,32,1,decInfo->fptr_stego_image);
    int ret = decode_lsb_to_num(num,buf);
    decInfo->extn_secret_file_size = ret;
    if(ret)
	return e_success;
    else
	return e_failure;
}

Status decode_file_extn(DecodeInfo *decInfo)                 //fetch file extension
{
    printf("INFO:Decoding secret file extension\n");
    char data[MAX_FILE_SUFFIX];
    char buf[8];
    for(int i=0;i<decInfo->extn_secret_file_size;i++)
    {
	fread(buf,8,1,decInfo->fptr_stego_image);
	decode_lsb_to_byte(&data[i],buf);
    }
    if(flag == 1)
    {
	strcat(decInfo->output_name,data);                                    //attach the extension decoded to the default file name created
	decInfo->output_name[10] = '\0';
	decInfo->fptr_output_txt = fopen(decInfo->output_name,"w");
    }
    printf("INFO:Done\n");
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)                       //secret file size decode
{
    int num=0;
    char buf[32];
    fread(buf,32,1,decInfo->fptr_stego_image);
    decInfo->size_secret_file = decode_lsb_to_num(num,buf);
    if(decInfo->size_secret_file)
	return e_success;
    else
	return e_failure;
}

Status decode_secret_data(DecodeInfo *decInfo)                           //decode secret data from image
{
    printf("INFO:Decoding secret data\n");
    char secret_data[decInfo->size_secret_file];
    char buf[8];
    for(int i=0;i<decInfo->size_secret_file;i++)
    {
	fread(buf,8,1,decInfo->fptr_stego_image);
	decode_lsb_to_byte(&secret_data[i],buf);
	fwrite(&secret_data[i],1,1,decInfo->fptr_output_txt);
    }
    printf("INFO:Done\n");
    return e_success;
}

Status decode_lsb_to_byte(char *temp_arr,char *image_buffer)                                  //fetch lsb 
{ 
    char data = 0;
    for(int i=0;i<sizeof(image_buffer);i++)
    {
	data = data | ((image_buffer[i] & 0x01) << i);
    }
    *temp_arr = data;
}

Status decode_lsb_to_num(uint num,char *image_buffer)                                    //fetching lsb of a number
{
    int i,j=0;
    for(i=0;i<32;i++)
    {
	num = num | ((image_buffer[i] & 0x01) << i);
    }
    return num;
}

