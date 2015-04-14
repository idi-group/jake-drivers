
#include "math.h"
#include "mex.h"   

#include "jake_driver.h"

static jake_device* dev = NULL;

int connect(int port) {
	if((dev = jake_init_device(port)) != NULL)
		return 1;
	return 0;
}

void close() {
	jake_free_device(dev);
	dev = NULL;
}

int write(int addr, int val) {
	if(jake_write(dev, addr, val) == jake_SUCCESS)
		return 1;
	return 0;
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if(nrhs < 1) {
		mexPrintf("Error, at least 1 parameter required!\n");
		return;
	}

    int inp_m = mxGetM(prhs[0]);
    int inp_n = mxGetN(prhs[0]);

	int opcode = (int)(mxGetScalar(prhs[0]));

	switch(opcode) {
		case 0: // connect
			{
				if(nrhs != 2) {
					mexPrintf("Error, expecting 2 parameters\n");
					return;
				}
				int port = (int)(mxGetScalar(prhs[1]));
				mexPrintf("Connecting on port %d\n", port);
				int success = connect(port);
				plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
				double* output = mxGetPr(plhs[0]);
				output[0] = success;
			}
			return;
		case 1: // acc/mag/heading
			{
				if(nrhs != 1) {
					mexPrintf("Error, too many parameters\n");
					return;
				}
				int axyz[3];
				int mxyz[3];
				jake_acc(dev, axyz);
				jake_mag(dev, mxyz);
				int heading = jake_heading(dev);
				int seq = jake_data_timestamp(dev);
				plhs[0] = mxCreateDoubleMatrix(1, 8, mxREAL);
				double* output = mxGetPr(plhs[0]);
				for(int i=0;i<3;i++)
					output[i] = axyz[i];
				for(int i=0;i<3;i++)
					output[3+i] = mxyz[i];
				output[6] = heading;
				output[7] = seq;
			}
			return;
		case 4: // write
			{
				if(nrhs != 3) {
					mexPrintf("Error, expecting 3 parameters\n");
					return;
				}
				int addr = (int)mxGetScalar(prhs[1]);
				int val = (int)mxGetScalar(prhs[2]);
				int success = write(addr, val);
				plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
				double* output = mxGetPr(plhs[0]);
				output[0] = success;
			}
			return;
		case 5: // close
			close();
			break;
		default:
			mexPrintf("Error, Unknown opcode\n");
			return;
	}
    return;
}
