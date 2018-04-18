#include <stdio.h>
#include <stdlib.h>

int scan(int* numV, int* numE, int* Start, int* Finish, FILE* oFile, FILE* iFile)
{
	fscanf(iFile, "%d", numV);
	if (*numV < 0 || *numV > 5000)
	{
		fprintf(oFile, "bad number of vertices");
		return 0;
	}

	fscanf(iFile, "%d %d", Start, Finish);
	if (*Start <= 0 || *Start > *numV || *Finish <= 0 || *Finish > *numV)
	{
		fprintf(oFile, "bad vertex");
		return 0;
	}

	fscanf(iFile, "%d", numE);
	if (*numE < 0 || *numE > *numV*(*numV + 1) / 2)
	{
		fprintf(oFile, "bad number of edges");
		return 0;
	}

	return 1;
}

int** fillEdges(int numV, int numE, FILE* iFile, FILE* oFile, int* error)
{
	unsigned int** edges = (unsigned int**)malloc(sizeof(unsigned int*)*(numV + 1));

	for (int i = 0; i < numV + 1; i++)
	{
		edges[i] = (unsigned int*)malloc(sizeof(unsigned int)*(numV + 1));
		memset(edges[i], 0, sizeof(unsigned int)*(numV + 1));
	}

	for (int i = 0; i < numE; i++)
	{
		int V1, V2;
		unsigned int length;

		//error 1
		if (feof(iFile))
		{
			*error = 1;
			return 0;
		}
		if (fscanf(iFile, "%d %d %d ", &V1, &V2, &length) < 1)
		{
			*error = 1;
			return 0;
		}

		//error 2
		if (numV < V1 || V1 <= 0 || numV < V2 || V2 <= 0)
		{
			*error = 2;
			return 0;
		}

		//error 3
		if (length <= 0 || length > INT_MAX) 
		{
			*error = 3;
			return 0;
		}
		edges[V1][V2] = length;
	}

	return edges;
}

void clearEdges(unsigned int** edges, int numV)
{
	for (int i = 0; i < numV + 1; i++)
		free(edges[i]);
}

void printErr(int error, FILE* oFile) 
{
	if (error == 1)
		fprintf(oFile, "bad number of lines");

	if (error == 2)
		fprintf(oFile, "bad vertex");

	if (error == 3)
		fprintf(oFile, "bad length");
}

void MinDistanceUpdate(unsigned int ** edges,  int* MinDistance, int new, long int DisToNew, int numV)
{

	MinDistance[new] = 0;
	for (int i = 1; i < numV + 1; i++)
		if (edges[new][i])
			if (MinDistance[i] < 0 || MinDistance[i] > DisToNew + edges[new][i])
				MinDistance[i] = DisToNew + edges[new][i];

}

int AddVertex(unsigned int ** edges, long int* MinDistance, int * AlreadyIn, int numV)
{
	long int tmp = LONG_MAX;
	int NewVer = 0;
	int t = 0;
	for (int i = 1; i < numV + 1; i++)
	{
		if (AlreadyIn[i] == 0)			//Может не обязательно
			if (MinDistance[i] > 0 || MinDistance[i] < tmp)
			{
				tmp = MinDistance[i];
				NewVer = i;
				t = 1;
			}
	}

	if (!t)
		return 0;

	AlreadyIn[NewVer] = 1;
	MinDistanceUpdate(edges, MinDistance, NewVer, MinDistance[NewVer], numV);
	return 1;
}

int dijkstra(int numV, int numE, int Start, int Finish, FILE* oFile, FILE* iFile)
{
	
	int error = 0;
	unsigned int** edges = fillEdges(numV, numE, iFile, oFile, &error);

	if (error)
	{
		printErr(error, oFile);
		clearEdges(edges, numV);
		return 0;
	}
	//int * Path = (int*)malloc(sizeof(int) * (numV + 1));
	//memset(Path, 0, sizeof(int) * (numV + 1));

	int * AlreadyIn = (int*)malloc(sizeof(int) * (numV + 1));
	memset(AlreadyIn, 0, sizeof(int) * (numV + 1));

	long int * MinDistance = (long int*)malloc(sizeof(long int) * (numV + 1));
	memset(MinDistance, -1, sizeof(long int) * (numV + 1));

	MinDistanceUpdate(edges, MinDistance, Start, 0, numV);

	AlreadyIn[Start] = 1;


	//TODO

	/*for (int i = 1; i < numV + 1; i++) {
		for (int j = 1; j < numV + 1; j++)
			fprintf(oFile, "%d ", edges[i][j]);
		fprintf(oFile, "\n");
	}
	*/

	clearEdges(edges, numV);
}


int main()
{
	FILE* iFile;
	iFile = fopen("in.txt", "r");
	FILE* oFile;
	oFile = fopen("out.txt", "w");

	int numV, numE, Start, Finish;

	if (!scan(&numV, &numE, &Start, &Finish, oFile, iFile))
		return 0;

	dijkstra(numV, numE, Start, Finish, oFile, iFile);
    return 0;
}