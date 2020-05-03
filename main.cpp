#include <iostream>
#include <vector>

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
            if (weights[i] != 0 && weights[i] < min) {
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
        bool isDuplicated = false;
        for (int i = 0; i < minEdges.size(); i++) {
            for (int j = 0; j < newVector.size(); j++) {
                Edge e1 = minEdges[i];
                Edge e2 = minEdges[j];
                if (e1.begin == e2.begin && e1.end == e2.end && e1.weight == e2.weight) {
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

    void print() {
        int sum = 0;
        for (int i = 0; i < minEdges.size(); i++) {
            cout<<this->minEdges[i];
            sum += this->minEdges[i].weight;
        }
        cout<<"Sum = "<<sum<<endl;
    }
};

// todo wpisywanie krawedzi z linii komend
int main() {
    int graphSize = 8;
    Graph graph = Graph(graphSize);

//    int weights0[] = {0, 3, 8, 4, 5, 0};
//    int weights1[] = {3, 0, 5, 0, 0, 0};
//    int weights2[] = {8, 5, 0, 0, 0, 0};
//    int weights3[] = {4, 0, 0, 0, 4, 9};
//    int weights4[] = {5, 0, 0, 4, 0, 8};
//    int weights5[] = {0, 0, 0, 9, 8, 0};
//
//    graph.addNodeFromArray(0, weights0, graphSize);
//    graph.addNodeFromArray(1, weights1, graphSize);
//    graph.addNodeFromArray(2, weights2, graphSize);
//    graph.addNodeFromArray(3, weights3, graphSize);
//    graph.addNodeFromArray(4, weights4, graphSize);
//    graph.addNodeFromArray(5, weights5, graphSize);

// graph 1 - sum 24
//    graph.addEdge(Edge(0, 1, 3));
//    graph.addEdge(Edge(0, 2, 8));
//    graph.addEdge(Edge(0, 3, 4));
//    graph.addEdge(Edge(0, 4, 5));
//    graph.addEdge(Edge(1, 2, 5));
//    graph.addEdge(Edge(3, 4, 4));
//    graph.addEdge(Edge(3, 5, 9));
//    graph.addEdge(Edge(4, 5, 8));


// graph 2 - sum 26
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

    cout<<"HERE\n\n\n";
    graph.findMinPath();
}
