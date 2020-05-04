#include <iostream>
#include <vector>
//#include "mpi.h"
#include <climits>
#include <fstream>

using namespace std;

class Edge {
public:
    int begin;
    int end;
    int weight;

    Edge() = default;
    Edge(int begin, int end, int weight) {
        if (begin > end) {
            int temp;
            temp = begin;
            begin = end;
            end = temp;
        }
        this->begin = begin;
        this->end = end;
        this->weight = weight;
    }
};

ostream & operator<<(ostream & out, Edge edge) {
    out << "(" << edge.begin << ", " << edge.end << ")" << " = " << edge.weight << endl;
    return out;
}

class Node {
public:
    int number{};
    vector<int> weights; // list of weights to other nodes

    Node() = default;
    Node(int number, vector<int> weights) {
        this->number = number;
        this->weights = vector<int>(weights.begin(), weights.end());
    }

    Edge findMinWeight() {
        int min = INT_MAX;
        int index = number;
        for (int i = 0; i < weights.size(); i++) {
            if (weights[i] != 0 && weights[i] < min) {  // weight 0 is for nodes where it cant get or for node itself
                min = weights[i];
                index = i;
            }
        }
        return Edge(number, index, weights[index]);
    }
};

class Graph {
public:
    int size;
    Node *nodes; // list of nodes in graph
    vector<Edge> minEdges; // list of minimal edges

    Graph() = default;
    Graph(int size) {
        this->size = size;
        nodes = new Node[size];
        for (int i = 0; i < size; i++) {
            nodes[i].number = i;
            nodes[i].weights = vector<int>(size, 0);    // initialization with zeros of each node weights
        }
    }

    void addNode(int number, Node node) {
        nodes[number] = node;
    }

    // was useful for first tests, now its to be removed probably
    void addNodeFromArray(int number, int weights[], int weightsSize) {
        nodes[number] = Node(number, vector<int>(weights, weights + weightsSize));
    }

    void addEdge(Edge edge) {
        nodes[edge.begin].weights[edge.end] = edge.weight;
        nodes[edge.end].weights[edge.begin] = edge.weight;
    }

    void findMinPath() {
        for (int i = 0; i < size; i++) {
            minEdges.push_back(this->nodes[i].findMinWeight());
        }
        removeDuplicates();
        print();
    }

    void removeDuplicates() {
        vector<Edge> newVector;
        for (int i = 0; i < minEdges.size(); i++) {
            if (newVector.size() >= size - 1) {
                break;
            }
            bool isDuplicated = false;
            for (int j = 0; j < newVector.size(); j++) {
                Edge e1 = minEdges[i];
                Edge e2 = newVector[j];
                if (e1.begin == e2.begin && e1.end == e2.end && e1.weight == e2.weight) {
                    lookForNewEdge(e1);
                    isDuplicated = true;
                    break;
                }
            }
            if (!isDuplicated) {
                newVector.push_back(minEdges[i]);
            }
        }
        minEdges = newVector;
    }

    void lookForNewEdge(Edge e1) {
        Node* n1 = &nodes[e1.begin];
        Node* n2 = &nodes[e1.end];
        n1->weights[n2->number] = 0;
        n2->weights[n1->number] = 0;

        // look for smaller edge
        Edge newEdge1 = n1->findMinWeight();
        Edge newEdge2 = n2->findMinWeight();
        if (newEdge1.weight < newEdge2.weight) {
            minEdges.push_back(newEdge1);
        } else {
            minEdges.push_back(newEdge2);
        }
    }

    void print() {
        int sum = 0;
        for (int i = 0; i < minEdges.size(); i++) {
            cout<<this->minEdges[i];
            sum += this->minEdges[i].weight;
        }
        cout<<"Sum = "<<sum<<endl;
    }
};

Graph initGraphFromFile(string path) {
    int graphSize, edgesCount, begin, end, weight;
    ifstream file(path);
    file >> graphSize;
    file >> edgesCount;
    Graph graph = Graph(graphSize);
    for (int i = 0; i < edgesCount; i++) {
        file >> begin;
        file >> end;
        file >> weight;
        graph.addEdge(Edge(begin, end, weight));
    }
    return graph;
}

Graph initGraphFromCMD() {
    int graphSize, edgesCount, begin, end, weight;
    cout << "Podaj liczbe wierzcholkow w grafie\n";
    cin >> graphSize;
    cout << "Podaj liczbe krawedzi w grafie\n";
    cin >> edgesCount;
    Graph graph = Graph(graphSize);
    for (int i = 0; i < edgesCount; i++) {
        cout << "Punkt poczatkowy:\n";
        cin >> begin;
        cout << "Punkt koncowy:\n";
        cin >> end;
        cout << "Waga:\n";
        cin >> weight;
        graph.addEdge(Edge(begin, end, weight));
        cout << endl;
    }
    return graph;
}

Graph initGraphFromDefault() {
// graph2 - 8 nodes, sum 26
    Graph graph = Graph(8);
    graph.addEdge(Edge(0, 1, 5));
    graph.addEdge(Edge(0, 3, 9));
    graph.addEdge(Edge(0, 6, 3));
    graph.addEdge(Edge(1, 2, 9));
    graph.addEdge(Edge(1, 4, 8));
    graph.addEdge(Edge(1, 5, 6));
    graph.addEdge(Edge(1, 7, 7));
    graph.addEdge(Edge(2, 3, 9));
    graph.addEdge(Edge(2, 4, 4));
    graph.addEdge(Edge(2, 6, 5));
    graph.addEdge(Edge(2, 7, 3));
    graph.addEdge(Edge(3, 6, 8));
    graph.addEdge(Edge(4, 5, 2));
    graph.addEdge(Edge(4, 6, 1));
    graph.addEdge(Edge(5, 6, 6));
    graph.addEdge(Edge(6, 7, 9));
    return graph;
}

int showMenu() {
    int option;
    cout << "Algorytm Prima wyznaczajacy minimalne drzewo rozpinajace\n\n";
    cout << "Wybierz opcje uruchomienia programu\n";
    cout << "1 - graf inicjalizowany z pliku\n";
    cout << "2 - graf inicjalizowany z linii komend\n";
    cout << "inna - obliczenie przykladowego przypadku\n";
    cin >> option;
    return option;
}

// TODO imitation of multiple processes
int main(int argc, char *argv[]) {
    int option = showMenu();
    string path;
    Graph graph;
    switch (option) {
        case 1:
            cout << "Podaj nazwe pliku (wraz z rozszerzeniem)\n";
            cin>>path;
            graph = initGraphFromFile(path);
            break;
        case 2:
            graph = initGraphFromCMD();
            break;
        default:
            graph = initGraphFromDefault();
            break;
    }

//    int myid, numprocs;
//    MPI_Init(&argc, &argv);
//    MPI_Comm world = MPI_COMM_WORLD;
//    MPI_Comm_size (world, &numprocs);
//    MPI_Comm_rank(world, &myid);



//    if (myid == 0) {
        graph.findMinPath();
//    }
}
