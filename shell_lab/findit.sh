findit() {
	if test $# -lt 2
	then
		echo "Usage: $0 PATH_ROOT PATTERN"
		echo "Example: $0 /usr hello"
	fi
	grep $2 -recursive -Hn --include=*.c --include=*.h $1
}