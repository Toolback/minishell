TODO :
~ Parsing ~

- Parse HereDoc
- handle export / env (export contain env with and without key when env contain only key with value)
- Handle History



valgrind --leak-check=full -s --show-leak-kinds=all --track-origins=yes --verbose


------ HereDoc Test ------
```bash
message=$(cat <<EOF
Hello,

This is a multi-line message.

Regards,
John
EOF
)

echo "$message"



command <<EOF
line 1
line 2
...
line n
EOF




cat <"./test_files/infile_big" | echo hi 


```




// TOI 
// export, unset, $? TAB, exit

// MOI
// signals, historique, rendre retour tdata, ESPACE


// cd


NORMINETE


Test qui passent pas : echo Hello wolrd' (free non alloué)
• Retirez le SPATH et vérifiez si les commandes ne fonctionnent plus.
• Mettez plusieurs répertoires à PATH (directory I :directory2) et vérifiez qu'ils sont bien évalués de gauche à droite.
