#ifndef SIMULATION_H
#define SIMULATION_H

#include "stdio.h"
#include "queue.h"
#include "inputs.h"
#include "string.h"

enum STATES {NEW, READY, RUNNING, BLOCKED, EXIT};
#define NUM_INPUTS 6

typedef struct {
    int pid;
    int program_id;
    int pc;
    int state;              // Estado atual
    int remaining_quantum;  // Tempo restante no quantum
    int blocked_until;
    int time_in_state;      // Tempo no estado atual - NEW,EXIT
    int* instructions;      // pointer para as instruções do programa
    int instruction_count;  // Número de instruções do programa
} PCB;

typedef struct {
    Queue* new_queue;
    Queue* ready_queue;
    Queue* blocked_queue;
    Queue* exit_queue;
    PCB* running_process;   // Processo em RUNNING
    PCB* processes[20];     // Array de todos os processos (máx 20)
    int next_pid;           // Próximo PID a ser atribuído
    int current_time;       // Instante atual da simulação
    int programs[5][20];    // Programas disponíveis (como no enunciado)
    int program_counts[5];  // instruction counts
    int program_lengths[5]; // Tamanhos dos programas
} SimulationSystem;

//System Simulation
void initialize_system_with_input(SimulationSystem* system, SimulationInput input);
void run_simulation(SimulationSystem* system);

//Queue operation/interaction
void update_blocked_processes(SimulationSystem* system);
void update_new_processes(SimulationSystem* system);
void update_exit_processes(SimulationSystem* system);

//Instruction/Process Execution
void execute_instruction(SimulationSystem* system, PCB* proc, int instruction);
void execute_running_process(SimulationSystem* system);

//Instruction/Process interaction
PCB* create_new_process(SimulationSystem* system, int prog_id);
void move_process_to_ready(SimulationSystem* system, PCB* process);

//Scheduling
void schedule_next_process(SimulationSystem* system);

//Output
void print_current_state(SimulationSystem* system, int time);

#endif