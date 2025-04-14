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

    // Read programs column-wise (each column is a program)
    for (int prog_id = 0; prog_id < 5; prog_id++) {
        system->program_lengths[prog_id] = 0;

        for (int step = 0; step < 20; step++) {
            int instruction = input.programs[step][prog_id];
            system->programs[prog_id][step] = instruction;

            if (instruction == 0) {
                system->program_lengths[prog_id] = step;
                break;
            }
            system->program_lengths[prog_id] = step + 1;
        }
    }

<<<<<<< Updated upstream
    PCB* first_process = create_new_process(system, 0);
    enqueue(system->new_queue, first_process);
=======
    for (int i = input.rows; i < 5; i++) {
        system->program_lengths[i] = 1;
        for (int j = 0; j < 20; j++) {
            system->programs[i][j] = 0;
        }
    }

    create_new_process(system, 1);

>>>>>>> Stashed changes
}


/* Queue operations */
<<<<<<< Updated upstream
void update_blocked_processes(SimulationSystem* system) { // BLOCKED to READY
    if (!system->blocked_queue) return;

    size_t size = queueSize(system->blocked_queue);
    for (size_t i = 0; i < size; i++) {
        PCB* proc = (PCB*)getQueueNodeAt(system->blocked_queue, i);
=======
void update_blocked_processes(SimulationSystem* system) {
    if (!system->blocked_queue){
        return;
    }

    size_t size = queueSize(system->blocked_queue);
    for (size_t i = 0; i < size; ++i) {
        PCB* proc = (PCB*) getQueueNodeAt(system->blocked_queue, i);
>>>>>>> Stashed changes

        proc->time_in_state++;
        if (proc->blocked_until <= system->current_time) {
            proc->state = READY;
            if (removeNodeByData(system->blocked_queue, proc)) {
                enqueue(system->ready_queue, proc);
<<<<<<< Updated upstream
                proc->pc++;
=======
                i--; size--;
>>>>>>> Stashed changes
            }
        }
    }
}

<<<<<<< Updated upstream
void update_new_processes(SimulationSystem* system) { // NEW to READY - 2 instants
    if (!system->new_queue) return;

    size_t size = queueSize(system->new_queue);
    for (size_t i = 0; i < size; i++) {
        PCB* proc = (PCB*)getQueueNodeAt(system->new_queue, i);

        proc->time_in_state++;
        if (proc->time_in_state >= 2) {
            proc->state = READY;
            if (removeNodeByData(system->new_queue, proc)) {
                enqueue(system->ready_queue, proc);
            }
        }
    }
}

void update_exit_processes(SimulationSystem* system) { // EXIT gone - 1 instant
    if (!system->exit_queue) return;

    size_t size = queueSize(system->exit_queue);
    for(size_t i = 0; i < size; i++){
        PCB* proc = (PCB*) getQueueNodeAt(system->exit_queue, i);

        proc->time_in_state++;
        if (proc->time_in_state >= 1){
            if (removeNodeByData(system->exit_queue, proc)) {
                if (proc->instructions) {
                    free(proc->instructions);
                }
            }
            if (proc->pid >= 0 && proc->pid <= 20) { // valid PID
                system->processes[proc->pid - 1] = NULL;
            }
            free(proc);
        }
    }
=======
void update_new_processes(SimulationSystem* system) {

}

void update_exit_processes(SimulationSystem* system) {

>>>>>>> Stashed changes
}

/* Instruction EXEC */
void execute_instruction(SimulationSystem* system, PCB* proc, int instruction) {
<<<<<<< Updated upstream
    if (!proc || !proc->instructions || proc->pc < 0 || proc->pc >= proc->instruction_count) {
        printf("Error: Invalid process or PC. PID: %d, PC: %d, inst: %d\n",
               proc ? proc->pid : -1, proc ? proc->pc : -1, instruction);
        if (proc) {
            proc->state = EXIT;
            proc->time_in_state = 0;
            enqueue(system->exit_queue, proc);
        }
        system->running_process = NULL;
        return;
    }

    if (instruction == 0) { // HALT
        printf("PID: %d, PC: %d, inst: %d, HALT\n", proc->pid, proc->pc, instruction);
        proc->state = EXIT;
        proc->time_in_state = 0;
        enqueue(system->exit_queue, proc);
        system->running_process = NULL;
        return;
    }

    if (instruction >= 101 && instruction <= 199) { // JUMP
        int jump = instruction - 100;
        printf("PID: %d, PC: %d, inst: %d, JUMP: %d\n", proc->pid, proc->pc, instruction, jump);
        proc->pc -= jump;
        if (proc->pc < 0) proc->pc = 0;
    } else if (instruction >= 201 && instruction <= 299) { // EXEC
        int program_id = instruction % 100;
        printf("PID: %d, PC: %d, inst: %d, EXEC: %d\n", proc->pid, proc->pc, instruction, program_id);

        if (system->next_pid <= 20 && program_id >= 0 && program_id < 5) {
=======
    if (proc == NULL || instruction == 0) {
        if (proc)
            proc->state = EXIT;
        return;
    }

    if (instruction >= 101 && instruction <= 199) { //JUMP
        int jump = instruction - 100;
        if (proc->pc - jump >= 0){
            proc->pc = proc->pc - jump;
            return;
        }else {
            proc->pc = 0;
            return;
        }
    }
    else if (instruction >= 201 && instruction <= 299) { //EXEC
        int program_id = instruction % 100;
        if (system->next_pid <= 20 && (program_id >= 0 && program_id <= 5)) {
>>>>>>> Stashed changes
            PCB* new_proc = create_new_process(system, program_id);
            if (new_proc) {
                enqueue(system->new_queue, new_proc);
                return;
            }
        }
<<<<<<< Updated upstream
        proc->pc++;
    } else if (instruction < 0) { // BLOCKED
        proc->state = BLOCKED;
        proc->blocked_until = system->current_time + (-instruction);
        printf("PID: %d, PC: %d, inst: %d, BLOCK: %d\n", proc->pid, proc->pc, instruction, proc->blocked_until);
        remove_process_from_all_queues(system, proc);
        enqueue(system->blocked_queue, proc);
        system->running_process = NULL;
        return;
    } else { // Other instructions
        printf("PID: %d, PC: %d, inst: %d, MISC\n", proc->pid, proc->pc, instruction);
=======
    }
    else if (instruction < 0) { //I/O - BLOCK
        proc->state = BLOCKED;
        proc->blocked_until = system->current_time + abs(instruction);
        enqueue(system->blocked_queue, proc);
        system->running_process = NULL;
        return;
    }

    if(!(instruction >= 101 && instruction <= 199)) { //NOT JUMP
>>>>>>> Stashed changes
        proc->pc++;
    }
}

PCB* create_new_process(SimulationSystem* system, int prog_id) {
    if (!system || system->next_pid > 20){
        return NULL;
    }

<<<<<<< Updated upstream
    PCB* new_process = (PCB*)calloc(1, sizeof(PCB));
    new_process->pid = system->next_pid++;  // Increment only once
=======
    PCB* new_process = (PCB*) calloc(1, sizeof(PCB));
    new_process->pid = system->next_pid++;
>>>>>>> Stashed changes
    new_process->program_id = prog_id;
    new_process->time_in_state = 0;
    new_process->state = NEW;
    new_process->remaining_quantum = 0;
    new_process->blocked_until = 0;
    new_process->pc = 0;

    int length = system->program_lengths[prog_id];
    new_process->instruction_count = length;
<<<<<<< Updated upstream
    new_process->instructions = (int*)malloc(length * sizeof(int));

    for (int i = 0; i < length; i++) {
=======
    new_process->instructions = (int*)malloc(length * sizeof(int)); //instruction memory
    if(!new_process->instructions){
        fprintf(stderr, "memory allocation failed for process instructions\n");
        free(new_process);
        return NULL;
    }

    for(int i = 0; i < length; i++){
>>>>>>> Stashed changes
        new_process->instructions[i] = system->programs[prog_id][i];
    }
    system->processes[new_process->pid - 1] = new_process;
    return new_process;
}

/* Process EXEC */
void execute_running_process(SimulationSystem* system) {

<<<<<<< Updated upstream
    PCB* running_proc = system->running_process;

    //DEBUG -- Inst wrong!
    printf(
            "Executing PID %d: PC=%d, Inst=%d, Quantum=%d\n",
            running_proc->pid, running_proc->pc, system->programs[running_proc->pid][running_proc->pc], running_proc->remaining_quantum
    );

    if (running_proc->pc > running_proc->instruction_count || // out of bounds or
        (running_proc->pc <= running_proc->instruction_count && // in bounds and
         running_proc->instructions[running_proc->pc] == 0)){ // HALT
        printf("PID %d, count: %d, Inst: %d, HALTing\n", running_proc->pid, running_proc->instruction_count, running_proc->instructions[running_proc->pc]);
        running_proc->state = EXIT;
        running_proc->time_in_state = 0;
        enqueue(system->exit_queue, running_proc);
        system->running_process = NULL;
        return;
    }

    int instruction = running_proc->instructions[running_proc->pc];
    execute_instruction(system, running_proc, instruction);

    if (running_proc->state == RUNNING){
        //DEBUG
        printf("Process %d: Running with Q=%d\n", running_proc->pid, running_proc->remaining_quantum);

        running_proc->remaining_quantum--;
        if(running_proc->remaining_quantum <= 0) { // done with Q=3
            running_proc->state = EXIT;
            running_proc->time_in_state = 0;
            enqueue(system->exit_queue, running_proc);
            system->running_process = NULL;
        }
    }
=======
>>>>>>> Stashed changes
}

/* Process Scheduling */
void schedule_next_process(SimulationSystem* system) {
<<<<<<< Updated upstream
    if (!system || system->running_process) return;  // NO running process

    if (!isEmpty(system->ready_queue)) {
        PCB* next_proc = dequeue(system->ready_queue);
        if (next_proc) {
            next_proc->state = RUNNING;
            next_proc->remaining_quantum = 3;  // Q = 3
            system->running_process = next_proc;
        }
    }
=======

>>>>>>> Stashed changes
}

/* Outputs */
void print_current_state(SimulationSystem* system, int time) {
    if (!system) return;

    if (time == 1) {
        printf("time inst\tproc1\tproc2\tproc3\tproc4\tproc5\tproc6\tproc7\tproc8\tproc9\tproc10\tproc11\tproc12\tproc13\tproc14\tproc15\tproc16\tproc17\tproc18\tproc19\tproc20\n");
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
        } else {
            state = "";
        }

        printf("\t%-8s", state);
    }
    printf("\n");
}

//DEBUG HELPER
void print_process_instructions(const SimulationSystem* system) {
    if (!system) return;

    printf("===== Process Instructions =====\n");
    for (int prog_id = 0; prog_id < 5; prog_id++) {
        printf("Program %d: [", prog_id);
        for (int i = 0; i < system->program_lengths[prog_id]; i++) {
            printf("%d", system->programs[prog_id][i]);
            if (i < system->program_lengths[prog_id] - 1)
                printf(", ");
        }
        printf("]\n");
        printf("length: %d", system->program_lengths[prog_id]);
    }
    printf("=================================\n");
}

void remove_process_from_all_queues(SimulationSystem* system, PCB* proc) {
    if (!system || !proc) return;

    removeNodeByData(system->new_queue, proc);
    removeNodeByData(system->ready_queue, proc);
    removeNodeByData(system->blocked_queue, proc);
    removeNodeByData(system->exit_queue, proc);
}

/* main flow */
void run_simulation(SimulationSystem* system) {
    if (!system) return;

    print_process_instructions(system);

    for (int time = 1; time <= 100; time++) {
        system->current_time = time;

<<<<<<< Updated upstream
        //queue updates
        update_new_processes(system);
        update_blocked_processes(system);
        update_exit_processes(system);

        if (system->running_process) // RUN process
            execute_running_process(system);

        if (!system->running_process) // no RUN process
            schedule_next_process(system);

        print_current_state(system, time); // print

        if (isEmpty(system->new_queue) && // free OS
            isEmpty(system->ready_queue) &&
            isEmpty(system->blocked_queue) &&
            isEmpty(system->exit_queue) &&
            !system->running_process) {
        }
=======

>>>>>>> Stashed changes
    }
}