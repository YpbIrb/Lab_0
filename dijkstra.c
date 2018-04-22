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

void ClearAll(unsigned int ** edges, long int* MinDistance, int* Path, int* AlreadyIn, int numV)
{
	clearEdges(edges, numV);
	free(Path);
	free(MinDistance);
	free(AlreadyIn);
}

void MinDistanceUpdate(unsigned int ** edges, long int* MinDistance,
								int new, int* Path, int* AlreadyIn,  int numV, FILE* oFile)
{
	//вывод всего изменгения MinDistance в зависимости от вершины new
	//fprintf(oFile, "New Vertex: %d\n", new);
	MinDistance[new] = MinDistance[Path[new]] + edges[Path[new]][new];
	for (int i = 1; i < numV + 1; i++)
	{
		//fprintf(oFile, "AlreadyIn[%d] = %d ", i, AlreadyIn[i]);
		//fprintf(oFile, "edges[%d][%d] = %d ", new, i, edges[new][i]);
		//fprintf(oFile, "MinDistance[%d] = %d\n", i, MinDistance[i]);
		if (!AlreadyIn[i])
		{
			if (edges[new][i])
			{
				if (MinDistance[i] < 0 || MinDistance[i] > MinDistance[new] + edges[new][i])
				{
					MinDistance[i] = MinDistance[new] + edges[new][i];
					Path[i] = new;
					//fprintf(oFile, "Path[%d] = %d MinDistance[%d] = %d\n", i, new, i, MinDistance[i]);

				}
			}
		}
	}
		
	//fprintf(oFile, "!!!\n");
}

int AddVertex(unsigned int ** edges, long int* MinDistance, int * AlreadyIn, int* Path, int numV, FILE* oFile)
{
	long int tmp = LONG_MAX;
	int NewVer = 0;
	int t = 0;
	for (int i = 1; i < numV + 1; i++)
	{
		if (!AlreadyIn[i])			//Может не обязательно
			if ((MinDistance[i] < tmp) && (MinDistance[i] >= 0))
			{
				tmp = MinDistance[i];
				NewVer = i;
				t = 1;
			}
	}

	if (!t)
		return 0;

	AlreadyIn[NewVer] = 1;
	//fprintf(oFile, "MinDistance[%d] = 1\n", tmp);
	//fprintf(oFile, "AlreadyIn[%d] = 1\n", NewVer);
	MinDistanceUpdate(edges, MinDistance, NewVer, Path, AlreadyIn, numV, oFile);
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
	int * Path = (int*)malloc(sizeof(int) * (numV + 1));
	memset(Path, 0, sizeof(int) * (numV + 1));

	int * AlreadyIn = (int*)malloc(sizeof(int) * (numV + 1));
	memset(AlreadyIn, 0, sizeof(int) * (numV + 1));

	long int * MinDistance = (long int*)malloc(sizeof(long int) * (numV + 1));
	memset(MinDistance, -1, sizeof(long int) * (numV + 1));

	AlreadyIn[Start] = 1;
	//MinDistance[Start] = 0;
	MinDistance[0] = 0;
	/*
	for (int i = 0; i < numV + 1; i++)
	{
		if (edges[Start][i])
			MinDistance[i] = edges[Start][i];
	}
	*/
	Path[Start] = 0;
	//edges[0][Start] = 0;
	MinDistanceUpdate(edges, MinDistance, Start, Path, AlreadyIn, numV, oFile);

	while (1) 
	{
		if (!AddVertex(edges, MinDistance, AlreadyIn, Path, numV, oFile))
			break;
	}

	/*
	for (int i = 1; i < numV + 1; i++)
		//fprintf(oFile, "%d ", MinDistance[i]);

	fprintf(oFile, "\n");

	for (int i = 1; i < numV + 1; i++)
		fprintf(oFile, "%d ", Path[i]);

	fprintf(oFile, "\n");

	for (int i = 1; i < numV + 1; i++)
		fprintf(oFile, "%d ", AlreadyIn[i]);

	for (int i = 1; i < numV + 1; i++) 
	{
		for (int j = 1; j < numV + 1; j++)
			fprintf(oFile, "%d ", edges[i][j]);
		fprintf(oFile, "\n");
	}
			*/

	for (int i = 1; i < numV + 1; i++) 
	{
		if (MinDistance[i] >= 0)
			fprintf(oFile, "%d ", MinDistance[i]);
		else
			fprintf(oFile, "oo ");
	}
	fprintf(oFile, "\n");
	if (MinDistance[Finish] > 0) 
	{
		if (MinDistance[Finish] < INT_MAX)
			fprintf(oFile, "%d", MinDistance[Finish]);
		else
			fprintf(oFile, "INT_MAX+");
	}
		
	else
		fprintf(oFile, "no path");
	fprintf(oFile, "\n%d", INT_MAX);
	ClearAll(edges, MinDistance, Path, AlreadyIn, numV);
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