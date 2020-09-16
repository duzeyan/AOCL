#include"eoEnv.h"
using namespace EOCL;



OCLEnvWrap::~OCLEnvWrap()
{
}

int OCLEnvWrap::_OEM_AddKernelCode(const string knlName, const string code)
{
	OCLProgram program;
	program._clKernelCode = code;
	program._knlName = knlName;
	program._kernel = nullptr;
	_OCLPrograms.push_back(program);
	return CL_SUCCESS;
}

int OCLEnvWrap::OEW_Init(const vector<pair<string, string>> &vKnls, shared_ptr<OCLPaltformBase> targetPlatfrom)
{
	int err = CL_SUCCESS;
	_targetPlatfrom = targetPlatfrom;

	for (auto &knl : vKnls)
	{
		_OEM_AddKernelCode(knl.first, knl.second);
	}

	err = _OEM_BuildOCL_Platform();
	CL_CHECK_RETURN(err);

	err = _OEW_BuildOCL_Device();
	CL_CHECK_RETURN(err);

	err = _OEW_BuildOCL_Context();
	CL_CHECK_RETURN(err);

	err = _OEW_BuildOCL_Program();
	CL_CHECK_RETURN(err);

	return err;
}


int OCLEnvWrap::_OEM_BuildOCL_Platform()
{
	int err = CL_SUCCESS;
	cl_uint num_of_platforms = 0;

	err = clGetPlatformIDs(0, NULL, &num_of_platforms);
	CL_CHECK_RETURN_COND(num_of_platforms < 1, err);
	cl_platform_id* platforms = (cl_platform_id*)malloc(num_of_platforms* sizeof(cl_platform_id));

	err = clGetPlatformIDs(num_of_platforms, platforms, NULL);
	CL_CHECK_RETURN(err);

	size_t i, targetIdx = num_of_platforms;
	for (i = 0; i < num_of_platforms; i++)
	{
		char pfrom_vendor[1024*10];
		clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(pfrom_vendor), &pfrom_vendor, NULL);
		
		string sTmp(pfrom_vendor);
		const char * pStr = _targetPlatfrom->getKeyWord();
		if (string::npos != sTmp.find(pStr))
		{
			_clPlatformID = platforms[i];
			targetIdx = i;
		}
		if (_debug) LOG_DEBUG_INFO("platform Name %d [%s] \n", i, pfrom_vendor);
	}
	free(platforms);
	if (targetIdx >= num_of_platforms)
		CL_CHECK_RETURN(CL_INVALID_PLATFORM);
	return err;
}

int OCLEnvWrap::_OEW_BuildOCL_Device()
{
	int err = CL_SUCCESS;

	cl_uint num_of_devices = 0;

	err = clGetDeviceIDs(_clPlatformID, CL_DEVICE_TYPE_GPU, 0, NULL, &num_of_devices);
	CL_CHECK_RETURN_COND(num_of_devices < 1, err);

	err = clGetDeviceIDs(_clPlatformID, CL_DEVICE_TYPE_GPU, 1, &_clDeviceID, NULL);
	CL_CHECK_RETURN(err);
	return err;
}

int OCLEnvWrap::_OEW_BuildOCL_Context()
{
	int err = CL_SUCCESS;
	_clContext = clCreateContext(NULL, 1, &_clDeviceID, NULL, NULL, &err);
	CL_CHECK_RETURN(err);

	_clCmdQue = clCreateCommandQueue(_clContext, _clDeviceID, CL_QUEUE_PROFILING_ENABLE, &err);
	CL_CHECK_RETURN(err);
	return err;
}

int OCLEnvWrap::_OEW_PrintOCLErrLog(cl_program program)
{
	//Get log.
	int err = 0;
	size_t log_length = 0;
	err = clGetProgramBuildInfo(program, _clDeviceID, CL_PROGRAM_BUILD_LOG,
		0, 0, &log_length);
	CL_CHECK_RETURN(err);

	char *log = (char *)malloc(log_length * sizeof(char));

	err = clGetProgramBuildInfo(program, _clDeviceID, CL_PROGRAM_BUILD_LOG,
		log_length, log, NULL);
	CL_CHECK_RETURN(err);

	FILE *stream = fopen("undistor_build_error.log", "wb");
	LOG_PARAM_INFO("%s \n", log);
	fwrite(log, sizeof(char), log_length, stream);
	fclose(stream);
	free(log);
	return err;
}

int OCLEnvWrap::_OEW_BuildOCL_Program()
{
	int err = CL_SUCCESS;
	if (_OCLPrograms.empty())
	{
		LOG_WARRING_INFO("program instance is null \n");
	}

	for (size_t i = 0; i < _OCLPrograms.size(); i++)
	{
		cl_program program = _OCLPrograms[i]._clProgram;
		const  char * kernel_site = _OCLPrograms[i]._clKernelCode.c_str();
		program = clCreateProgramWithSource(_clContext, 1, (const char **)(&kernel_site), NULL, &err);
		CL_CHECK_RETURN(err);

		{
			err = clBuildProgram(program, 1, &_clDeviceID, _buildFLAG.c_str(), NULL, NULL);
			if (err == CL_BUILD_PROGRAM_FAILURE)
			{
				_OEW_PrintOCLErrLog(program);
				CL_CHECK_RETURN(err);
				return CL_BUILD_PROGRAM_FAILURE;
			}
			_OCLPrograms[i]._kernel = clCreateKernel(program, _OCLPrograms[i]._knlName.c_str(), &err);
			CL_CHECK_RETURN(err);
		}
	}
	return err;
}





