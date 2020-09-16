#include<DemoReduction.h>

#define PROJECT_ROOT "../../"

void testReduction()
{
	//testReduction(4096, 4, PROJECT_ROOT "src/00reduction/cl/reduction.cl",20);
	testReduction(4096, 8, PROJECT_ROOT "src/00reduction/cl/reduction.cl",1);
	//testReduction(4096, 16, PROJECT_ROOT "src/00reduction/cl/reduction.cl",1);
}

int main(int argc, char *argv[])
{
	testReduction();
	return 0;
}