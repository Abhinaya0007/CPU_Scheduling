import random

def generate_workload(filename, num_processes, burst_range, arrival_max, priority_range):
    with open(filename, 'w') as f:
        for pid in range(1, num_processes + 1):
            arrival = random.randint(0, arrival_max)
            burst = random.randint(*burst_range)
            priority = random.randint(*priority_range)
            f.write(f"{pid} {arrival} {burst} {priority}\n")

# 1. CPU-bound Workload
generate_workload("workload_cpubound.txt", 50, (50, 200), 20, (1, 5))

# 2. Interactive Workload
generate_workload("workload_interactive.txt", 50, (1, 10), 100, (1, 10))

# 3. Mixed Workload with Convoy Effect (Long process arriving at t=0)
with open("workload_mixed.txt", "w") as f:
    f.write("1 0 200 5\n") # Convoy trigger
    for pid in range(2, 51):
        arrival = random.randint(0, 10)
        burst = random.randint(1, 10)
        priority = random.randint(1, 5)
        f.write(f"{pid} {arrival} {burst} {priority}\n")

print("Generated 3 workload files successfully!")