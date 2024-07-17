/* Program to perform multi-digit integer arithmetic.

   Skeleton program written by Alistair Moffat, ammoffat@unimelb.edu.au,
   August 2023, with the intention that it be modified by me
   to add functionality, as required by the assignment specification.
   All included code is (c) Copyright University of Melbourne, 2023

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>

/* All necessary #defines provided as part of the initial skeleton */

#define INTSIZE	500	/* max number of digits per integer value */
#define LINELEN	999	/* maximum length of any input line */
#define NVARS	26	/* number of different variables */
#define MAX_POWR 1661	/* largest possible power value */

#define CH_A	 'a'	/* character 'a', first variable name */

#define ERROR	(-1)	/* error return value from some functions */
#define PROMPT	"> "	/* the prompt string for interactive input */

#define PRINT	'?'	/* the print operator */
#define ASSIGN	'='	/* the assignment operator */
#define PLUS	'+'	/* the addition operator */
#define MULT	'*'	/* the multiplication operator */
#define POWR	'^'	/* the power-of operator */
#define DIVS	'/'	/* the division operator */
#define ALLOPS  "?=+*^/"

#define CH_ZERO  '0'	/* character zero */
#define CH_ONE   '1'	/* character one */

#define CH_COM   ','	/* character ',' */
#define PUT_COMMAS 3	/* interval between commas in output values */

#define INT_ZERO 0	/* integer 0 */
#define INT_ONE  1	/* integer 1 */
#define INT_TEN  10	/* integer 10 */
#define LONG_ZERO 0	/* long 0 to be stored in longint_t array */
#define LONG_ONE  1	/* long 1 to be stored in longint_t array */

#define NEWLINE  '\n'	/* character newline */
#define CH_NULL  '\0'	/* character NULL */
#define TRUE	1	/* integer 1 as True */
#define FALSE	0	/* integer 0 as False */

/* data structure for recording information about one variable */
typedef struct {
	int size;
	int digits[INTSIZE];
} longint_t;

/**********************************************************************/

/* A "magic" additional function needing explicit declaration */
int fileno(FILE *);

/* Skeleton program function prototypes */
void print_prompt(void);
void print_tadaa();
void print_error(char *message);
int  read_line(char *line, int maxlen);
void process_line(longint_t vars[], char *line);
int  get_second_value(longint_t vars[], char *rhsarg,
	longint_t *second_value);
int  to_varnum(char ident);
void do_print(int varnum, longint_t *var);
void do_assign(longint_t *var1, longint_t *var2);
void do_plus(longint_t *var1, longint_t *var2);
void do_mult(longint_t *var1, longint_t *var2);
void do_powr(longint_t *var1, longint_t *var2);
void do_divs(longint_t *var1, longint_t *var2);
int do_subs(longint_t *result_holder, int *array1, int *array2, 
	int size, int index, int one_off); 
void zero_vars(longint_t vars[]);
longint_t parse_num(char *rhs);
int max_int(int first_num, int second_num);
void exit_if(int condition, char *message);
int compare_array(int *array1, int *array2, int size, int 
	extra_index, int one_off);
void reverse_array(longint_t *var);

/**********************************************************************/

/* Main program controls all the action */
int
main(int argc, char *argv[]) {
	char line[LINELEN + 1] = {INT_ZERO};
	longint_t vars[NVARS];
	
	zero_vars(vars);
	print_prompt();
	while (read_line(line, LINELEN)) {
		if (strlen(line) > 0) {
			/* non empty line, so process it */
			process_line(vars, line);
		}
		print_prompt();
	}

	print_tadaa();
	return 0;
}

/**********************************************************************/

/* Prints the prompt indicating ready for input, but only if it
   can be confirmed that the input is coming from a terminal.
   Plus, output might be going to a file, that's why the prompt,
   if required, is written to stderr and not stdout */
void
print_prompt(void) {
	if (isatty(fileno(stdin))) {
		fprintf(stderr, PROMPT);
		fflush(stderr);
	}
}

void
print_tadaa() {
	/* all done, so pack up bat and ball and head home,
	   getting the exact final lines right is a bit tedious,
	   because input might be coming from a file and output
	   might be going to a file */
	if (isatty(fileno(stdin)) && isatty(fileno(stdout))) {
		printf("\n");
	}
	printf("ta daa!!!\n");
	if (isatty(fileno(stdin)) && !isatty(fileno(stdout))) {
		fprintf(stderr, "\n");
	}
}

void
print_error(char *message) {

	/* need to write an error message to the right place(s) */
	if (isatty(fileno(stdin)) || isatty(fileno(stdout))) {
		fprintf(stderr, "%s\n", message);
		fflush(stderr);
	}
	if (!isatty(fileno(stdout))) {
		printf("%s\n", message);
	}
}

