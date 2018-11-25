#pragma once

#ifndef GV_NUMBERICAL_H
#define GV_NUMBERICAL_H

// numerical collections.

namespace GV_Numerical
{
namespace LinearProblemSolution
{

template < INT n, INT m >
BOOL GaussJordan(FLOAT (&a)[n][n], FLOAT (&b)[n][m])
// Linear equation solution by Gauss-Jordan elimination, equation (2.1.1) above.
// a[0..n-1][0..n-1]
// is the input matrix. b[0..n-1][0..m-1] is input containing the m right-hand
// side vectors. On
// output, a is replaced by its matrix inverse, and b is replaced by the
// corresponding set of solution
// vectors.
{
	// The integer arrays ipiv, indxr, and indxc are used for bookkeeping on the
	// pivoting.
	INT indxc[n];
	INT indxr[n];
	INT ipiv[n];

	INT i, icol, irow, j, k, l, ll;
	FLOAT big, dum, pivinv;

	for (j = 0; j < n; j++)
		ipiv[j] = -1;
	for (i = 0; i < n; i++)
	{
		// This is the main loop over the columns to bereduced.
		big = 0.0;
		for (j = 0; j < n;
			 j++) // This is the outer loop of the search for a pivot	element.
			if (ipiv[j] != 0)
				for (k = 0; k < n; k++)
				{
					if (ipiv[k] == -1)
					{
						if (fabs(a[j][k]) >= big)
						{
							big = fabs(a[j][k]);
							irow = j;
							icol = k;
						}
					}
				}
		++(ipiv[icol]);
		// We now have the pivot element, so we interchange rows, if needed, to put
		// the pivot
		//	element on the diagonal. The columns are not physically interchanged,
		//only relabeled:
		// indxc[i], the column of the ith pivot element, is the ith column that is
		// reduced, while
		//	indxr[i] is the row in which that pivot element was originally located.
		//If indxr[i] =
		//	indxc[i] there is an implied column interchange. With this form of
		//bookkeeping, the
		//	solution b¡¯s will end up in the correct order, and the inverse matrix
		//will be scrambled
		//	by columns.
		if (irow != icol)
		{
			for (l = 0; l < n; l++)
				TSwap(a[irow][l], a[icol][l]);
			for (l = 0; l < m; l++)
				TSwap(b[irow][l], b[icol][l]);
		}
		indxr[i] = irow;
		// We are now ready to divide the pivot row by the
		// pivot element, located at irow and icol.
		indxc[i] = icol;
		if (a[icol][icol] == 0.0)
		{
			ERROR_OUT("gaussj: Singular Matrix");
			return FALSE;
		}
		pivinv = 1.0 / a[icol][icol];
		a[icol][icol] = 1.0;
		for (l = 0; l < n; l++)
			a[icol][l] *= pivinv;
		for (l = 0; l < m; l++)
			b[icol][l] *= pivinv;
		for (ll = 0; ll < n; ll++) // Next, we reduce the rows...
			if (ll != icol)
			{ //...except for the pivot one, of course.
				dum = a[ll][icol];
				a[ll][icol] = 0.0;
				for (l = 0; l < n; l++)
					a[ll][l] -= a[icol][l] * dum;
				for (l = 0; l < m; l++)
					b[ll][l] -= b[icol][l] * dum;
			}
	}
	// This is the end of the main loop over columns of the reduction. It only
	// remains to unscramble
	// the solution in view of the column interchanges. We do this by
	// interchanging pairs of
	// columns in the reverse order that the permutation was built up.
	for (l = n - 1; l >= 0; l--)
	{
		if (indxr[l] != indxc[l])
			for (k = 0; k < n; k++)
				TSwap(a[k][indxr[l]], a[k][indxc[l]]);
	} // And we are done.
	return TRUE;
}

const FLOAT TINY = 1.0e-20f; // A small number.

template < INT n >
Ludcmp(FLOAT (&a)[n][n], INT (&indx)[n], FLOAT& d)
// Given a matrix a[0..n-1][0..n-1], this routine replaces it by the LU
// decomposition of a rowwise
//	permutation of itself. a and n are input. a is output, arranged as in
//equation (2.3.14) above;
// indx[0..n-1] is an output vector that records the row permutation effected by
// the partial
//	pivoting; d is output as ¡À1 depending on whether the number of row
//interchanges was even
// or odd, respectively. This routine is used in combination with lubksb to
// solve linear equations
//	or invert a matrix.
{
	INT i, imax, j, k;
	FLOAT big, dum, sum, temp;
	FLOAT vv[n]; // vv stores the implicit scaling of each row.
	d = 1.0;	 // No row interchanges yet.
	for (i = 0; i < n;
		 i++)
	{ // Loop over rows to get the implicit scaling information.
		big = 0.0;
		for (j = 0; j < n; j++)
			if ((temp = fabs(a[i][j])) > big)
				big = temp;
		if (big == 0.0)
		{
			ERROR_OUT("Singular matrix in routine ludcmp");
			return FALSE;
		}
		// No nonzero largest element.
		vv[i] = 1.0f / big; // Save the scaling.
	}
	for (j = 0; j < n; j++)
	{ // This is the loop over columns of Crout¡¯s method.
		for (i = 0; i < n; i++)
		{ // This is equation (2.3.12) except for i = j.
			sum = a[i][j];
			for (k = 0; k < i; k++)
				sum -= a[i][k] * a[k][j];
			a[i][j] = sum;
		}
		big = 0.0; // Initialize for the search for largest pivot element.
		for (i = j; i < n; i++)
		{
			// This is i = j of equation (2.3.12) and i = j+1. . .N		of
			// equation (2.3.13).
			sum = a[i][j];
			for (k = 0; k < j; k++)
				sum -= a[i][k] * a[k][j];
			a[i][j] = sum;
			if ((dum = vv[i] * fabs(sum)) >= big)
			{
				// Is the figure of merit for the pivot better than the best so far?
				big = dum;
				imax = i;
			}
		}
		if (j != imax)
		{ // Do we need to interchange rows?
			for (k = 0; k < n; k++)
			{ // Yes, do so...
				dum = a[imax][k];
				a[imax][k] = a[j][k];
				a[j][k] = dum;
			}
			d = -d;			  //...and change the parity of d.
			vv[imax] = vv[j]; // Also interchange the scale factor.
		}
		indx[j] = imax;
		if (a[j][j] == 0.0)
			a[j][j] = TINY;
		// If the pivot element is zero the matrix is singular (at least to the
		// precision of the
		//	algorithm). For some applications on singular matrices, it is desirable
		//to substitute
		//	TINY for zero.
		if (j != n - 1)
		{ // Now, finally, divide by the pivot element.
			dum = 1.0 / (a[j][j]);
			for (i = j + 1; i < n; i++)
				a[i][j] *= dum;
		}
	} // Go back for the next column in the reduction.
	return TRUE;
}

// Here is the routine for forward substitution and backsubstitution,
// implementing
// equations (2.3.6) and (2.3.7).
template < INT n >
BOOL Lubksb(FLOAT (&a)[n][n], INT (&indx)[n], FLOAT (&b)[n])
//	Solves the set of n linear equations A¡¤X = B. Here a[1..n][1..n] is
//input, not as the matrix
//	A but rather as its LU decomposition, determined by the routine ludcmp.
//indx[1..n] is input
//	as the permutation vector returned by ludcmp. b[1..n] is input as the
//right-hand side vector
//	B, and returns with the solution vector X. a, n, and indx are not
//modified by this routine
//	and can be left in place for successive calls with different right-hand
//sides b. This routine takes
//	into account the possibility that b will begin with many zero elements,
//so it is efficient for use
//	in matrix inversion.
{
	INT i, ii = -1, ip, j;
	FLOAT sum;
	for (i = 0; i < n; i++)
	{
		// When ii is set to a positive value, it will become the
		// index of the first nonvanishing element of b. Wenow
		// do the forward substitution, equation (2.3.6). The
		// only new wrinkle is to unscramble the permutation
		// as we go.
		ip = indx[i];
		sum = b[ip];
		b[ip] = b[i];
		if (ii != -1)
			for (j = ii; j <= i - 1; j++)
				sum -= a[i][j] * b[j];
		else if (sum)
			ii = i;
		// A nonzero element was encountered, so from now on we
		// will have to do the sums in the loop above. b[i]=sum;
	}
	for (i = n - 1; i >= 0; i--)
	{
		// Now we do the backsubstitution, equation (2.3.7).
		sum = b[i];
		for (j = i + 1; j < n; j++)
			sum -= a[i][j] * b[j];
		b[i] = sum / a[i][i];
		// Store a component of the solution vector X.
	} // All done!
	return TRUE;
}

} // endof namespace linear problem.
};

#endif