import matplotlib.pyplot as plt
import numpy as np

sd_cost = np.zeros((6,6))
sd_e = np.zeros((6,6))
sd_t = np.zeros((6,6))
sd_te = np.zeros((6,6))

sd_cost[0,:] = [76.52, 21.49, 82.43, 8.52, 7.11, 1.22]
sd_cost[1,:] = [63.34, 28.97, 91.47, 3.67, .18,.26]
sd_cost[2,:] = [39.18, 32.27, 42.79, 5.03, 2.01, .81]
sd_cost[3,:] = [89.22, 10.63, 40.83, 4.12, 1.63, 1.31]
sd_cost[4,:] = [65.64, 25.05, 57.84, 2.76, 3.08, .39]
sd_cost[5,:] = [61.22, 25.04, 95.02, 1.94, 4.6,1.15]

sd_e[0,:]=[7.5, 4.4, 7.8, 2.7, 2.6, 2.4]
sd_e[1,:]=[9.1, 4.3, 10.6, 4.2, 3.8, 3.2]
sd_e[2,:]=[3.8, 4.8, 1.6, 5.7, 5.2, 5.2]
sd_e[3,:]=[5.6, 2.1, 4.6, 2.01, 1.8, 1.6]
sd_e[4,:]=[6.94, 2.89, 7.8, 2.8, 2.5, 2.4]
sd_e[5,:]=[9, 4.33, 11.5, 4.3, 4.3, 4.4]

sd_t[0,:]= [75, 65, 70, 50, 50, 50]
sd_t[1,:]= [54.9, 41.2, 90.2, 76.5, 66.6, 56.8]
sd_t[2,:]= [64.4, 69.4, 62.4, 19.4, 55.9, 55.9]
sd_t[3,:]= [65, 65, 80, 65, 50, 50]
sd_t[4,:]= [60, 65, 85, 70, 30, 25]
sd_t[5,:]= [38, 33.1, 69.7, 49.2, 18.3, 31.7]

objects=('20(C)','40(C)','80(C)', '20(M)',
        '40(M)','80(M)')
x=[1,2,3,4]
bar_width = 0.15
y_pos = np.arange(len(objects))

#cost graph
plt.bar(y_pos-bar_width,sd_cost[:,0],bar_width,align='center',color='b',label='MLOD',yerr=sd_e[:,0])
plt.bar(y_pos+.01,sd_cost[:,1],bar_width,align='center',color='grey',label='ALLD',yerr=sd_e[:,1])
plt.bar(y_pos+bar_width+.01,sd_cost[:,2],bar_width,align='center',color='brown',label='FLARES(0)',yerr=sd_e[:,2])
plt.bar(y_pos+2*bar_width+.01,sd_cost[:,3],bar_width,align='center',color='c',label= 'FLARES(1)',yerr=sd_e[:,3])
plt.bar(y_pos+3*bar_width+.01,sd_cost[:,4],bar_width,align='center',color='red',label= '0/1 RM',yerr=sd_e[:,4])
plt.bar(y_pos+4*bar_width+.01,sd_cost[:,5],bar_width,align='center',color='black',label= 'ACARM-0/1 RM',yerr=sd_e[:,5])


plt.xticks(y_pos,objects,fontsize=12)
plt.ylabel('Difference from Optimal (%)',fontweight='bold',fontsize=12)
plt.xlabel('Sailing Problem',fontweight='bold',fontsize=12)
plt.title('Average Difference In Cost From Optimal (%)',fontweight='bold',fontsize=12)
plt.legend()
plt.show()

#PV graph
plt.bar(y_pos-bar_width,sd_t[:,0],bar_width,align='center',color='b',label='MLOD')
plt.bar(y_pos+.01,sd_t[:,1],bar_width,align='center',color='grey',label='ALLD')
plt.bar(y_pos+bar_width+.01,sd_t[:,2],bar_width,align='center',color='brown',label='FALRES(0)')
plt.bar(y_pos+2*bar_width+.01,sd_t[:,3],bar_width,align='center',color='c',label= 'FLARES(1)')
plt.bar(y_pos+3*bar_width+.01,sd_t[:,4],bar_width,align='center',color='red',label= '0/1 RM')
plt.bar(y_pos+4*bar_width+.01,sd_t[:,5],bar_width,align='center',color='black',label= 'ACARM-0/1 RM')
plt.xticks(y_pos,objects)
plt.ylabel('Runtime Savings (%)',fontsize=12,fontweight='bold')
plt.xlabel('Sailing Problem',fontweight='bold',fontsize=12)
plt.title('Runtime Savings (%)',fontweight='bold',fontsize=12)
plt.legend()
plt.show()
