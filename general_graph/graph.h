#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <string>
#include <vector>
#include <unordered_set>
#include <functional>
#include <stdexcept>

namespace complex_networks {

struct Vertex {
	Vertex(unsigned long id): id(id) { }
	Vertex(const Vertex &rhs): id(rhs.id) { }
	~Vertex() { }
	Vertex& operator=(const Vertex &rhs) { id = rhs.id; }

	unsigned long id;
};

struct Edge {
	Edge(const Vertex &startVertex, const Vertex &endVertex):
		startVertex(startVertex), endVertex(endVertex) { }
	Edge(unsigned long startId, unsigned endId):
		startVertex(startId), endVertex(endId) { }
	Edge(const Edge &rhs):
		startVertex(rhs.startVertex), endVertex(rhs.endVertex) { }
	~Edge() { }
	inline Edge& operator=(const Edge &rhs);
	inline void setVertices(const Vertex &startVertex, const Vertex &endVertex);

	Vertex startVertex;
	Vertex endVertex;
};

// assignment operator for class Edge
Edge& Edge::operator=(const Edge &rhs) {
	startVertex = rhs.startVertex;
	endVertex = rhs.endVertex;
}

// set vertices for this edeg
void Edge::setVertices(const Vertex &startVertex, const Vertex &endVertex) {
	this->startVertex = startVertex;
	this->endVertex = endVertex;
}

inline bool operator==(const Vertex &lhs, const Vertex &rhs)
{
	return lhs.id == rhs.id;
}

inline bool operator==(const Edge &lhs, const Edge &rhs)
{
	return (lhs.startVertex == rhs.startVertex &&
		lhs.endVertex == rhs.endVertex) ||
		(lhs.startVertex == rhs.endVertex &&
		 lhs.endVertex == rhs.startVertex);
}

}

namespace std {

template<> struct hash<complex_networks::Vertex>
{
	size_t operator()(const complex_networks::Vertex &vertex) const
	{
		return hash<unsigned long>()(vertex.id);
	}
};

template<> struct hash<complex_networks::Edge>
{
	size_t operator()(const complex_networks::Edge &edge) const
	{
		size_t h1 = hash<complex_networks::Vertex>()(edge.startVertex);
		size_t h2 = hash<complex_networks::Vertex>()(edge.endVertex);
		return h1 ^ (h2 << 1);
	}
};

}

namespace complex_networks {

class Graph {
public:
	Graph(const std::string &path);
	Graph(const Graph &rhs):
		links(rhs.links), vertices(rhs.vertices), edges(rhs.edges) { }
	~Graph() { }
	inline Graph& operator=(const Graph &rhs);
	size_t getVertexSize() { return links.size(); }
	inline void setVertexSize(size_t size);
	size_t getEdgeSize() { return edges.size(); }
	const std::vector<std::vector<Vertex>>& getLinks() { return links; }
	const std::unordered_set<Vertex>& getVertices() { return vertices; }
	const std::unordered_set<Edge>& getEdges() { return edges; }
	bool addEdge(const Edge &edge);
	bool isOneComponent();
	inline void shrinkMemory();
	void print();

protected:
	inline bool addVertex(const Vertex &vertex);

private:
	std::vector<std::vector<Vertex>> links;
	std::unordered_set<Vertex> vertices;
	std::unordered_set<Edge> edges;
};

// assignment operator for class Graph
Graph& Graph::operator=(const Graph &rhs)
{
	links = rhs.links;
	vertices = rhs.vertices;
	edges = rhs.edges;
}

// set vertices size
void Graph::setVertexSize(size_t size)
{
	if (size < getVertexSize()) {
		throw std::runtime_error("new size is less than old size");
	}

	links.resize(size);
}

// shrink the memory usage for this graph
void Graph::shrinkMemory()
{
	for (auto& it: links) {
		it.shrink_to_fit();
	}
}

// add vertex to the graph
bool Graph::addVertex(const Vertex &vertex)
{
	if (vertex.id >= getVertexSize()) {
		return false;
	}

	vertices.insert(vertex);
	return true;
}

}

#endif
