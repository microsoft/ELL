import {CompiledMap} from 'ell'


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

export class LabeledPrototype {
    constructor(label: number, prototype: DoubleVectorVector);
    Label(): number;
    Dimension(): number;
    Prototype(): DoubleVectorVector;
}

export class PrototypeList {
    size(): number;
    get(index: number): LabeledPrototype;
    add(vec: LabeledPrototype);
}

export function GenerateDTWClassifier(prototype: DoubleVectorVector): CompiledMap;
export function GenerateMulticlassDTWClassifier(prototype: PrototypeList): CompiledMap;
export function GetDTWClassifierCode(prototype: DoubleVectorVector): string;
