# x86-64 CPU Architecture Simulation
![Example](https://github.com/AnisBdz/CPU/blob/docs/docs/example.jpg?raw=true)

## Getting Started

### Prerequisites
Make sure you have installed all of the following prerequisites on your machine:
* `flex>=2.6.4`
* `bison>=3.3.2`
* `cmake>=3.8`
* a `C++17` compiler `gcc, g++, ...`

### Build
```bash
# make sure to build in a seperate folder
mkdir build
cd build
cmake ..
make
```

Pour Debian based operating systems you can juste use the command:
```bash
sudo apt-get install flex bison cmake g++ git && git clone https://github.com/AnisBdz/CPU && cd CPU && mkdir build && cd build && cmake .. && make
```

## Usage
```
Intel's x86-64 Architecture Simulation
Usage:
    ./app [OPTION...] filename

    -f, --filename arg  Input filename
    -i, --interactive   Enable interactive execution
    -s, --speed arg     Execution speed (instruction per minute) (default: 100)
    -h, --help          Print usage
```

## Examples
> examples/add_loop.asm
```assembly
; example program that calculates the sum of two integer vectors
; it showcases data declaration, function calls and some instructions

section .data
	x dd 1, 2, 3, 4
	y dd 5, 6, 7, 8
	z dd 0, 0, 0, 0


section .text
	; the execution starts here
	call main


	; you should always end your program with the halt instruction
	; or else you program will result in a segmentation falt
	hlt

main:
	; enter
	push ebp
	mov ebp, esp

	mov ecx, 0

	mov esi, x
	mov edi, y
	mov edx, z

	loop:
		mov eax, [esi + ecx * 4]
		add eax, [edi + ecx * 4]
		mov [edx + ecx * 4], eax

		add ecx, 1
		cmp ecx, 4
		jne loop

	; leave
	mov esp, ebp
	pop ebp

	ret
```

Check the examples folder for more... examples, ya know!


### Unit Tests
This project uses [Catch2](https://github.com/catchorg/Catch2).
The tests are available in the `tests` folder.
After building the project, a `unit_tests` executable will be created.
```
$ ./unit_tests
===============================================================================
All tests passed (18 assertions in 3 test cases)
```

## Thanks
Special thanks to [@ArthurSonzogni](https://github.com/ArthurSonzogni) for his amazing UI library [FTXUI](https://github.com/ArthurSonzogni/FTXUI)
