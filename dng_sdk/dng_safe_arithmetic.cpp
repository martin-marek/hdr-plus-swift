/*****************************************************************************/
// Copyright 2015-2019 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

#include "dng_safe_arithmetic.h"

#include <limits>

#include "dng_exceptions.h"

// Implementation of safe integer arithmetic follows guidelines from
// https://www.securecoding.cert.org/confluence/display/c/INT30-C.+Ensure+that+unsigned+integer+operations+do+not+wrap
// and
// https://www.securecoding.cert.org/confluence/display/c/INT32-C.+Ensure+that+operations+on+signed+integers+do+not+result+in+overflow

namespace {

// Template functions for safe arithmetic. These functions are not exposed in
// the header for the time being to avoid having to add checks for the various
// constraints on the template argument (e.g. that it is integral and possibly
// signed or unsigned only). This should be done using a static_assert(), but
// we want to be portable to pre-C++11 compilers.

// Returns the result of adding arg1 and arg2 if it will fit in a T (where T is
// a signed or unsigned integer type). Otherwise, throws a dng_exception with
// error code dng_error_unknown.
template <class T>
T SafeAdd(T arg1, T arg2) {
  // The condition is reformulated relative to the version on
  // www.securecoding.cert.org to check for valid instead of invalid cases. It
  // seems safer to enumerate the valid cases (and potentially miss one) than
  // enumerate the invalid cases.
  // If T is an unsigned type, the second half of the condition always evaluates
  // to false and will presumably be compiled out by the compiler.
  if ((arg1 >= 0 && arg2 <= std::numeric_limits<T>::max() - arg1) ||
	  (arg1 < 0 && arg2 >= std::numeric_limits<T>::min() - arg1)) {
	return arg1 + arg2;
  } else {
	ThrowOverflow ("Arithmetic overflow in SafeAdd");

	// Dummy return statement.
	return 0;
  }
}

// Returns the result of multiplying arg1 and arg2 if it will fit in a T (where
// T is an unsigned integer type). Otherwise, throws a dng_exception with error
// code dng_error_unknown.
template <class T>
T SafeUnsignedMult(T arg1, T arg2) {
  if (arg1 == 0 || arg2 <= std::numeric_limits<T>::max() / arg1) {
	return arg1 * arg2;
  } else {
	ThrowOverflow ("Arithmetic overflow in SafeUnsignedMult");

	// Dummy return statement.
	return 0;
  }
}

}  // namespace

bool SafeInt32Add(int32 arg1, int32 arg2, int32 *result) {
  try {
	*result = SafeInt32Add(arg1, arg2);
	return true;
  } catch (const dng_exception &) {
	return false;
  }
}

int32 SafeInt32Add(int32 arg1, int32 arg2) {
  return SafeAdd<int32>(arg1, arg2);
}

int64 SafeInt64Add(int64 arg1, int64 arg2) {
  return SafeAdd<int64>(arg1, arg2);
}

bool SafeUint32Add(uint32 arg1, uint32 arg2,
				   uint32 *result) {
  try {
	*result = SafeUint32Add(arg1, arg2);
	return true;
  } catch (const dng_exception &) {
	return false;
  }
}

uint32 SafeUint32Add(uint32 arg1, uint32 arg2) {
  return SafeAdd<uint32>(arg1, arg2);
}

bool SafeInt32Sub(int32 arg1, int32 arg2, int32 *result) {
  if ((arg2 >= 0 && arg1 >= std::numeric_limits<int32_t>::min() + arg2) ||
	  (arg2 < 0 && arg1 <= std::numeric_limits<int32_t>::max() + arg2)) {
	*result = arg1 - arg2;
	return true;
  } else {
	return false;
  }
}

int32 SafeInt32Sub(int32 arg1, int32 arg2) {
  int32 result = 0;

  if (!SafeInt32Sub(arg1, arg2, &result)) {
	ThrowOverflow ("Arithmetic overflow in SafeInt32Sub");
  }

  return result;
}