/**********************************************************************/

/* Reads a line of input into the array passed as argument,
   returns false if there is no input available.
   All whitespace characters are removed on the way through. */
int
read_line(char *line, int maxlen) {
	int i = 0, c;

	while (((c = getchar()) != EOF) && (c != NEWLINE)) {
		if (i < maxlen && !isspace(c)) {
			line[i++] = c;
		}
	}
	line[i] = CH_NULL;

	/* then, if the input is coming from a file or the output
	   is going to a file, it is helpful to echo the input line
	   and record what the command was */
	if (!isatty(fileno(stdin)) || !isatty(fileno(stdout))) {
		printf("%s%s\n", PROMPT, line);
	}
	return ((i > 0) || (c != EOF));
}

/**********************************************************************/

/* Process a command by parsing the input line into parts */
void
process_line(longint_t vars[], char *line) {
	int varnum, optype, status;
	longint_t second_value;

	/* determine the LHS variable, it must be first character in 
	compacted line */
	varnum = to_varnum(line[0]);
	if (varnum == ERROR) {
		print_error("invalid LHS variable");
		return;
	}

	/* more testing for validity */
	if (strlen(line) < 2) {
		print_error("no operator supplied");
		return;
	}

	/* determine the operation to be performed, it must be second 
	   character of compacted line */
	optype = line[1];
	if (strchr(ALLOPS, optype) == NULL) {
		print_error("unknown operator");
		return;
	}

	/* determine the RHS argument (if one is required), it must start 
	   in the third character of compacted line */
	if (optype != PRINT) {
		if (strlen(line) < 3) {
			print_error("no RHS supplied");
			return;
		}
		status = get_second_value(vars, line + 2, &second_value);
		if (status == ERROR) {
			print_error("RHS argument is invalid");
			return;
		}
	}

	/* finally, do the actual operation */
	if (optype == PRINT) {
		do_print(varnum, vars + varnum);
	} else if (optype == ASSIGN) {
		do_assign(vars + varnum, &second_value);
	} else if (optype == PLUS) {
		do_plus(vars + varnum, &second_value);
	} else if (optype == MULT) {
		do_mult(vars + varnum, &second_value);
	} else if (optype == POWR) {
		do_powr(vars + varnum, &second_value);
	} else if (optype == DIVS) {  
		do_divs(vars + varnum, &second_value);  
	} 
	return;
}

/**********************************************************************/

/* Convert a character variable identifier to a variable number */
int
to_varnum(char ident) {
	int varnum;
	varnum = ident - CH_A;

	if (0 <= varnum && varnum < NVARS) {
		return varnum;
	} else {
		return ERROR;
	}
}

/**********************************************************************/

/* Process the input line to extract the RHS argument, which
   should start at the pointer that is passed */
int
get_second_value(longint_t vars[], char *rhsarg,
			longint_t *second_value) {
	char *p;
	int varnum2;	

	if (isdigit(*rhsarg)) {
		/* first character is a digit, so RHS is a number
		   now check the rest of RHS for validity */
		for (p = rhsarg + 1; *p; p++) {
			if (!isdigit(*p)) {
				/* found an illegal character */
				return ERROR;
			}
		}
		/* nothing wrong, ok to convert */
		*second_value = parse_num(rhsarg);
		return !ERROR;

	} else {
		/* argument is not a number, so should be a variable */
		varnum2 = to_varnum(*rhsarg);
		if (varnum2 == ERROR || strlen(rhsarg) != 1) {
			/* nope, not a variable either */
			return ERROR;
		}
		/* and finally, get that variable's value */
		do_assign(second_value, vars + varnum2);
		return !ERROR;
	}
	return ERROR;
}

/* Set the vars array to all zero values */
void
zero_vars(longint_t vars[]) {
	int i;
	longint_t zero = {LONG_ZERO, {LONG_ZERO}}; 

	for (i = 0; i < NVARS; i++) {
		do_assign(vars + i, &zero);
	}
	return;
}

/**********************************************************************/

/* Create an internal-format number out of a string */
longint_t
parse_num(char *rhs) {
	/* indigit indicates in-between digits to avoid prefix zero */
	int var_size = 0, zero_prefix_count = 0, indigit = FALSE;
	longint_t second_value_arr = {LONG_ZERO, {LONG_ZERO}};	 
	
	/* Convert and store each digit as integer in second_value_arr */
	while (*rhs) {
		exit_if(var_size >= INTSIZE, 
		"integer overflow, program terminated");

		/* Check for any prefix zero */
		if (!indigit) {	   
			if (*rhs != CH_ZERO) {
				indigit = TRUE;
			} else {
				zero_prefix_count++;
			}
		}  
		if (indigit) {
			second_value_arr.digits[var_size - 
			zero_prefix_count] = *rhs - CH_ZERO;
		} 
		rhs++;
		var_size++;
	}   	
	second_value_arr.size = var_size - zero_prefix_count;   
	
	/* Reverse digit order */
	if (second_value_arr.size) {
		reverse_array(&second_value_arr);
	}
	return second_value_arr;
}

