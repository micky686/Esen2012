#!/bin/sh

CMD="aspell --lang=$LANG -t -c "

for x in $@
do
	if [ -d $x ]
	then

		for i in `ls $x`
		do
			$CMD $x/$i
		done

	elif [ -e $x ]
	then
		$CMD $x	
	else
		echo "Unknown file or directory '$x'."
	fi
done
