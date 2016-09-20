import EMLL

def test():
    params = EMLL.SGDIncrementalTrainerParameters()
    params.regularization = 0.01
    loss = EMLL.LogLoss()

    trainer = EMLL.LogLossSGDTrainer(21, loss, params)
    print type(trainer)

    dataset = EMLL.GetDataset("../../../examples/data/testData.txt")
    print type(dataset)

    ex = dataset.GetExample(0)
    print type(ex)

    iterator = dataset.GetIterator(0, dataset.NumExamples()) 
    print type(iterator)

    try:
        trainer.Update(iterator)
        print "trainer.Update(...) succeeded"
    except TypeError, msg:
        print "trainer.Update(...) threw a TypeError exception", msg
        return 1
    return 0
