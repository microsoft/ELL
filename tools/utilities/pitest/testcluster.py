import dask.threaded
from dask import compute, delayed

def process(data):   
   return data*2

values = [delayed(process)(x) for x in range(1000)]
results = compute(*values, get=dask.threaded.get)

print(results)