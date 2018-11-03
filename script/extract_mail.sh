#!/bin/sh

OUTPUT="csv.txt"
touch $OUTPUT
echo "NAME\tMAIL\tDATE" > $OUTPUT

for FILE in ./*.eml
do
	TEST=$(grep -E -o "test license" $FILE)
	INVITE=$(grep -E -o "invited" $FILE)
	UPGRADE=$(grep -E -o "Upgrade" $FILE)

	MAIL=$(grep -E -o "^To: [A-Za-z0-9._%+-]*@[A-Za-z0-9.-]+\.[A-Za-z]{2,6}" $FILE | sed  's .\{4\}  ')
	NAME=$(grep -E -o "<p>Hello .*,</p>" $FILE | sed 's .\{9\}  ' | sed 's .\{5\}$  ' | sed 's/=/\\x/g')
	DATE=$(grep -E -o "Delivery-date: .*" $FILE | sed 's .\{20\}  ' | sed 's .\{15\}$  ')

	RECORD="$NAME\t$MAIL"

	# if [[ ! -z "$INVITE" ]]
	# if [[ ! -z "$UPGRADE" ]]
	if [[ ! -z "$TEST" ]]
	then
		RECORD="$RECORD\t$DATE"

	    echo $RECORD
	    echo $RECORD >> $OUTPUT
	fi
done
