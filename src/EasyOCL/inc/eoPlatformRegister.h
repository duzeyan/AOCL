#ifndef _OCL_WRAP_HELPER_H_
#define _OCL_WRAP_HELPER_H_

#define DECL_PLATFORM_KEY_WORD(platform,keyword) \
class OCLPlatForm##platform : public OCLPaltformBase{ \
public: \
const char *getKeyWord() override { return #keyword; } \
};

namespace EOCL{

	//
	class OCLPaltformBase{
	public:
		virtual const char *getKeyWord() = 0;
	};

	DECL_PLATFORM_KEY_WORD(NVIDIA, NVIDIA);
	DECL_PLATFORM_KEY_WORD(INTEL, Intel);

	//
	int ReadOCLCodeFromFile(const char * path, unsigned char **raw_data, int* pSize);
}

#endif
