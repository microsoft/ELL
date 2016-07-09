import {DataVector, InputFeature, Feature, Model, CoordinateList} from "./emll";

export class ModelGraph {
    //	GetNode(Node::NodeId id):Node;;
    Size(): number;

    GetNodeIterator(): NodeIterator;
    GetNodeIterator(outputNode: Node): NodeIterator;;
    //	GetNodeIterator(const std::vector<const Node*>& outputNodes): NodeIterator;;
}

export function LoadModelGraph(filename: string): ModelGraph;

export class Node { }

export class MeanNode extends Node {
    constructor(); 
}
