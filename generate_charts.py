import matplotlib.pyplot as plt
import pandas as pd

# Load the generated CSV data
df = pd.read_csv('results.csv')

# 1. Bar Chart: Average Waiting Time (WT) per Algorithm
plt.figure(figsize=(10, 5))
plt.bar(df['Algorithm'], df['Avg_WT'], color='skyblue', edgecolor='black')
plt.title('Average Waiting Time by Scheduling Algorithm')
plt.ylabel('Waiting Time (ms)')
plt.xticks(rotation=20, ha='right')
plt.tight_layout()
plt.savefig('bar_waiting_time.png')
print("[+] Saved bar chart: bar_waiting_time.png")

# 2. Bar Chart: Turnaround Time (TAT) vs Response Time (RT)
plt.figure(figsize=(11, 5))
x = range(len(df['Algorithm']))
width = 0.35

plt.bar([i - width/2 for i in x], df['Avg_TAT'], width=width, label='Turnaround Time (TAT)', color='salmon')
plt.bar([i + width/2 for i in x], df['Avg_RT'], width=width, label='Response Time (RT)', color='lightgreen')

plt.xlabel('Algorithm')
plt.ylabel('Time (ms)')
plt.title('Comparison of Turnaround Time and Response Time')
plt.xticks(x, df['Algorithm'], rotation=20, ha='right')
plt.legend()
plt.tight_layout()
plt.savefig('bar_tat_vs_rt.png')
print("[+] Saved comparison chart: bar_tat_vs_rt.png")