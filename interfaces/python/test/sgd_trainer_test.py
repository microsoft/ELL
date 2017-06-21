from __future__ import print_function
import ELL

def test():
    params = ELL.SGDIncrementalTrainerParameters()
    params.regularization = 0.01
    loss = ELL.LossArguments()

    trainer = ELL.GetSGDIncrementalTrainer(21, loss, params)
    print(type(trainer))

    dataset = ELL.GetDataset("../../../../examples/data/testData.txt")
    print(type(dataset))

    ex = dataset.GetExample(0)
    print(type(ex))

    iterator = dataset.GetIterator(0, dataset.NumExamples()) 
    print(type(iterator))

    try:
        trainer.Update(iterator)
        print("trainer.Update(...) succeeded")
    except msg as TypeError:
        print("trainer.Update(...) threw a TypeError exception", msg)
        return 1
    return 0
