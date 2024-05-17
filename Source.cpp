#include<iostream>
#include<conio.h>
#include<windows.h>
#include <cstring>
#include <fstream>
#include <cstdlib>
#include <climits>
#include <ctime>

#pragma comment(lib, "winmm.lib")


using namespace std;


#define nN 20
#define INF 999

/*const int ROWS = 20;
const int COLS = 20;
const int INF = 1e9;
const int N = ROWS * COLS;*/  // Total number of nodes

class knode
{
public:
	int x;
	int y;
	knode* next;
	knode(int a, int b) {
		x = a;
		y = b;
		next = NULL;
	}
};

int** convertAdj(char[][20], int, int);
int calcmindistance(int[], bool[], int, int);
void dijkstra(int**, int, int, int, int, char[][20]);
void floydWarshall(int**, int, int, int, int, char[][20]);
void primMST(int**);
int minKey(int[], bool[]);
void printMST(int[], int**);
void kruskal(int**, int, int);
void printkruskalmst(int cost, knode* n);

class avlNode {
public:
	int id; //generated randomly
	int rewardScore; //score of a particular reward you obtained
	int count; //to avoid duplicate id nodes (maintain count of the id)
	int height;
	avlNode* left;
	avlNode* right;

	avlNode(int new_id, int new_reward_score) {
		id = new_id;
		rewardScore = new_reward_score;
		count = 1;
		left = NULL;
		right = NULL;
		height = 1;
	}

	~avlNode() {}
};

class avlTree {
public:
	avlNode* root;
	int node_count;

	avlTree() {
		root = NULL;
		node_count = 0;
	}

	avlNode* returnRoot() {
		return root;
	}

	int getHeight(avlNode* node) {
		if (node == NULL) {
			return 0;
		}
		return node->height;
	}

	int calcbalanceFactor(avlNode* node) {
		if (node == NULL) {
			return 0;
		}
		return getHeight(node->left) - getHeight(node->right);
	}

	avlNode* rotateRight(avlNode* y) {
		avlNode* x = y->left;
		avlNode* t2 = x->right;

		// perform rotation
		x->right = y;
		y->left = t2;

		// update heights

		y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
		x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

		// return new root
		return x;
	}

	avlNode* rotateLeft(avlNode* x) {
		avlNode* y = x->right;
		avlNode* t2 = y->left;

		// perform rotation
		y->left = x;
		x->right = t2;

		// update heights
		x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
		y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

		// return new root
		return y;
	}

	avlNode* balancing(avlNode* node) {
		getHeight(node);
		int balanceFactor = calcbalanceFactor(node);
		if (balanceFactor > 1) { //LL
			if (calcbalanceFactor(node->left) < 0) { //LR
				node->left = rotateLeft(node->left);
			}
			return rotateRight(node);
		}
		if (balanceFactor < -1) { //RR
			if (calcbalanceFactor(node->right) > 0) { //RL
				node->right = rotateRight(node->right);
			}
			return rotateLeft(node);
		}
		return node;
	}

	avlNode* insert(avlNode* newNode, int newId, int newrewardScore) {
		if (newNode == NULL) {
			node_count++;
			return new avlNode(newId, newrewardScore);
		}

		if (newId == newNode->id && newrewardScore == newNode->rewardScore) {
			newNode->count++;
		}
		else if (newId < newNode->id) {
			newNode->left = insert(newNode->left, newId, newrewardScore);
		}
		else {
			newNode->right = insert(newNode->right, newId, newrewardScore);
		}

		return balancing(newNode);
	}

	void insert(int newScore) {
		if (root == NULL) {
			root = new avlNode(100, newScore);
			node_count++;
		}
		else {
			int new_id = rand() % 201; // generate random id between 0-200
			root = insert(root, new_id, newScore);
		}
	}

	~avlTree()
	{}
};

void print_inorder(avlNode* node) {
	if (node == NULL) {
		return;
	}
	print_inorder(node->left);
	for (int i = 0; i < node->count; i++) {
		cout << "[ID:" << node->id << "," << "Score:" << node->rewardScore << "," << "Node count: " << node->count << "] ";
	}
	print_inorder(node->right);
}


