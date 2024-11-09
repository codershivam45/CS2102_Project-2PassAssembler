    ldc 7            ; Load 7 into the accumulator (A := 7)
    stl 0            ; Store it in a local variable at [SP + 0]
    
    ldc 5            ; Load 5 into the accumulator (A := 5)
    ldl 0            ; Load the first value (7) from [SP + 0] into A
    add              ; Add the values in B and A (A := B + A, so A := 7 + 5)
    
    stl 1            ; Store the result (12) in [SP + 1]
    HALT             ; Stop the emulator
