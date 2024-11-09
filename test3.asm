; Example Assembly Code to Detect Common Errors

start:               ; A valid label
    add 45             ; No operands, valid usage

data1:              ; A valid label
    data 42         ; Initialize memory with value 42

2data2:              ; A valid label
    ldc 10        ; Load constant 10 into accumulator A
    adc 5         ; ERROR: Wrong number of operands for 'adc'

loop:               ; A valid label
    brz end         ; Branch to 'end' if A == 0
    br loop        ; Correct usage of label in branch

wrongLabel:        ; A valid label
    ;jmp wrong       ; ERROR: Unknown mnemonic 'jmp'

    sub             ; Subtraction; valid no operand instruction

duplicate:         ; A valid label
    add             ; Using 'add' again, but let's create a duplicate

duplicate:         ; ERROR: Duplicate label 'duplicate'
    return          ; Return; valid usage

end:               ; Valid label
    halt          ; Stop the emulator
