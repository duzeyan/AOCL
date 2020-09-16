#ifndef _EO_GLOBAL_DEF_H_
#define _EO_GLOBAL_DEF_H_

#include<CL\opencl.h>
#include<iostream>
using namespace std;

#define LOG_ERR_INFO printf
#define LOG_PARAM_INFO printf
#define LOG_DEBUG_INFO  
#define LOG_WARRING_INFO printf

#define CL_CHECK_RETURN(err) do{ if(0!=err) return err; }while(0)
#define CL_CHECK_RETURN_COND(flag,err) do{ if((flag)||0!=err) return err; }while(0)

typedef struct ImageSizeSt
{
	int n;
	int c;
	int h;
	int w;
}ImageSizeSt;

#endif