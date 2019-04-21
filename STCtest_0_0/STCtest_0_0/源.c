#include "stdio.h"
#include <stdint.h>
#include "new_1.h"
#include"stegoUtil.h"
#include"math.h"
#include <stdlib.h>
int main()
{
	coverConstruct();
	msgConstruct();
	stegoConstruct();
	rhoConstruct();
	execSTC();
	decinital();
	constructMatH();
	getMsg();
	return 0;


}
