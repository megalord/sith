#include <stdio.h>
int main () {
    fprintf(stderr, "foo");
}

.LC0:
  .string "foo"
main:
  push rbp
  mov rbp, rsp
  mov rax, QWORD PTR stderr[rip]
  mov rcx, rax
  mov edx, 3
  mov esi, 1
  mov edi, OFFSET FLAT:.LC0
  call fwrite
  mov eax, 0
  pop rbp
  ret



https://godbolt.org/
https://llvm.org/docs/LangRef.html#other-values
https://stackoverflow.com/questions/26053553/syscall-sysenter-on-llvm
https://en.wikibooks.org/wiki/X86_Assembly/Interfacing_with_Linux#Making_a_syscall
https://en.wikibooks.org/wiki/X86_Assembly/X86_Architecture (register list)

https://linux-kernel-labs.github.io/master/lectures/syscalls.html
https://blog.packagecloud.io/eng/2016/04/05/the-definitive-guide-to-linux-system-calls/

https://github.molgen.mpg.de/git-mirror/glibc/blob/glibc-2.15/sysdeps/unix/syscalls.list
https://github.molgen.mpg.de/git-mirror/glibc/blob/glibc-2.15/sysdeps/unix/make-syscalls.sh

http://man7.org/linux/man-pages/man2/syscalls.2.html
