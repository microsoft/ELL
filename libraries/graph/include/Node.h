#pragma once
//
// Node
// 

#include <string>

class Node
{
public:
    
    Node();
//    Node(const std::vector<Edge>& inputs);

//    const std::vector<Edge>& GetInputEdges() const { return _inputs; }
//    void AddDependent(const std::shared_ptr<Node>& dependent);

    void GetOutputHandle(size_t index); // ???
    void GetOutputHandle(std::string name);  // ???

protected:
    void RegisterOutputs(); // ???
private:
};

#include "../tcc/Node.tcc"