int main()
{

	int rows = 20;
	int cols = 20;
	char map[20][20];
	//char** map = new char* [rows]; //Dynamic 2d Array
	//for (int i = 0; i < rows; i++) {
	//    map[i] = new char[cols];
	//}


	ifstream inputFile("finalMap.txt");   // Open the file for reading

	if (inputFile.is_open())
	{
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; )
			{
				char c = inputFile.get();    // Read the next character from the file

				if (c != ' ' && c != '\n') {  // Check if the character is not a space or newline

					map[i][j] = c;           // Add the character to the array
					j++;                     // Increment the column index
				}
			}
		}
		inputFile.close();     // Close the file
	}

	int srcx = 0;
	int srcy = 0;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (map[i][j] == '*')
			{
				srcx = i;
				srcy = j;
			}
		}
		//cout << endl;
	}

	int dest = srcx * 20 + srcy;
	int defsrc = 0 * 20 + 0;

	int** adjMat = convertAdj(map, rows, cols);

	// Creating and displaying the menu

	HANDLE col;
	col = GetStdHandle(STD_OUTPUT_HANDLE);

	int choice = 1;
	char ch = '\0';

	cout << endl;
	cout << endl;
	cout << endl;

	do
	{
		system("cls");

		SetConsoleTextAttribute(col, 14);

		cout << "\t\t\t\t -----------------------------------------------" << endl;
		cout << "\t\t\t\t|\t\t\t\t\t\t|" << endl;
		cout << "\t\t\t\t|\t\t\t\t\t\t|" << endl;
		cout << "\t\t\t\t|\tTHE QUEST FOR THE CRYSTAL KINGDOM \t|" << endl;
		cout << "\t\t\t\t|\t\t\t\t\t\t|" << endl;
		cout << "\t\t\t\t|\t\t\t\t\t\t|" << endl;
		cout << "\t\t\t\t -----------------------------------------------" << endl;

		SetConsoleTextAttribute(col, 7); // reset to default color


		if (choice == 1)
		{
			SetConsoleTextAttribute(col, 14);
			cout << "  > Display Game Map" << endl << endl;
		}

		else
		{
			SetConsoleTextAttribute(col, 7);
			cout << "  Display Game Map " << endl << endl;
		}

		if (choice == 2)
		{
			SetConsoleTextAttribute(col, 14);
			cout << "  > Display Score" << endl << endl;
		}

		else
		{
			SetConsoleTextAttribute(col, 7);
			cout << "  Display Score " << endl << endl;
		}

		if (choice == 3)
		{
			SetConsoleTextAttribute(col, 14);
			cout << "  > Use Dijkstra's Algorithm From Default Location " << endl << endl;
		}

		else
		{
			SetConsoleTextAttribute(col, 7);
			cout << "  Use Dijkstra's Algorithm From Default Location " << endl << endl;
		}

		if (choice == 4)
		{
			SetConsoleTextAttribute(col, 14);
			cout << "  > Use Dijkstra's Algorithm From Custom Location " << endl << endl;
		}

		else
		{
			SetConsoleTextAttribute(col, 7);
			cout << "  Use Dijkstra's Algorithm From Custom Location " << endl << endl;
		}


		if (choice == 5)
		{
			SetConsoleTextAttribute(col, 14);
			cout << "  > Use Floyd's Algorithm From Default Location " << endl << endl;
		}

		else
		{
			SetConsoleTextAttribute(col, 7);
			cout << "  Use Floyds's Algorithm From Default Location " << endl << endl;
		}

		if (choice == 6)
		{
			SetConsoleTextAttribute(col, 14);
			cout << "  > Use Floyd's Algorithm From Custom Location " << endl << endl;
		}

		else
		{
			SetConsoleTextAttribute(col, 7);
			cout << "  Use Floyds's Algorithm From Custom Location " << endl << endl;
		}

		if (choice == 7)
		{
			SetConsoleTextAttribute(col, 14);
			cout << "  > Use Prims Algorithm To Find Shortest Path " << endl << endl;
		}

		else
		{
			SetConsoleTextAttribute(col, 7);
			cout << "  Use Prims Algorithm To Find Shortest Path " << endl << endl;
		}

		if (choice == 8)
		{
			SetConsoleTextAttribute(col, 14);
			cout << "  > Use Kruskal's Algorithm To Find Shortest Path " << endl << endl;
		}

		else
		{
			SetConsoleTextAttribute(col, 7);
			cout << "  Use Kruskal's Algorithm To Find Shortest Path " << endl << endl;
		}

		SetConsoleTextAttribute(col, 7);
		cout << " Press Q to quit " << endl;

		ch = _getch();

		switch (ch)
		{
		case 72: // up arrow key
			if (choice > 1)
			{
				choice--;

				PlaySoundW(L"C:\\Users\\Ibrahim Saqib\\Desktop\\uni\\Fast\\sem 4\\ds\\Projectx\\ok1.wav", NULL, SND_ASYNC);
			}

			break;

		case 80: // down arrow key
			if (choice < 8)
			{
				choice++;

				PlaySoundW(L"C:\\Users\\Ibrahim Saqib\\Desktop\\uni\\Fast\\sem 4\\ds\\Projectx\\ok1.wav", NULL, SND_ASYNC);
			}

			break;

		case 'q':
		case 'Q':
			break;

		case 13: // enter key
			system("cls"); // clear the screen

			PlaySoundW(L"C:\\Users\\Ibrahim Saqib\\Desktop\\uni\\Fast\\sem 4\\ds\\Projectx\\ok3.wav", NULL, SND_ASYNC);

			if (choice == 1)
			{
				do
				{
					system("cls");

					SetConsoleTextAttribute(col, 12);
					for (int i = 0; i < rows; i++)
					{
						for (int j = 0; j < cols; j++)
						{
							cout << map[i][j] << " ";
						}

						cout << endl;

					}

					ch = _getch();

					//PlaySoundW(L"D:\\STUDY\\Semester 4\\DS\\Project\\Project1\\Project1\\ok2.wav", NULL, SND_ASYNC);
				} while (ch != 'B' && ch != 'b');
			}

			if (choice == 2)
			{
				system("cls");

				do
				{
					SetConsoleTextAttribute(col, 14);
					cout << " The score will be displayed once you use the algorithms" << endl;
					ch = _getch();

				} while (ch != 'B' && ch != 'b');

			}

			if (choice == 3)
			{
				system("cls");

				do
				{
					SetConsoleTextAttribute(col, 10);
					dijkstra(adjMat, defsrc, dest, rows, cols, map);
					ch = _getch();

				} while (ch != 'B' && ch != 'b');

			}

			if (choice == 4)
			{
				system("cls");

				do
				{
					SetConsoleTextAttribute(col, 13);

					int clx = 0;
					int cly = 0;
					cout << "Enter custom location X-axis: ";
					cin >> clx;
					cout << endl;
					cout << "Enter custom location Y-axis: ";
					cin >> cly;
					int customsrc = clx * 20 + cly;

					dijkstra(adjMat, customsrc, dest, rows, cols, map);
					ch = _getch();

				} while (ch != 'B' && ch != 'b');

			}

			if (choice == 5)
			{
				system("cls");

				do
				{
					SetConsoleTextAttribute(col, 11);

					floydWarshall(adjMat, defsrc, dest, rows, cols, map);
					ch = _getch();

				} while (ch != 'B' && ch != 'b');

			}

			if (choice == 6)
			{
				system("cls");

				do
				{
					SetConsoleTextAttribute(col, 9);

					int clx = 0;
					int cly = 0;
					cout << "Enter custom location X-axis: ";
					cin >> clx;
					cout << endl;
					cout << "Enter custom location Y-axis: ";
					cin >> cly;
					int customsrc = clx * 20 + cly;

					floydWarshall(adjMat, customsrc, dest, rows, cols, map);
					ch = _getch();

				} while (ch != 'B' && ch != 'b');

			}

			if (choice == 7)
			{
				system("cls");

				do
				{
					SetConsoleTextAttribute(col, 8);
					primMST(adjMat);
					ch = _getch();

				} while (ch != 'B' && ch != 'b');

			}

			if (choice == 8)
			{
				system("cls");

				do
				{
					SetConsoleTextAttribute(col, 3);
					kruskal(adjMat, rows, cols);
					ch = _getch();

				} while (ch != 'B' && ch != 'b');

			}

		default:
			cout << "Invalid input\n";
			break;
		}

	} while (ch != 'q' && ch != 'Q');


	return 0;
}


