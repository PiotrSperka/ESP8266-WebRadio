#!/bin/bash
python3 css-html-js-minify.py style.css
xxd -i style.min.css > style
rm style.min.css
python3 css-html-js-minify.py script.js
xxd -i script.min.js > script
rm script.min.js
mv index.html index.htm
python3 css-html-js-minify.py index.htm
xxd -i index.html > index
rm index.html
mv index.htm index.html
