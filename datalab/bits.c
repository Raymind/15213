/*
 * CS:APP Data Lab
 *
 * Aniruddh Chaturvedi; achaturv
 *
 * Can only !, ~, &, ^, |, +, <<, >>
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:

  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code
  must conform to the following style:

  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>

  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.


  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function.
     The max operator count is checked by dlc. Note that '=' is not
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 *
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce
 *      the correct answers.
 */


#endif
/*
 * bitXor - x^y using only ~ and &
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  return ~(x&y) & ~(~x&~y);
  /*
    ~(x&y) : when x and y both have 1s, it returns a 0, and otherwise
    it returns a 1
    ~(~x&~y) :when x and y both have 0s, it returns a 0, otherwise it
    returns a 1.
    ~(x&y)& ~(~x&~y) : it will return a 1 only when the bits are
    different and will return a 0 otherwise (when either both 0s or
    both 1s), which is what the xor operator does
   */
}

/*
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
  int A = 0xAA + (0xAA<<8) + (0xAA<<16) + (0xAA<<24);
  return !((x&A) ^ A);
  /* all odd-numbered bits are set to 1 only in 0xAAAAAAAA, which
     corresponds to 10101010101010101010101010101010 in binary.
     we check this by constructing this number by left shifting
     and adding with 8 bits at a time (max number restriction
     was 55)
     Then we do x&A, which checks if x and A have the same bits.
     if they have the same bits, it results in A. then we do
     result ^ A, which returns 0 if all the bits are equal
     and 1 otherwise. then we do !(result) so that it returns
     1 if the bits are equal (if the result is 0) and 0 otherwise
     (bits are not equal)
   */

}
/*
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
  n = n << 3;
  return ((x >>n)&0xFF);
  /*
    since we want the nth byte, and since we know that one byte
    contains 8 bits, we left shift n by 3 in order to get it to be a
    multiple of 8. then, we right-shift x by that many bit positions
    and then and it with 0xFF in order to just get the lower 8 bits
    /lowest one byte, which is what we want.
   */
}
/*
 * reverseBytes - reverse the bytes of x
 *   Example: reverseBytes(0x01020304) = 0x04030201
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 25
 *   Rating: 3
 */
int reverseBytes(int x) {
  int a1 = x & 0xFF;
  int a2 = (x>>8) & 0xFF;
  int a3 = (x>>16) & 0xFF;
  int a4 = (x>>24) & 0xFF;
  return (a1<<24) + (a2<<16) + (a3<<8) + a4;

  /* we extract each of the four bytes, store them in different variables
     and then add the bytes back in a reverse order so as to return
     the reversed byte
   */

}
/*
 * bitMask - Generate a mask consisting of all 1's
 *   lowbit and highbit
 *   Examples: bitMask(5,3) = 0x38
 *   Assume 0 <= lowbit <= 31, and 0 <= highbit <= 31
 *   If lowbit > highbit, then mask should be all 0's
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int bitMask(int highbit, int lowbit) {
  int i = ~0; //0xFF
  int hi = i << highbit;
  int lo = i << lowbit;
  hi = hi << 1 ;
  return (hi^lo) & lo;

  /* we first create a number consisting of all 1s.
     then, we left-shift by highbit and lowbit so that everything
     above highbit is a 1, and everything above lowbit is a 1. Then,
     we need to left shift the highbit number by 1 because bit counting
     begins with 0. After this, we xor hi and lo in order to obtain
     the bits in which there are 1s in high and 0s in lo.
     and then we & with lo in order to just get those particular bits.
     after that we have the bitmask as required.
   */
}
/*
 *   bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
int bang(int x) {
  return  (~( x | (~x+1)) >> 31) & 1;

  /* if x == 0 : x | -x will be 0, then ~(x | -x) is 0xFFFFFFFF.
     if x > 0: x | -x makes most significant bit 1, and then
     right-shifts by 31 and & with 1 in order to retrieve the
     sign bit.
  */
}
/*
 * isNegative - return 1 if x < 0, return 0 otherwise
 *   Example: isNegative(-1) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int isNegative(int x) {
  return (x>>31)&1;

  //if sign bit is 1, then negative

}
/*
 * negate - return -x
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return ~x+1;
  //regular twos complement arithmetic for a signed int.
}
/*
 * subOK - Determine if can compute x-y without overflow
 *   Example: subOK(0x80000000,0x80000000) = 1,
 *            subOK(0x80000000,0x70000000) = 0,
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int subOK(int x, int y) {
  int diff = x + (~y+1); //x-y
  int sx = (x>>31) & 1; // sign bit of x
  int sy = (y>>31) & 1; // sign bit of y
  int sd = (diff>>31) & 1; //sign bit of difference
/*  * case 1: x positive, y positive: overflow doesn't occur
    * case 2 : x negative, y negative : overflow doesn't occur
    * case 3 : x positive y negative : overflow could occur if sign
    of diff is negative
    * case 4 :  x negative, y positive : overflow could occur if sign of
    diff is positive
*/

  return !((!(sx) & sy & sd) | (sx & !(sy) & !(sd)));
}
/*
 * isLess - if x < y  then return 1, else return 0
 *   Example: isLess(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLess(int x, int y) {
  int sx = (x >> 31) & 1; //sign bit of x
  int sy = (y >> 31) & 1; //sign bit of y
  int sdiff = ((x + (~(y)+1)) >> 31) & 1; // sign bit of difference
  int possign = (sx ^ sy); //0 if same sign, 1 otherwise
/* if x is positive, y is positive and x-y is negative, then x<y is true
  * if x is negative, y is positive, then x<y is true
  * if x is positive, y is negative, then x< y is false
*/
  return ( ( !(possign) & sdiff) | (sx & !(sy)));
}

