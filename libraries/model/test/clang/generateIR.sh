set -e

clang -S -emit-llvm -o DotProduct.ll DotProduct.cpp

echo "// IR code for dot product" > DotProductIR.h
echo "const char* GetDotProductFunctionName()" >> DotProductIR.h
echo "{" >> DotProductIR.h
echo "    return \"_Node__DotProduct\";" >> DotProductIR.h
echo "}" >> DotProductIR.h

echo "const char* GetDotProductIR()" >> DotProductIR.h
echo "{" >> DotProductIR.h
echo "    return R\"xx(" >> DotProductIR.h
cat DotProduct.ll >> DotProductIR.h
echo ")xx\";" >> DotProductIR.h
echo "}" >> DotProductIR.h
