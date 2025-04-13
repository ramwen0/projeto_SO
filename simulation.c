#include "include/simulation.h"

/* Init */
void initialize_system_with_input(SimulationSystem* system, SimulationInput input) {
    memset(system, 0, sizeof(SimulationSystem));

    system->ready_queue = createQueue();
    system->new_queue = createQueue();
    system->blocked_queue = createQueue();
    system->exit_queue = createQueue();
    system->running_process = NULL;
    system->next_pid = 1;
    system->current_time = 0;

    for (int i = 0; i < 20; ++i) {
        system->processes[i] = NULL;
    }

    for(int i = 0; i < input.rows && i < 5; i++) {  // Ensure i < 5
        for (int j = 0; j < 20; j++) {
            system->programs[i][j] = input.programs[i][j];
        }

        system->program_lengths[i] = 20;
        for (int j = 0; j < 20; j++) {
            if (input.programs[i][j] == 0) {
                system->program_lengths[i] = j + 1;
                break;
            }
        }
    }

    for (int i = input.rows; i < 5; i++) {
        system->program_lengths[i] = 1;
        for (int j = 0; j < 20; j++) {
            system->programs[i][j] = 0;
        }
    }

    PCB* first_process = (PCB*)calloc(1, sizeof(PCB));
    first_process->pid = 1;
    first_process->state = NEW;       // Start as NEW
    first_process->time_in_state = 0; // Not yet eligible for READY
    first_process->pc = 0;
    first_process->instruction_count = system->program_lengths[0];
    first_process->instructions = (int*)malloc(first_process->instruction_count * sizeof(int));

    if (!first_process->instructions) {
        fprintf(stderr, "Memory allocation failed for first process instructions\n");
        free(first_process);
        exit(1);
    }

    for (int i = 0; i < first_process->instruction_count; i++) {
        first_process->instructions[i] = system->programs[0][i];
    }

    system->processes[0] = first_process;
    enqueue(system->new_queue, first_process);
    system->next_pid = 2;
}

/* Queue operations */
void update_blocked_processes(SimulationSystem* system) {
    if (!system->blocked_queue) return;

    size_t size = queueSize(system->blocked_queue);
    for (size_t i = 0; i < size; i++) {
        PCB* proc = (PCB*)getQueueNodeAt(system->blocked_queue, i);
        if (!proc) continue;

        if (proc->blocked_until <= system->current_time) {
            proc->state = READY;
            if (removeNodeByData(system->blocked_queue, proc)) {
                enqueue(system->ready_queue, proc);
                i--;
                size--;
            }
        }
    }
}

void update_new_processes(SimulationSystem* system) {
    if (!system->new_queue) return;

    size_t size = queueSize(system->new_queue);
    for (size_t i = 0; i < size; i++) {
        PCB* proc = (PCB*)getQueueNodeAt(system->new_queue, i);
        if (!proc) continue;

        proc->time_in_state++;
        if (proc->time_in_state > 2) {  // Wait 2 full time units
            proc->state = READY;
            if (removeNodeByData(system->new_queue, proc)) {
                enqueue(system->ready_queue, proc);
                i--;  // Adjust index after removal
                size--;
            }
        }
    }
}

void update_exit_processes(SimulationSystem* system) {
    if (!system->exit_queue) return;

    size_t size = queueSize(system->exit_queue);
    for (size_t i = 0; i < size; i++) {
        PCB* proc = (PCB*)getQueueNodeAt(system->exit_queue, i);
        if (!proc) continue;

        proc->time_in_state++;
        if (proc->time_in_state >= 1) {
            if (removeNodeByData(system->exit_queue, proc)) {
                if (proc->instructions) {
                    free(proc->instructions);
                }
                if (proc->pid > 0 && proc->pid <= 20) {
                    system->processes[proc->pid - 1] = NULL;
                }
                free(proc);
                i--;
                size--;
            }
        }
    }
}

/* Instruction EXEC */
void execute_instruction(SimulationSystem* system, PCB* proc, int instruction) {
    if (proc == NULL || proc->instructions == NULL) {
        if (proc) proc->state = EXIT;
        return;
    }

    if (!proc->instructions) {
        proc->state = EXIT;
        return;
    }

    if (instruction == 0 || proc->pc < 0 || proc->pc >= proc->instruction_count) {
        proc->state = EXIT;
        return;
    }

    if (instruction >= 101 && instruction <= 199) { // JUMP
        int jump = instruction - 100;
        proc->pc = (proc->pc - jump >= 0) ? proc->pc - jump : 0;
    }

    else if (instruction >= 201 && instruction <= 299) { // EXEC
        int program_id = instruction % 100;
        if (system->next_pid <= 20 && program_id >= 0 && program_id < 5) {
            PCB* new_proc = create_new_process(system, program_id);
            if (new_proc) {
                new_proc->state = NEW;
                new_proc->time_in_state = 0; // Start counting from 0
                enqueue(system->new_queue, new_proc); // Add immediately
            }
        }
    }

    if (instruction < 0) { // I/O
        proc->state = BLOCKED;
        proc->blocked_until = system->current_time + (-instruction);
        return;
    }

    if (!(instruction >= 101 && instruction <= 199)) {
        proc->pc++;
    }
}