int** convertAdj(char map[][20], int rows, int cols) {
	int N = rows * cols;
	// Initialize adjacency matrix with infinite weights
	//int adj[N][N];
	int** adjArr = new int* [N]; //Dynamic 2d Array
	for (int i = 0; i < N; i++) {
		adjArr[i] = new int[N];
	}
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			adjArr[i][j] = 0;
		}
	}

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			int node = i * cols + j; // Compute node number
			if (map[i][j] != '#' && map[i][j] != '%')
			{ // If not an obstacle


				if (i > 0 && map[i - 1][j] != '#' && map[i - 1][j] != '#' && map[i - 1][j] != '%')
				{ // Check top neighbor

					adjArr[node][node - cols] = 1;

				}

				else if (i > 0 && map[i - 1][j] == '#')
				{
					adjArr[node][node - cols] = 100;
				}

				else if (i > 0 && map[i - 1][j] == '%')
				{
					adjArr[node][node - cols] = 2;
				}


				if (j > 0 && map[i][j - 1] != '#' && map[i][j - 1] != '%') { // Check left neighbor
					adjArr[node][node - 1] = 1;
				}

				else if (j > 0 && map[i][j - 1] == '#')
				{
					adjArr[node][node - 1] = 100;
				}

				else if (j > 0 && map[i][j - 1] == '%')
				{
					adjArr[node][node - 1] = 2;
				}

				if (i < rows - 1 && map[i + 1][j] != '#' && map[i + 1][j] != '%') { // Check bottom neighbor
					adjArr[node][node + cols] = 1;
				}

				else if (i < rows - 1 && map[i + 1][j] == '#')
				{
					adjArr[node][node + cols] = 100;
				}

				else if (i < rows - 1 && map[i + 1][j] == '%')
				{
					adjArr[node][node + cols] = 2;
				}

				if (j < cols - 1 && map[i][j + 1] != '#' && map[i][j + 1] != '%') { // Check right neighbor
					adjArr[node][node + 1] = 1;
				}

				else if (j < cols - 1 && map[i][j + 1] == '#')
				{
					adjArr[node][node + 1] = 100;
				}

				else if (j < cols - 1 && map[i][j + 1] == '%')
				{
					adjArr[node][node + 1] = 2;
				}
			}
			else { // If obstacle
				//adjArr[node][node] = 100; // Set self-loop to weight 100
				if (i > 0 && map[i - 1][j] != '#')
				{ // Check top neighbor

					adjArr[node][node - cols] = 100;

				}

				else if (i > 0 && map[i - 1][j] == '#')
				{
					adjArr[node][node - cols] = 100;
				}


				if (j > 0 && map[i][j - 1] != '#') { // Check left neighbor
					adjArr[node][node - 1] = 100;
				}

				else if (j > 0 && map[i][j - 1] == '#')
				{
					adjArr[node][node - 1] = 100;
				}

				if (i < rows - 1 && map[i + 1][j] != '#') { // Check bottom neighbor
					adjArr[node][node + cols] = 100;
				}

				else if (i < rows - 1 && map[i + 1][j] == '#')
				{
					adjArr[node][node + cols] = 100;
				}

				if (j < cols - 1 && map[i][j + 1] != '#') { // Check right neighbor
					adjArr[node][node + 1] = 100;
				}

				else if (j < cols - 1 && map[i][j + 1] == '#')
				{
					adjArr[node][node + 1] = 100;
				}
			}
		}
	}

	return adjArr;
}

