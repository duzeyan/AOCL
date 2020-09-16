#include<easyOCL.h>
#include<iostream>
using namespace EOCL;

void testInnerReduction(int size, int blockSize, const char *clCode,int iter)
{
	// init opencl device
	int err = 0;
	vector<pair<string, string>> vKnls;

	unsigned char *pCode = NULL;
	int codeSize;
	ReadOCLCodeFromFile(clCode, &pCode, &codeSize);
	vKnls.push_back(std::make_pair("reductionInner", (char*)pCode));

	OCLEnvWrap env;
	env.OEW_SetDebug(true);
	env.OEW_SetBuildOption("-cl-fast-relaxed-math -cl-mad-enable -cl-denorms-are-zero");
	if (0 != env.OEW_Init(vKnls, make_shared<OCLPlatFormNVIDIA>())){
		printf("Init fail \n");
	}

	// new cpu memory
	int h, w, bw, bh;
	h = w = size;
	bh = bw = blockSize;
	int bNum = w / blockSize;
	int ori_srcStride = w;
	int ori_dstStride = w / blockSize;

	float* pBufferHost0 = new float[w*h];

	for (int i = 0; i < w * h; i++) // fill fake data
	{
		pBufferHost0[i] = 1.0f;
	}

	OCLImageWrap bufferGPU0(env.OEW_GetContext(), env.OEW_GetCommand());
	bufferGPU0.OIW_CreateBuffer_RW(w*h*sizeof(float));
	bufferGPU0.OIW_WriteBuffer(static_cast<void*>(pBufferHost0), w*h*sizeof(float));

	OCLImageWrap bufferGPU1(env.OEW_GetContext(), env.OEW_GetCommand());
	bufferGPU1.OIW_CreateBuffer_RW(bNum*bNum*sizeof(float));
	bool bTurn = true;
	{
		eoTimer timerTotal("testInnerReduction Total", iter);
		eoTimer timerWrite("timerWrite", iter);
		timerTotal.Start();
		// reduction in host 
		for (int k = 0; k < iter; k++)
		{

			cl_kernel kernel = env.OEW_GetKernel("reductionInner");
			size_t curH = h;
			size_t curW = w;
			bNum = w / blockSize;
			bTurn = true;
			int iter = 0;

			
			timerWrite.Start();
			bufferGPU0.OIW_WriteBuffer(static_cast<void*>(pBufferHost0), w*h*sizeof(float));
			timerWrite.End();
			while (bNum > 0)
			{
				//enTimer timer("testInnerReduction", 1);
				cl_mem cl_input, cl_output;
				size_t srcStride, dstStride;
				if (bTurn)
				{
					cl_input = bufferGPU0.OIW_GetData();
					cl_output = bufferGPU1.OIW_GetData();
					srcStride = ori_srcStride;
					dstStride = ori_dstStride;
					bTurn = false;
				}
				else
				{
					cl_output = bufferGPU0.OIW_GetData();
					cl_input = bufferGPU1.OIW_GetData();
					dstStride = ori_srcStride;
					srcStride = ori_dstStride;
					bTurn = true;
				}

				size_t idx = 0;
				err |= clSetKernelArg(kernel, idx++, sizeof(cl_mem), &cl_input);
				err |= clSetKernelArg(kernel, idx++, sizeof(cl_int), &blockSize);
				err |= clSetKernelArg(kernel, idx++, sizeof(cl_int), &blockSize);
				err |= clSetKernelArg(kernel, idx++, sizeof(cl_int), &srcStride);
				err |= clSetKernelArg(kernel, idx++, sizeof(cl_int), &dstStride);
				err |= clSetKernelArg(kernel, idx++, sizeof(cl_mem), &cl_output);
				err |= clSetKernelArg(kernel, idx++, sizeof(float)*blockSize*blockSize, NULL);

				// run
				size_t global_offset_size[] = { 0, 0 };
				size_t global_work_size[] = { curW, curH };
				size_t global_local_size[] = { blockSize, blockSize };

				eoTimer timerRound("round", 1);
				timerRound.Start();
				cl_event event;
				err |= clEnqueueNDRangeKernel(env.OEW_GetCommand(),
					kernel,
					2,
					global_offset_size,
					global_work_size,
					global_local_size,
					0,
					NULL,
					&event
					);

				clWaitForEvents(1, &event);
				timerRound.End();
				timerRound.Print();
				//update 
				iter++;
				LOG_DEBUG_INFO("iter NO.%2d [%3d,%3d]->", iter, curH, curW);
				curH = bNum;
				curW = bNum;
				bNum = curH / blockSize;
				LOG_DEBUG_INFO("[%3d,%3d] \n", curH, curW);
			}
		}
		timerTotal.End();
		auto total = timerTotal.Print();
		auto wtime = timerWrite.Print();
		LOG_WARRING_INFO("Run Time : %lf ms\n", total - wtime);
	}
	clFinish(env.OEW_GetCommand());

	// getData
	float fsum = 0.0f;
	if (bTurn)
	{
		bufferGPU0.OIW_ReadBuffer(&fsum, sizeof(float));
	}
	else
	{
		bufferGPU1.OIW_ReadBuffer(&fsum, sizeof(float));
	}
	LOG_PARAM_INFO("Result %f  \n", fsum);
	delete[] pBufferHost0;
}

