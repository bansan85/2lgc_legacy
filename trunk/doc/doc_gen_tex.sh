#!/bin/sh
rm -f list_sources_c.tmp list_sources_h.tmp *.tmp
find ../src/lib -name "*.c" |sort > list_sources_c.tmp
cat list_sources_c.tmp | while read LINE ; do
    nom=`echo $(basename $LINE)`
    echo $nom
    cat "$LINE" | sed 's/^ .*$//g' | sed 's/}//g' | sed 's/{//g' | sed 's/^\/.*$//g' | sed 's/^#.*$//g' | sed '/^$/d' | sed 's/(.*$//g' | sed 's~*~\\*~g' > $nom".tmp"
    cat $nom".tmp" | while read LINE2 ; do
        nom2=`echo $LINE2 |sed 's/^.* //'`
        echo "$LINE2"
        
        rm -f "$nom2"_func.tex.tmp
        echo "\\noindent\\hrulefill\\subsubsection{`echo "$LINE2" |sed 's/\\\*//g' |sed 's/^.* //g' |sed 's/_/\\\_/g' |sed 's/(//g'`}\\small\\begin{verbatim}" > "$nom2"_func.tex.tmp
        awk "/""$LINE2""/ , /\*\//" "$LINE" >> "$nom2"_func.tex.tmp
        echo "\\end{verbatim}\\normalsize{}" >> "$nom2"_func.tex.tmp
        
        rm -f "$nom2"_com.tex.tmp "$nom2"_com.tex.tmp_1.tmp "$nom2"_com.tex.tmp_2.tmp
        echo "\\begin{itemize}\\item\\textbf{Présentation de l'algorithme :}\\end{itemize}\\small\\begin{verbatim}" > "$nom2"_com.tex.tmp
        awk "/""$LINE2""/ , /^}$/" "$LINE" > "$nom2"_com.tex.tmp_1.tmp
        awk "/^{$/,/^}$/" "$nom2"_com.tex.tmp_1.tmp > "$nom2"_com.tex.tmp_2.tmp
        awk "/\/\//" "$nom2"_com.tex.tmp_2.tmp >> "$nom2"_com.tex.tmp
        sed -i 's/^.*\/\/ //' "$nom2"_com.tex.tmp
        echo "\\end{verbatim}" >> "$nom2"_com.tex.tmp
        echo "\\par\\noindent\\hrulefill\\normalsize" >> "$nom2"_com.tex.tmp
    done
done

find ../src/lib -name "*.h" |sort > list_sources_h.tmp
cat list_sources_h.tmp | while read LINE ; do
    nom=`echo $(basename $LINE)`
    echo $nom
    awk "/#define/ , /\*\//" $LINE |grep -v "__" > $nom".1.tmp"
    echo -n "\\noindent\\hrulefill\\small\\begin{verbatim}" > $nom"_define.tex.tmp"
    awk "/#define/ , /\*\//" $nom".1.tmp" |grep -v "__" >> $nom"_define.tex.tmp"
    echo "\\end{verbatim}\\normalsize{}" >> $nom"_define.tex.tmp"
    
    echo -n "\\noindent\\hrulefill\\small\\begin{verbatim}" > $nom"_struct.tex.tmp"
    awk "/typedef/ , /^} .*$/" $LINE | sed 's/^typedef/\\end{verbatim}\\par\\hrulefill\\begin{verbatim}typedef/g'>> $nom"_struct.tex.tmp"
    echo "\\end{verbatim}\\par\\noindent\\hrulefill\\normalsize" >> $nom"_struct.tex.tmp"
done
exit 0
