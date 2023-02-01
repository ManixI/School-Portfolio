from pipelineFunc import pipeline as pl
import time


# run this to test pipeline funcion

# helper function for multi-threading
'''
def test_seed(seed, loops, csvFile, out):
    output = {
        "seed": [seed],
        "loops": [loops],
        "accuracy": [],
    }
    test = pl(csvFile,seed)
    output["accuracy"].append(test.train_test(loops,''))
    out.append(output) 
    '''

csvFile = "AmesHousing.csv"
seed = 20000
outFile = ''
# if loops > 1, the .list_cols_before_dummies dosn't work
loops = 50
num_seeds = 1
#threads = 2
minuits = 120
run_time = minuits*60


t = time.time()

'''
test = pl(csvFile,seed)
result = test.train_test(loops,outFile)
print(result)
'''
'''
time_delta = time.time() - t
time_delta_cuml = time_delta
print(str(time_delta_cuml)+" "+str(time_delta))
'''

import warnings
warnings.filterwarnings('ignore')


for j in range(6)
    time_delta_cuml = 0
    reslut_list = list()
    with open('seed-accuracy.txt'+str(j), 'w') as file:
        tmp = 'loops: '+str(loops)+'\n'
        file.write(tmp)
        file.write('\n')
        file.write('run time: '+str(round(run_time/60))+'\n\n')
        i = seed
        try:
            #for i in range(seed,seed+num_seeds):
            while time_delta_cuml < run_time:
                try:
                    test = pl(csvFile,i)
                    #try:
                    acr = test.train_test(loops,outFile)
                    #except:
                    #    tmp = 'value error\n'
                    #file.write('accuracy: '+str(acr))
                    #file.write('\n')
                    scr = test.score
                    file.write('score: '+str(scr)+'\n')
                    sd = i
                    file.write('seed: '+str(sd))
                    file.write('\n')
                    time_delta = time.time() - t
                    time_delta_cuml += time_delta
                    t = time.time()
                    file.write('loop run time: '+str(time_delta)+'\n')
                    file.write('cumulative run time: '+str(time_delta_cuml)+'\n')
                    file.write(str(test.list_cols_before_dummies)+'\n')
                    #file.write(str(test.best_params)+'\n\n')
                    file.write("\n")
                    print(str(i)+" "+str(round(time_delta_cuml/60))+":"+str(round(time_delta_cuml%60)))
                    testrun = (scr, sd, time_delta)
                    reslut_list.append(testrun)
                    #print(type(test.best[0]))
                    #print(type(test.best))
                    i+=1
                except KeyboardInterrupt:
                    # on ^C write create list and write it to file
                    break
                    
                except:
                    # this is to catch the occasional error where a value ecxeeds max float size
                    # just skip that seed rather then trying to fix
                    print("error!")
                    i+=1

        # if ended early, still generate sorted list
        # this is redundant now, can be removed
        except KeyboardInterrupt:
            reslut_list = sorted(reslut_list)
            for i in range(len(reslut_list)):
                file.write(str(reslut_list[i])+'\n')
            print(reslut_list[-1])
            

        reslut_list = sorted(reslut_list)
        for i in range(len(reslut_list)):
            file.write(str(reslut_list[i])+'\n')
    print(reslut_list[-1])


# code to multi-thread
# dosn't increse speed      
'''
time_delta_cuml = 0
thread_list = [None] * threads
with open('seed-accuracy.txt', 'w') as file:
    tmp = 'loops: '+str(loops)+'\n'
    file.write(tmp)
    file.write('\n')
    out = list()
    for i in range(1,num_seeds):
        for j in range(threads):
            thread_list[j] = threading.Thread(target=test_seed, args=(i+(j*num_seeds),loops, csvFile, out))
            thread_list[j].start()
        for j in range(threads):
            thread_list[j].join()
    file.write(str(out)+"\n")
    print(time.time() - t)
'''

# one core, t = 72
# four? cores t=33
# no threading t=33

