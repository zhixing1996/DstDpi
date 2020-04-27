# e+e- --> DstDpi

This is a repository to seach for D2(2460).

## Install

> mkdir -p /besfs/groups/cal/dedx/$USER/DstDpi

> cd /besfs/groups/cal/dedx/$USER/DstDpi

> git clone https://github.com/zhixing1996/DstDpi.git

## Initialize and Setup BOSS

> source setup.sh

## Build code

> ./build.sh

## Submit Simulation and Reconstruction jobs(MC), Generate root files(MC and data), and Execute and submit analysis jobs

> ./submit.sh

## For developers

- Fork the code with your personal github ID. See [details](https://help.github.com/articles/fork-a-repo/)

> git clone https://github.com/zhixing1996/DstDpi.git

- Make your change, commit and push

> git commit -a -m "Added feature A, B, C"

> git push

- Make a pull request. See [details](https://help.github.com/articles/using-pull-requests/)

## Some styles to follow
- Minimize the number of main c++ files
- Keep functions length less than one screen
- Seperate hard-coded cuts into script file
- Use pull-request mode on git
- Document well the high-level bash file for work flow
