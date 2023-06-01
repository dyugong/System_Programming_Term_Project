<h1>Profiling Tool(Strace & Pidstat)</h1>
<ul>
  <h2><li>Strace</li></h2>
  <h4><li>strace -c 명령어를 구현</li></h4>
  <h4><li>실행 파일의 system call의 호출 횟수와 걸린 시간에 대한 정보</li></h4>
  <h2><li>Pidstat</li></h2>
  <h4><li>pidstat -l & pidstat -r 명령어를 구현</li></h4>
  <h4><li>실행 파일의 Cpu 와 Memory 사용량에 대한 정보</li></h4>
</ul>

<h2>실행 방법</h2>
<ul>
    <h4><li>Finite proces: ./profile [command]</li></h4>
    <h4><li>Continuous process: ./profile [command] => strace 종료 => pidstat 종료</li></h4>
</ul>
<h2>구현 환경</h2>
<ul>
  <h4><li>Ubuntu 18+</li></h4>
  <h4><li>Language: C</li></h4>
  <h4><li>IDE: linux terminal</li></h4>
</ul>