void testOutReduction(int size, int blockSize, const char *clCode, int iter)
{
	// init opencl device
	int err = 0;
	vector<pair<string, string>> vKnls;

	unsigned char *pCode = NULL;
	int codeSize;
	ReadOCLCodeFromFile(clCode, &pCode, &codeSize);
	vKnls.push_back(std::make_pair("reductionOut", (char*)pCode));

	OCLEnvWrap env;
	env.OEW_SetDebug(true);
	env.OEW_SetBuildOption("-cl-fast-relaxed-math -cl-mad-enable -cl-denorms-are-zero");
	if (0 != env.OEW_Init(vKnls, make_shared<OCLPlatFormNVIDIA>())){
		printf("Init fail \n");
	}

	// new cpu memory
	int h, w, bw, bh;
	h = w = size;
	bh = bw = blockSize;
	int bNum = w / blockSize;

	float* pBufferHost0 = new float[w*h];

	for (int i = 0; i < w * h; i++) // fill fake data
	{
		pBufferHost0[i] = 1.0f;
	}

	OCLImageWrap bufferGPU0(env.OEW_GetContext(), env.OEW_GetCommand());
	bufferGPU0.OIW_CreateBuffer_RW(w*h*sizeof(float));
	bufferGPU0.OIW_WriteBuffer(static_cast<void*>(pBufferHost0), w*h*sizeof(float));

	OCLImageWrap bufferGPU1(env.OEW_GetContext(), env.OEW_GetCommand());
	bufferGPU1.OIW_CreateBuffer_RW(bNum*bNum*sizeof(float));

	bool bTurn = true;
	// reduction in host 
	eoTimer timerTotal("testOutReduction Total", iter);
	eoTimer timerWrite("timerWrite", iter);
	timerTotal.Start();
	{
		for (int k = 0; k < iter; k++)
		{

			cl_kernel kernel = env.OEW_GetKernel("reductionOut");
			size_t curH = h;
			size_t curW = w;
			bNum = w / blockSize;
			bTurn = true;
			int iter = 0;

			
			timerWrite.Start();
			bufferGPU0.OIW_WriteBuffer(static_cast<void*>(pBufferHost0), w*h*sizeof(float));
			timerWrite.End();

			while (bNum > 0)
			{
				cl_mem cl_input, cl_output;
				size_t srcStride, dstStride;
				if (bTurn)
				{
					cl_input = bufferGPU0.OIW_GetData();
					cl_output = bufferGPU1.OIW_GetData();
					srcStride = w;
					dstStride = w / blockSize;
					bTurn = false;
				}
				else
				{
					cl_output = bufferGPU0.OIW_GetData();
					cl_input = bufferGPU1.OIW_GetData();
					dstStride = w;
					srcStride = w / blockSize;
					bTurn = true;
				}

				size_t idx = 0;
				err |= clSetKernelArg(kernel, idx++, sizeof(cl_mem), &cl_input);
				err |= clSetKernelArg(kernel, idx++, sizeof(cl_int), &blockSize);
				err |= clSetKernelArg(kernel, idx++, sizeof(cl_int), &blockSize);
				err |= clSetKernelArg(kernel, idx++, sizeof(cl_int), &srcStride);
				err |= clSetKernelArg(kernel, idx++, sizeof(cl_int), &dstStride);
				err |= clSetKernelArg(kernel, idx++, sizeof(cl_mem), &cl_output);

				// run
				size_t global_offset_size[] = { 0, 0 };
				size_t global_work_size[] = { bNum, bNum };
				size_t global_local_size[] = { 16, 16 };
				if (global_local_size[0] > (size_t)bNum)
				{
					global_local_size[0] = bNum;
					global_local_size[1] = bNum;
				}

				eoTimer timerRound("round", 1);
				timerRound.Start();
				cl_event event;
				err |= clEnqueueNDRangeKernel(env.OEW_GetCommand(),
					kernel,
					2,
					global_offset_size,
					global_work_size,
					global_local_size,
					0,
					NULL,
					&event
					);

				clWaitForEvents(1, &event);
				timerRound.End();
				timerRound.Print();
				//update 
				iter++;
				LOG_DEBUG_INFO("iter NO.%2d [%3d,%3d]->", iter, curH, curW);
				curH = bNum;
				curW = bNum;
				bNum = curH / blockSize;
				LOG_DEBUG_INFO("[%3d,%3d] \n", curH, curW);

			}
		}
	}
	timerTotal.End();
	auto total = timerTotal.Print();
	auto wtime = timerWrite.Print();
	LOG_WARRING_INFO("Run Time : %lf ms\n", total - wtime);
	clFinish(env.OEW_GetCommand());

	// getData
	float fsum = 0.0f;
	if (bTurn)
	{
		bufferGPU0.OIW_ReadBuffer(&fsum, sizeof(float));
	}
	else
	{
		bufferGPU1.OIW_ReadBuffer(&fsum, sizeof(float));
	}
	delete[] pBufferHost0;
	LOG_PARAM_INFO("Result %f  \n", fsum);
}


void testReduction(int size, int blockSize, const char *clCode,int iter)
{
	testInnerReduction(size,blockSize,clCode, iter);
	testOutReduction(size, blockSize, clCode, iter);
}