bool SafeUint32Mult(uint32 arg1, uint32 arg2,
					uint32 *result) {
  try {
	*result = SafeUint32Mult(arg1, arg2);
	return true;
  } catch (const dng_exception &) {
	return false;
  }
}

bool SafeUint32Mult(uint32 arg1, uint32 arg2, uint32 arg3,
					uint32 *result) {
  try {
	*result = SafeUint32Mult(arg1, arg2, arg3);
	return true;
  } catch (const dng_exception &) {
	return false;
  }
}

bool SafeUint32Mult(uint32 arg1, uint32 arg2, uint32 arg3,
					uint32 arg4, uint32 *result) {
  try {
	*result = SafeUint32Mult(arg1, arg2, arg3, arg4);
	return true;
  } catch (const dng_exception &) {
	return false;
  }
}

uint32 SafeUint32Mult(uint32 arg1, uint32 arg2) {
  return SafeUnsignedMult<uint32>(arg1, arg2);
}

uint32 SafeUint32Mult(uint32 arg1, uint32 arg2,
							 uint32 arg3) {
  return SafeUint32Mult(SafeUint32Mult(arg1, arg2), arg3);
}

uint32 SafeUint32Mult(uint32 arg1, uint32 arg2,
							 uint32 arg3, uint32 arg4) {
  return SafeUint32Mult(SafeUint32Mult(arg1, arg2, arg3), arg4);
}

std::size_t SafeSizetMult(std::size_t arg1, std::size_t arg2) {
  return SafeUnsignedMult<std::size_t>(arg1, arg2);
}

int64 SafeInt64Mult(int64 arg1, int64 arg2) {
  bool overflow = true;

  if (arg1 > 0) {
	if (arg2 > 0) {
	  overflow = (arg1 > INT64_MAX / arg2);
	} else {
	  overflow = (arg2 < INT64_MIN / arg1);
	}
  } else {
	if (arg2 > 0) {
	  overflow = (arg1 < INT64_MIN / arg2);
	} else {
	  overflow = (arg1 != 0 && arg2 < INT64_MAX / arg1);
	}
  }

  if (overflow) {
	ThrowOverflow ("Arithmetic overflow");

	// Dummy return.
	return 0;
  } else {
	return arg1 * arg2;
  }
}

uint32 SafeUint32DivideUp(uint32 arg1, uint32 arg2) {
  // It might seem more intuitive to implement this function simply as
  //
  //   return arg2 == 0 ? 0 : (arg1 + arg2 - 1) / arg2;
  //
  // but the expression "arg1 + arg2" can wrap around.

  if (arg2 == 0) {
	ThrowProgramError("Division by zero");

	// Dummy return to avoid compiler error about missing return statement.
	return 0;
  } else if (arg1 == 0) {
	// If arg1 is zero, return zero to avoid wraparound in the expression
	//	 "arg1 - 1" below.
	return 0;
  } else {
	return (arg1 - 1) / arg2 + 1;
  }
}

bool RoundUpUint32ToMultiple(uint32 val, uint32 multiple_of,
							 uint32 *result) {
  if (multiple_of == 0) {
	return false;
  }

  const uint32 remainder = val % multiple_of;
  if (remainder == 0) {
	*result = val;
	return true;
  } else {
	return SafeUint32Add(val, multiple_of - remainder, result);
  }
}

bool ConvertUint32ToInt32(uint32 val, int32 *result) {
  const uint32 kInt32MaxAsUint32 =
	  static_cast<uint32>(std::numeric_limits<int32>::max());

  if (val <= kInt32MaxAsUint32) {
	*result = static_cast<int32>(val);
	return true;
  } else {
	return false;
  }
}

/*****************************************************************************/

dng_safe_uint32::dng_safe_uint32 (const dng_safe_int32 &x)
	{
	
	if (x.Get () < 0)
		{
		ThrowOverflow ("Overflow in dng_safe_uint32");
		}

	fValue = static_cast<uint32> (x.Get ());
	
	}

/*****************************************************************************/

dng_safe_int32::dng_safe_int32 (const dng_safe_uint32 &x)
	{
	
	Set_uint32 (x.Get ());
	
	}

/*****************************************************************************/
