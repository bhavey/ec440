#!/bin/bash
HOST=ftp.benhavey.com
USER=a5358528
PASS=inuasha121

ftp -inv $HOST << EOF

user $USER $PASS

put ./test.html /public_html/test.html

bye

EOF
