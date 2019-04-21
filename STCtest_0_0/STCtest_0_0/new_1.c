#include "stdio.h"
#include <stdint.h>
#include "new_1.h"
#include"math.h"
#include <stdlib.h>

typedef signed  long long int64_t;
#define   INIFINITY 2147483647;
typedef struct stcparams{
	int *cover;
	int *message;
	int *stego;
	int coverLen;
	int numOfH_hat;
	int alpha;
	int *H_hat;
	int heightOfH_hat;
	int widthOfH_hat;
	int64_t *rho;
	int64_t *embedding_cost;
}STCParams;

/*define the struct of stego parameters*/
typedef struct paramsba{
	STCParams stcParams;
}STEGOParams;

STEGOParams stegoParams;

void execSTC()
{
	
	trellis_code(stegoParams.stcParams.cover, stegoParams.stcParams.coverLen, stegoParams.stcParams.message,
		stegoParams.stcParams.H_hat, stegoParams.stcParams.heightOfH_hat, stegoParams.stcParams.widthOfH_hat,
		stegoParams.stcParams.numOfH_hat, stegoParams.stcParams.rho, stegoParams.stcParams.stego, &(stegoParams.stcParams.embedding_cost));
}

void coverConstruct()
{
	stegoParams.stcParams.alpha = 2;
	stegoParams.stcParams.coverLen = 8;

	//coverLen:载体长度；
	//alpha:1/负载率,eg:alpha = 2->payload = 1/2;
	//H_hat:小矩阵
	//numOfH_hat:小矩阵的个数
	//rho:每个点的代价
	//heightOfH_hat:H的高度
	//widthOfH_hat:小矩阵的宽度
	stegoParams.stcParams.coverLen = stegoParams.stcParams.coverLen / stegoParams.stcParams.alpha * stegoParams.stcParams.alpha;
	stegoParams.stcParams.cover = (int*)malloc(sizeof(int) * (stegoParams.stcParams.coverLen + 1));
	//for (int i = 1; i <= stegoParams.stcParams.coverLen; i++)
	//{
	//	stegoParams.stcParams.cover[i] = (stegoParams.mbParamsList[i - 1].mv[0] + stegoParams.mbParamsList[i - 1].mv[1]) & 1;
	//}
//	stegoParams.stcParams.cover[0] = 1;
	stegoParams.stcParams.cover[1] = 1;
	stegoParams.stcParams.cover[2] = 0;
	stegoParams.stcParams.cover[3] = 1;
	stegoParams.stcParams.cover[4] = 1;
	stegoParams.stcParams.cover[5] = 0;
	stegoParams.stcParams.cover[6] = 0;
	stegoParams.stcParams.cover[7] = 0;
	stegoParams.stcParams.cover[8] = 1;

	stegoParams.stcParams.H_hat = (int *)malloc(sizeof(int) * (2 + 1));
	stegoParams.stcParams.H_hat[1] = 3;
	stegoParams.stcParams.H_hat[2] = 2;
	stegoParams.stcParams.heightOfH_hat = 2;
	stegoParams.stcParams.widthOfH_hat = 2;
}

void msgConstruct()
{
	stegoParams.stcParams.numOfH_hat = stegoParams.stcParams.coverLen / stegoParams.stcParams.alpha;
	stegoParams.stcParams.message = (int *)malloc(sizeof(int) * (stegoParams.stcParams.numOfH_hat + 1));
	//for(int i = 1; i <= stegoParams.stcParams.numOfH_hat; i ++)
	//{
	//	stegoParams.stcParams.message[i] = rand() % 2;
	//	//printf("%d ", stegoParams.stcParams.message[i]);
	//}
	stegoParams.stcParams.message[1] = 0;
	stegoParams.stcParams.message[2] = 1;
	stegoParams.stcParams.message[3] = 1;
	stegoParams.stcParams.message[4] = 1;
}

void stegoConstruct()
{
	stegoParams.stcParams.stego = (int*)malloc(sizeof(int) * (stegoParams.stcParams.coverLen + 1));
}

