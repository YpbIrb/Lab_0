#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/*
unsigned int scanLength(FILE* oFile)
{
unsigned length = 0;
char c;
}
*/

unsigned int** fillEdges(int numV, int numE, FILE* iFile, FILE* oFile, int* error)
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
		long int length;

		//error 1
		if (feof(iFile))
		{
			*error = 1;
			return 0;
		}
		if (fscanf(iFile, "%d %d %ld ", &V1, &V2, &length) < 1)
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
			//length = 0;
			*error = 3;
			return 0;
		}
		//fprintf(oFile, "\n!!!%d!!!\n", length);
		edges[V1][V2] = length;
		edges[V2][V1] = length;
	}

	return edges;
}

void clearEdges(unsigned int** edges, int numV, FILE* oFile)
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

void ClearAll(unsigned int ** edges, long int* MinDistance,
	int* Path, int* AlreadyIn, int numV, FILE* oFile)
{
	//clearEdges(edges, numV, oFile);			//!!!!
	free(Path);
	free(MinDistance);
	free(AlreadyIn);
}

void MinDistanceUpdate(unsigned int ** edges, long int* MinDistance,
	int newV, int* Path, int* AlreadyIn, int numV, FILE* oFile, int* DisOverflow)
{
	/*
	if (MinDistance[Path[newV]] == LONG_MAX)
		MinDistance[newV] == LONG_MAX;
	else
		MinDistance[newV] = MinDistance[Path[newV]] + edges[Path[newV]][newV];
	*/

	//fprintf(oFile, "New Vertex: %d\n", newV);
	if (MinDistance[Path[newV]] + edges[Path[newV]][newV] > LONG_MAX)
	{
		MinDistance[newV] = LONG_MAX;
		DisOverflow[newV] = 1;
	}
	else 
		MinDistance[newV] = MinDistance[Path[newV]] + edges[Path[newV]][newV];
	

	for (int i = 1; i < numV + 1; i++)
	{
		/*
		fprintf(oFile, "AlreadyIn[%d] = %d ", i, AlreadyIn[i]);
		fprintf(oFile, "edges[%d][%d] = %d ", newV, i, edges[newV][i]);
		fprintf(oFile, "MinDistance[%d] = %d\n", i, MinDistance[i]);
		*/
		if (!AlreadyIn[i])
		{
			if (edges[newV][i])
			{
				if (MinDistance[i] < 0 || MinDistance[i] > MinDistance[newV] + edges[newV][i])
				{
					if (MinDistance[newV] + edges[newV][i] > INT_MAX || DisOverflow[newV] == 1) {
						MinDistance[i] = LONG_MAX;
						DisOverflow[i] = 1;
					}
						
					else
						MinDistance[i] = MinDistance[newV] + edges[newV][i];
					Path[i] = newV;
					//fprintf(oFile, "Path[%d] = %d MinDistance[%d] = %d\n", i, newV, i, MinDistance[i]);
				}
			}
		}

		if (edges[newV][i] && AlreadyIn[i])
		{
			AlreadyIn[newV]++;
			//fprintf(oFile, "ALreadyIn[%d] = %d\n", newV, AlreadyIn[newV]);
		}
		
	}

	//fprintf(oFile, "\n!!!\n");
}

int overflowCheck(unsigned int ** edges, int* Path, int Finish, int* AlreadyIn, int numV)
{
	int tmp = Finish;
	if (AlreadyIn[Finish] > 1)
		return 1;

	for (int i = 1; i < numV + 1; i++)
	{
		if (AlreadyIn[i] > 1 && edges[i][Finish])
			return 1;
	}

	return 0;

}

int AddVertex(unsigned int ** edges, long int* MinDistance, int * AlreadyIn, int* Path, int* DisOverflow, int numV, FILE* oFile)
{
	long long tmp = LONG_MAX;
	int NewVer = 0;
	int t = 0;
	for (int i = 1; i < numV + 1; i++)
	{
		if (!AlreadyIn[i])
			if ((MinDistance[i] <= tmp) && (MinDistance[i] != -1) && DisOverflow[i] == 0)
			{
				tmp = MinDistance[i];
				NewVer = i;
				t = 1;
				
			}
	}

	if (!t)
		for (int i = 1; i < numV + 1; i++)
		{
			if (!AlreadyIn[i])
				if ((MinDistance[i] <= tmp) && (MinDistance[i] != -1))
				{
					tmp = MinDistance[i];
					NewVer = i;
					t = 1;

				}
		}

	if (!t)
		return 0;


	//fprintf(oFile, "MinDistance[%d] = %lld\n", NewVer, tmp);
	//fprintf(oFile, "AlreadyIn[%d] = %d\n", NewVer, AlreadyIn[NewVer]);

	MinDistanceUpdate(edges, MinDistance, NewVer, Path, AlreadyIn, numV, oFile, DisOverflow);
	//AlreadyIn[NewVer] = 1;
	return 1;
}

