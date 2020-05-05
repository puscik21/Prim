#include <iostream>
#include <vector>
#include "mpi.h"
#include <climits>
#include <fstream>

#define rootId 0;

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

    void findMinPath(int mpiId, int numprocs) {
        int edgeValues[3];
        if (mpiId == 0) {
            MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
            do {
                MPI_Recv(edgeValues, 3, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);     // master receive edge
                Edge edge = Edge(edgeValues[0], edgeValues[1], edgeValues[2]);
                minEdges.push_back(edge);
            } while (minEdges.size() < size);
            removeDuplicates();
            print();
        } else {
            MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
            for (int i = mpiId - 1; i < size; i+=numprocs-1) {  // -1 because its without master
                Edge edge = this->nodes[i].findMinWeight();
                edgeValues[0] = edge.begin;
                edgeValues[1] = edge.end;
                edgeValues[2] = edge.weight;
                MPI_Send(edgeValues, 3, MPI_INT, 0, 0, MPI_COMM_WORLD);     // send edge to master
            }
        }
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

void initGraphFromFile(int &graphSize, vector<int> &edges, int &edgesCount) {
    string path;
    cout << "Podaj nazwe pliku (wraz z rozszerzeniem)\n";
    cin>>path;
    ifstream file(path);
    file >> graphSize;
    file >> edgesCount;
    int begin;
    int end;
    int weight;

    for (int i = 0; i < edgesCount; i++) {
        file >> begin;
        file >> end;
        file >> weight;
        edges.push_back(begin);
        edges.push_back(end);
        edges.push_back(weight);
    }
}

void initGraphFromCMD(int &graphSize, vector<int> &edges, int &edgesCount) {
    int begin, end, weight;
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
        cout << endl;

        edges.push_back(begin);
        edges.push_back(end);
        edges.push_back(weight);
    }
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
    int myid, numprocs;
    int graphSize, edgesCount;
    int option;     // program mode
    vector<int> edges;
    int* edgesArray;
    Graph graph;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    if (myid == 0) {            // master asks user for mode, slaves wait
        option = showMenu();
        switch (option) {
            case 1:
                initGraphFromFile(graphSize, edges, edgesCount);
                break;
            case 2:
                initGraphFromCMD(graphSize, edges, edgesCount);
                break;
            default:
                graph = initGraphFromDefault();
                break;
        }
        MPI_Bcast(&option, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (option == 1 || option == 2) {
            MPI_Bcast(&graphSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&edgesCount, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Send(&edges[0], edgesCount * 3, MPI_INT, 1, 0, MPI_COMM_WORLD);
        }
    } else {
        MPI_Bcast(&option, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (option == 1 || option == 2) {
            MPI_Bcast(&graphSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&edgesCount, 1, MPI_INT, 0, MPI_COMM_WORLD);
            edges.resize(edgesCount * 3);
            MPI_Recv(&edges[0], edgesCount * 3, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    if (option == 1 || option == 2) {
        graph = Graph(graphSize);
        for (int i = 0; i < edgesCount * 3 - 2; i+=3) {
            graph.addEdge(Edge(edges[i], edges[i+1], edges[i+2]));
        }
    } else {
        graph = initGraphFromDefault();
    }
    graph.findMinPath(myid, numprocs);

    MPI_Finalize();
}
