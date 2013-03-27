#!/bin/bash
echo "<html>" > test.html
echo "<title>" >> test.html
echo "Message Written From Raspberry Pi Server" >> test.html
echo "</title>" >> test.html
echo "<body>" >> test.html
echo $1 >> test.html
echo "</body>" >> test.html
echo "</html>" >> test.html
