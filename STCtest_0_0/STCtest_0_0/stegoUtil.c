#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stegoUtil.h"
//#include "global.h"

long msgFileSize;
char msgFileExtName[8];
char *msgBuf = NULL;
FILE *msgFile = NULL;
long numParsed = 0;
int done = 0;	/* whether the process of extraction has finished */

typedef struct  deccodeargs
{
	int**H;
	int heightOfH;
	int widthOfH;
	int widthOfH_hat;
	int heightOfH_hat;
	int *msg;
	int stegoLen;
	int msgLen;
	int *stego;

}PMECodecArgs;

PMECodecArgs pmeArgs;

int pmeH_hat[2][2] = {{1,0},{1,1}}; 

//void pmpDecInit(VideoParameters *v)
//{
//	int mb_xy = v->FrameSizeInMbs;
//	
//	pmpArgs.p8x8IndicatorList = (int *)malloc(sizeof(int) * mb_xy);
//	pmpArgs.p8x8BitMap = (int *)malloc(sizeof(int) * mb_xy);
//
//	pmpArgs.heightOfH_hat = 7;
//	pmpArgs.widthOfH_hat = 2;
//
//	pmpArgs.dataBitDecCount = 0;
//
//	memset(&msgFileSize, 0, sizeof(long));
//	memset(msgFileExtName, 0, sizeof(char) * 8);
//}
void decinital()//初始化一些参数
{
	pmeArgs.stegoLen = 8;
	pmeArgs.widthOfH_hat = 2;
	pmeArgs.heightOfH_hat = 2;
	pmeArgs.msgLen = pmeArgs.stegoLen / pmeArgs.widthOfH_hat;
	pmeArgs.heightOfH = pmeArgs.msgLen;
	pmeArgs.widthOfH = pmeArgs.stegoLen;

	pmeArgs.msg = (int*)malloc(sizeof(int) * (pmeArgs.msgLen + 1));

	pmeArgs.stego = (int*)malloc(sizeof(int) * (pmeArgs.stegoLen + 1));
	pmeArgs.stego[1] = 0;
	pmeArgs.stego[2] = 0;
	pmeArgs.stego[3] = 1;
	pmeArgs.stego[4] = 1;
	pmeArgs.stego[5] = 1;
	pmeArgs.stego[6] = 0;
	pmeArgs.stego[7] = 0;
	pmeArgs.stego[8] = 1;

}


void constructMatH()
{
	int col, row, colH_hat;

	//if(stegoAlg == PMP)
	//{
	//	pmpArgs.heightOfH = pmpArgs.stegoLen >> 1;
	//	pmpArgs.widthOfH = pmpArgs.stegoLen;

	//	pmpArgs.H = (int **)malloc(sizeof(int *) * pmpArgs.heightOfH);
	//	for(row = 0; row < pmpArgs.heightOfH; row ++)
	//	{
	//		pmpArgs.H[row] = (int *)malloc(sizeof(int) * pmpArgs.widthOfH);
	//		memset(pmpArgs.H[row], 0, sizeof(int) * pmpArgs.widthOfH);
	//	}

	//	for(col = 0; col < pmpArgs.widthOfH; col ++)
	//	{
	//		int rowShift = col / pmpArgs.widthOfH_hat;
	//		int row2Copy = (pmpArgs.heightOfH_hat + rowShift) > pmpArgs.heightOfH ? (pmpArgs.heightOfH - rowShift) : pmpArgs.heightOfH_hat;

	//		colH_hat = col % pmpArgs.widthOfH_hat;
	//		for(row = rowShift; row < rowShift + row2Copy; row ++)
	//		{
	//			pmpArgs.H[row][col] = pmpH_hat[row - rowShift][colH_hat];
	//		}
	//	}
	//}
	//else if(stegoAlg == PME)
	//{


		pmeArgs.H = (int **)malloc(sizeof(int *) * pmeArgs.heightOfH);
		for(row = 0; row < pmeArgs.heightOfH; row ++)
		{
			pmeArgs.H[row] = (int *)malloc(sizeof(int) * pmeArgs.widthOfH);
			memset(pmeArgs.H[row], 0, sizeof(int) * pmeArgs.widthOfH);//初始值为0
		}

		for(col = 0; col < pmeArgs.widthOfH; col ++)
		{
			int rowShift = col / pmeArgs.widthOfH_hat;
			int row2Copy = (pmeArgs.heightOfH_hat + rowShift) > pmeArgs.heightOfH ? (pmeArgs.heightOfH - rowShift) : pmeArgs.heightOfH_hat;

			colH_hat = col % pmeArgs.widthOfH_hat;
			for(row = rowShift; row < rowShift + row2Copy; row ++)
			{
				pmeArgs.H[row][col] = pmeH_hat[row - rowShift][colH_hat];
			}
		}
	//}

	//for(row = 0; row < pmpArgs.heightOfH; row ++)
	//{
	//	for(col = 0; col < pmpArgs.widthOfH; col ++)
	//	{
	//		printf("%ld ", pmpArgs.H[row][col]);
	//	}
	//	printf("\n");
	//}
}

