# Interpreter
Built an interpreter for Faux Racket (self-made primitive functional language).

# How to Run
Save the files in a directory and run the following commands:
  1) gcc -std=c99 *.c -o out      (To compile the files)
  2) ./out                        (To run the compiled files)

# Grammar for Faux Racket
‹expr› ::= ‹num›
         | ‹var›
         | (+ ‹expr› ‹expr› )
         | (* ‹expr› ‹expr› )
         | (- ‹expr› ‹expr› )
         | (/ ‹expr› ‹expr› )
         | ( ‹expr› ‹expr› )
         | (fun ( ‹var› ) ‹expr› )
         | (with (( ‹var› ‹expr› )) ‹expr› )
