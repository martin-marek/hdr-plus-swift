/*****************************************************************************/
// Copyright 2006-2019 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

#ifndef __dng_spline__
#define __dng_spline__

/*****************************************************************************/

#include "dng_1d_function.h"
#include "dng_memory.h"

/*****************************************************************************/

inline real64 EvaluateSplineSegment (real64 x,
								     real64 x0,
								     real64 y0,
								     real64 s0,
								     real64 x1,
								     real64 y1,
								     real64 s1)
	{
	
	real64 A = x1 - x0;

	real64 B = (x - x0) / A;

	real64 C = (x1 - x) / A;

	real64 D = ((y0 * (2.0 - C + B) + (s0 * A * B)) * (C * C)) +
			   ((y1 * (2.0 - B + C) - (s1 * A * C)) * (B * B));
			   
	return D;
	
	}

/*****************************************************************************/

class dng_spline_solver: public dng_1d_function
	{
	
	protected:
	
		dng_std_vector<real64> X;
		dng_std_vector<real64> Y;
		
		dng_std_vector<real64> S;
		
	public:
	
		dng_spline_solver ();
		
		virtual ~dng_spline_solver ();
			
		void Reset ();

		void Add (real64 x, real64 y);

		virtual void Solve ();

		virtual bool IsIdentity () const;

		virtual real64 Evaluate (real64 x) const;

	};

/*****************************************************************************/

#endif
	
/*****************************************************************************/
