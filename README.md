# fork-exec-pipe
classic fork, execv, pipe

Staging:
One program reads a list of words from the 'nonsense_words.txt' file, the letters are rearranged in places in each word. It creates a child process for each word and passes the number of the read word (array index) to the child process via pipe. The child process receives the number and calls another program(!). This other program (./match_word) searches the 'dictionary.txt' file for a normal (readable) word that matches the selected letter set. And this found word has to be returned to the parent process via pipe. The master program must finally print out these "recovered" words in sequence.

C-only. Usage of fork, exec, pipe, write, read, open system calls.

The result of the program's work:
$ ./pipe nonsense_words.txt 

cheer up! after covid lockdown we are going to have a party! 
