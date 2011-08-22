rm -f $3 "$3"_1.tmp "$3"_2.tmp
echo "\\begin{itemize}\\item\\textbf{Présentation de l'algorithme :}\\end{itemize}\\small\\begin{verbatim}" > "$3"
awk "/""$1""/ , /^}$/" "$2" > "$3"_1.tmp
awk "/^{$/,/^}$/" "$3"_1.tmp > "$3"_2.tmp
awk "/\/\//" "$3"_2.tmp >> "$3"
sed -i 's/^.*\/\/ //' "$3"
echo "\\end{verbatim}" >> "$3"
echo "\\par\\noindent\\hrulefill\\normalsize" >> $3
if [ $(stat -c%s "$3") == 0 ] ; then
    exit 1
fi
