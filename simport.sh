socat -dd PTY,raw,link=/tmp/ttyFAKE,echo=0 "EXEC:'./grbl_sim.exe 1 -n -s step.out -b block.out',pty,raw,echo=0"
