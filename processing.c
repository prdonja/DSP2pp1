#include "processing.h"
#include "iir.h"

void calculateShelvingCoeff(float c_alpha, Int16* output)
{
	Int16 k1, k2;
	k1 = c_alpha * 32768;
	k2  = -32768*(c_alpha);

	if(output[0] > 32768) {
		output[0] = 32768;
	}
	else {
		output[0] = k1;
	}

	output[1] = -32768;
	output[2] = 32797;

	if(output[3]  < -32768){
		output[3] = -32768;
	}
	else {
		output[3] = k2;
	}
	/* Your code here */
}

void calculatePeekCoeff(float c_alpha, float c_beta, Int16* output)
{

	output[0] = c_alpha * 32768;
	output[1] =	(-c_beta * (32768 + c_alpha))/2;
	output[2] = 32768;
	output[3] = 32768;
	output[4] = (-c_beta * (32768 + c_alpha))/2;
	output[5] = c_alpha * 32768;


	/* Your code here */
}

Int16 shelvingHP(Int16 input, Int16* coeff, Int16* x_history, Int16* y_history, Int16 k)
{
	/* Your code here */
	return 0;
}

Int16 shelvingLP(Int16 input, Int16* coeff, Int16* x_history, Int16* y_history, Int16 k)
{
	/* Your code here */
	return 0;
}

Int16 shelvingPeek(Int16 input, Int16* coeff, Int16* x_history, Int16* y_history, Int16 k)
{
	/* Your code here */
	return 0;
}
