# Simulated-Disk-Drive-and-File-System
My final project for a Operating Systems Concepts class I took in the fall of 2023. This program simulates a disk drive and different ways that it can allocate memory within the drive. It has three allocation methods: Contiguous places a file in the first space that it will fit and keeps the entire file adjacent in memory; Chained splits up the file into different blocks across memory to save space and connects them with pointers similar to a linked list; Indexed also splits files across memory but uses a single block of memory to store all the indexes of where the file's data is.
The program can copy files from your system into the simulated file system and vice versa, and display files and representations of the memory on the disk

Runs in the command line

To compile: g++ -o main.exe project3.cpp

To run: ./main.exe <>

Replace the <> in the run command with whichever of the three allocation methods you want the system to use: contiguous, chained, indexed