/**********************************************************************/

/* Print out longint value */
void
do_print(int varnum, longint_t *var) {
	int i, indigit = FALSE; 
	printf("register %c: ", varnum + CH_A);

	/* Handle number 0 */
	if (!var->size) {
		printf("%d\n", LONG_ZERO);
		return;
	} 
	/* Print out a longint value */
	for (i = var->size; i >= 1; i--) {
		/* Add a comma after every index divisible by 3 */
		if (indigit && (i % PUT_COMMAS == 0)) {
			putchar(CH_COM);
		} else if (i == var->size) {
			indigit = TRUE;
		}
		printf("%d", var->digits[i - 1]);   
	}
	putchar(NEWLINE);
}

/**********************************************************************/

/* Assign a longint value, could do this with just an assignment
   statement, because structs can be assigned, but this is more
   elegant, and only copies over the array elements (digits) that
   are currently in use: var1 = var2 */
void
do_assign(longint_t *var1, longint_t *var2) {
	*var1 = *var2;
}

/**********************************************************************/

/* Update var1 by doing an addition var1 + var2 */
void
do_plus(longint_t *var1, longint_t *var2) {	
	if (!var2->size) {
		return;
	} 

	/* tenth_holder stores 'tenth' value when previous calculation 
	   exceeds 10 */
	int tenth_holder = INT_ZERO; 
	int i, max_size = max_int(var1->size, var2->size);  

	/* Sum 2 digits from var1 and var2 of the same index */
	for (i = 0; i < max_size; i++) {
		var1->digits[i] += var2->digits[i] + tenth_holder;
		tenth_holder = var1->digits[i] / INT_TEN;
		var1->digits[i] %= INT_TEN;   
	}
	var1->size = max_size;

	/* Check for any remaining extra digit */  
	if (tenth_holder) {
		exit_if(max_size + 1 > INTSIZE, 
		"integer overflow, program terminated");
		var1->digits[max_size] += tenth_holder;
		var1->size++;
	} 
}

/**********************************************************************/

/* Perform multiplication by looping internally through var2 to 
   multiply var2 digits with var1 digits. Then use addition-based  
   looping for an outer loop (var1) to sum all products. Update
   result to var1 */
void 
do_mult(longint_t *var1, longint_t *var2) {
	/* 'holder' stores values during calculations */
	longint_t holder1 = {LONG_ZERO, {LONG_ZERO}};

	/* Handle 0 and 1 multipliers */
	if (!var2->size) {
		*var1 = holder1;
		return;
	} else if (var2->size == INT_ONE && var2->digits[0] == LONG_ONE) {
		return;
	}

	/* Perform mulplication using holder1 and holder2 to store values 
	   in-between calculations */
	int i, j;
	for (i = 0; i < var1->size; i++) {
		/* Keep track of the increasing holder2 size */
		exit_if(var2->size + i > INTSIZE, 
		"integer overflow, program terminated");

		longint_t holder2 = {var2->size + i, {LONG_ZERO}};
		for (j = 0; j < var2->size; j++) {  
			holder2.digits[j + i] = var1->digits[i] * 
			var2->digits[j];
		}
		do_plus(&holder1, &holder2);
	}
	do_assign(var1, &holder1);
}

/**********************************************************************/

/* Update var1 by raising var1 to power of var2 via repeated
   multiplication for half the size of 'power' value */
void 
do_powr(longint_t *var1, longint_t *var2) {
	int i, power = INT_ZERO;
	longint_t one_digit = {LONG_ONE, {LONG_ONE}};
	
	/* Handle bases 1 and 0 and power of 1 */
	if (!var1->size || (var1->size == INT_ONE && var1->digits[0] 
	== LONG_ONE) || (var2->size == INT_ONE && var2->digits[0] == 
	LONG_ONE)) {
		return;
	}
	/* Handle power of 0 */
	if (!var2->size) {
		do_assign(var1, &one_digit);
		return;
	}
	/* Get the power value */
	for (i = 0; i < var2->size; i++) {
		power += var2->digits[i] * ((int) pow(INT_TEN, i));
		exit_if(power >= MAX_POWR, 
		"integer overflow, program terminated");
	}
	
	/* Self-multiply for half the 'power' times before multiplying 
	   with another half */
	int half_power = power / 2, remainder = power % 2;
	longint_t holder = *var1;
	for (i = 0; i < half_power - 1; i++) {
		do_mult(&holder, var1);
	} 
	do_mult(&holder, &holder);
	
	/* Compute remainder for odd-numbered 'power' */
	if (remainder) {
		do_mult(&holder, var1);
	}
	do_assign(var1, &holder);
}

