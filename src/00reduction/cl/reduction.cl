

__kernel void reductionInner(__global  float *src,
					int blockH, // group height
					int blockW, // group width
					int srcStride,
					int dstStride,
					__global float *dst,
					__local float *aLocalArray /// local mem 
					)
{
	int x_gid = get_global_id(0);
	int y_gid = get_global_id(1);

	int x_lid = get_local_id(0);
	int y_lid = get_local_id(1);

	// load data
	aLocalArray[y_lid*blockH + x_lid] = src[y_gid*srcStride + x_gid];
	barrier(CLK_LOCAL_MEM_FENCE);

	// reduction in x
	int step = blockW >> 1;
	for (; step > 0; step = (step >> 1))
	{
		if (x_lid < step)
			aLocalArray[y_lid*blockW + x_lid] += aLocalArray[y_lid *blockW + x_lid + step];
		barrier(CLK_LOCAL_MEM_FENCE);
	}

	// reduction in y
	step = blockH >> 1;
	for (; step > 0; step = (step >> 1))
	{
		if (y_lid < step && x_lid == 0)
			aLocalArray[y_lid*blockW] += aLocalArray[(y_lid + step) *blockW];
		barrier(CLK_LOCAL_MEM_FENCE);
	}

	// save
	if (x_lid == 0 && y_lid == 0)
	{
		int x_bid = x_gid / get_local_size(0);
		int y_bid = y_gid / get_local_size(1);
		
		dst[y_bid*dstStride + x_bid] = aLocalArray[0];
	}
	return;
}


__kernel void reductionOut(__global  float *src,
	int blockH,
	int blockW,
	int srcStride,
	int dstStride,
	__global float *dst
	)
{
	int x_gid = get_global_id(0);
	int y_gid = get_global_id(1);
	int x_start = x_gid*blockW;
	int y_start = y_gid*blockH;
	int x_end = x_start + blockW;
	int y_end = y_start + blockH;
	float fsum = 0.0f;
	for (; y_start < y_end; y_start++)
	{
		__global  float *srcCur = src + y_start*srcStride;
		for (int x = x_start; x < x_end; x++)
		{
			fsum += srcCur[x];
		}
	}
	dst[y_gid*dstStride + x_gid] = fsum;
	return;
}