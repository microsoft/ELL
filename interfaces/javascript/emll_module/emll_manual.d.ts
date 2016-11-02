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

export function GenerateDTWClassifier(prototype: DoubleVectorVector):ELL_CompiledMap;
export function GetDTWClassifierCode(prototype: DoubleVectorVector): string;
