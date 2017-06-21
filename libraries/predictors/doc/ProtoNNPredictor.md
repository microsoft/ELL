# ProtoNN: Compressed and Accurate kNN for Resource-scarce Scenarios
Several real-world applications require real-time prediction on resource-scarce devices such as an Internet of Things (IoT) sensor. Such applications demand prediction models with small storage and computational complexity while not compromising signi?cantly on accuracy. This project provides prediction code for ProtoNN (Prototype-based Nearest Neighbours), the state-of-the-art algorithm in this domain. 

## Prediction model
We wish to make a label prediction for a $$D$$-dimensional datapoint $$X$$. Suppose also that there are $$L$$ labels.

ProtoNN learns 3 parameters:
- A projection matrix $$W$$ of dimension $$(d,\space D)$$ projects the datapoints to a small dimension $$d$$
- $$m$$ prototypes in the projected space, each $$d$$-dimensional: $$B_1,\space B_2, ... \space B_m$$
- $$m$$ labels for each of the prototypes to allow a single prototype to store information for multiple labels, each $$L$$-dimensional: $$Z_1,\space Z_2, ... \space Z_m$$

ProtoNN also assumes an RBF-kernel parametrized by a single parameter $$\gamma$$.

ProtoNN then computes the following $$L$$ dimensional-vector. This may also be referred to as the ProtoNN model:

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;$$Y_{score}=\sum_{j=0}^{m}\space \left(exp\left[-\gamma^2||W\cdot X - B_j||^2\right]\cdot Z_j\right)$$ 

The prediction label is then $$\space max(Y_{score})$$