void getMsg()
{
	int i, j; 

	//if(stegoAlg == PMP)
	//{
	//	memset(pmpArgs.msg, 0, sizeof(int) * (pmpArgs.msgLen + 1));

	//	for(i = 1; i <= pmpArgs.msgLen; i ++)
	//	{
	//		for(j = 1; j <= pmpArgs.widthOfH; j ++)
	//		{
	//			pmpArgs.msg[i] += pmpArgs.H[i - 1][j - 1] * pmpArgs.stego[j];
	//		}
	//		pmpArgs.msg[i] &= 1;
	//	}
	//}
	//else if(stegoAlg == PME)
	//{
		memset(pmeArgs.msg, 0, sizeof(int) * (pmeArgs.msgLen + 1));

		for(i = 1; i <= pmeArgs.msgLen; i ++)
		{
			for(j = 1; j <= pmeArgs.widthOfH; j ++)
			{
				pmeArgs.msg[i] += pmeArgs.H[i - 1][j - 1] * pmeArgs.stego[j];
			}
			pmeArgs.msg[i] &= 1;
		}
	//}
}

//void pmeDecInit(VideoParameters *v)
//{
//	pmeArgs.b16x16MvSumLSB = (int*)malloc(sizeof(int) * v->FrameSizeInMbs);
//	pmeArgs.stegoLen = 0;
//	pmeArgs.dataBitDecCount = 0;
//	pmeArgs.heightOfH_hat = 7;
//	pmeArgs.widthOfH_hat = 2;
//	memset(&msgFileSize, 0, sizeof(long));
//	memset(msgFileExtName, 0, sizeof(char) * 8);
//}

//void pmeDecExec(Slice *currSlice)
//{
//	int msgBitInd = 1, msgBitInd2 = 1;
//
//	constructMatH();
//	getMsg();
//	
//	if(pmeArgs.dataBitDecCount < 8*8)	/* store data decoded into "char msgFileExtName[8]" */
//	{
//		for(msgBitInd = 1; msgBitInd <= pmeArgs.msgLen; msgBitInd ++)
//		{
//			msgFileExtName[pmeArgs.dataBitDecCount / 8] |= pmeArgs.msg[msgBitInd] << (7 - pmeArgs.dataBitDecCount % 8);
//			if(++ pmeArgs.dataBitDecCount>= 8*8)
//			{
//				msgBitInd ++;
//				break;
//			}
//		}
//	}
//
//	if(pmeArgs.dataBitDecCount >= 8*8 && pmeArgs.dataBitDecCount < 16*8) /* store data decoded into "long msgFileSize" */
//	{
//		for(msgBitInd2 = msgBitInd; msgBitInd2 <= pmeArgs.msgLen; msgBitInd2 ++)
//		{
//			((char*)&msgFileSize)[pmeArgs.dataBitDecCount / 8 - 8] |= pmeArgs.msg[msgBitInd2] << (7 - pmeArgs.dataBitDecCount % 8);
//			if(++ pmeArgs.dataBitDecCount >= 16*8)
//			{
//				msgBitInd2 ++;
//				break;
//			}
//		}
//	}
//
//	//if(pmeArgs.dataBitDecCount >= 16*8)	/* store data decoded into "msgBuf" */
//	//{
//	//	int i;
//	//	//FILE *fp = fopen("D:\\rec.txt", "a");
//
//	//	if(msgBuf == NULL)
//	//	{
//	//		msgBuf = (char *)malloc(sizeof(char) * msgFileSize);
//	//		memset(msgBuf, 0, sizeof(char) * msgFileSize);
//	//	}
//
//	//	for(i = msgBitInd2; i <= pmeArgs.msgLen; i ++)
//	//	{
//	//		//fprintf(fp, "%ld", pmpArgs.msg[i]);
//
//	//		msgBuf[pmeArgs.dataBitDecCount / 8 - 16] |= pmeArgs.msg[i] << (7 - pmeArgs.dataBitDecCount % 8);
//	//		if(++ pmeArgs.dataBitDecCount >= 16 * 8 + msgFileSize * 8)
//	//		{
//	//			done = 1;
//	//			break;
//	//		}
//	//	}
//	//}
//}

//void pmeDecPost()
//{
//	int row;
//	for(row = 0; row < pmeArgs.heightOfH; row ++)
//	{
//		free((void*)pmeArgs.H[row]);
//	}
//	free((void*)pmeArgs.H);
//	free((void*)pmeArgs.msg);
//	free((void*)pmeArgs.stego);
//}