export function InitializeFeatures(): void;

export class DoubleVector {
    constructor();
    Append(item: number): void;
}

export class StringVector {
    constructor();
    add(item: string): void;
    size(): number;
    get(index: number): string;
}

export class Feature {
}

export class FeatureSet {
    constructor();
    static GetRegisteredTypes(): StringVector;
    CreateFeatureFromDescription(sv: StringVector): Feature;
}
