import {DataVector, InputFeature, Feature, Model, CoordinateList} from "./emll";

export class NodeIterator {
    IsValid(): Boolean;
    Next();
    Get(): Node;
}

export class ModelGraph {
    //	GetNode(Node::NodeId id):Node;
    Size(): number;

    GetNodeIterator(): NodeIterator;
    GetNodeIterator(outputNode: Node): NodeIterator;;
    //	GetNodeIterator(const std::vector<const Node*>& outputNodes): NodeIterator;;
}

export class Node {
    GetId(): UniqueId;
    GetInputPorts(): InputPortVector;
    GetOutputPorts(): OutputPortVector;
    GetInputNodes(): ConstNodeVector;
    GetDependentNodes(): ConstNodeVector;
    GetRuntimeTypeName(): string;
}

export class Port {
    GetNode(): Node;
    Size(): number;
}

export class InputPortBase extends Port {
    GetOutputPortElements: OutputPortElementVector;
    GetInputNodes(): ConstNodeVector;
}

export class OutputPortBase extends Port {
}

export class OutputPortElement {
    GetIndex(): number;
    ReferencedPort(): OutputPortBase;
}

// template collections
export class NodeVector {
    size(): number;
    get(index: number): Node;
}

export class ConstNodeVector {
    size(): number;
    get(index: number): Node;
}

export class InputPortVector {
    size(): number;
    get(index: number): InputPortBase;
}

export class OutputPortVector {
    size(): number;
    get(index: number): OutputPortBase;
}

export class OutputPortElementVector {
    size(): number;
    get(index: number): OutputPortElement;
}

//
// nodes
//
export class MeanNode extends Node {
    constructor();
}

//
// misc
//
export function LoadModelGraph(filename: string): ModelGraph;
export class UniqueId { }
export class Variant {}
