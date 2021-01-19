/* File: ArchA2
 * Author: Andrew Mercer
 * School: Dalhousie University 
 * Dept: Electrical and Computer Eng 
 * 
 * Purpose: To emeulate loading an dumping data from memory. 
 * 
 * Note: A2.c
 * 
 * Last Modified: 2020.06.25
 * 
 */

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include"header.h"

/* Function: DEBUGGER
 * 
 * Description: Used to parse user commands and call appropriate functions
 * 
 * Arguments: fin
 * 
 */
void DEBUGGER(FILE* fin)
{
    char command_line1[COMMAND_LINE_ARG_SIZE];
    char command_line2[COMMAND_LINE_ARG_SIZE];
    char command;
    char file_name;
    char *start;
    char *end;
    
    
    while(1)
    {
        puts("Option:");
        scanf("%c",&command);
        /*Parsing command portion of user input*/
        /*Detecting for load command*/
        if(tolower(command) == 'l')
        {
            scanf("%s",&command_line1);
            fin = fopen(command_line1,"r");
            /*If file not valid file*/           
            if(fin == NULL)
            {
                exit(1);
            }
            LOADER(fin);
        }
        /*Detecting for dump command*/
        else if(tolower(command) == 'd')
        {
            scanf("%s %s",&command_line1, &command_line2);
            DUMP(command_line1, command_line2);
        }
        /*Detecting for exit command*/
        else if(tolower(command) == 'x')
        {
            exit(1);
        }
    }

} 

/* Function: DUMP
 * 
 * Description: Dumps values from memory from a start address to an end address in groups
 * of 16 bytes. 
 * 
 * Arguments: start[], end[]
 * 
 */
void DUMP(char start[], char end[])
{   
    
    unsigned short data_portion;
    unsigned short end_address = (int)strtol(end, NULL, BUS_TRANSFER_SIZE);
    unsigned short start_address = (int)strtol(start, NULL, BUS_TRANSFER_SIZE);
    char *symbols;
    int count = 0;
    int current_address = start_address;
    int mar = 0;
    /*Top loop used to print groupings of 16*/
    while(current_address <= end_address)
    {
        printf("%04x ",current_address);
        /*Fetches and prints individual bytes in each grouping of 16*/
        while(count < BUS_TRANSFER_SIZE)
        {
            mar = current_address + count;
            bus(mar, &data_portion, READ, BYTE);
            /*symbols table used to print ASCII version of each byte later*/
            symbols[count] = (char)data_portion;
            printf("%02X ",data_portion);
            count++;
        }
        count = 0; 
        /*Used to print ASCII version of each byte*/
        while(count < BUS_TRANSFER_SIZE)
        {
            mar = current_address + count;
            bus(mar, &data_portion, READ, BYTE);
            /*If outside of printable ASCII range*/
            if (symbols[count] < INVALID_ASCII_SIZE)
            {
                printf(".");
            }
            else
            {
                printf("%c",symbols[count]);
            }
            count++;
        }
        count = 0;
        printf("\n");
        /*increasing by 16 each time due to dumping in groups of 16*/
        current_address += BUS_TRANSFER_SIZE;
    }
    return;
}

/* Function: LOADER
 * 
 * Description: Loads s records from file into memory. 
 * 
 * Arguments: fin
 * 
 */
void LOADER(FILE * fin)
{
    
    enum BOOL valid_s_record;
    char record[MAX_RECORD_SIZE];
    int mar;
    int count;
    int i = 0;
    char *test;
    char name_of_file[MAX_RECORD_SIZE];
    int starting_address = 0;
    unsigned int file_name;
    /*New line each loop*/
    while(fgets(record,MAX_RECORD_SIZE,fin) != NULL)
    {
        
            /*starts at 8 to pass Sx, length and address portions*/
            count = DATA_STARING_COUNT;
            /*S9 records only at end, returns if s9 record entered*/
            if(record[1] == '9')
            {
                /*prints starting address from s9 record*/
                starting_address = address_parse(record);
                printf("Starting Address: %04X\n", starting_address);
                printf("Data Loaded Correctly\n");
                break;
                
            }
            valid_s_record = s_record_check(record);
            
            /*if s_record_check returns a false, record is invalid*/
            if(valid_s_record == false)
            {
                printf("INVALID S RECORD\n");
                return;
            }
            else
            {
                /*check for S0 records, used to print name of file*/
                if(record[1] == '0')
                {
                    
                    /* Gets data portion of S0 record by cutting off sx, length,
                     * address, and checksum portions.
                     */
                    while(count <= (strlen(record) - DATA_BACKEND_CUTOFF))
                    {
                        /*Gets byte of data portion and converts to hex*/
                        name_of_file[i] = record[count];
                        name_of_file[i + 1] = record[count + 1];
                        file_name = (int)strtol(name_of_file, NULL, HEX_CONVERSION);
                        count = count + COUNT_INCREASE;
                        printf("%c",file_name);
                    }
                    printf("\n");
                }
                
                mar = address_parse(record);
                printf("Starting Address: %04X\n",mar);
                mbr_load(record,mar);
                printf("Data Loaded Correctly\n");
            }
    }  
    return;
}
/* Function: s_record_check
 * 
 * Description: Returns false flag if any S record is invalid
 * 
 * Arguments: record
 * 
 */