int calcmindistance(int dist[], bool sptSet[], int rows, int cols) { //selects vertex with minimum distance from source that hasnt been visited
	int min = INT_MAX, min_index;

	for (int v = 0; v < rows * cols; v++) {
		if (!sptSet[v] && dist[v] <= min) {
			min = dist[v], min_index = v;
		}
	}
	return min_index;
}


void sendtoAvl(char* charArr, int pathlen) {
	cout << endl;
	avlTree obj;
	int score = 0;
	for (int i = 0; i < pathlen; i++) {
		if (charArr[i] == 'P') {
			obj.insert(70);
			score = score + 70;
		}
		else if (charArr[i] == 'J') {
			obj.insert(50);
			score = score + 50;
		}
		else if (charArr[i] == 'W') {
			obj.insert(30);
			score = score + 30;
		}
		else if (charArr[i] == '@') {
			score = score - 30;
		}
		else if (charArr[i] == '$') {
			score = score - 70;
		}
		else if (charArr[i] == '&') {
			score = score - 50;
		}
		else if (charArr[i] == 'C') {
			continue;
		}

	}
	cout << endl;
	cout << "Inventory Items:" << endl;
	cout << "-----------------" << endl;
	avlNode* rootx = obj.returnRoot();
	print_inorder(rootx);
	cout << "-----------------" << endl;
	cout << "Game Score at the end: ";
	cout << score;
}

