#include"eoDataDef.h"
#include"eoEnv.h"
using namespace EOCL;

int OCLImageWrap::OIW_CreateBuffer_flag(size_t size, size_t flag)
{
	int err = 0;
	if (nullptr != _data)
	{
		clReleaseMemObject(_data);
		_data = nullptr;
	}
	_data = clCreateBuffer(_context,
							flag,
							size,
							NULL,
							&err);
	if (CL_SUCCESS != err)
		_data = nullptr;
	return err;
}

int OCLImageWrap::OIW_WriteBuffer(void *pSrc, size_t size)
{
	int err = CL_SUCCESS;
	unsigned char *pfTmp = (unsigned char*)clEnqueueMapBuffer(_clCmdQue,
		_data,
		CL_TRUE,
		CL_MAP_WRITE,
		0,
		size,
		0,
		NULL,
		NULL,
		&err);
	CL_CHECK_RETURN(err);
	memcpy(pfTmp, pSrc, size);
	clEnqueueUnmapMemObject(_clCmdQue, _data, pfTmp, 0, NULL, NULL);
	return err;
}

int OCLImageWrap::OIW_ReadBuffer(void *pSrc, size_t size)
{
	int err = CL_SUCCESS;
	unsigned char *pfTmp = (unsigned char*)clEnqueueMapBuffer(_clCmdQue,
		_data,
		CL_TRUE,
		CL_MAP_WRITE,
		0,
		size,
		0,
		NULL,
		NULL,
		&err);
	CL_CHECK_RETURN(err);
	memcpy(pSrc, pfTmp, size);
	clEnqueueUnmapMemObject(_clCmdQue, _data, pfTmp, 0, NULL, NULL);
	return err;
}

