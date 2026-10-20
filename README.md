# Case Study 07: CPU Scheduling Algorithms Simulator & Comparative Evaluation

**Course:** Operating Systems  
**Unit/CO:** Unit II | CO2  
**Batch:** B07  

### Team Members
* **160124749019** - SRISHTI KEDIA (Batch-1) — *Team Lead & Theoretical Analysis*
* **160124749020** - TALAMALA ABHINAYA (Batch-1) — *Design & C Simulator Implementation*
* **160124749021** - THATI VYSHALI (Batch-1) — *Experiments, Workloads & Data Analytics*

---

##  Project Overview
This project delivers a high-performance **CPU Scheduling Simulator written in C** along with an automated **Python Data Analysis Pipeline** and an interactive **Web-based Dashboard**. The simulator evaluates six fundamental CPU scheduling algorithms under varying workload characteristics and context-switch overheads, comparing execution metrics against standard textbook benchmarks (Silberschatz, Galvin, and Gagne, *Operating System Concepts*, 9th Edition).

### Implemented Algorithms
1. **FCFS** (First-Come, First-Served)
2. **SJF** (Shortest Job First – Non-Preemptive)
3. **SRTF** (Shortest Remaining Time First – Preemptive SJF)
4. **Priority Scheduling** (Non-Preemptive without Aging)
5. **Priority Scheduling with Aging** (Preemptive starvation prevention)
6. **Round Robin (RR)** (Configurable Time Quantum $q$)

---

##  Repository Structure

```text
├── scheduler.c              # Core C Simulator implementation
├── Makefile                 # Build configuration for GCC compilation
├── generate_workloads.py    # Python script to generate synthetic 50-process workloads
├── generate_charts.py       # Matplotlib visualization script for bar/line plots
├── plot_quantum.py          # Script for Round Robin time quantum sweep (1ms to 50ms)
├── index.html               # Interactive Web Dashboard (Tailwind CSS + Chart.js)
├── galvin_fcfs.txt          # Galvin 9e FCFS validation dataset
├── galvin_srtf.txt          # Galvin 9e SRTF / SJF validation dataset
├── galvin_priority.txt      # Galvin 9e Priority validation dataset
├── workload_cpubound.txt    # Synthetic workload: CPU-bound processes
├── workload_interactive.txt # Synthetic workload: Interactive short-burst processes
├── workload_mixed.txt       # Synthetic workload: Mixed processes with Convoy Effect
└── README.md                # Project documentation