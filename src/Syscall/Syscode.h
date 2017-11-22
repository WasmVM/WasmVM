#ifndef SYSCODE_DEF
#define SYSCODE_DEF

/* SYS_stdin - input data to standard input
 * PARAMETER 
 * 	buf : Memory address of input data 
 * 	len : Length of maximun data length
 * RETURN
 *  Real input length if success, -1 if error occur and throw error
 */
#define SYS_stdin 0x00

/* SYS_stdout - Output data to standard output
 * PARAMETER 
 * 	buf : Memory address of output data 
 * 	len : Length of output data
 * RETURN
 *  Real output length if success, -1 if error occur and throw error
 */
#define SYS_stdout 0x01

/* SYS_stderr - Output data to standard error
 * PARAMETER 
 * 	buf : Memory address of output data 
 * 	len : Length of output data
 * RETURN
 *  Real output length if success, -1 if error occur and throw error
 */
#define SYS_stderr 0x02

#endif
