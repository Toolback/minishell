TODO :
~ Parsing ~

- Parse HereDoc
- handle export / env (export contain env with and without key when env contain only key with value)
- Handle History

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

```
