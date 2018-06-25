import sys
import matplotlib.pyplot as plt; plt.rcdefaults()
import matplotlib.pyplot as plt
import numpy as np

def getavg(f1,f2,f3):
    f = open(f1,"r")
    instance = 0
    i=0
    alg=""
    lao={}
    det={}
    prm={}
	fullmodel=0

    for line in f:
        line = line.strip()
        if("******************" in line):
            instance+=1
        if("******ALG =" in line):
            if("LAO" in alg):
                lao[instance] = [estCost, avgCost, stdDev, totalTime]
            if "DET" in alg:
                det[instance] = [estCost, avgCost, stdDev, totalTime]
            if "prm" in alg:
                prm[instance-1]=[estCost, avgCost, stdDev, totalTime]

            alg = line.split('******ALG =')[-1]
        if ("Estimated cost" in  line):
            line = line.split('Estimated cost')
            estCost = float(line[1].split()[0])
            line =  line[1].split("Avg. Exec cost")
            avgCost = float(line[1].split()[0])
            line = line[1]
            line = line.split("Std. Dev.")
            stdDev=float(line[1].split()[0])
            line = line[1]
			
            line = line.split("Total time")
            totalTime = float(line[1].split()[0])
		if(alg == "prm" and "%fullmodel in line):
			line = line.split('%fullmodel = ')
			fullmodel = fullmodel + float(line[1])
    #		print alg,instance,  estCost, avgCost, stdDev, totalTime
    prm[instance] = [estCost, avgCost, stdDev, totalTime]
	
    f = open(f2,"r")
    instance = 0
    alg=""
    flares1={}
    flares0={}
    for line in f:
            line = line.strip()
            if("******************" in line):
                    instance+=1
            if("******ALG =" in line):
                    if("flares(0)" in alg):
                            flares0[instance] = [estCost, avgCost, stdDev, totalTime]
                    if "flares(1)" in alg:
                            flares1[instance-1] = [estCost, avgCost, stdDev, totalTime]

                    alg = line.split('******ALG =')[-1]
            if ("Estimated cost" in  line):
                    line = line.split('Estimated cost')
                    estCost = float(line[1].split()[0])
                    line =  line[1].split("Avg. Exec cost")
                    avgCost = float(line[1].split()[0])
                    line = line[1]
                    line = line.split("Std. Dev.")
                    stdDev=float(line[1].split()[0])
                    line = line[1]

                    line = line.split("Total time")
                    totalTime = float(line[1].split()[0])

    #                print alg,instance,  estCost, avgCost, stdDev, totalTime

    flares1[instance] = [estCost, avgCost, stdDev, totalTime]

    f = open(f3,"r")
    instance = 0
    alg=""
    alld={}
    acarm={}
    for line in f:
            line = line.strip()
            if("******************" in line):
                    instance+=1
            if("******ALG =" in line):
                    if("ALLD" in alg):
                            alld[instance] = [estCost, avgCost, stdDev, totalTime]
                    if "ACARM" in alg:
                            acarm[instance-1] = [estCost, avgCost, stdDev, totalTime]

                    alg = line.split('******ALG =')[-1]
            if ("Estimated cost" in  line):
                    line = line.split('Estimated cost')
                    estCost = float(line[1].split()[0])
                    line =  line[1].split("Avg. Exec cost")
                    avgCost = float(line[1].split()[0])
                    line = line[1]
                    line = line.split("Std. Dev.")
                    stdDev=float(line[1].split()[0])
                    line = line[1]

                    line = line.split("Total time")
                    totalTime = float(line[1].split()[0])

    #                print alg,instance,  estCost, avgCost, stdDev, totalTime

    acarm[instance] = [estCost, avgCost, stdDev, totalTime]


    i=instance
    numInstance = instance

    detcostLst = []
    prmcostLst=[]
	avgprmExecCostLst=[]
	avgprmExecTimeLst=[]
    flares0costLst=[]
    flares1costLst=[]
    alldcostLst = []
    acarmcostLst = []
    timedetLst=[]
    timeprmLst=[]
    timeflares0Lst=[]
    timeflares1Lst=[]
    timealldLst = []
    timeacarmLst = []

    while instance > 0:
        # print(det[instance][1],",",instance)
        detcostLst.append(abs((det[instance][1] - lao[instance][1])/lao[instance][1]) *100)
        prmcostLst.append(abs((prm[instance][1] - lao[instance][1])/lao[instance][1]) *100)
		avgprmExecCostLst.append(prm[instance][1])
        flares0costLst.append(abs((flares0[instance][1] - lao[instance][1])/lao[instance][1]) *100)
        flares1costLst.append(abs((flares1[instance][1] - lao[instance][1])/lao[instance][1]) *100)
        alldcostLst.append(abs((alld[instance][1] - lao[instance][1])/lao[instance][1]) *100)
        acarmcostLst.append(abs((acarm[instance][1] - lao[instance][1])/lao[instance][1]) *100)
        timedetLst.append (abs(det[instance][3] - lao[instance][3]))
        timeprmLst.append (abs(prm[instance][3] - lao[instance][3]))
		avgprmExecTimeLst.append(prm[instance][3])
        timeflares0Lst.append (abs(flares0[instance][3] - lao[instance][3]))
        timeflares1Lst.append (abs(flares1[instance][3] - lao[instance][3]))
        timealldLst.append (abs(alld[instance][3] - lao[instance][3]))
        timeacarmLst.append(abs(acarm[instance][3] - lao[instance][3]))
        instance-=1


    avgdet=np.mean(detcostLst)
    avgprm=np.mean(prmcostLst)
    avgflares0=np.mean(flares0costLst)
    avgflares1=np.mean(flares1costLst)
    avgalld= np.mean(alldcostLst)
    avgacarm= np.mean(acarmcostLst)

    errdet= np.std(detcostLst)*1.0/np.sqrt(numInstance)
    errprm = np.std(prmcostLst)*1.0/np.sqrt(numInstance)
    errflares0 = np.std(flares0costLst)*1.0/np.sqrt(numInstance)
    errflares1 = np.std(flares1costLst)*1.0/np.sqrt(numInstance)
    erralld= np.std(alldcostLst)*1.0/np.sqrt(numInstance)
    erracarm= np.std(acarmcostLst)*1.0/np.sqrt(numInstance)

    timedet=np.mean(timedetLst)
    timeprm=np.mean(timeprmLst)
    timeflares0=np.mean(timeflares0Lst)
    timeflares1=np.mean(timeflares1Lst)
    timealld=np.mean(timealldLst)
    timeacarm= np.mean(timeacarmLst)

    er_runtime_det= np.std(timedetLst)*1.0/np.sqrt(numInstance)
    er_runtime_prm = np.std(timeprmLst)*1.0/np.sqrt(numInstance)
    er_runtime_f0 = np.std(timeflares0Lst)*1.0/np.sqrt(numInstance)
    er_runtime_f1 = np.std(timeflares1Lst)*1.0/np.sqrt(numInstance)
    er_runtime_alld= np.std(timealldLst)*1.0/np.sqrt(numInstance)
    er_runtime_acarm= np.std(timeacarmLst)*1.0/np.sqrt(numInstance)

    print("Avg difference in execution cost is:")
    print("DET:", avgdet,errdet)
    print("alld:", avgalld,erralld)
    print("flares(0):", avgflares0, errflares0)
    print("flares(1):", avgflares1, errflares1)

    print("Avg difference in total time is:")
    print("DET:", timedet)
    print("prm:",  timeprm)
    print("flares(0):",  timeflares0)
    print("flares(1):",  timeflares1)
    return[avgdet, avgalld, avgflares0, avgflares1, avgprm, avgacarm,
           errdet, erralld, errflares0, errflares1, errprm, erracarm,
           timedet, timealld, timeflares0, timeflares1, timeprm, timeacarm,
           er_runtime_det, er_runtime_alld, er_runtime_f0, er_runtime_f1,
           er_runtime_prm, er_runtime_acarm ]

	print("avg prm full model usage  = ", (fullmodel/numInstance))
	print("avg prm exec cost =", np.mean(avgprmExecCostLst))
	print("avg prm exec time =", np.mean(avgprmExecTimeLst))
	
num_files = int(sys.argv[1])
out = np.zeros((num_files,24))
input=0
arg_num=0
while arg_num < num_files*3:
    print("******************",sys.argv[arg_num+2]+","+sys.argv[arg_num+3]+","+sys.argv[arg_num+4])
    out[input,:] = getavg(sys.argv[arg_num+2],sys.argv[arg_num+3],sys.argv[arg_num+4] )
    input += 1
    arg_num +=3


indices = ("RF-1", "RF-2","RF-3","RF-4")
# objects = ('DET', 'prm', 'flares(0)', 'flares(1)')
xobj = [0,1,2,3]
xobj2 = [1.15,2.15, 3.15, 4.15,5.15, 6.15]
xobj3 = [1.3,2.3,3.3,4.3, 5.3, 6.3]
xobj4 = [1.45,2.45,3.45,4.45, 5.45, 6.45]
xobj5 = [1.6,2.6,3.6,4.6, 5.6, 6.6]
xobj6 = [1.75,2.75,3.75,4.75, 5.75, 6.75]
ypos = np.arange(len(indices))

plt.bar(ypos, out[:,0],.15,yerr= out[:,6], color='b',label='MLOD')
plt.bar(ypos+.15, out[:,1],.15, yerr=out[:,7], color='grey', label='ALLD')
plt.bar(ypos+.3, out[:,2],.15, yerr=out[:,8], color='brown', label='FLARES(0)')
plt.bar(ypos+.45, out[:,3],.15, yerr=out[:,9], color='c', label='FLARES(1)')
plt.bar(ypos+.6, out[:,4],.15, yerr=out[:,10], color='red', label='0/1 RM')
plt.bar(ypos+.75, out[:,5],.15, yerr=out[:,11], color='black', label='ACARM-0/1 RM')

plt.xticks(xobj, indices)
plt.legend()
plt.ylabel('Difference from Optimal (%)',fontsize=12,fontweight='bold')
plt.xlabel('Reward Function',fontsize=12,fontweight='bold')
plt.title('Average Difference In Cost From Optimal (%)',fontweight='bold',fontsize=12)
plt.show()

# plt.bar(ypos, out[:,12],.15, color='b',label='MLOD')
# plt.bar(ypos+.15, out[:,13],.15, color='grey', label='ALLD')
# plt.bar(ypos+.3, out[:,14],.15, color='brown', label='FLARES(0)')
# plt.bar(ypos+.45, out[:,15],.15, color='c', label='FLARES(1)')
# plt.bar(ypos+.6, out[:,16],.15, color='red', label='0/1 RM')
# plt.bar(ypos+.75, out[:,17],.15, color='black', label='ACARM-0/1 RM')
#
# plt.xticks(xobj, indices)
# plt.legend()
# plt.ylabel('Average difference in runtime')
# plt.xlabel('Reward Function')
# plt.title('Average Runtime Savings (%)',fontweight='bold',fontsize=12)
# plt.show()
