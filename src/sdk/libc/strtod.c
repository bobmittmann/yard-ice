/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the YARD-ICE.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file strtod.c
 * @brief YARD-ICE libc
 */ 

/* 
 * strtod.c --
 *
 *	Source code for the "strtod" library procedure.
 *
 * Copyright (c) 1988-1993 The Regents of the University of California.
 * Copyright (c) 1994 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: strtod.c,v 1.1.1.4 2003/03/06 00:09:04 landonf Exp $
 */

#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>

static int max_exponent = 511;	/* Largest possible base 10 exponent.  Any
								 * exponent larger than this will already
								 * produce underflow or overflow, so there's
								 * no need to worry about additional digits.
								 */

static double powers_of_10[] = {	/* Table giving binary powers of 10.  Entry */
	10.,			/* is 10^2^i.  Used to convert decimal */
	100.,			/* exponents into floating-point numbers. */
	1.0e4,
	1.0e8,
	1.0e16,
	1.0e32,
	1.0e64,
	1.0e128,
	1.0e256
};


/*
 *----------------------------------------------------------------------
 *
 * strtod --
 *
 *	This procedure converts a floating-point number from an ASCII
 *	decimal representation to internal double-precision format.
 *
 * Results:
 *	The return value is the double-precision floating-point
 *	representation of the characters in string.  If endPtr isn't
 *	NULL, then *endPtr is filled in with the address of the
 *	next character after the last one that was part of the
 *	floating-point number.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

double strtod(const char * __s, char ** __endp)
{
	bool exp_sign = false;
	double fraction;
	double dbl_exp;
	double * d;
	bool sign;
	char * cp;
	int c;
	int exp = 0;		/* Exponent read from "EX" field. */
	int frac_exp = 0;		/* Exponent that derives from the fractional
							 * part.  Under normal circumstatnces, it is
							 * the negative of the number of digits in F.
							 * However, if I is very long, the last digits
							 * of I get dropped (otherwise a long I with a
							 * large negative exponent could cause an
							 * unnecessary overflow on I alone).  In this
							 * case, frac_exp is incremented one for each
							 * dropped digit. */
	int mant_size;		/* Number of digits in mantissa. */
	int dec_pt;			/* Number of mantissa digits BEFORE decimal
						 * point. */
	char * cp_exp;		/* Temporarily holds location of exponent
							 * in string. */

	/*
	 * Strip off leading blanks and check for a sign.
	 */

	cp = (char *)__s;
	while (isspace(*cp)) {
		cp += 1;
	}
	if (*cp == '-') {
		sign = true;
		cp += 1;
	} else {
		if (*cp == '+') {
			cp += 1;
		}
		sign = false;
	}

	/*
	 * Count the number of digits in the mantissa (including the decimal
	 * point), and also locate the decimal point.
	 */

	dec_pt = -1;
	for (mant_size = 0; ; mant_size += 1) {
		c = *cp;
		if (!isdigit(c)) {
			if ((c != '.') || (dec_pt >= 0)) {
				break;
			}
			dec_pt = mant_size;
		}
		cp += 1;
	}

	/*
	 * Now suck up the digits in the mantissa.  Use two integers to
	 * collect 9 digits each (this is faster than using floating-point).
	 * If the mantissa has more than 18 digits, ignore the extras, since
	 * they can't affect the value anyway.
	 */

	cp_exp  = cp;
	cp -= mant_size;
	if (dec_pt < 0) {
		dec_pt = mant_size;
	} else {
		mant_size -= 1;			/* One of the digits was the point. */
	}
	if (mant_size > 18) {
		frac_exp = dec_pt - 18;
		mant_size = 18;
	} else {
		frac_exp = dec_pt - mant_size;
	}
	if (mant_size == 0) {
		fraction = 0.0;
		cp = (char *)__s;
		goto done;
	} else {
		int frac1, frac2;
		frac1 = 0;
		for ( ; mant_size > 9; mant_size -= 1) {
			c = *cp;
			cp += 1;
			if (c == '.') {
				c = *cp;
				cp += 1;
			}
			frac1 = 10*frac1 + (c - '0');
		}
		frac2 = 0;
		for (; mant_size > 0; mant_size -= 1) {
			c = *cp;
			cp += 1;
			if (c == '.') {
				c = *cp;
				cp += 1;
			}
			frac2 = 10*frac2 + (c - '0');
		}
		fraction = (1.0e9 * frac1) + frac2;
	}

	/*
	 * Skim off the exponent.
	 */

	cp = cp_exp;
	if ((*cp == 'E') || (*cp == 'e')) {
		cp += 1;
		if (*cp == '-') {
			exp_sign = true;
			cp += 1;
		} else {
			if (*cp == '+') {
				cp += 1;
			}
			exp_sign = false;
		}
		if (!isdigit(*cp)) {
			cp = cp_exp;
			goto done;
		}
		while (isdigit(*cp)) {
			exp = exp * 10 + (*cp - '0');
			cp += 1;
		}
	}
	if (exp_sign) {
		exp = frac_exp - exp;
	} else {
		exp = frac_exp + exp;
	}

	/*
	 * Generate a floating-point number that represents the exponent.
	 * Do this by processing the exponent one bit at a time to combine
	 * many powers of 2 of 10. Then combine the exponent with the
	 * fraction.
	 */

	if (exp < 0) {
		exp_sign = true;
		exp = -exp;
	} else {
		exp_sign = false;
	}

	if (exp > max_exponent) {
		exp = max_exponent;
		errno = ERANGE;
	}

	dbl_exp = 1.0;
	for (d = powers_of_10; exp != 0; exp >>= 1, d += 1) {
		if (exp & 1)
			dbl_exp *= *d;
	}

	if (exp_sign) {
		fraction /= dbl_exp;
	} else {
		fraction *= dbl_exp;
	}

done:
	if (__endp != NULL)
		*__endp = (char *)cp;

	if (sign)
		return -fraction;

	return fraction;

}

