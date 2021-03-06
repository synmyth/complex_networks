#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include "graph.h"

using namespace std;

namespace complex_networks {

// wrapper function for opening file for read
static inline void openFileForRead(ifstream &infile, const string &path)
{
	if (infile.is_open()) {
		infile.close();
	}

	infile.clear();
	infile.open(path);
	if (!infile) {
		throw runtime_error("open file error");
	}
}

// get vertex number from opened file
static size_t getVertexSizeFromFile(ifstream &infile)
{
	unsigned long size = 0;
	string line;
	while (getline(infile, line)) {
		istringstream stream(line);
		unsigned long startVertex, endVertex;
		stream >> startVertex >> endVertex;
		size = max(size, max(startVertex, endVertex));
	}

	return size + 1;
}

// constructor for class Graph
Graph::Graph(const string &path)
{
	// set vertex size first
	ifstream infile;
	openFileForRead(infile, path);
	setVertexSize(getVertexSizeFromFile(infile));
	infile.close();

	// read edges
	openFileForRead(infile, path);
	string line;
	while (getline(infile, line)) {
		istringstream stream(line);
		unsigned long startVertex, endVertex;
		stream >> startVertex >> endVertex;
		Edge edge(startVertex, endVertex);
		if (!addEdge(edge)) {
			infile.close();
			throw runtime_error("add edge to graph failed");
		}
	}
	shrinkMemory();

	infile.close();
}

// add edge to the graph
bool Graph::addEdge(const Edge &edge)
{
	auto startId = edge.startVertex.id;
	auto endId = edge.endVertex.id;
	if (startId >= getVertexSize() || endId >= getVertexSize()) {
		return false;
	}

	// in general, this is an undirected graph
	links.at(startId).push_back(edge.endVertex);
	links.at(endId).push_back(edge.startVertex);
	edges.insert(edge);

	return true;
}

// check this graph has only one component
bool Graph::isOneComponent() const
{
	enum Color {
		WHITE, GRAY, BLACK
	};

	if (links.size() == 0) {
		throw runtime_error("no vertex in this graph");
	}

	vector<Color> visitFlag(links.size(), WHITE);
	queue<unsigned long> q;

	// find the first exists vertex
	unsigned long start = 0;
	while (links.at(start).size() == 0) {
		start++;
	}
	visitFlag.at(start) = GRAY;
	q.push(start);

	// traverse the neighbor vertex
	while (!q.empty()) {
		auto current = q.front();
		q.pop();
		if (visitFlag.at(current) == BLACK) {
			continue;
		}

		visitFlag.at(current) = BLACK;
		for (auto& neighbor: links.at(current)) {
			if (visitFlag.at(neighbor.id) == WHITE) {
				visitFlag.at(neighbor.id) = GRAY;
				q.push(neighbor.id);
			}
		}
	}

	for (auto id = start; id != getVertexSize(); ++id) {
		if (visitFlag.at(id) != BLACK && links.at(id).size() > 0) {
			return false;
		}
	}
	return true;
}

// print the graph to stdout
void Graph::print() const
{
	cout << "vertex size " << getVertexSize() << endl;
	cout << "edge size " << getEdgeSize() << endl;
	cout << "links:" << endl;
	for (auto ix = 0; ix != getVertexSize(); ++ix) {
		cout << ix << ": ";
		for (auto& neighbor: links.at(ix)) {
			cout << neighbor.id << " ";
		}
		cout << endl;
	}
}

}