enum BOOL s_record_check(char record[MAX_RECORD_SIZE])
{
    char length;
    char tens;
    char ones;
    int actual_length = 0;
    char c;
    char *temp;
    temp = strtok(record,"\r\n\0");
    /*checks if first character is 'S'*/
    if (temp[0] != 'S')
    {
        return false;
    }
    /*Checks if second character is one of the valid s record types*/
    if((temp[1] != '0') && (temp[1] != '1') && (temp[1] != '9'))
    {
        return false;
    }
    /*This block of code converts 2 array indexes to a single hex value*/
    c = temp[LENGTH_TENS];
    tens = chartoint(c);
    c = temp[LENGTH_ONES];
    ones = chartoint(c);
    length = (tens << TENS_SHIFT_VALUE); 
    length = (length | ones); 
    /* Gets actual number of bytes in S record*/
    actual_length = strlen(temp);
    actual_length = actual_length - DATA_BACKEND_CUTOFF;
    actual_length = (actual_length / BYTE_TO_BIT );
    if(actual_length != length)
    {
        return false;
    }

    if((check_sum(record)) == false)
    {
        return false;
    }
    return true;
}
/* Function: check_sum
 * 
 * Description: calculates the check sum and compares it with 
 * the S record check sum.
 * 
 * Arguments: record
 * 
 */
enum BOOL check_sum(char record[150])
{
    char c;
    int count = COUNT_START; 
    unsigned int total_check_sum = 0;
    unsigned int running_check_sum; 
    unsigned int length = strlen(record) - SX_CUTOFF;
    /*Gets check sum values from s record and saves it as single hex value*/
    c =record[length];
    unsigned char tens = chartoint(c);
    
    c = record[length + 1];
    unsigned int ones = chartoint(c);
    
    unsigned int check_sum = (tens << TENS_SHIFT_VALUE);
    check_sum = (check_sum | ones);
    /*Loops though every byte except for Sx*/
    while(count <= (length - SX_CUTOFF))
    {
        c = record[count];
        tens = chartoint(c);
        c = record[count+1];
        ones = chartoint(c);
        running_check_sum = tens << TENS_SHIFT_VALUE;
        running_check_sum = running_check_sum | ones;

        /*Get total check sum*/
        total_check_sum = total_check_sum + running_check_sum;

        count = count + COUNT_INCREASE;
    }
    /*removed upper bit*/
    total_check_sum = total_check_sum & UPPER_BYTE_MASK;
    /* Ones complement */
    total_check_sum = ~total_check_sum;
    total_check_sum = total_check_sum & UPPER_BYTE_MASK;

    if(total_check_sum != check_sum)
    {
        return false;
    }
    return true;
}
/* Function: chartoint
 * 
 * Description: converts ASCII value to hex value.
 * 
 * Arguments: c
 * 
 */
int chartoint(char c)
{
    int value;

    if(c >= '0' && c <= '9')
    {
        value = c -'0';
    }
    else if(c >= 'a' && c <= 'f')
    {
        value = c -('a' - ASCII_CASE_SHIFT);
    }
    else if (c >= 'A' && c <= 'F')
    {
        value = c -('A' - ASCII_CASE_SHIFT);
    }
    else
    {
        value = ERROR_RETURN; 
    }

    return value; 
}
/* Function: address_parse
 * 
 * Description: Gets Address as int from record string
 * 
 * Arguments: record
 * 
 */

int address_parse(char record[MAX_RECORD_SIZE])
{
    unsigned int ones;
    unsigned int tens;
    unsigned int hundreds;
    unsigned int thousands; 
    unsigned int address;
    char c;
    /*converts ASCII values to hex*/
    c = record[THOUSANDS_INDEX  ];
    thousands = chartoint(c);
    c = record[HUNDREDS_INDEX ];
    hundreds = chartoint(c);
    c = record[TENS_INDEX ];
    tens = chartoint(c);
    c = record[ONES_INDEX ];
    ones = chartoint(c);

    thousands = thousands << THOUSANDS_SHIFT;
    hundreds = hundreds << HUNDREDS_SHIFT;
    tens = tens << TENS_SHIFT ;

    address = thousands | hundreds;
    address = address | tens;
    address = address | ones;

    return address;
}
/* Function: mbr_load
 * 
 * Description: load data into mbr and call bus function.
 * 
 * Arguments: record, mar 
 * 
 */
void mbr_load(char record[MAX_RECORD_SIZE], int mar)
{
    
    unsigned short data_length = strlen(record) - DATA_BACKEND_CUTOFF;
    unsigned short *mbr;
    unsigned int index = DATA_STARING_COUNT;
    char *data;
    /*Allocate Data for data variable*/
    data = (char *)malloc(SIZE_DOUBLE  * sizeof(char));
    /*for all data bytes of the record*/
    while(index <= data_length)
    {
        /*load byte into mbr as hex value*/
        data[0] = record[index];
        data[1] = record[index + 1];
        mbr = (unsigned short *)strtol(data,NULL,HEX_CONVERSION);
        bus(mar, mbr,WRITE,BYTE);
        mar++;
        index = index + INDEX_INCREASE;
    }
    printf("\n");
    return;
}
/* Function: bus
 * 
 * Description: Used to load data into memory and pass data back to be displayed .
 * 
 * Arguments: record, mar 
 * 
 */
void bus(unsigned short mar,unsigned short *mbr,enum ACTION rw,enum SIZE bw)
{
    
    if(rw == WRITE)
    {
        memory[mar] = (unsigned short)mbr;
                
    }
    if(rw == READ)
    {
        *mbr = memory[mar];
    }
}
