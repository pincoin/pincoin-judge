# fork

## wait
* WNOHANG
* WUNTRACED
* WCONTINUED

* EINTR

* WIFEXITED: 0이면 자식이 정상종료, 0이 아니면 WEXITSTATUS 매크로로 반환값 확인
* WIFSIGNALED: 0이 아니면 uncaught signal 때문에 자식이 종료, WTERMSIG 매크로로 종료시킨 시그널 확인
* WIFSTOPPED: 0이 아니면 uncaught signal 때문에 자식이 멈춤, WSTOPSIG 매크로로 중지시킨 시그널 확인
