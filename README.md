# StableMatchingCodes

These are the various codes used for [Mathematical models for stable matching problems with incomplete lists and ties](https://doi.org/10.1016/j.ejor.2019.03.017). Each sub-folder in this repository is a coding of a different model, and the paper describes the differences between all the models.
Additionally, improved preprocessing has been introduced in 
[Improving solution times for stable matching problems through preprocessing](https://www.sciencedirect.com/science/article/pii/S0305054820302458).

## Compiling

These all require an operational [Gurobi](https://gurobi.com) runtime installed, and you will have to edit the makefiles to point to the correct location to find the Gurobi library and headers. Once that is done, compiling is handled by running `make`


## Execution

Note that some codes have additional preprocessing added, which leads to two
different ways of calling programs. If additional preprocessing has not been
added, use the following:
```
$ ./EXECUTABLE /path/to Problem.txt Logfile.txt
```

where the `/path/to` is the path to where the problems are stored, `Problem.txt` is a problem file (described below) and Logfile.txt is a place to write the logfile. The solution is written to standard output, and we recommend redirecting this to a file as well.

If additional preprocessing has been added, you will need to run

```
$ ./EXECUTABLE /path/to Problem.txt Logfile.txt PreprocessMode
```

where PreprocessMode is one of the options given in the README.md file in the
folder for each problem type.


## Problem files

Problem files must conform to the following standard exactly. Any deviation may result in unexpected behaviour (although most likely, crashing the program). If you wish to see examples, all of our instances are available at http://researchdata.gla.ac.uk/664/

### SMTI-GRP problem files

```
X
Y
TABLE
```
where `X` is the number of rows and `Y` is the number of columns in the table, and TABLE is a table of floating point numbers, with columns separated by spaces.

### SMTI problem files

```
0
X
Y
PREFERENCES
```

where X is the number of agents of the first type, and Y is the number of agents of the second type. Preferences are given with each line corresponding to the preferences of one agent. For the first set of agents, the line first contains an identifier, and then the preferences in descending order with ties indicated by brackets.

For agents of the second type, the line first contains an identifier, and then the number 1, and then finally the preferences in descending order with ties indicated by brackets. 

### HRT problem files

```
0
X
Y
PREFERENCES
```

Again, X is the number of doctors, and Y is the number of hospitals. The doctors preferences come first, with each line containing first an identifier and then the preferences in descending order with ties indicated by brackets.
Hospital preferences first contain an identifier, and then the hospital capacity, and then the preferences in descending order with ties indicated by brackets.

