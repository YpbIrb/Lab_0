
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct edge 
{
	int v1, v2;
	int length;
} edge;

int scan(int* numV, int* numE, FILE* oFile, FILE* iFile)
{
	fscanf(iFile, "%d", numV);
	if (*numV < 0 || *numV > 5000)
	{
		fprintf(oFile, "bad number of vertices");
		return 0;
	}

	fscanf(iFile, "%d", numE);
	if (*numE < 0 || *numE > *numV*(*numV) / 2)
	{
		fprintf(oFile, "bad number of edges");
		return 0;
	}

	return 1;
}

edge* fillEdges(int numV, int numE, FILE* iFile, int* error)
{
	edge* edges = (edge*)malloc(sizeof(edge*)*(numE));

	for (int i = 0; i < numE; i++)
	{
		int V1, V2;
		int length = 0;


		if (feof(iFile))
		{
			*error = 1;
			break;
		}

		if (fscanf(iFile, "%d %d ", &V1, &V2) < 1)
		{
			*error = 1;
			break;
		}


		if (numV < V1 || V1 <= 0 || numV < V2 || V2 <= 0)
		{
			*error = 2;
			break;
		}

		fscanf(iFile, "%d", &length);

		if (length <= 0 || length > INT_MAX)
		{
			*error = 3;
			break;
		}

		edges[i].v1 = V1 - 1;
		edges[i].v2 = V2 - 1;
		edges[i].length = length;
	}

	return edges;
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

int checkEdges(edge* edges, int numE, int numV)
{
	int* color = (int*)malloc(sizeof(int)*numE);
	memset(color, 0, sizeof(int) * (numE));
	color[edges[0].v1] = 1;
	for (int i = 0; i < numE; ++i)
	{
		color[edges[i].v1] = 1;
		color[edges[i].v2] = 1;
	}

	for (int i = 0; i < numV; ++i)
		if (!color[i])
		{
			free(color);
			return 0;
		}
	free(color);
	return 1;
}

int compare(const void* e1, const void* e2)
{
	return (*(edge*)e1).length - (*(edge*)e2).length;
}

void UpdateConnectComp(int* connectComp, int V1, int V2, int numV)
{
	int tmp;
	if (connectComp[V1] != -1 && connectComp[V2] != -1)
	{
		tmp = connectComp[V2];
		for (int j = 0; j < numV; ++j)
			if (connectComp[j] == tmp)
				connectComp[j] = connectComp[V1];
		return;
	}

	if (connectComp[V1] == -1 && connectComp[V2] != -1)
	{
		connectComp[V1] = connectComp[V2];
		return;
	}
		
	if (connectComp[V1] != -1 && connectComp[V2] == -1)
	{
		connectComp[V2] = connectComp[V1];
		return;
	}

	if (connectComp[V1] == -1 && connectComp[V2] == -1)
	{
		connectComp[V1] = V1;
		connectComp[V2] = V1;
		return;
	}
}

int Kruskal(int numV, int numE, FILE* oFile, FILE* iFile)
{
	int error = 0;
	edge* edges = fillEdges(numV, numE, iFile, &error);

	if (error)
	{
		printErr(error, oFile);
		//free(edges);
		return 0;
	}

	if (!checkEdges(edges, numE, numV))
	{
		fprintf(oFile, "no spanning tree");
		//free(edges);
		return 0;
	}

	qsort(edges, numE, sizeof(edge), compare);

	int* connectComp = (int*)malloc(sizeof(int)*numV);
	memset(connectComp, -1, sizeof(int));

	for (int i = 0; i < numE; ++i)
	{
		int V1 = edges[i].v1, V2 = edges[i].v2;

		if (connectComp[V1] = connectComp[V2] && connectComp[V1] != -1)
			continue;
		
		UpdateConnectComp(connectComp, V1, V2, numV);

		fprintf(oFile, "%d %d", V1 + 1, V2 + 1);

	}
	return 0;
}

int main()
{
	FILE* iFile;
	iFile = fopen("in.txt", "r");
	FILE* oFile;
	oFile = fopen("out.txt", "w");

	int numV, numE;

	if (!scan(&numV, &numE, oFile, iFile))
		return 0;
	Kruskal(numV, numE, oFile, iFile);
	fclose(oFile);
	fclose(iFile);

    return 0;
}