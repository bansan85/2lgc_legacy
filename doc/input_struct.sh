rm -f $3;
echo -n "\\noindent\\hrulefill\\small\\begin{verbatim}" > $3
awk "/""$1""/ , /^} .*$/" "$2" >> $3 
echo "\\end{verbatim}\\par\\noindent\\hrulefill\\normalsize" >> $3
exit 0
