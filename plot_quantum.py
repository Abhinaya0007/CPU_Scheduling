import matplotlib.pyplot as plt
import subprocess
import re

quantums = list(range(1, 51))
avg_tats = []
avg_rts = []

# Sweep quantum from 1 to 50 ms on mixed workload
for q in quantums:
    # Compile and execute C scheduler
    cmd = f"./scheduler workload_mixed.txt 0.1"
    output = subprocess.check_output(cmd, shell=True).decode('utf-8')
    
    # Parse metrics for RR
    tat_match = re.search(r"Average Turnaround Time:\s+([\d\.]+)", output)
    rt_match = re.search(r"Average Response Time:\s+([\d\.]+)", output)
    
    if tat_match and rt_match:
        avg_tats.append(float(tat_match.group(1)))
        avg_rts.append(float(rt_match.group(1)))
    else:
        avg_tats.append(0)
        avg_rts.append(0)

# Plotting
plt.figure(figsize=(10, 5))
plt.plot(quantums, avg_tats, label='Avg Turnaround Time (TAT)', color='blue', linewidth=2)
plt.plot(quantums, avg_rts, label='Avg Response Time (RT)', color='red', linewidth=2)

plt.title('RR Time Quantum Sweep vs Performance Metrics')
plt.xlabel('Time Quantum (ms)')
plt.ylabel('Time (ms)')
plt.legend()
plt.grid(True)
plt.savefig('quantum_sweep.png')
print("Saved plot to quantum_sweep.png!")