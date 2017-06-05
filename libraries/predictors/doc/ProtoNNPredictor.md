# ProtoNN predictor

# Terminology

  - D Input dimension
  - d Projected dimension
  - m Number of prototypes
  - W Projection Matrix (d * D)
  - Z Label embeddings (l * m)
  - B Prototype matrix (d * m)

### Prediction formulation

- Projected input WX = W * X
- Similarity with ith prototype = exp( -(gamma^2) * || B(i) - WX||^2) )
- Output Label = max(Z * similarity)
