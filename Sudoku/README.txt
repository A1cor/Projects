#Compile and run on Linux#
(The project uses GUROBI to solve the board with ILP and LP)

Compile by writing "make all" followed by:
"setenv GUROBI_HOME /usr/local/lib/gurobi563
setenv PATH ${PATH}:${GUROBI_HOME}/bin
setenv LD_LIBRARY_PATH ${GUROBI_HOME}/lib"
and "grbgetkey 704d772a-57b7-11ea-a8fb-020d093b5256".



COMMANDS:

1. solve X:

a. Starts a puzzle in Solve mode, loaded from a file with the name "X", where X
includes a full or relative path to the file.
b. When loading the file, you should ensure it is a valid file – the format is
correct, enough values are provided, values provided are in the correct
range, fixed cells are legal (i.e., when ignoring non-fixed cells the board is
not erroneous), etc. Do not validate the entire board.
c. Note that this command is always available, in Solve, Edit, and Init modes.
Any unsaved work on the current game board is lost.


2. edit [X]:

a. Starts a puzzle in Edit mode, loaded from a file with the name "X", where X
includes a full or relative path to the file.
b. The parameter X is optional. If no parameter is supplied, the program should
enter Edit mode with an empty 9x9 board. There is no command to create a
board of different size (the user must edit a file for that, explained later).
c. If supplied, ensure that the file is valid, but do not check whether the board
is erroneous or not. Any properly formatted file should be accepted.
d. Recall that in Edit mode we always mark errors, regardless of the value of
the "mark errors" parameter (which remains whatever value it contained
previously).
e. Note that this command is always available, in Solve, Edit, and Init modes.
Any unsaved work on the current game board is lost.


3. mark_errors X:

a. Sets the "mark errors" setting to X, where X is either 0 or 1.
b. This command is only available in Solve mode.


4. print_board:

a. Prints the board to the user.
b. This command is only available in Edit and Solve modes.


5. set X Y Z:

a. Sets the value of cell <X,Y> to Z (X is the column, Y is the row).
b. This command is only available in Edit and Solve modes.
c. Indices and values are 1-based. The user may empty a cell by setting Z=0.
d. In solve mode, fixed cells may not be updated.
e. Recall that an erroneous input (such as entering 5 when column X already
contains 5) is allowed (but might be marked when printing the board if in
Solve mode and the "mark errors" parameter is set to 1).


6. validate:

a. Validates the current board using ILP, ensuring it is solvable.
b. This command is only available in Edit and Solve modes.
c. If the board is erroneous, the program prints an error message and the
command is not executed.
d. The command prints whether the board is found to be solvable, or not.


7. guess X:

a. Guesses a solution to the current board using LP (not ILP!), with threshold X.
b. This command is only available in Solve mode.
c. The parameter X is a float and represents the threshold we use for the LP
solution.
d. If the board is erroneous, the program prints an error message and the
command is not executed.
e. The command fills all cell values with a score of X or greater. If several
values hold for the same cell, randomly choose one according to the score
(i.e., a score of 0.6 has double the chance of 0.3).
f. Make sure not to fill illegal values created along the way – ignore invalid cell
values regardless of their score.


8. generate X Y:

a. Generates a puzzle by randomly filling X empty cells with legal values,
running ILP to solve the board, and then clearing all but Y random cells.
b. X and Y should both be integers that satisfy: X>=0, Y>0.
c. This command is only available in Edit mode.
d. If the board does not contain X empty cells, then that is an error and the
user should be notified.
e. To generate a board: randomly choose X cells and fill each selected cell with
a random legal value. Then, run ILP to solve the resulting board. After the
board is solved, randomly choose Y cells out of the entire board and clear
the values of all other cells (i.e., except the Y chosen cells).
f. Note that the X chosen cells are unrelated to the Y chosen cells – we choose
X empty cells from the current board state and, after solving the entire
board with ILP, choose any Y cells from the board to keep.
g. If one of the X randomly chosen cells has no legal value available, or the
resulting board has no solution (i.e., the ILP solver fails), reset the board
back to its original state (before executing the command) and repeat the
command. After 1000 iterations, treat it as an error in the puzzle generator.


