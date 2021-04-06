# Shell-Implementation-in-C

###### tags: `GitHub`

109-02 NCU CE3002B Operating System

## Test Environment

1. VM: VMware Workstation 16 Player
2. OS: ubuntu-20.04.2.0-desktop-amd64
3. Make version: GNU Make 4.2.1 x86_64-pc-linux-gnu

## fork.c

![](https://i.imgur.com/OLOBR91.png)

### argv

-h: Print this help.  
-d: Implement fork in default mode.  
-c: Implement fork in child first mode.  
-p: Implement fork in parent first mode.  

#### ./fork -d

![](https://i.imgur.com/cxR1p5y.png)

#### ./fork -c

![](https://i.imgur.com/Kug3KDw.png)

#### ./fork -p

![](https://i.imgur.com/RzvCXZq.png)

## shell.c

![](https://i.imgur.com/qNNeeEX.png)

### Usage

Any Linux commands that can execute by exec system call, like execl, execlp, execle, execv, execvp, execvpe.  
And include execute infinite pipe `|`, redirect to file `>`, `>>` and also redirect from file `<`.

> cd is not in part of exec, therefore determine in other function.  
> exit can exit the shell

> TAB, up arrow, down arrow, left arrow, right arrow can not work as in the normal terminal.

#### ./shell

![](https://i.imgur.com/5UVwPjx.png)

> ls -a -l

![](https://i.imgur.com/lyE20kE.png)

> ls -al | tail -7 | head -5 | grep git

![](https://i.imgur.com/xMhnSPE.png)

> ls -al > test.txt > test1.txt

![](https://i.imgur.com/81oR92M.png)

> grep git < test.txt < test2.txt

![](https://i.imgur.com/CMUqIkN.png)

> grep git < test.txt < test2.txt > test3.txt

![](https://i.imgur.com/3Lbbmlr.png)
