#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <sstream>
#include <stdio.h>
#include <assert.h>
#include <climits>

#include "timer.c"
using namespace std;


struct AdjListNode
{
    int vertex;
    float weight;
    AdjListNode *next;
};
struct AdjList
{
    AdjListNode *head;
};
AdjListNode *newNode(int vertex, float weight)
{
    AdjListNode* node = new AdjListNode();
    node->vertex = vertex;
    node->weight = weight;
    node->next = NULL;
    return node;
}
class Graph {
public:
    int size;
    AdjList *array;
    Graph()
    {
        
    }
    Graph(int num_verticies)
    {
        size = num_verticies;
        array = new AdjList[num_verticies];
        for(int i = 0; i < num_verticies; i++)
        {
            array[i].head = NULL;
        }
    }
    void addGraphEdge(int s, int d, float weight)
    {
        // d = destination edge, s = source edge
        AdjListNode *node = newNode(d, weight);
        node->next = array[s].head;
        array[s].head = node;
        
        node = newNode(s, weight);
        node->next = array[d].head;
        array[d].head = node;
    }
};
struct PriorityQueueNode
{
    int vertex;
    float weight;
};
class PriorityQueue
{
public:
    PriorityQueueNode **array;
    int *pos;
    int pq_size;
    int capacity;
    PriorityQueue()
    {
        
    }
    PriorityQueue(int size)
    {
        pq_size = 0;
        array = new PriorityQueueNode*[size];
        pos = new int[size];
        capacity = size;
    }
    int Parent(int i)
    {
        return (i - 1) / 2;
    }

    int Left(int i)
    {
        return (2 * i + 1);
    }

    int Right(int i)
    {
        return (2 * i + 2);
    }
    // fix heap
    void minHeapify(int i)
    {
        int left = Left(i);
        int right = Right(i);

        int smallest = i;

        if (left < pq_size && array[left]->weight < array[i]->weight)
            smallest = left;

        if (right < pq_size && array[right]->weight < array[smallest]->weight)
            smallest = right;

        if (smallest != i)
        {
            PriorityQueueNode *smallest_pqn = array[smallest];
            PriorityQueueNode *current = array[i];
            pos[smallest_pqn->vertex] = i;
            pos[current->vertex] = smallest;
            swap(array[i], array[smallest]);
            minHeapify(smallest);
        }
    }
    bool empty()
    {
        return pq_size == 0;
    }
    void PrintArray()
    {
        cout << "{";
        for(int i = 0; i < pq_size; i++)
        {
            cout << array[i]->vertex << " ";
        }
        cout << "}" << endl;
    }
    // insert key into array
    void insertKey(int v, float key)
    {
        PriorityQueueNode* pqnode = (struct PriorityQueueNode*)malloc(sizeof(struct PriorityQueueNode));
        pqnode->vertex = v;
        pqnode->weight = key;
        array[v] = pqnode;
    }
    // remove root node and heapify
    PriorityQueueNode* extractMin()
    {
        if(isEmpty())
        {
            return NULL;
        }
        if(pq_size == 1)
        {
            pq_size--;
            return array[0];
        }
        PriorityQueueNode *temp = array[0];
        PriorityQueueNode *last = array[pq_size-1];
        array[0] = last;
        pos[temp->vertex] = pq_size-1;
        pos[last->vertex] = 0;
        pq_size--;
        minHeapify(0);
        return temp;
    }
    void decreaseKey(int i, float num)
    {
        int w = pos[i];
        array[w]->weight = num;
        while (w && array[w]->weight < array[Parent(w)]->weight)
        {
            pos[array[w]->vertex] = Parent(w);
            pos[array[Parent(w)]->vertex] = w;
            swap(array[w], array[Parent(w)]);
            w = Parent(w);
        }
    }
    bool Exists(int vertex)
    {
        if(pos[vertex] < pq_size)
        {
            return true;
        }
        return false;
    }
    PriorityQueueNode* top()
    {
        return array[0];
    }
    bool isEmpty()
    {
        return (pq_size == 0);
    }
};
void printMST(int parent_set[], float key[], int num_verticies)
{
    ofstream outputfile;
    outputfile.open("mst.txt");
    float sum = 0;
    for(int i = 0; i < num_verticies; i++)
    {
        if(parent_set[i] == -1)
        {
            continue;
        }
        sum += key[i];
    }
    outputfile << sum << endl;
    for(int i = 1; i < num_verticies; i++)
    {
        if(parent_set[i] == -1)
        {
            continue;
        }
        outputfile << parent_set[i] << " " <<  i << " " << key[i] << endl;
    }
}
void Prim(Graph graph)
{
    stopwatch_init ();
    struct stopwatch_t* timer = stopwatch_create ();
    assert (timer);
    stopwatch_start (timer);
    long double t_ms = 0;
    int num_verticies = graph.size;
    int parent_set[num_verticies];
    float key[num_verticies];
    PriorityQueue pq(num_verticies);
    for(int i = 1; i < num_verticies; i++)
    {
        parent_set[i] = -1; // initialization
        key[i] = INT_MAX; // basically infinity
        pq.insertKey(i, key[i]);
        pq.pos[i] = i;
    }
    key[0] = 0;
    pq.insertKey(0, key[0]);
    pq.pos[0] = 0;
    pq.pq_size = num_verticies;
    while(!pq.isEmpty())
    {
        PriorityQueueNode *node = pq.extractMin();
        int u = node->vertex;
        AdjListNode *current = graph.array[u].head;
        while(current != NULL)
        {
            int v = current->vertex;
            if(pq.Exists(v) && current->weight < key[v])
            {
                key[v] = current->weight;
                parent_set[v] = u;
                pq.decreaseKey(v, key[v]);
            }
            current = current->next;
        }
    }
    t_ms = stopwatch_stop (timer);
    cout << "Time: " << t_ms << endl;
    printMST(parent_set, key, num_verticies);
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        cout << "Missing arguments! Please run in the following format: ./prim filename" << endl;
        return 1;
    }
    ifstream inputfile;
    inputfile.open(argv[1]);
    string line;
    string verticies;
    getline(inputfile, verticies);
    int size = atoi(verticies.c_str());
    Graph graph(size);
    while(getline(inputfile, line))
    {
        if(line == "")
        {
            continue;
        }
        int s, d;
        float weight;
        istringstream iss(line);
        iss >> s;
        iss >> d;
        iss >> weight;
        graph.addGraphEdge(s, d, weight);
    }
    Prim(graph);
    inputfile.close();
    return 0;
}
