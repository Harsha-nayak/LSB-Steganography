#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "common.h"
/* Function Definitions */
Status do_encoding(EncodeInfo *encInfo)                                                                        //do_encoding
{
    printf("INFO:## Encoding procedure started ##\n");
    printf("INFO:Opening required files\n");
    if(open_files(encInfo) == e_success)                                                                        //open_files
    {
	encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);                              //image capacity
	encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);                                        //secret file size
	if(check_capacity(encInfo) == e_success)                                                                //verify capacity of bmp and secret file
	{
	    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)                 //copy bmp header
	    {
		if(encode_magic_string(MAGIC_STRING,encInfo) == e_success)                                     //encode magic string
		{
		    if(encode_size_of_extn(sizeof(encInfo->extn_secret_file),encInfo) == e_success)            //encode extn size
		    {
			if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo) == e_success)             //encode extenion
			{
			    if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_success)         //encode secret file size
			    {
				if(encode_secret_file_data(encInfo) == e_success)                               //encode secret file data
				{
				    copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image);
				    printf("INFO: ## Encoding Done Successfully ##\n");
				}
				else
				{
				    printf("Error:Secret file encoding failed !\n");

				}
			    }
			    else
			    {
				printf("Error:file size is not encoded\n");
			    }
			}
			else
			{
			    printf("Error:secret file extension is not encoded\n");
			}
		    }
		    else
		    {
			printf("Error:secret file extension encoding failed\n");
		    }

		}
		else
		{
		    printf("Error:Magic string is not encoded\n");
		}			
	    }
	    else
	    {
		printf("Error:Bmp header file copy failed!\n");
	    }
	}
	else
	{
	    printf("Error:%s has not enough space to hold %s file data\n",encInfo->src_image_fname,encInfo->secret_fname);
	}


    }
}

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

Status check_capacity(EncodeInfo *encInfo)                    //this function checks capacity of beautiful.bmp to hold secret data
{
    uint secret_capacity;
    secret_capacity = (2+sizeof(int)+strlen(encInfo->extn_secret_file)+sizeof(encInfo->size_secret_file)+encInfo->size_secret_file)*8;
    printf("INFO: Checking for beautiful.bmp capacity to handle secret.txt\n");
    if(encInfo->image_capacity>=secret_capacity)
    {
	printf("INFO:Found ok\n");
	return e_success;
    }
    else
    {
	return e_failure;  
    }
}

uint get_file_size(FILE *fptr)                                 //get secret file size
{
    fseek(fptr,0,SEEK_END);
    int pos = ftell(fptr);
    return pos;
}

uint get_image_size_for_bmp(FILE *fptr_image)                 //get bmp size
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)                                                        //open files
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

	return e_failure;
    }
    printf("INFO:Opened beautiful.bmp\n");
    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

	return e_failure;
    }
    printf("INFO:Opened secret.txt\n");

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

	return e_failure;
    }
    printf("INFO:Opened %s\n",encInfo->stego_image_fname);
    // No failure return e_success
    return e_success;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE  *fptr_dest_image)                         //copy bmp header file
{
    char buffer[54];
    rewind(fptr_src_image);
    fread(buffer,54,1,fptr_src_image);
    fwrite(buffer,54,1,fptr_dest_image);
    printf("INFO:Copying bmp header\n");
    if(ftell(fptr_dest_image) == 54)
    {
	printf("INFO:Done\n");
	return e_success;
    }
    else
	return e_failure;

}

Status encode_magic_string(const char *magic_string,EncodeInfo *encInfo)                       //encode magic string
{
    char buf[8];
    for(int i=0;i<strlen(MAGIC_STRING);i++)
    {
	fread(buf,8,1,encInfo->fptr_src_image);
	encode_byte_to_lsb(magic_string[i],buf);
	fwrite(buf,8,1,encInfo->fptr_stego_image);
    }	
    printf("INFO:Encoding magic string\n");
    printf("INFO:Done\n");
    return e_success;

} 



Status encode_size_of_extn(uint file_extn_size , EncodeInfo *encInfo)                      //encode extension file size
{
    char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_num_to_lsb(file_extn_size,buffer);
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    return e_success;

}


Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)            //encode .txt 
{
    char buf[8];
    for(int i=0;i<sizeof(encInfo->extn_secret_file);i++)
    {
	fread(buf,8,1,encInfo->fptr_src_image);
	encode_byte_to_lsb(file_extn[i],buf);
	fwrite(buf,8,1,encInfo->fptr_stego_image);
    }
    printf("INFO:Encoding %s file extension\n",encInfo->secret_fname);
    printf("INFO:Done\n");
    return e_success;
}

Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)     //encode secret file size
{
    char buf[sizeof(int)*8];
    fread(buf,sizeof(int)*8,1,encInfo->fptr_src_image);
    encode_num_to_lsb(file_size,buf);
    fwrite(buf,sizeof(int)*8,1,encInfo->fptr_stego_image);
    printf("INFO:Encoding %s file size\n",encInfo->secret_fname);
    printf("INFO:Done\n");
    return e_success;	
}
Status encode_secret_file_data(EncodeInfo *encInfo)                        //encode secret file data
{
    char buf[encInfo->size_secret_file];
    rewind(encInfo->fptr_secret);
    fread(buf,encInfo->size_secret_file,1,encInfo->fptr_secret);
    encode_data_to_image(buf,encInfo->size_secret_file,encInfo->fptr_src_image,encInfo->fptr_stego_image);
    printf("INFO:Encoding %s file data\n",encInfo->secret_fname);
    printf("INFO:Done\n");
    return e_success;
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)        //encode harsha to stego.img
{
    char buf[8];
    for(int i=0;i<size;i++)
    {		
	fread(buf,8,1,fptr_src_image);
	encode_byte_to_lsb(data[i],buf);
	fwrite(buf,8,1,fptr_stego_image);
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)                                      //copy remaining bytes
{
    char buf;
    while( fread(&buf,1,1,fptr_src) )
	fwrite(&buf,1,1,fptr_dest);
    printf("INFO:Copying left over data\n");
    printf("INFO:Done\n");
    return e_success;
}

Status encode_byte_to_lsb(char data,char *image_buffer)                                              //encode byte to lsb
{
    for(int i=0;i<8;i++)
    {
	image_buffer[i] = (data & (1<<i)) >> i | (image_buffer[i] & 0xfe);
    }
}
                   
Status encode_num_to_lsb(uint data, char *image_buffer1)                                           //encode to o/p bmp file
{
    for(int i=0;i<32;i++)
    {
	image_buffer1[i] = (data & (1<<i)) >> i | (image_buffer1[i] & 0xfe);
    }
}
