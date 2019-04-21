#pragma once
void coverConstruct();
void msgConstruct();
void stegoConstruct();
void rhoConstruct();
void trellis_code(int *cover, int coverLen, int *message, int *H_hat, int heightOfH_hat, int widthOfH_hat,
	int numOfH_hat, int64_t* rho, int* stego, int64_t* embedding_cost);
void execSTC();