9. undo:

a. Undo a previous move done by the user.
b. This command is only available in Edit and Solve modes.
c. Set the current move pointer to the previous move and update the board
accordingly. This does not add or remove any item to/from the list, only
updates our pointer into the list.
d. If there are no moves to undo, it is an error.
e. If there was a move to undo, clearly print to the user the change that was
made. The value of empty cells (with a value of 0) can be printed as "0".


10. redo:

a. Redo a move previously undone by the user.
b. This command is only available in Edit and Solve modes.
c. Set the current move pointer to the next move and update the board
accordingly. This does not add or remove any item to/from the list, only
updates our pointer into the list.
d. If there are no moves to redo, it is an error.
e. If there was a move to redo, clearly print to the user the change that was
made. The value of empty cells (with a value of 0) can be printed as "0".


11. save X:

a. Saves the current game board to the specified file, where X includes a full or
relative path to the file.
b. This command is only available in Edit and Solve modes.
c. In Edit mode, erroneous boards may not be saved.
d. In Edit mode, boards without a solution may not be saved. Validate the
board before saving and notify accordingly.
e. File formats for save files are described below.
f. Saving the puzzle to a file does not modify the Redo/Undo list in any way,
and a reset command (described below) will still revert to the state of the
originally loaded file.
g. In Edit mode, cells containing values are marked as "fixed" in the saved file.


12. hint X Y:

a. Give a hint to the user by showing the solution of a single cell X,Y.
b. This command is only available in Solve mode.
c. If the board is erroneous, cell <X,Y> is fixed, or cell <X,Y> already contains a
value, it is an error (in that order).
d. Run ILP to solve the board. If the board is unsolvable, it is an error.
Otherwise, print to the user the value of cell <X,Y> found by the ILP solution.



13. guess_hint X Y:

a. Show a guess to the user for a single cell X,Y.
b. This command is only available in Solve mode.
c. If the board is erroneous, cell <X,Y> is fixed, or cell <X,Y> already contains a
value, it is an error (in that order).
d. Run LP (not ILP!) for the current board. If the board is unsolvable, it is an
error. Otherwise, print to the user all legal values of cell <X,Y> and their
scores. You should only print values with a score greater than 0.
14. num_solutions
a. Print the number of solutions for the current board.
b. This command is only available in Edit and Solve modes.
c. If the board is erroneous it is an error.
d. Run an exhaustive backtracking for the current board. The exhaustive
backtracking algorithm exhausts all options for the current board, instead of
terminating when a solution is found (as in HW3). Once done, the program
prints the number of solutions found by the algorithm.
e. The exhaustive backtracking algorithm is described below.


15. autofill:

a. Automatically fill "obvious" values – cells which contain a single legal value.
b. This command is only available in Solve mode.
c. If the board is erroneous it is an error.
d. Go over the board and check the legal values of each empty cell. If a cell
<X,Y> has a single legal value, fill it with the value and notify the user of the
update. Repeat for all cells in the board.
e. Note that only cells that had a single legal value before the command was
executed should be filled, e.g., if cell <5,5> has two legal values, but by
updating cell <4,5> we eliminate one of the legal values of cell <5,5>, then
cell <5,5> should not be auto-filled! Some cells may be filled by values that
are erroneous (two neighbors with the same legal value) – that is OK!
f. After performing this command, additional cells may contain obvious values.
However, this command performs only a single iteration and these cells
should not be filled.


16. reset:

a. Undo all moves, reverting the board to its original loaded state.
b. This command is only available in Edit and Solve modes.
c. The command goes over the entire undo/redo list and reverts all moves (no
output is provided).
d. The undo/redo list is not cleared, we just move our pointer to the head of
the list, as if the user entered multiple "undo" commands.
e. This command has no output (except printing the board once done). We do
not output every move that was undone.


17. exit:

a. Terminates the program.
b. All memory resources must be freed, and all open files must be closed.
c. Print an exit message.
d. Note that this command is always available, in any mode. Any unsaved work
is lost.
e. EOF should be treated the same as if the user input the "exit" command.