/*
 * multFiveEighths - multiplies by 5/8 rounding toward 0.
 *   Should exactly duplicate effect of C expression (x*5/8),
 *   including overflow behavior.
 *   Examples: multFiveEighths(77) = 48
 *             multFiveEighths(-22) = -13
 *             multFiveEighths(1073741824) = 13421728 (overflow)
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 3
 */
int multFiveEighths(int x) {
  int mult = (x<<2) + x; //5*x
  int sign = (mult >> 31); //extracting sign bit from result
  //... could have been done with x also, but still.
  int neground = ((mult + 7) >> 3) & sign;
  //rounding procedure as described in lecture notes
  return neground + ((mult& ~sign)>>3);
  //taking the rounding result, if sign is 1(negative) then return
  // just the rounded part, and if not, then return just the
  //latter part of return statement wherein we divide by 8.
}

/*
 * float_abs - Return bit-level equivalent of absolute value of f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument..
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */

unsigned float_abs(unsigned uf) {
  int expt = (uf<<1)>>24; //exponent bits
  int frac = uf << 9; // fraction
  if(((expt^0xFF)==0) && (frac!=0x00)) //checks for NaN condition
    return uf;
  else
    return uf & 0x7fffffff;
  // return result of lower 31 bits thus getting rid of sign bit
}

/*
 * float_f2i - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */

int float_f2i(unsigned uf) {
  int exponent = (uf & (0xFF<<23)) >> 23; //retrieves exponent
  int exp_sign = uf>>31; //retrieves sign of the number
  int frac = uf & 0xFFFFFF; //retrieves fraction value
  int normal_frac = 1<<23 | frac; //for normalized case, add implied 1
  int biased_exp = exponent - 127; //applies bias to exponent
  if (uf == 0) //if entire number is 0, then return 0
    return 0;
  if (exponent == 0xFF || biased_exp >=31)
    return 0x80000000;
    //NaN if exponent is all 1s or exponent with bias is >=31
    //because overflow otherwise
  if (biased_exp < 0)
    return 0;
  //if negative exponent, fractional number, not an integer
  else if (biased_exp < 23)
  {
    normal_frac = normal_frac >> (23-biased_exp);
  }
  else if (biased_exp > 23){
    normal_frac = normal_frac << (biased_exp - 23);
  }
  if(exp_sign == 1){ //finally apply negative sign if negative.
    normal_frac = -normal_frac;
  }
  return normal_frac;
}
