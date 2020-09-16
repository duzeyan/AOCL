#ifndef _OCL_DATA_UTIL_H_
#define _OCL_DATA_UTIL_H_


#include"eoGlobal_Def.h"

namespace EOCL{

	class OCLImageWrap{
	public:
		OCLImageWrap(cl_context context, cl_command_queue clCmdQue) :_context(context), _clCmdQue(clCmdQue)
		{ _data = nullptr; }

		inline int OIW_CreateBuffer_RW(size_t size)
		{
			return OIW_CreateBuffer_flag(size, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR);
		}

		inline int OIW_CreateBuffer_R(size_t size)
		{
			return OIW_CreateBuffer_flag(size, CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR);
		}

		inline int OIW_CreateBuffer_W(size_t size)
		{
			return OIW_CreateBuffer_flag(size, CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR);
		}

		int OIW_CreateBuffer_flag(size_t size, size_t flag);

		int OIW_ReadBuffer(void *pDst, size_t size);

		int OIW_WriteBuffer(void *pSrc, size_t size);

		inline cl_mem OIW_GetData() { return _data; }
	private:
		cl_context _context;
		cl_command_queue  _clCmdQue;
		cl_mem _data;
	};

}

#endif