//-----------------
// Instructions
//-----------------
        TEXT 30

        // Programme principal
main    EQU *
        LOAD R00, #1
        LOAD R01, #2
        ADD R00, #2
        CALL NC, @0x0031
        STORE R00, @0x0030
        HALT

        END
        
//-----------------
// Données et pile
//-----------------
        DATA 24
        
        WORD 0
result  WORD 0
op1     WORD 20
op2     WORD 5
        
        END
