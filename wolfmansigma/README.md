openMP compile
g++ -fopenmp -o openMP.exe openMP.cpp

MPI compile
mpic++ -o MPI.exe MPI.cpp
mpirun -np 8 ./MPI.exe

CUDA compile
nvcc -o CUDA.exe CUDA.cpp
./CUDA.exe

openCL compile
g++ -o openCL.exe openCL.cpp -lOpenCL
./openCL.exe

Vector Register
g++ -o vectorReg.exe vectorReg.cpp -msse -msse2 -mavx -mfma -O3
./vectorReg.exe