void rhoConstruct()
{
	stegoParams.stcParams.rho = (int64_t*)malloc(sizeof(int64_t) * (stegoParams.stcParams.coverLen + 1));
	//先所有权重都为1测试、
	for (int i=1;i<=stegoParams.stcParams.coverLen;i++)
	{
		stegoParams.stcParams.rho[i] = 1;
	}
	/*for(int i = 1; i <= stegoParams.stcParams.coverLen; i ++)
	{
		switch(stegoParams.mbParamsList[i - 1].localOptimalMvNum)
		{
		case 0:
			stegoParams.stcParams.rho[i] = 50;
			break;
		case 1:
			stegoParams.stcParams.rho[i] = 20;
			break;
		case 2:
			stegoParams.stcParams.rho[i] = 5;
			break;
		case 3:
			stegoParams.stcParams.rho[i] = 2;
			break;
		case 4:
			stegoParams.stcParams.rho[i] = 1;
			break;
		default:
			exit(-1);
		}
	}*/
}

//cover, message, rho, H_hat indexed from 1
//notice: heightOfH_hat is needed!
void trellis_code(int *cover, int coverLen, int *message, int *H_hat, int heightOfH_hat, int widthOfH_hat,
		int numOfH_hat, int64_t* rho, int* stego, int64_t* embedding_cost)
{
	int numOfState;
	int64_t *wght;
	int64_t *newwght;
	int indx = 1;
	int indm = 1;
	int blockInd, colInd, state;
	int64_t w0, w1;
	int **path; //coverLen * numOfState
	int i;
	int bestState = 0;
	int **H;

	//initialize H, index different part of H by the number of H_hat
	H = (int**) malloc(sizeof(int*) * (numOfH_hat + 1));
	for (blockInd = 1; blockInd <= numOfH_hat; blockInd++)
	{
		H[blockInd] = (int*) malloc(sizeof(int) * (widthOfH_hat + 1));
		for (i = 1; i <= widthOfH_hat; i++)
		{
			H[blockInd][i] = H_hat[i];
		}
		if (blockInd - 1 + heightOfH_hat > numOfH_hat)
		{
			for (i = 1; i <= widthOfH_hat; i++)
			{
				H[blockInd][i] &= ((int) pow(2.0, numOfH_hat - blockInd + 1)
						- 1);
			}
		}
	}

	//initialize state array
	numOfState = (int) pow(2.0, (numOfH_hat > heightOfH_hat) ? heightOfH_hat : numOfH_hat);

	//initialize numOfState
	wght = (int64_t *) malloc(sizeof(int64_t) * numOfState);
	newwght = (int64_t*) malloc(sizeof(int64_t) * numOfState);

	//initialize path
	path = (int **) malloc(sizeof(int *) * (coverLen + 1));
	for (i = 1; i <= coverLen; i++)
	{
		path[i] = (int*) malloc(sizeof(int) * numOfState);
	}

	//initialize wght
	wght[0] = 0;
	for (i = 1; i < numOfState; i++)
	{
		wght[i] = INIFINITY;
	}

	//forward part of trellis code
	for (blockInd = 1; blockInd <= numOfH_hat; blockInd++)
	{
		H_hat = H[blockInd];
		for (colInd = 1; colInd <= widthOfH_hat; colInd++)
		{
			for (state = 0; state < numOfState; state++)
			{
				w0 = wght[state] + rho[indx] * cover[indx];
				w1 = wght[state ^ H_hat[colInd]] + rho[indx]
						* (1 - cover[indx]);
				path[indx][state] = w1 < w0 ? 1 : 0;
				newwght[state] = w1 < w0 ? w1 : w0;
			}
			indx++;
			//update wght
			for (state = 0; state < numOfState; state++)
			{
				wght[state] = newwght[state];
			}
		}
		//prune
		for (state = 0; state < numOfState / 2; state++)
		{
			wght[state] = wght[2 * state + message[indm]];
		}
		for (; state < numOfState; state++)
		{
			wght[state] = INIFINITY;
		}
		indm++;
	}

	//backward part of Viterbi algorithm
	*embedding_cost = wght[0];
	indx--;
	indm--;
	state = message[indm];
	for (blockInd = numOfH_hat; blockInd >= 1; blockInd--)
	{
		H_hat = H[blockInd];
		for (colInd = widthOfH_hat; colInd >= 1; colInd--)
		{
			stego[indx] = path[indx][state];
			state = state ^ (stego[indx] * H_hat[colInd]);
			indx--;
		}
		state = (2 * state + message[--indm]) % numOfState;
	}

	// free memory
	for (i = 1; i <= coverLen; i++)
	{
		free(path[i]);
		path[i] = NULL;
	}
	free(path);
	path = NULL;

	free(newwght);
	newwght = NULL;

	free(wght);
	wght = NULL;

	for (blockInd = 1; blockInd <= numOfH_hat; blockInd++)
	{
		free(H[blockInd]);
		H[blockInd] = NULL;
	}
	free(H);
	H = NULL;
}


