# pposim
Ping-Pong Of Slightly Increased Morbidity 
("Ping-Pong of Death" sounded too harsh)

An MPI benchmark that for each rank:

- iterates through every other rank in ascending order and does an MPI_Isend
- opens an output file called "pp_out.\<MY_RANK\>.dat"
- iterates in reverse order calling MPI_Recv from every rank and writes the resulting data to the ouptut file