PCB* create_new_process(SimulationSystem* system, int prog_id) {
    if (!system || prog_id < 0 || prog_id >= 5 || system->next_pid > 20) {
        return NULL;
    }

    PCB* new_process = (PCB*)calloc(1, sizeof(PCB));
    new_process->pid = system->next_pid++;
    new_process->program_id = prog_id;
    new_process->state = NEW;
    new_process->time_in_state = 0; // Critical initialization
    new_process->remaining_quantum = 0;
    new_process->blocked_until = 0;

    int length = system->program_lengths[prog_id];
    new_process->instruction_count = length;
    new_process->instructions = (int*)malloc(length * sizeof(int));

    if (!new_process->instructions) {
        fprintf(stderr, "Memory allocation failed for process instructions\n");
        free(new_process);
        return NULL;
    }

    for (int i = 0; i < length; i++) {
        new_process->instructions[i] = system->programs[prog_id][i];
    }

    system->processes[new_process->pid - 1] = new_process;
    return new_process;
}

/* Process EXEC */
void execute_running_process(SimulationSystem* system) {
    if (!system || !system->running_process) return;

    PCB* proc = system->running_process;

    if (proc->pc >= proc->instruction_count ||
        (proc->pc < proc->instruction_count && proc->instructions[proc->pc] == 0)) {
        proc->state = EXIT;
        proc->time_in_state = 0;
        enqueue(system->exit_queue, proc);
        system->running_process = NULL;
        return;
    }

    int instruction = proc->instructions[proc->pc];
    execute_instruction(system, proc, instruction);

    if (proc->state == RUNNING) {
        proc->remaining_quantum--;
        if (proc->remaining_quantum == 0) {
            proc->state = READY;
            enqueue(system->ready_queue, proc);
            system->running_process = NULL;
        }
    }
}

/* Process Scheduling */
void schedule_next_process(SimulationSystem* system) {
    if (!system || system->running_process) return; // Already running

    if (!isEmpty(system->ready_queue)) {
        PCB* next = dequeue(system->ready_queue);
        if (next) {
            next->state = RUNNING;
            next->remaining_quantum = 3;
            system->running_process = next;
        }
    }
}

/* Outputs */
void print_current_state(SimulationSystem* system, int time) {
    if (!system) return;

    if (time == 1) {
        printf("time inst\tproc1\t\tproc2\t\tproc3\t\tproc4\t\tproc5\t\tproc6\t\tproc7\t\tproc8\t\tproc9\t\tproc10\t\tproc11\t\tproc12\t\tproc13\t\tproc14\t\tproc15\t\tproc16\t\tproc17\t\tproc18\t\tproc19\t\tproc20\n");
    }

    printf("%-8d", time);

    for (int i = 0; i < 20; i++) {
        const char* state = "";

        if (system->processes[i]) {
            switch (system->processes[i]->state) {
                case NEW:     state = "NEW";     break;
                case READY:   state = "READY";   break;
                case RUNNING: state = "RUN";     break;
                case BLOCKED: state = "BLOCKED"; break;
                case EXIT:    state = "EXIT";    break;
                default:      state = "?";       break;
            }
        }

        printf("\t%-8s", state);
    }
    printf("\n");
}

/* main flow */
void run_simulation(SimulationSystem* system) {
    for (int time = 1; time <= 100; time++) {
        system->current_time = time;

        // 1. First update all process states
        update_exit_processes(system);
        update_blocked_processes(system);
        update_new_processes(system);

        // 2. Execute current running process (may create new processes)
        if (system->running_process) {
            execute_running_process(system); // New processes created here
        }

        // 3. Schedule next process
        schedule_next_process(system);

        print_current_state(system, time);
    }
}

/* Cleanup */
void cleanup_simulation(SimulationSystem* system) {
    if (!system) return;

    // Clean up all processes
    for (int i = 0; i < 20; i++) {
        if (system->processes[i]) {
            if (system->processes[i]->instructions) {
                free(system->processes[i]->instructions);
            }
            free(system->processes[i]);
            system->processes[i] = NULL;
        }
    }

    // Clean up queues
    if (system->new_queue) deleteQueue(system->new_queue);
    if (system->ready_queue) deleteQueue(system->ready_queue);
    if (system->blocked_queue) deleteQueue(system->blocked_queue);
    if (system->exit_queue) deleteQueue(system->exit_queue);
}