void printcharNodes(char* charArr, int pathlen) {
	cout << endl;
	cout << "Path taken by Player: ";
	for (int i = 0; i < pathlen; i++) {
		cout << " " << charArr[i] << " ";
	}
}

char* printShortestPath(int* shortestPath, int pathLen, char map[][20]) {
	char* charPath = new char[pathLen];
	for (int i = 0; i < pathLen; i++) {
		if (i == pathLen - 1) {
			cout << "(" << shortestPath[i] / 20 << ", " << shortestPath[i] % 20 << ")";
		}
		else {
			cout << "(" << shortestPath[i] / 20 << ", " << shortestPath[i] % 20 << ") -> ";
		}
		charPath[i] = map[shortestPath[i] / 20][shortestPath[i] % 20];
	}
	cout << endl;
	return charPath;

}

int* findShortestPath(int parent[], int i, int& pathLen) { //traverses the parent nodes from the destination node back to the source node to construct shortest path.
	int* path = new int[400];
	pathLen = 0;

	while (parent[i] != -1) {
		path[pathLen++] = i;
		i = parent[i];
	}

	path[pathLen++] = i; //source node added to path array

	int* shortestPath = new int[pathLen];
	for (int j = pathLen - 1, k = 0; j >= 0; j--, k++) {
		shortestPath[k] = path[j];
	}

	delete[] path;

	return shortestPath;
}

void dijkstra(int** graph, int src, int dest, int rows, int cols, char map[][20]) {
	int distance[20 * 20];
	bool visited[20 * 20];
	int parent[20 * 20];

	for (int i = 0; i < rows * cols; i++) {
		distance[i] = INT_MAX;
		visited[i] = false;
		parent[i] = -1;
	}

	distance[src] = 0;

	for (int count = 0; count < rows * cols - 1; count++) {
		int u = calcmindistance(distance, visited, rows, cols); // u is the index of node having min distance 
		visited[u] = true;

		if (u == dest) {
			break;
		}

		for (int v = 0; v < rows * cols; v++) {
			if (graph[u][v] && !visited[v] && distance[u] != INT_MAX && distance[u] + graph[u][v] < distance[v])
			{
				distance[v] = distance[u] + graph[u][v];
				parent[v] = u;
			}
		}
	}

	if (parent[dest] == -1) {
		cout << "No path exists between (" << src << ") and (" << dest << ")";
	}
	else {
		cout << "Shortest path from (" << src << ") to (" << dest << ") is: ";
		cout << endl;
		int pathlen;
		int* shortestPath = findShortestPath(parent, dest, pathlen);
		char* charNodes = printShortestPath(shortestPath, pathlen, map);
		cout << " with cost = " << distance[dest];
		printcharNodes(charNodes, pathlen);
		sendtoAvl(charNodes, pathlen);
	}
}

