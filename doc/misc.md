# fork

## wait
* WNOHANG
* WUNTRACED
* WCONTINUED

* EINTR

* WIFEXITED: 0이면 자식이 정상종료, 0이 아니면 WEXITSTATUS 매크로로 반환값 확인
* WIFSIGNALED: 0이 아니면 uncaught signal 때문에 자식이 종료, WTERMSIG 매크로로 종료시킨 시그널 확인
* WIFSTOPPED: 0이 아니면 uncaught signal 때문에 자식이 멈춤, WSTOPSIG 매크로로 중지시킨 시그널 확인

## 시그널
* signal: 프로세스에 이벤트 발생을 알리는 통지(알림, 메시지)
* generate: 시그널을 발생시키는 이벤트가 발생하면 시그널이 만들어진다.
* delivered: 시그널은 시그널 핸들러를 등록한 프로세스에 배달(전달)된다.
* lifetime: 시그널이 살아있는 시간은 발생부터 배달되는 시간까지이다. generation -> pending -> delivery
* catch: 프로세스는 시그널을 잡아 시그널 핸들러로 처리한다.
* signal handler: sigaction 함수를 호출해서 시그널 핸들러를 등록한다. sigaction 함수는 시그널 핸들러 대신에 SIG_DFL, SIG_IGN으로 호출될 수도 있다.
* SIG_DFL: 기본 동작을 처리하도록 한다
* SIG_IGN: 시그널을 무시하도록 한다.

시그널 동작
* Term : 프로세스 종료
* Core : 프로세스 종료 + 코어 덤프 생성
* Ign : 시그널 무시
* Stop : 프로세스 멈춤 (Ctrl+Z)
* Cont : 멈췄던 프로세스 계속

* SIGABRT: 프로세스 중지 -> 구현에 따라 다름
* SIGALARM: 알람 시계 -> 비정상 종료
* SIGCHLD: 자식 프로세스 종료, 멈춤, 재시작 -> 무시
* SIGCONT: 재시작 -> 재시작
* SIGHUP: 제어 터미널 프로세스에 hang-up (death) -> 비정상종료
* SIGINT: Ctrl + C 인터럽트 -> 비정상 종료
* SIGKILL: 종료(잡거나 무시할 수 없음) -> 비정상 종료
* SIGQUIT: Ctrl + I 코어덤프 종료 -> 구현에 따라 다름
* SIGSTOP: 멈춤(잡거나 무시할 수 없음) -> 멈춤
* SIGTERM: 종료 -> 비정상 종료

