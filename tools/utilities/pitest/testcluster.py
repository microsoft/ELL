import dask.threaded
from dask import compute, delayed


def test():
   def process(data):
      return data * 2

   values = [delayed(process)(x) for x in range(1000)]
   results = compute(*values, get=dask.threaded.get)

   print(results)


if __name__ == "__main__":
   test()
