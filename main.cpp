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

// TODO remove
//bool operator==(Edge edge1, Edge edge2) {
//    return edge1.begin == edge2.begin && edge1.end == edge2.end && edge1.weight == edge2.weight;
//}

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

// TODO inicjalizacja edgami
class Graph {
public:
    int size;
    int currentSize;
    Node *nodes; // list of nodes in graph
    vector<Edge> minEdges; // list of minimal edges

    Graph(int size) {
        this->size = size;
        this->currentSize = 0;
        nodes = new Node[size];
    }

    void addNode(int number, Node node) {
        nodes[number] = node;
    }

    void addNodeFromArray(int number, int weights[], int weightsSize) {
        nodes[number] = Node(number, vector<int>(weights, weights + weightsSize));
    }

    void findMinPath() {
        for (int i = 0; i < size; i++) {
            minEdges.push_back(this->nodes[i].findMinWeight());
        }
        removeDuplicates();
        print();
    }

    void removeDuplicates() {
        for (auto i = minEdges.begin(); i != minEdges.end()-1; i++) {
            for (auto j = i + 1; j != minEdges.end(); j++) {
                if (i->begin == j->begin && i->end == j->end && i->weight == j->weight) {
                    minEdges.erase(j);
                }
            }
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

int main() {
    //    int weightsSize = sizeof(weights0) / sizeof(weights0[0]);
    int graphSize = 6;
    Graph graph = Graph(graphSize);

    int weights0[] = {0, 3, 8, 4, 5, 0};
    int weights1[] = {3, 0, 5, 0, 0, 0};
    int weights2[] = {8, 5, 0, 0, 0, 0};
    int weights3[] = {4, 0, 0, 0, 4, 9};
    int weights4[] = {5, 0, 0, 4, 0, 8};
    int weights5[] = {0, 0, 0, 9, 8, 0};

    graph.addNodeFromArray(0, weights0, graphSize);
    graph.addNodeFromArray(1, weights1, graphSize);
    graph.addNodeFromArray(2, weights2, graphSize);
    graph.addNodeFromArray(3, weights3, graphSize);
    graph.addNodeFromArray(4, weights4, graphSize);
    graph.addNodeFromArray(5, weights5, graphSize);

    cout<<"HERE\n\n\n";
    graph.findMinPath();
}
