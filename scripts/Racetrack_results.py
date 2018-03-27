import matplotlib.pyplot as plt
import numpy as np

rt_cost = np.zeros((4,6))
rt_e = np.zeros((4,6))
rt_t = np.zeros((4,6))
rt_te = np.zeros((4,6))

rt_cost[0,:] = [37.01, 24.02, 2.58, .62, .18, .18]
rt_cost[1,:] = [26.82, 23.32, 3.13, 6.63,.43,.14]
rt_cost[2,:] = [39.37, 28.47, 29.04, 35.95, 15.34, 13.89]
rt_cost[3,:] = [30.62, 28.25, 10.47, 9.15, 8.31, 8.61 ]

rt_e[0,:]=[.24, .52, .18, .28, .5,.2]
rt_e[1,:]=[.66, .56, .22, .2, .15,.42]
rt_e[2,:]=[.96, 1.2, 1.05, .62, .94,.9]
rt_e[3,:]=[1.15, 1.16, .7, .8, 1.4, 1.2]

rt_t[0,:]= [71.27, 71.43, 74.6, 65.08, 38.09, 38.09]
rt_t[1,:]= [71.39, 70.79, 68.14, 64.01, 54.57, 54.27]
rt_t[2,:]= [52.22, 38.88, 61.11, 38.88, 27.77, 27.77]
rt_t[3,:]= [27.12, 38.98, 64.4, 48.3, 44.07,44.91]

objects=('Square-4','Square-5','Ring-5', 'Ring-6')
x=[1,2,3,4]
bar_width = 0.15
y_pos = np.arange(len(objects))

#cost graph
plt.bar(y_pos-bar_width,rt_cost[:,0],bar_width,align='center',color='b',label='MLOD',yerr=rt_e[:,0])
plt.bar(y_pos+.01,rt_cost[:,1],bar_width,align='center',color='grey',label='ALLD',yerr=rt_e[:,1])
plt.bar(y_pos+bar_width+.01,rt_cost[:,2],bar_width,align='center',color='brown',label='FLARES(0)',yerr=rt_e[:,2])
plt.bar(y_pos+2*bar_width+.01,rt_cost[:,3],bar_width,align='center',color='c',label= 'FLARES(1)',yerr=rt_e[:,3])
plt.bar(y_pos+3*bar_width+.01,rt_cost[:,4],bar_width,align='center',color='red',label= '0/1 RM',yerr=rt_e[:,4])
plt.bar(y_pos+4*bar_width+.01,rt_cost[:,5],bar_width,align='center',color='black',label= 'ACARM-0/1 RM',yerr=rt_e[:,5])
plt.xticks(y_pos,objects,fontsize=12)
plt.ylabel('Difference from Optimal (%)',fontweight='bold',fontsize=12)
plt.xlabel('Ractetrack Problem',fontweight='bold',fontsize=12)
plt.title('Average Difference In Cost From Optimal (%)',fontweight='bold',fontsize=12)
plt.legend()
plt.show()

#PV graph
plt.bar(y_pos-bar_width,rt_t[:,0],bar_width,align='center',color='b',label='MLOD')
plt.bar(y_pos+.01,rt_t[:,1],bar_width,align='center',color='grey',label='ALLD')
plt.bar(y_pos+bar_width+.01,rt_t[:,2],bar_width,align='center',color='brown',label='FALRES(0)')
plt.bar(y_pos+2*bar_width+.01,rt_t[:,3],bar_width,align='center',color='c',label= 'FLARES(1)')
plt.bar(y_pos+3*bar_width+.01,rt_t[:,4],bar_width,align='center',color='red',label= '0/1 RM')
plt.bar(y_pos+4*bar_width+.01,rt_t[:,5],bar_width,align='center',color='black',label= 'ACARM-0/1 RM')
plt.xticks(y_pos,objects)
plt.ylabel('Runtime Savings (%)',fontsize=12,fontweight='bold')
plt.xlabel('Ractetrack Problem',fontweight='bold',fontsize=12)
plt.title('Runtime Savings (%)',fontweight='bold',fontsize=12)
plt.legend()
plt.show()
