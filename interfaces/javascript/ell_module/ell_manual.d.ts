// template collections
export class DoubleVector {
  size(): number;
  get(index: number): number;
  add(value: number);
}

export class DoubleVectorVector {
  size(): number;
  get(index: number): DoubleVector;
  add(vec: DoubleVector);
}

//
// misc
//
export class ELL_CompiledMap {
    GetCodeString(): string;
    Compute(inputData: DoubleVector) : DoubleVector;
}

export class ELL_LabeledPrototype {
  constructor(label:number, prototype: DoubleVectorVector);
  Label(): number;
  Dimension(): number;
  Prototype(): DoubleVectorVector;
}

export class PrototypeList {
  size(): number;
  get(index: number): ELL_LabeledPrototype;
  add(vec: ELL_LabeledPrototype);
}

export function GenerateDTWClassifier(prototype: DoubleVectorVector):ELL_CompiledMap;
export function GenerateMulticlassDTWClassifier(prototype: PrototypeList):ELL_CompiledMap;
export function GetDTWClassifierCode(prototype: DoubleVectorVector): string;
