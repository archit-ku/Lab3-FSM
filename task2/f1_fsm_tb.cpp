#include "Vf1_fsm.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "vbuddy.cpp" // Include the vbuddy library

#define MAX_SIM_CYC 200000 // Define a maximum simulation time

int main(int argc, char **argv, char **env) {
    int i; 
    vluint64_t sim_time = 0; 
    
    // Setup Verilated command arguments
    Verilated::commandArgs(argc, argv);
    
    // Instantiate the design module (Vf1_fsm assumes your module is named f1_fsm)
    Vf1_fsm* top = new Vf1_fsm;

    // Setup VCD tracing (for waveform viewing)
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace (tfp, 99);
    tfp->open ("f1_fsm.vcd");

    // Open vbuddy (for user interaction and display)
    if (vbdOpen() != 1) return(-1);
    vbdHeader("Lab 3: F1 Lights FSM");

    // Initialise inputs
    top->clk = 0;
    top->rst = 1; // Start in reset state
    top->en = 0;
    
    // Run simulation for a fixed number of cycles or until 'q' is pressed
    for (i = 0; i < MAX_SIM_CYC; i++) {
        // Apply reset for the first two clock cycles only
        top->rst = (i < 2); 
        
        // Drive the 'en' signal using the vbuddy switch (mode 1)
        // vbdFlag() returns 1 when the switch is pressed, 0 otherwise.
        top->en = vbdFlag(); 

        // ------------------ Clock Toggle ------------------
        // Toggle the clock signal
        for (int tick = 0; tick < 2; tick++) {
            tfp->dump(sim_time); 
            top->clk = !top->clk;
            top->eval();
            sim_time++;
        }
        // --------------------------------------------------

        // Display the output on the NeoPixel Bar
        // Mask data_out with 0xFF as vbdBar() expects an 8-bit value (0-255).
        vbdBar(top->data_out & 0xFF);
        
        // Update vbuddy display
        vbdCycle(i);

        // Check for finish or 'q' key press
        if ((Verilated::gotFinish()) || (vbdGetkey() == 'q')) 
            break;
    }

    // Cleanup
    vbdClose();
    tfp->close();
    
    // Final evaluation and VCD dump
    top->eval();
    tfp->dump(sim_time);
    
    delete top;
    exit(0);
}