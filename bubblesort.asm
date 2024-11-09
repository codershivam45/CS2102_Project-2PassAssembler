    ldc 6                    ; Number of elements in the array
    stl 0                    ; Store the array length in memory[SP + 0]
    ; Initialize array at SP + 1, SP + 2, SP + 3, SP + 4, SP + 5
    ldc 10                   ; Load 10 into A
    stl 1                    ; Store 10 at memory[SP + 1]
    ldc 5                    ; Load 5 into A
    stl 2                    ; Store 5 at memory[SP + 2]
    ldc 12                   ; Load 12 into A
    stl 3                    ; Store 12 at memory[SP + 3]
    ldc 2                    ; Load 2 into A
    stl 4                    ; Store 2 at memory[SP + 4]
    ldc 3                    ; Load 3 into A
    stl 5                    ; Store 3 at memory[SP + 5]
outer_loop:
    ldl 0                    ; Load array length (5) into A
    adc -1                   ; Subtract 1 (5 - 1 = 4)
    brz end                  ; If array length - 1 == 0, sorting is complete, go to end
    stl 0                    ; Store (array length - 1) back in memory[SP + 0]
    ldc 1                    ; Initialize inner loop index to 1
    stl 6                    ; Store inner loop index in memory[SP + 6]
inner_loop:
    ldl 0                    ; Load array length - 1
    ldl 6                    ; Load inner loop index
    sub                      ; Subtract inner loop index from array length - 1
    brz outer_loop           ; If we've reached the end of the inner loop, go to outer loop
    ; Load array[i] into A
    ldl 6                    ; Load inner loop index into A
    ldnl 0                   ; Load memory[SP + index] into A (array[i])
    ; Load array[i+1] into B
    ldl 6                    ; Load inner loop index into A
    adc 1                    ; Add 1 to index
    ldnl 0                   ; Load memory[SP + (index + 1)] into A, effectively B = array[i+1]
    ; Compare array[i] and array[i+1]
    sub                      ; A = array[i] - array[i+1]
    brlz skip_swap           ; If A < 0, no swap needed, go to skip_swap
    ; Swap array[i] and array[i+1]
    ldl 6
    ldnl 0
    stl 7
    ldl 6
    ldnl 1
    ldl 6
    stnl 0
    ldl 7
    ldl 6
    stnl 1
skip_swap:
    ldc 1                    ; Load 1 into A
    ldl 6                    ; Load inner loop index
    add                      ; Increment index by 1
    stl 6                    ; Store updated index back in memory[SP + 6]
    br inner_loop            ; Repeat inner loop

end:
    HALT                     ; Stop execution

