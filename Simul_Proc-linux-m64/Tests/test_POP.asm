//-----------------
// Instructions
//-----------------
        TEXT 5

        // Programme principal
main    EQU *
        PUSH @op1
        POP @op2
        PUSH #7
        POP @result
        HALT

        END
        
//-----------------
// Données et pile
//-----------------
        DATA 5
        
        WORD 0
result  WORD 0
op1     WORD 20
op2     WORD 5
        
        END
