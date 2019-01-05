/*	S_rec.h
 *	purpose and definitions
 *			- adds hex values to Srec
 *			- outputs a full Srec buffer to .s19 file
 *	global variable: 
 			- *LIS_f , *Srec_f
 *	Author:	Nick Stanwood
 **********************************************************************************/
 
 #include <stdio.h>
 
 #define 	max_data 	32

struct bit_field {unsigned int byte:8;};

union Srecord{
	unsigned char car;
	unsigned short shrt;
	struct bit_field decimal;
}; 

extern int PC_init; 
static union Srecord Srec_data[];
static unsigned int Srec_addr;
static signed int check_sum;
static unsigned int Srec_len;
static unsigned int index;  
FILE *Srec_f;
FILE *LIS_f;
 
/*	emit_word()
 *	puts a word into Srec_data[]
 *	if the Srec is empty it puts the LC into Srec_addr
 *	increases the LC by 2
 */ 
extern unsigned int emit_word(unsigned short address , unsigned int LC);
 
/*	emit_byte()
 *	puts a single byte into Srec_data[]
 *	if the Srec is empty it puts LC into Srec_addr
 *	increases the LC by 1
 */ 
extern unsigned int emit_byte(unsigned short val , unsigned int LC);
 
/*	emit_char()
 *	puts a char into Srec_data[]
 *	used only with the ASCII directive
 *	increases the LC by 1
 */ 
unsigned int emit_char(unsigned char byte , unsigned int LC); 
 
/*	output_srec()
 *	called when Srec_data[] is full, or an ORG directive was found
 *	prints out the current Srec to the .s19 file
 */ 
extern void output_Srec(void);

/*	Srec_s9()
 *	called at the end of the program to output an s9 record to the .s19 file
 */
extern void Srec_s9(void);
/*	Srec_init()
 *	used to initialze the Srec and open the .s19 file
 */
extern void Srec_init(void);