/**********************************************************************/

/* Divide var1 by var2 by first comparing values of the same index 
   to determine whether an extra prefix is required to perform 
   positive substraction. Then use a subtraction-based looping 
   to get result. Update result to var1*/
void 
do_divs(longint_t *var1, longint_t *var2) {
	/* Handle 0 denominator */
	exit_if(var2->size <= 0, 
	"zero division error, program terminated");

	longint_t result = {LONG_ZERO, {LONG_ZERO}};
	/* Handle lesser numerator than denominator */
	if (var2->size > var1->size || !var1->size) {
		*var1 = result;
		return;
	}
	
	/* one_off is True when extra digit is required to 
	   perform positive substraction */
	int i, one_off = FALSE;
	longint_t holder = *var1;
	
	/* Perform division through array size comparison and 
	   subtraction-based looping */
	for (i = var1->size; i >= var2->size; i--) {
		/* extra_index indicates size difference between
		   var1 and var2 */
		int extra_index = i - var2->size;
		if (compare_array(holder.digits, var2->digits, var2->size,
		 extra_index, one_off)) {
			one_off = do_subs(&result, holder.digits, 
			var2->digits, var2->size, extra_index, one_off);

		} else {
			if (i != var1->size) {
				result.digits[result.size++] = LONG_ZERO;
			}
			one_off = TRUE;
		}
	}
	reverse_array(&result);
	do_assign(var1, &result);
}

/**********************************************************************/

/* Compare 2 arrays and return TRUE if array1 has at least 1
   digit value greater than that of array2 at the same index 
   or all values are equal */
int
compare_array(int *array1, int *array2, int size, int extra_index, 
int one_off) {
	int i;
	for (i = size - 1 + one_off; i >= 0; i--) {
		if (array1[i + extra_index] > array2[i]) {
			return TRUE;
		} else if (array1[i + extra_index] < array2[i]) {
			return FALSE;
		}
	}
	/* Return True when all values are equal */
	return TRUE;
}

/* Substract digits in array2 from array1 and count rounds of 
   positive substraction, while taking into account array size 
   difference via 'index' and 'one_off' */
int 
do_subs(longint_t *result, int *array1, int *array2, int size, 
int index, int one_off) {
	int i, subst_count = 0, remainder = FALSE;

	/* Substract digits in array2 from array1 */
	while (compare_array(array1, array2, size, index, one_off)) {
		/* tenth_holder is True when previous substraction 
		   results in negative value */
		int tenth_holder = FALSE;
		for (i = 0; i < size + one_off; i++) {
			int subst = array1[i + index] - (array2[i] + 
			tenth_holder);
			if (subst < 0) {
				tenth_holder = TRUE;
				subst += INT_TEN;
			} else {
				tenth_holder = FALSE;
			}
			array1[i + index] = subst;

			/* Any non-zero remainder indicates requirement for a 
			   one-off digit for future calculations */
			if (subst) {
				remainder = TRUE;
			}
		}
		subst_count++;
	}
	result->digits[result->size++] = subst_count;

	return remainder;
}

/* Compare and return the larger of between two integers. 
   This function adapted from max_2_ints() provided in Grok Ex:5.1, 
   written by Jianzhong Qi, and accessed via 
   https://groklearning.com/learn/unimelb-comp10002-2023-s2/
   chapter5/3/?ignore_lc=true 02 September 2023.
   Altered the names of the arguments */
int
max_int(int first_num, int second_num) {
	if (first_num > second_num) {
		return first_num;
	}
	return second_num;
}

/* Exit failure if the digit size exceeds INTSIZE
   This function adapted from exit_if() provided in Grok Ex:5.4, 
   written by Alistair Moffat, and accessed via 
   https://groklearning.com/learn/unimelb-comp10002-2023-s2/
   chapter5/7/solution/?ignore_lc=true 02 September 2023.
   Altered name of an argument and added a new print_error() */
void
exit_if(int condition, char *message) {
	if (condition) {
		print_error(message);
		exit(EXIT_FAILURE);
	}
}

/* Reverse order of array values */
void
reverse_array(longint_t *var) {
	int i, holder;
	for (i = 0; i < var->size / 2; i++) {
		holder = var->digits[i];
		var->digits[i] = var->digits[var->size - i - 1];
		var->digits[var->size - i - 1] = holder;
	}
}

/***********************************************************************
Algorithms are fun !!!
************************************************************************/