int dijkstra(int numV, int numE, int Start, int Finish, FILE* oFile, FILE* iFile)
{
	int error = 0;
	unsigned int** edges = fillEdges(numV, numE, iFile, oFile, &error);

	if (error)
	{
		printErr(error, oFile);
		//clearEdges(edges, numV, oFile);		//!!!!
		return 0;
	}

	int * Path = (int*)malloc(sizeof(int) * (numV + 1));
	memset(Path, 0, sizeof(int) * (numV + 1));

	int * AlreadyIn = (int*)malloc(sizeof(int) * (numV + 1));
	memset(AlreadyIn, 0, sizeof(int) * (numV + 1));

	long int * MinDistance = (long int*)malloc(sizeof(long int) * (numV + 1));
	memset(MinDistance, -1, sizeof(long int) * (numV + 1));

	int* DisOverflow = (int*)malloc(sizeof(int) * (numV + 1));
	memset(DisOverflow, 0, sizeof(int) * (numV + 1));

	AlreadyIn[Start] = 1;
	MinDistance[Start] = 0;
	MinDistance[0] = 0;

	/*
	for (int i = 0; i < numV + 1; i++)
	{
	if (edges[Start][i])
	MinDistance[i] = edges[Start][i];
	}
	*/

	Path[Start] = 0;
	MinDistanceUpdate(edges, MinDistance, Start, Path, AlreadyIn, numV, oFile, DisOverflow);

	while (1)
	{
		if (!AddVertex(edges, MinDistance, AlreadyIn, Path, DisOverflow, numV, oFile))
			break;
	}

	/*
	fprintf(oFile, "overflowCheck = %d", overflowCheck(edges, Path, Finish, numV));
	fprintf(oFile, "AlreadyIn: ");
	for (int i = 1; i < numV + 1; i++)
	fprintf(oFile, "%d ", AlreadyIn[i]);
	fprintf(oFile, "\n");
	for (int i = 1; i < numV + 1; i++)
	fprintf(oFile, "%d ", MinDistance[i]);
	fprintf(oFile, "\n");
	for (int i = 1; i < numV + 1; i++)
	fprintf(oFile, "%d ", Path[i]);
	fprintf(oFile, "\n");
	for (int i = 1; i < numV + 1; i++)
	fprintf(oFile, "%d ", AlreadyIn[i]);
	fprintf(oFile, "\n");
	for (int i = 1; i < numV + 1; i++)
	{
	for (int j = 1; j < numV + 1; j++)
	fprintf(oFile, "%d ", edges[i][j]);
	fprintf(oFile, "\n");
	}
	*/


	for (int i = 1; i < numV + 1; i++)
	{
		if (DisOverflow[i] == 1)
			fprintf(oFile, "INT_MAX+ ");
		else
		{
			if (MinDistance[i] == -1)
				fprintf(oFile, "oo ");
			else
				fprintf(oFile, "%d ", MinDistance[i]);
		}

	}

	fprintf(oFile, "\n");


	//fprintf(oFile, "\n!!%d!!\n", overflowCheck(edges, Path, Finish, AlreadyIn, numV));
	if (DisOverflow[Finish] == 1)
	{
		if (overflowCheck(edges, Path, Finish, AlreadyIn, numV))
			fprintf(oFile, "overflow");
		else
		{
			while (Path[Finish] != 0)
			{
				fprintf(oFile, "%d ", Finish);
				Finish = Path[Finish];
			}
			fprintf(oFile, "%d ", Finish);
		}

	}
	else
		if (MinDistance[Finish] != -1)
		{
			while (Path[Finish] != 0)
			{
				fprintf(oFile, "%d ", Finish);
				Finish = Path[Finish];
			}
			fprintf(oFile, "%d ", Finish);

		}
		else
			fprintf(oFile, "no path");

	//fprintf(oFile, "\n%d", INT_MAX);
	ClearAll(edges, MinDistance, Path, AlreadyIn, numV, oFile);
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