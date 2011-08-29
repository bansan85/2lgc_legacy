rm -f $3;
echo "\\noindent\\hrulefill\\subsubsection{`echo $1 |sed 's/\\\*//g' |sed 's/^.* //g' |sed 's/_/\\\_/g' |sed 's/(//g'`}\\small\\begin{verbatim}" > $3
awk "/""$1""/ , /\*\//" "$2" >> $3 
echo "\\end{verbatim}\\normalsize{}" >> $3
exit 0