void floydWarshall(int** graph, int src, int dest, int rows, int cols, char map[][20])
{

	int** dist = new int* [rows * cols];

	for (int i = 0; i < rows * cols; i++)
	{
		dist[i] = new int[rows * cols];
	}

	int** parent = new int* [rows * cols];

	for (int i = 0; i < rows * cols; i++)
	{
		parent[i] = new int[rows * cols];
	}


	for (int i = 0; i < rows * cols; i++) {
		for (int j = 0; j < rows * cols; j++) {
			dist[i][j] = graph[i][j];
			parent[i][j] = j;
		}
	}

	for (int i = 0; i < rows * cols; i++)
	{
		for (int j = 0; j < rows * cols; j++)
		{
			if (dist[i][j] == 0)
			{
				dist[i][j] = INF;
			}
		}

	}


	for (int k = 0; k < rows * cols; k++)
	{
		for (int i = 0; i < rows * cols; i++)
		{
			for (int j = 0; j < rows * cols; j++)
			{
				if (dist[i][k] != INF && dist[k][j] != INF && dist[i][k] + dist[k][j] < dist[i][j])
				{
					dist[i][j] = dist[i][k] + dist[k][j];
					//cout << dist[i][j] << " ";
					parent[i][j] = parent[i][k];


				}

			}
		}
	}


	if (dist[src][dest] == INF)
	{
		cout << "No path exists between (" << src << ") and (" << dest << ")";
	}

	else
	{
		cout << "Shortest path from (" << src << ") to (" << dest << ") is: ";
		int current = src;
		int current2 = src;
		//cout << current << " ";
		cout << "(" << current / 20 << ", " << current % 20 << ") -> ";

		while (current != dest)
		{
			current = parent[current][dest];
			//cout << current << " ";
			cout << "(" << current / 20 << ", " << current % 20 << ") -> ";
		}

		cout << endl;
		cout << "with cost= " << dist[src][dest] << " ";

		cout << endl;
		cout << "Path taken by Player: " << map[current2 / 20][current2 % 20];

		while (current2 != dest)
		{
			current2 = parent[current2][dest];
			cout << " " << map[current2 / 20][current2 % 20];
		}
	}
}


int minKey(int key[], bool visited[]) {
	int min = INF, min_index;
	for (int v = 0; v < 20 * 20; v++) {
		if (visited[v] == false && key[v] < min) {
			min = key[v];
			min_index = v;
		}
	}
	return min_index;
}

void printMST(int parent[], int** graph) {
	cout << "Minimum Spanning Tree:" << endl;
	for (int i = 1; i < 20 * 20; i++) {
		cout << "(" << parent[i] / 20 << ", " << parent[i] % 20 << ") -- (" << i / 20 << ", " << i % 20 << ")   weight: " << graph[i][parent[i]] << endl;
	}
}

void primMST(int** graph) {
	int parent[20 * 20];
	int key[20 * 20];
	bool visited[20 * 20];

	for (int i = 0; i < 20 * 20; i++) {
		key[i] = INF;
		visited[i] = false;
	}

	key[0] = 0;
	parent[0] = -1;

	for (int count = 0; count < (20 * 20) - 1; count++) {
		int u = minKey(key, visited);

		visited[u] = true;

		for (int v = 0; v < 20 * 20; v++) {
			if (graph[u][v] && visited[v] == false && graph[u][v] < key[v]) {
				parent[v] = u;
				key[v] = graph[u][v];
			}
		}
	}

	printMST(parent, graph);
}

void printkruskalmst(int cost, knode* n) {
	knode* temp = n;
	cout << "The minimum spanning tree is : " << endl;
	while (temp != NULL) {
		cout << "(" << temp->x << "," << temp->y << ")" << ",";
		temp = temp->next;
	}
	cout << endl << "The cost of the MST is " << cost << endl;
}
void kruskal(int** adjmat, int rows, int cols) {
	bool flag = true;
	bool flag2 = true;
	knode* head = new knode(1, 1);
	int cost = 0;
	int N = rows * cols;
	cout << "The minimum spanning tree is : " << endl;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (adjmat[i][j] == 1) {
				if (flag == true) {
					cout << "(" << i << "," << j << ")" << ",";
					knode* newn = new knode(i, j);
					head->next = newn;
					flag = false;
					cost += adjmat[i][j];
					head = head->next;
				}
				else {
					knode* temp = head;
					knode* pretemp;
					while (temp != NULL && flag2) {
						if ((temp->x == i && temp->y == j) || (temp->x == j && temp->y == i)) {
							//creating cycle
							flag2 = false;
						}
						else {
							//not creating a cycle and adding edge
							flag2 = true;
						}
						pretemp = temp;
						temp = temp->next;
					}
					if (flag2 == true) {
						cout << "(" << i << "," << j << ")" << ",";
						//knode* n2 = new knode(i, j);
						//pretemp = n2;
						cost += adjmat[i][j];
					}
				}
			}
		}
	}
	//printkruskalmst(cost, head);

	cout << endl << "The cost of the MST is " << cost << endl;
}
