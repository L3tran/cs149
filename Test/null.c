/**
 * 
 * @file null.c 
 * @author Vivian Letran & Anh Dinh 
 * vivian.letran@sjsu.edu & anh.dinh@sjsu.edu
 * @date 2023-04-23
 * 
 * 1) For null.c , when you try to run the program to print the value of *x , you will recieve a 
 * segmenation fault. This is due to the null pointer as program trys to access memory that it does not have permission to.
 * It terminates with a segmentation fault signal and return nonzero exit status.  
 * 
 * 2) using gcc -g null.c -o null and running the gdb debugger, it outputs that the program received singal SIGSEGV, Segmentation fault.
 * We are able to see the regiser,code,and stack.It shows the reason why the program terminated as stopped reason: SIGSEGV due to memory access from
 * 0x0000555555555168 in main (argc=0x1, argv=0x7fffffffe0e8) at null.c:6
 * Line 6 of the program is printf("%d\n",*x). 
 * 
 * From the register, we are able to see how the values arestored in the CPU registers when the program recieved a 
 * segmentation fault. Regsiter RAX contains 0x0 as a result of dereferencing a null pointer x. 
 * RIP contains the memory addresss where the segmentation fault took place. Instruction  mov eax,DWORD PTR [rax]
 * may be the result of trying to dereference the null pointer x.
 * Looking at the assembly instruction code, address 0x555555555164, the value of [rbp-0x8] (0x0) is loaded into rax. 
 * Then address 0x555555555168, the value of the memory location pointed to by rax (NULL) is loaded into eax. 
 * At address 0x55555555516a, the value of eax is moved into esi, where the argument is passed to the printf function.
 * At address 0x55555555516c, the address of the string "Segmentation fault\n" is loaded into rdi.
 * Looking at the stack, it shows six stack frames: where we see the memory address of the stack frame and value of the stack
 * The fourth frame has a memory address of 0x7fffffffdfe8 and contains the value 0x0, which is the initial value of the x pointer.
 * In our fifth frame, it has a memory address of 0x7fffffffdff0 and contains the value 0x0, which is the value of *x
 * at the time of the segmentation fault. Bottom frame has a memory address of 0x7fffffffdff8 and 
 * contains the memory address of the __libc_start_main function
 * 
 * 3) using valgrind --leak-check=yes null when you run this you receive the a memory error detector that shows
 * the command: null has an invalid read of size 4 at 0x109168: main (null.c:6)
 * and the address 0x0 is not stack'd, malloc'd or (recently free'd) It show the fault signal of the 
 * process termination with default action of signal 11(SIGSEGV) as access not within mapped region of address 0x0
 * at 0x109168: main (null.c:6) Valgrind tell us that the address being accessed at 0x0 which is not allocated or freed
 * gives a segmenation fault as access is outside the region of the program's memory. This signify that in the code
 * there is an uninitalized pointer issue as the program trys to read from a null pointer.
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
     int *x = NULL;
     printf("%d\n",*x);
     return 0;
}