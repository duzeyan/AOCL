#ifndef _OCL_UTIL_H_
#define _OCL_UTIL_H_

#include"eoGlobal_Def.h"
#include<unordered_map>
#include"eoPlatformRegister.h"
#include<memory>
#include<algorithm>

typedef pair<string, cl_kernel> MapKnlName2Knl;

namespace EOCL{

class OCLProgram{
public:
	cl_program	_clProgram;
	string _knlName;
	cl_kernel _kernel;
	string _clKernelCode;
};


class OCLEnvWrap{
public:
	int OEW_Init(const vector<pair<string, string>> &knls, shared_ptr<OCLPaltformBase> targetPlatfrom);

	void OEW_Build();

	inline void OEW_SetDebug(bool debug) { _debug = debug; }

	inline void OEW_SetBuildOption(string str) { _buildFLAG = str; }

	inline cl_context OEW_GetContext() { return _clContext; }

	inline cl_command_queue OEW_GetCommand() { return _clCmdQue; }

	inline cl_kernel OEW_GetKernel(string target) 
	{
		auto it = std::find_if(_OCLPrograms.begin(), _OCLPrograms.end(), [&target](const OCLProgram &ele){ return target == ele._knlName; });
		if (_OCLPrograms.end() != it)
			return it->_kernel; 
		return nullptr;
	}

	~OCLEnvWrap();
private:
	int _OEM_BuildOCL_Platform();

	int _OEW_BuildOCL_Device();

	int _OEW_BuildOCL_Context();

	int _OEW_BuildOCL_Program();

	int _OEW_PrintOCLErrLog(cl_program program);

	int _OEM_AddKernelCode(const string knlName, const string code);
private:
	cl_platform_id    _clPlatformID;
	cl_device_id      _clDeviceID;
	cl_context        _clContext;
	cl_command_queue  _clCmdQue;
	vector<OCLProgram>	_OCLPrograms;
	string _buildFLAG;
	bool _debug;
	shared_ptr<OCLPaltformBase> _targetPlatfrom;
};



};

#endif