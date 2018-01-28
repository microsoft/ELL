import os
import functools

import ell


def GetName(node): return node.GetId()


def GetNodeIndex(index, node): return index[GetName(node)]


def GetNodes(model):
    '''Return an array holding all the Node objects of the model.
The output will we of the form [node_1, node_2, ..., node_v]'''
    return [n for n in GetNodeGenerator(model.GetNodes())]


def GetNodeDictionary(nodes):
    '''Return a dictionary mapping Node names to indices. The range
of indices is 1 .. V. This means that 0 cannot be the index of a
valid Node object.

If the input is [node_1, node_2, ..., node_V] then the output is
a mapping 'index' such that index[node_n] = n where n = 1 ... V.
This means that node indices are one-based.

'''
    def g(i): return i + 1, GetName(nodes[i])

    def f(ans, i_name):
        i, name = i_name
        ans[name] = i
        return ans
    temp = [g(i) for i in range(len(nodes))]
    return functools.reduce(f, temp, {})


def GetNodesAndIndex(model):
    '''
    Returns a 1-based array of nodes of the model and mapping from
    the node names to the 1-based indices
    '''
    nodes = GetNodes(model)             # nodes: [0 .. V-1] -> Node
    index = GetNodeDictionary(nodes)    # index: string -> [1 .. V]
    nodes.insert(0, None)               # align nodes with index so that
    # index[GetName[nodes[i]]] == i is True for 1 .. V
    return nodes, index


def CheckAdjacencyList(adj):
    '''Checks the adjacency list for symmetry of edges'''
    V = len(adj) - 1
    for i in range(1, V + 1):
        js = adj[i]
        for j in js:
            k = adj[j].index(i)


def GetNodeGenerator(iter):
    while iter.IsValid():
        yield iter.Get()
        iter.Next()


def GetAdjacencyList(nodes, index):
    def GetIndex(node): return GetNodeIndex(index, node)

    def GetParents(node):
        if node == None:
            return []
        else:
            parents = GetNodeGenerator(node.GetParents())
            return [GetIndex(p) for p in parents]

    def GetDependents(node):
        if node == None:
            return []
        else:
            children = GetNodeGenerator(node.GetDependents())
            return [GetIndex(c) for c in children]
    parentList = [GetParents(n) for n in nodes]
    dependentList = [GetDependents(n) for n in nodes]
    V = len(index)
    # confirm parents and dependents are doubly linked
    # If any there exists a non doubly linked edge
    # then this part will throw an exception
    for i in range(1, V + 1):
        for j in parentList[i]:
            k = dependentList[j].index(i)
        for j in dependentList[i]:
            k = parentList[j].index(i)
    # create the adjacency list
    adj = parentList[:]
    for i in range(1, V + 1):
        adj[i].extend(dependentList[i])
    CheckAdjacencyList(adj)
    return adj


def DFS(u, baggage):
    '''
    Depth first search starting at vertex u where 1 <= u <= V
    baggage contains state
    '''
    (adj, vertexStates, parents, processEdge,
     processVertex, vertices, edges) = baggage
    vertexStates[u] = 'D'
    processVertex(u, vertices)
    for v in adj[u]:
        processEdge(u, v, edges)
        if vertexStates[v] == 'U':
            parents[v] = u
            DFS(v, baggage)
    vertexStates[u] = 'C'


def processVertex(u, vertices):
    try:
        i = vertices.index(u)
        # we should never get here
        print("already seen vertex", u)
    except ValueError:
        # mark the vertex as discovered
        vertices.append(u)


def processEdge(u, v, edges):
    '''
    Record bi-directional edges as you encounter new ones
    '''
    try:
        i = edges.index((u, v))
        # if you get here then you have seen this before
        # no need to record it
    except ValueError:
        # never seen this edge before
        # record the edge in both directions
        edges.append((u, v))
        edges.append((v, u))


def GetGraphStats(adj):
    '''
    Enumerates the subgraphs of a graph

    adj = one-based symmetric adjacency list defining an undirected graph

        e.g.   1 -- 2 -- 3 has adj = [None,[2],[1,3],[2]]

    Returns an array of (V,E) pairs where V and E are the count of
    vertices and edges of each disjoint subgraph
    '''
    parents = [None for x in adj]
    vertexStates = ['U' for x in adj]
    vertexStates[0] = None
    edges = []
    vertices = []
    baggage = (adj, vertexStates, parents, processEdge,
               processVertex, vertices, edges)
    V = 0
    E = 0
    stats = []
    for u in range(1, len(adj)):
        if vertexStates[u] == 'U':
            DFS(u, baggage)
            V1 = len(vertices)
            E1 = len(edges) / 2
            stats.append(((V1 - V), (E1 - E)))
            V = V1
            E = E1
    return stats


def testModel(filename):
    model = ell.model.Model(filename)
    nodes, index = GetNodesAndIndex(model)
    adj = GetAdjacencyList(nodes, index)
    print(filename, end="\t")
    stats = GetGraphStats(adj)
    n = len(stats)
    if n == 0:
        print("No subgraphs")
    else:
        if n > 1:
            print("%d subgraphs" % (len(subgraphs)))
        elif n == 1:
            print("One subgraph")
        print("    Subgraph Vertices Edges Cycles")
        for i in range(n):
            (V, E) = stats[i]
            C = E - V + 1  # number of cycles
            print("    %-8d %-8d %-5d %-5d" % (i, V, E, C))


def test():
    prefix = "../../../examples/models"
    fileNames = ["model_1.model", "model_2.model"]
    for name in fileNames:
        testModel(os.path.join(prefix, name))
    return 0


if __name__ == '__main__':
    test()
