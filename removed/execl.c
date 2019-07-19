#include <stdio.h>
#include <unistd.h>

int main(void) {
    execl("/bin/uname", "uname", "-a", NULL);

    //execl("/usr/bin/java", "java", "test/Test", NULL);
    //execl("/usr/bin/mono", "mono", "test/test.exe", NULL);
    //execl("/usr/bin/python3", "python", "test/test.py", NULL);
    //execl("test/testc.out", "test.out", NULL);
    //execl("test/testcpp.out", test.out", NULL);
    //execl("/usr/bin/nodejs", "nodejs", "test/test.js", NULL);

    return 0;
}
