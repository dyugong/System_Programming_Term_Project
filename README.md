<head>
<h1>System_Programming_Term_Project</h1>
</head>
<body>
<h2>Strace</h2>
<ul>
<li>파일 결과</li>
<li>time(usec, usecs/call)</li>
<li>calls, errors</li>
<li>System call</li>
<li>"-o" 옵션(txt에 저장or 출력)</li>
</ul>
<h2>Pidstat</h2>
<ul>
<li>시간, UID, PID</li>
<li>USR, System, Guest, wait, CPU</li>
<li>command</li>
<li>path, dirname</li>
<li>ls(execution file name) 결과</li>
<li>옵션 추가 (-p)</li>
</ul>
<h2>Problem</h2>
<ul>
<li>ls & -> 어떻게 없앨지</li>
<li>$(pgrep ls) -> 어떻게 없앨지</li> ->execvp 등을 이용해 해결
<li>interval은 입력을 받을건지, 임의로 설정할 것인지?</li>
</ul>
<h2>Progress</h2>
<ul>
<li>ls 정적이게 strace한번, pidstat 한번 출력</li>
</ul>
</body>
