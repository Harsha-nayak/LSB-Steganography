#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int flag = 0;

int main(int argc,char **argv)
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    uint img_size;
    if(argc >= 3)
    {
	if(check_operation_type(argv) == e_encode)
	{
	    if(argc == 3)
	    {
		printf("Please pass 4 arguments\nLike: ./a.out -e filename.bpm secret.txt output.bmp\n");
		exit(0);
	    }
	    else if(read_and_validate_encode_args(argv,&encInfo) == e_success)
	    {
		do_encoding(&encInfo);                                                                             //encoding operation is called
	    }
	    else
	    {
		printf("Please pass .bmp image file for encoding\n");
	    }
	}
	else if(check_operation_type(argv) == e_decode)
	{
	    if(read_and_validate_decode_args(argv,&decInfo) == e_success)
	    { 
		do_decoding(&decInfo);                                                                            //decoding operation is called
	    }
	    else
	    {

		printf("Please provide .bmp image file for decoding\n");
	    }
	}
	else if(check_operation_type(argv) == e_unsupported)
	{
	    printf("Program terminated\n");
	    exit(0);
	}	   
    }
    else
    {
	printf("Please pass 4 arguments\nLike: ./a.out -e filename.bpm secret.txt output.bmp\n");
    }

    // Fill with sample filenames
    encInfo.src_image_fname = "beautiful.bmp";
    encInfo.secret_fname = "secret.txt";
    encInfo.stego_image_fname = "stego_img.bmp";
    //Test open_files
    /*if (open_files(&encInfo) == e_failure)
      {
      printf("ERROR: %s function failed\n", "open_files" );
      return 1;
      }
      else
      {
      printf("SUCCESS: %s function completed\n", "open_files" );
      }

    // Test get_image_size_for_bmp
    img_size = get_image_size_for_bmp(encInfo.fptr_src_image);
    printf("INFO: Image size = %u\n", img_size);
     */
    return 0;
}


OperationType check_operation_type(char *argv[])                                      //this function checks encoding or decoding operation to perform
{
    if(!strcmp(argv[1],"-e"))
	return e_encode;
    else if(!strcmp(argv[1],"-d"))
	return e_decode;
    else
	return e_unsupported;
}


Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    char ext1[5],ext3[5];
    char *ptr1 = strrchr(argv[2],'.');
    char *ptr2 = strrchr(argv[3],'.');

    sprintf(ext1,"%s",ptr1);
    sprintf(encInfo->extn_secret_file,"%s",ptr2);
    if(!strcmp(ext1,".bmp"))                                                //check for beautiful.bmp
    {
	encInfo->src_image_fname = argv[2];                                //copy file name
	if(!strcmp(encInfo->extn_secret_file,".txt") || !strcmp(encInfo->extn_secret_file,".c") || !strcmp(encInfo->extn_secret_file,".sh"))
	{
	    encInfo -> secret_fname = argv[3]; 
	    if(argv[4])                                                   //if output filename is given
	    {
		char *ptr3 = strrchr(argv[4],'.');
		sprintf(ext3,"%s",ptr3);
		if(!strcmp(ext3,".bmp"))                                 //if output filename is .bmp
		{
		    encInfo -> stego_image_fname = argv[4];              //store file name
		    return e_success;
		}
		else
		{
		    return e_failure;
		}
	    }
	    else
	    {
		encInfo->stego_image_fname = "stego_img.bmp";            //create new output file
		encInfo->fptr_stego_image = fopen("stego_img.bmp","w");
		fclose(encInfo->fptr_stego_image);
		return e_success;
	    }
	}
	else
	{
	    return e_failure;
	}
    }
    else
    {
	return e_failure;
    }
}


Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)                   //to check bmp file and create file to store the secret data
{
    char extn[4];
    char *ptr1 = strrchr(argv[2],'.');
    sprintf(extn,"%s",ptr1);
    if(!strcmp(extn,".bmp"))                                                          //compare image file is bmp extension
    {
	decInfo->stego_image = argv[2];
	if(argv[3])                                                                  //check file is given or create default file
	{
	    decInfo->output_fname = argv[3];
	    return e_success; 	    
	}
	else
	{
	    flag = 1;
	    char default_name[7] = "output";
	    decInfo->output_fname = default_name;
	    strcpy(decInfo->output_name,default_name);
	    return e_success;
	}
    }
    else
    {
	return e_failure;
    }
}

