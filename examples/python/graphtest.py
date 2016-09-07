import EMLL

def GetName(node): return str(node.GetId())

def GetNodeIndex(index, node): return index[GetName(node)]

def GetNodes(model):
    '''Return an array holding all the Node objects of the model.
The output will we of the form [node_1, node_2, ..., node_v]'''
    ans = []
    nodes = model.GetNodeIterator()
    while nodes.IsValid():
        ans.append(nodes.Get())
        nodes.Next()
    return ans

def GetNodeDictionary(nodes):
    '''Return a dictionary mapping Node names to indices. The range
of indices is 1 .. V. This means that 0 cannot be the index of a
valid Node object.

If the input is [node_1, node_2, ..., node_V] then the output is 
a mapping 'index' such that index[node_n] = n where n = 1 ... V.
This means that node indices are one-based.

'''
    def g(i): return i+1, GetName(nodes[i])
    def f(ans, (i, name)):
        ans[name] = i
        return ans
    temp = map(g, range(len(nodes)))
    return reduce(f, temp, {})

def GetNodesAndIndex(model):
    '''
    Returns a 1-based array of nodes of the model and mapping from
    the node names to the 1-based indices
    '''
    nodes = GetNodes(model)             # nodes: [0 .. V-1] -> Node
    index = GetNodeDictionary(nodes)    # index: string -> [1 .. V]
    nodes.insert(0, None)               # align nodes with index so that
    return nodes, index                 # index[GetName[nodes[i]]] == i is True for 1 .. V

def BFS(adj, s):
    '''Breadth First Search of the adjacency list starting 
    at root index 's'

    adj[i][j] is one-based index of the j'th neighbor of node i

    where
                      
                      1 <= i <= V
                      0 <= j < #Neighbors of Node i

    It is expected that the adj represents an undirected graph
    so the edges must be symmetric, that is 

               adj[adj[i][j]] contains i
               for i in 1 ..V and j in 0 <= j < len(adj[i])

    Returns a state and parent list. The states can be either
    'U' (Undiscovered) or 'C' (Completely discoverd). If 
    state[i] == 'C' for 1 <= i <= V then the graph with 
    root node s is connected. The parent list is not used here.
    '''
    p = map(lambda x: None, adj)
    state = map(lambda x: 'U', adj)
    Q = [s]
    while len(Q) > 0:
        u = Q[0]
        Q = Q[1:]
        for v in adj[u]:
            if state[v] == 'U':
                state[v] = 'D'
                p[v] = u
                Q.append(v)
        state[u] = 'C'
    return state, p

def CheckAdjacencyList(adj):
    '''Checks the adjacency list for symmetry of edges'''
    V = len(adj) - 1
    for i in range(1,V+1):
        js = adj[i]
        for j in js:
            k = adj[j].index(i)

def GetAdjacencyList(nodes, index):
    def GetIndex(node): return GetNodeIndex(index, node)
    def GetParents(node):
        if node == None: return []
        else: return map(GetIndex, node.GetParentNodes())
    def GetDependents(node):
        if node == None: return []
        else: return map(GetIndex, node.GetDependentNodes())
    parentList = map(GetParents, nodes)
    dependentList = map(GetDependents, nodes)
    V = len(index)
    # confirm parents and dependents are doubly linked 
    for i in range(1, V+1):
        for j in parentList[i]:
            k = dependentList[j].index(i)
        for j in dependentList[i]:
            k = parentList[j].index(i)
    # create the adjacency list
    adj = parentList[:]
    for i in range(1, V+1):
        adj[i].extend(dependentList[i])
    CheckAdjacencyList(adj)
    return adj

def work(key):
    model = EMLL.LoadModel(key)
    nodes, index = GetNodesAndIndex(model)
    adj = GetAdjacencyList(nodes, index)
    state, p = BFS(adj, 1)
    # determine if the graph is connected
    V = len(index)
    ans = True
    for i in range(1, V+1):
        if state[i] != 'C':
            ans = False
            break
    if ans:
        print key, "is connected"
    else:
        print key, "is disjoint"

def test():
    keys = ['[1]','[2]','[3]','[tree_0]','[tree_1]','[tree_2]','[tree_3]']
    try:
        for key in keys: 
            work(key)
        return 0
    except:
        return 1
