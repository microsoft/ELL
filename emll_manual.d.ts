/// <reference path="emll.d.ts" />

export class ModelGraph
{
//	GetNode(Node::NodeId id):Node;;
	Size(): number;

	GetNodeIterator(): NodeIterator;
	GetNodeIterator(outputNode:Node): NodeIterator;;
//	GetNodeIterator(const std::vector<const Node*>& outputNodes): NodeIterator;;
}

export class MeanNode { // class id:23880
	constructor(); // ctor id:23945
	ProcessInputData(inData: DataVector): boolean; // member id:23968
	HasOutput(): boolean; // member id:23985
	GetOutput(): DataVector; // member id:23998
	Reset(): void; // member id:24011
	GetInputFeature(): InputFeature; // member id:24023
	GetOutputFeature(): Feature; // member id:24036
	SetOutputFeature(output: any /*!!!p.features::Feature*/): void; // member id:24049
	GetFeature(featureId: string): Feature; // member id:24067
	CreateFeatureFromDescription(description: vector<string>): Feature; // member id:24103
	AddToModel(model: Model, inputCoordinates: CoordinateList): CoordinateList; // member id:24158
}
