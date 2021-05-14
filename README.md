# Interpreter
Built an interpreter for Faux Racket (self-made primitive functional language).

# How to Run
Save the files in a directory and run the following commands:
  1) gcc -std=c99 *.c -o out      (To compile the files)
  2) ./out                        (To run the compiled files)

# Grammar for Faux Racket
‹expr› ::= ‹num›  \n
          | ‹var› \n
          | (+ ‹expr› ‹expr› )  \n
          | (* ‹expr› ‹expr› )  \n
          | (- ‹expr› ‹expr› )  \n
          | (/ ‹expr› ‹expr› )  \n
          | ( ‹expr› ‹expr› )   \n
          | (fun ( ‹var› ) ‹expr› ) \n
          | (with (( ‹var› ‹expr› )) ‹expr› ) \n
