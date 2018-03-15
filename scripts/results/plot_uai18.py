import numpy as np
import matplotlib.pyplot as plt
import xlwt

filename='grid-gussp-results'
f = open(filename+'.txt', 'r')
book = xlwt.Workbook()
sh1 = book.add_sheet("Sheet 1")
sh2 = book.add_sheet("Sheet 2")
sh3 = book.add_sheet("Sheet 3")
col1_name = 'Problem (S,A,P_G)'
bookname=""
bookname = filename+'.xls'

solvers = ('lao',
           'flares(0)-zero',
           'flares(0)-hmin',
           'flares(0)-gussp',
           'flares(1)-zero',
           'flares(1)-hmin',
           'flares(1)-gussp')

n = 9
i = 0
problems = []
states = []
actions = []
goals = []
stderr = []
costs = []
times = []
decisionTime= []
current_problem_str = ''
current_solver_str = ''
for line in f:
  if i % n == 1:
    # tmp = line.rstrip('\n').split('|')
    tmp = line.split('|')
    current_problem_str = tmp[0]
    current_solver_str = tmp[1]
    problems.append(current_problem_str)
    # if current_problem_str not in problem_results:
    #   problem_results[current_problem_str] = {}
    #   problem_results[current_problem_str][current_solver_str] = {}
    #   problem_results[current_problem_str][current_solver_str]['costs'] = []
    #   problem_results[current_problem_str][current_solver_str]['std-error'] = []
    #   problem_results[current_problem_str][current_solver_str]['times'] = []
    #   problem_results[current_problem_str][current_solver_str]['states'] = []
    #   problem_results[current_problem_str][current_solver_str]['actions'] = []
    #   problem_results[current_problem_str][current_solver_str]['goals'] = []
  elif 'cost' in line:
        tmp = line.split(' ')
        costs.append(float(tmp[6]))
        times.append(1000 * float(tmp[12]))
        stderr.append(0.001 * float(tmp[9]))
        # problem_results[current_problem_str][current_solver_str]['costs'].append(
        # float(tmp[6]))
        # problem_results[current_problem_str][current_solver_str]['costs'].append(
        # 0.001 * float(tmp[9])) #converting std dev to std error when #nsims=1000
        # problem_results[current_problem_str][current_solver_str]['times'].append(
        # 1000 * float(tmp[12]))
  elif 'states =' in line:
      tmp = line.split(' ')
      states.append(tmp[3])
      actions.append(tmp[7].strip())
      # problem_results[current_problem_str][current_solver_str]['states'].append(
      # (tmp[3]))
      # problem_results[current_problem_str][current_solver_str]['actions'].append(
      # (tmp[7]))
  elif 'goal size =' in line:
      tmp = line.split(' ')
      # problem_results[current_problem_str][current_solver_str]['goals'].append(
      # (tmp[4]))
      goals.append(tmp[4].strip())
  elif 'Avg. time per decision' in line:
      tmp = line.split(' ')
      decisionTime.append(float(tmp[4].strip()))
  i += 1

# for problem in problem_results.keys():
#   for solver in solvers:
#     order = np.argsort(problem_results[problem][solver]['times'])
#     times = np.array(problem_results[problem][solver]['times'])[order]
#     costs = np.array(problem_results[problem][solver]['costs'])[order]
#     plt.plot(times, costs, label=solver)
#   plt.legend()
#   plt.show()


# n=1
## write column names:
# sh1.write(0,0,col1_name)
# for i in range(len(solvers)):
#     sh1.write(0,i+1,solvers[i])

# for i in problem_results.keys():
#     # for solver in solvers:
#         to_write=' '
#         to_write+=i+' ('+problem_results[i][0]['states'][n-1]
#         sh1.write(n,0,to_write)
#         n+=1

# book.save(bookname)
f.close()

stateId = 0
costId=0
print('****'+str(len(states)))
print('****'+str(len(solvers)-1))
unique_content = int(len(problems)/(len(solvers)))
print(unique_content)
sh1.write(0,0,'Problem Name (S,A,P_G)')
sh2.write(0,0,'Problem Name (S,A,P_G)')
sh3.write(0,0,'Problem Name (S,A,P_G)')
index = 0
j=0
while index < (len(solvers)*2):
    sh1.write(0,(index+1),solvers[j])
    sh1.write(0,(index+2), 'std error')
    sh2.write(0,(j+1),solvers[j])
    sh3.write(0,(j+1),(solvers[j]+"-time/decision"))
    index+=2
    j+=1

for i in range(unique_content):
    costId = i*len(solvers)
    stateId = i*len(solvers)
    probname= problems[stateId]+" ("+states[stateId]+", "+ actions[stateId]+", "+goals[stateId]+")"
    sh1.write((i+1),0,probname)
    sh2.write((i+1),0,probname)
    sh3.write((i+1),0,probname)
    ## writing avg costs to sheet 1:
    sh1.write((i+1),1,costs[costId])
    sh1.write((i+1),2,stderr[costId])
    sh1.write((i+1),3,costs[costId+1])
    sh1.write((i+1),4,stderr[costId+1])
    sh1.write((i+1),5,costs[costId+2])
    sh1.write((i+1),6,stderr[costId+2])
    sh1.write((i+1),7,costs[costId+3])
    sh1.write((i+1),8,stderr[costId+3])
    sh1.write((i+1),9,costs[costId+4])
    sh1.write((i+1),10,stderr[costId+4])
    sh1.write((i+1),11,costs[costId+5])
    sh1.write((i+1),12,stderr[costId+5])
    sh1.write((i+1),13,costs[costId+6])
    sh1.write((i+1),14,stderr[costId+6])
    ## writing times (in seconds) to sheet 2:
    sh2.write((i+1),1,times[costId])
    sh2.write((i+1),2,times[costId+1])
    sh2.write((i+1),3,times[costId+2])
    sh2.write((i+1),4,times[costId+3])
    sh2.write((i+1),5,times[costId+4])
    sh2.write((i+1),6,times[costId+5])
    sh2.write((i+1),7,times[costId+6])
    ## writing avg time per decision to sheet 3:
    sh3.write((i+1),1,decisionTime[costId])
    sh3.write((i+1),2,decisionTime[costId+1])
    sh3.write((i+1),3,decisionTime[costId+2])
    sh3.write((i+1),4,decisionTime[costId+3])
    sh3.write((i+1),5,decisionTime[costId+4])
    sh3.write((i+1),6,decisionTime[costId+5])
    sh3.write((i+1),7,decisionTime[costId+6])
book.save(bookname)
