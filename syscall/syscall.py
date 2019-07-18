import contextlib
import re

from contextlib import closing
from urllib.request import urlopen


class SystemCall:
    LINUX_SYSCALLS_64 = 'https://raw.githubusercontent.com/torvalds/linux/master/arch/x86/entry/syscalls/syscall_64.tbl'
    LINUX_SYSCALLS_GENERIC = 'https://raw.githubusercontent.com/torvalds/linux/master/include/uapi/asm-generic/unistd.h'

    @classmethod
    def generate_linux_x64_table(cls):
        with open('linux-x64.tbl', 'w') as x64, closing(urlopen(cls.LINUX_SYSCALLS_64)) as data:
            for line in data:
                if line.startswith(b'#') or line.isspace():
                    continue

                syscall = line.split()

                number = int(syscall[0])
                arch = syscall[1]
                name = syscall[2]

                if arch in (b'common', b'64'):
                    print('%d\t%s' % (number, name.decode('utf-8')), file=x64)

    @classmethod
    def generate_linux_generic_table(cls):
        renr = re.compile(b'#define\s+__NR(?:3264)?_([a-z0-9_]+)\s+(\d+)')

        with open('linux-generic.tbl', 'w') as generic, closing(urlopen(cls.LINUX_SYSCALLS_GENERIC)) as data:
            for line in data:
                if b'#undef __NR_syscalls' in line:
                    break

                match = renr.search(line)

                if match:
                    name, number = match.groups()

                    if name in ('arch_specific_syscall', 'sync_file_range2'):
                        continue

                    print('%d\t%s' % (int(number), name.decode('utf-8')), file=generic)


if __name__ == '__main__':
    SystemCall.generate_linux_x64_table()
    SystemCall.generate_linux_generic_table()

