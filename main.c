#include <string.h>
#include <stdio.h>
#include <stdlib.h>  // Add for exit()
#include "include/inputs.h"
#include "include/simulation.h"

int main() {
    SimulationInput inputs[NUM_INPUTS] = {
            {input00, 5}, {input01, 5}, {input02, 4}, {input03, 5}, {input04, 11}, {input05, 11}
    };

    for (int i = 0; i < NUM_INPUTS; i++) {
        SimulationSystem system;
        char filename[20];
        snprintf(filename, sizeof(filename), "output%02d.out", i);

        FILE* output_file = freopen(filename, "w", stdout);
        if (output_file == NULL) {
            perror("Error opening output file");
            continue;  // Skip this input if file fails
        }

        initialize_system_with_input(&system, inputs[i]);
        run_simulation(&system);

        // Cleanup processes safely
        for (int j = 0; j < 20; j++) {
            if (system.processes[j] != NULL) {
                if (system.processes[j]->instructions) {
                    free(system.processes[j]->instructions);
                }
                free(system.processes[j]);
                system.processes[j] = NULL;  // Prevent double-free
            }
        }

        // Cleanup queues safely
        if (system.new_queue) deleteQueue(system.new_queue);
        if (system.ready_queue) deleteQueue(system.ready_queue);
        if (system.blocked_queue) deleteQueue(system.blocked_queue);
        if (system.exit_queue) deleteQueue(system.exit_queue);

        fclose(stdout);
    }

    return 0;
}