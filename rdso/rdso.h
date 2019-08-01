//	Reed-Solomon Code

//	To bufsize-2*NErr bytes of information added 2*NErr control.
//	If no more than NErr bytes are spoiled in the received bufsize buffer, 
//	all errors can be completely restored.
//	For example, if 235 information bytes to add 20 control, 
//	get the size of the encoded buffer is 255.

//	If it spoils any 10 bytes, it will be noticed and fixed. 
//	A greater number of errors can be noticed, but the fix will not succeed.
//	In this example NErr = 10, bufsize = 255.
	
#include "stdint.h"
// Preparing for (de)coding
// Nerr    - the maximal number of errors in buffer
// bufsize - buffer size, <= 255, must be: 2*NErr < bufsize
int c_form ( int NErr, int bufsize );

// The buffer must be a size of bufsize. The first (bufsize-2*NErr) a information for coding.
// After coding, the first 2*NErr bytes are control, the latter is the source information.
void c_code  ( uint8_t *buf );

// Decode buf of size (bufsize),  receive TextBuf if size (bufsize-2*NErr)
int c_decode( uint8_t *buf );
