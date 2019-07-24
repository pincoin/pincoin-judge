# 시스템콜
## GCC
* hello.s
```
.data

msg:
    .ascii "Hello, world!\n"
    len = . - msg

.text
    .global _start

_start:
    movq  $1, %rax
    movq  $1, %rdi
    movq  $msg, %rsi
    movq  $len, %rdx
    syscall

    movq  $60, %rax
    xorq  %rdi, %rdi
    syscall
```

* 컴파일 및 로드, 실행
```
gcc -c hello.s
ld hello.o
./a.out
```
## NASM
* hello.s
```
[bits 64]

section .text
global _start
 _start:               ; ELF entry point
    mov rax, 1             ; sys_write
    mov rdi, 1             ; STDOUT
    mov rsi, message       ; buffer
    mov rdx, [messageLen]  ; length of buffer
    syscall
    mov rax, 60            ; sys_exit
    mov rdi, 0             ; 0
    syscall

section .data
    messageLen: dq message.end-message
    message: db 'Hello World', 10
 .end:
 ```

 어셈블 및 로드, 실행
 ```
 nasm -felf64 hello.s
 ld hello.o
 ./a.out
 ```

## 호출 방법
### 32비트
- 시스템콜 번호를 EAX 레지스터에 저장
- 아규먼트는 차례대로 EAX, ECX, EDX, ESI, EDI, EBP에 저장
- int 0x80 인터럽트로 시스템콜 호출
- 시스템콜 반환값은 EAX에 저장
- 모든 레지스터 값은 유지

### 64비트
- 시스템콜 번호를 RAX 레지스터에 저장
- 아규먼트는 차례대로 RDI, RSI, RDX, R10, R8, R9에 저장
- syscall 호출
- 시스템콜 반환값은 RAX에 저장
- RCX와 R11 빼고 나머지 레지스터 값 유지

