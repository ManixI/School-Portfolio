from pipelineFunc import pipeline as pl



csvFile = "AmesHousing.csv"
seed = 7545
outFile = ''
loops = 50




test = pl(csvFile, seed)
acr = test.train_test(loops, outFile)
print(acr)
print(test.best_params)

# 10055 - .7467
# 7545  - .7515
# 100135 - .7603
# 100132 - .7450
# 100157 - 0.7721