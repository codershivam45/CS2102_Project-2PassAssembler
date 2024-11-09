    ldc 7            ; Load the first number (3) into the accumulator
    stl 0            ; Store it in a local variable [SP + 0] (multiplicand)
    
    ldc 9            ; Load the second number (4) into the accumulator
    stl 1            ; Store it in [SP + 1] (multiplier)
    
    ldc 0            ; Initialize the result to 0
    stl 2            ; Store the result in [SP + 2]
    
mult_loop:
    ldl 1            ; Load the multiplier into the accumulator
    brz end_mult     ; If multiplier is zero, exit the loop
    
    ldl 2            ; Load the current result
    ldl 0            ; Load the multiplicand
    add              ; Add multiplicand to result (A := B + A)
    stl 2            ; Store updated result in [SP + 2]
    
    ldl 1            ; Load the multiplier again
    adc -1           ; Decrement the multiplier by 1
    stl 1            ; Store the updated multiplier
    
    br mult_loop    ; Go back to mult_loop if multiplier is greater than zero
    
end_mult:
    HALT             ; Stop the emulator

