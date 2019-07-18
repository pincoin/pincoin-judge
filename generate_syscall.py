import contextlib

from contextlib import closing
from urllib.request import urlopen


LINUX_SYSCALLS_64 = 'https://raw.githubusercontent.com/torvalds/linux/master/arch/x86/entry/syscalls/syscall_64.tbl'


with open('linux-x64.tbl', 'w') as x64, closing(urlopen(LINUX_SYSCALLS_64)) as data:
    for line in data:
        if line.startswith(b'#') or line.isspace():
            continue

        syscall = line.split()

        number = int(syscall[0])
        arch = syscall[1]
        name = syscall[2].strip(b'_')

        if arch in (b'common', b'64'):
            print('%d\t%s' % (number, name.decode('utf-8')), file=x64)
