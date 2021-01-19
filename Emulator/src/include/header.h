/* File: ArchA2
 * Author: Andrew Mercer
 * School: Dalhousie University 
 * Dept: Electrical and Computer Eng 
 * 
 * Purpose: To emeulate loading an dumping data from memory. 
 * 
 * Note: header.h
 * 
 * Last Modified: 2020.06.25
 * 
 */

#ifndef HEADER_H
#define HEADER_H

#define COMMAND_LINE_ARG_SIZE 100
#define BUS_TRANSFER_SIZE     16
#define INVALID_ASCII_SIZE    32
#define MAX_RECORD_SIZE       100
#define DATA_STARING_COUNT    8
#define DATA_BACKEND_CUTOFF   4
#define HEX_CONVERSION        16
#define COUNT_INCREASE        2
#define LENGTH_TENS           2
#define LENGTH_ONES           3
#define TENS_SHIFT_VALUE      4
#define BYTE_TO_BIT           2
#define COUNT_START           2
#define SX_CUTOFF             2
#define COUNT_INCREASE        2
#define UPPER_BYTE_MASK       0x0FF
#define ASCII_CASE_SHIFT      10
#define ERROR_RETURN          -1
#define THOUSANDS_INDEX       4 
#define HUNDREDS_INDEX        5
#define TENS_INDEX            6
#define ONES_INDEX            7
#define THOUSANDS_SHIFT       12
#define HUNDREDS_SHIFT        8
#define TENS_SHIFT            4
#define SIZE_DOUBLE           2
#define INDEX_INCREASE        2
#define NO_ARGS               1
#define TWO_ARGS              3
#define FIRST_ARG             2

enum BOOL   {true = 1, false = 0};
enum ACTION {READ = 1, WRITE = 0};
enum SIZE   {BYTE = 1, WORD =  0};

/*Every index is one byte*/
unsigned short memory[65535];
enum BOOL   debugger_flag;
enum ACTION rw;
enum SIZE   bw;

void DUMP(char start[], char end[]);
int chartoint(char c);
int address_parse(char record[150]);
enum BOOL s_record_check(char record[150]);
enum BOOL check_sum(char record[150]);
void bus(unsigned short mar,unsigned short *mbr,enum ACTION rw,enum SIZE bw);
void mbr_load(char record[150], int mar);
void DEBUGGER(FILE* fin);
void LOADER(FILE * fin);

#endif